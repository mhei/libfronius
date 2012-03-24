/*
 * Copyright © 2009-2012 Michael Heimpold <mhei@heimpold.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _FRONIUS_PRIVATE_H_
#define _FRONIUS_PRIVATE_H_

#include <sys/time.h>
#include <termios.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "fronius.h"

FRONIUS_BEGIN_DECLS

/* content of start sequence bytes */
#define FRONIUS_START_SEQUENCE   0x80

/* offset of protocol fields */
#define FRONIUS_OF_START	0
#define FRONIUS_OF_LENGTH	3
#define FRONIUS_OF_DEVICE	4
#define FRONIUS_OF_NUMBER	5
#define FRONIUS_OF_COMMAND	6
#define FRONIUS_OF_DATA		7

/* length of protocol fields */
#define FRONIUS_FL_START	3
#define FRONIUS_FL_LENGTH	1
#define FRONIUS_FL_DEVICE	1
#define FRONIUS_FL_NUMBER	1
#define FRONIUS_FL_COMMAND	1
#define FRONIUS_FL_CHECKSUM	1

/* maximum length of data field */
#define FRONIUS_MAX_DATALEN	127

/* length of header fields (without start sequence) */
#define FRONIUS_HEADERLEN	( FRONIUS_FL_LENGTH  + \
				  FRONIUS_FL_DEVICE  + \
				  FRONIUS_FL_NUMBER  + \
				  FRONIUS_FL_COMMAND )

/* minimum packet len of the communication protocol */
#define FRONIUS_MIN_PKTLEN	( FRONIUS_FL_START    + \
				  FRONIUS_HEADERLEN   + \
				  FRONIUS_FL_CHECKSUM )

/* maximum packet len of the communication protocol */
#define FRONIUS_MAX_PKTLEN	( FRONIUS_FL_START    + \
				  FRONIUS_HEADERLEN   + \
				  FRONIUS_MAX_DATALEN + \
				  FRONIUS_FL_CHECKSUM )

struct fronius_pkt {
	uint8_t start[FRONIUS_FL_START];
	uint8_t length;
	uint8_t device;
	uint8_t number;
	uint8_t command;
	uint8_t data[FRONIUS_MAX_DATALEN];
	uint8_t checksum;
};

struct fronius_dev {
	/* file handle */
	int			fd;
	/* type of attached Fronius device */
	fronius_ifc_type_t	ifc_type;
	/* baudrate of serial connection */
	fronius_baudrate_t	baudrate;
	struct termios		*old_tio;

	/* various flags */
	int			debug:1;
	int			rawdump:1;

	/* statistical values */
	struct timeval		last_sent, last_received;
	int			bytes_sent, bytes_received;
	int			pkts_sent, pkts_received;
	int			checksum_errors;
};

/**
 * Fronius packet API
 **/

/*
 * Initialize a packet.
 */
void fronius_pkt_init(struct fronius_pkt *);

/*
 * Calculate the packet checksum.
 */
void fronius_pkt_checksum(struct fronius_pkt *);

/*
 * Validate a packet.
 */
fronius_error_t fronius_pkt_validate_header(const struct fronius_pkt *);
fronius_error_t fronius_pkt_validate(const struct fronius_pkt *);

/*
 * Packet is error message?
 */
fronius_error_t fronius_pkt_iserror(const struct fronius_pkt *);

/*
 * Dump a packet
 */
int fronius_pkt_rawdump(FILE *, const char *, const struct fronius_pkt *);
int fronius_pkt_headerdump(FILE *, const char *, const struct fronius_pkt *);
int fronius_pkt_dump(FILE *, const char *, const struct fronius_pkt *, int);

/*
 * Send a packet to Fronius device.
 */
int fronius_pkt_send(struct fronius_dev *, struct fronius_pkt *);

/*
 * Receive a packet from Fronius device.
 */
int fronius_pkt_recv(struct fronius_dev *, const struct fronius_pkt *);


#define FRONIUS_CMD_IFCARD_GETVERSION		0x01
#define FRONIUS_CMD_IFCARD_GETDEVICETYPE	0x02
#define FRONIUS_CMD_IFCARD_GETTIME		0x03
#define FRONIUS_CMD_IFCARD_GETACTIVEINVERTERS	0x04
#define FRONIUS_CMD_IFCARD_GETACTIVESENSORS	0x05
#define FRONIUS_CMD_IFCARD_GETLOCALNETSTATUS	0x06

#define FRONIUS_CMD_INVERTER_GETPOWERNOW	0x10
#define FRONIUS_CMD_INVERTER_GETENERGYTOAL	0x11
#define FRONIUS_CMD_INVERTER_GETENERGYDAY	0x12
#define FRONIUS_CMD_INVERTER_GETENERGYYEAR	0x13
#define FRONIUS_CMD_INVERTER_GETOPERATINGHOURS_DAY	0x1e

#define FRONIUS_VALUE_OVERFLOW	0x0B
#define FRONIUS_VALUE_UNDERFLOW	0xFC

FRONIUS_END_DECLS

#endif /* _FRONIUS_PRIVATE_H_ */
