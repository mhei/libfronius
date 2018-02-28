/*
 * Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <config.h>

#include "fronius-private.h"

int fronius_pkt_send(struct fronius_dev *dev, struct fronius_pkt *pkt)
{
    uint8_t buf[FRONIUS_MAX_PKTLEN];
    int bytes_sent = 0, count;

    /* calculate checksum */
    fronius_pkt_checksum(pkt);

    /* copy to buffer */
    memcpy(&buf[FRONIUS_OF_START], pkt->start,
           FRONIUS_FL_START + FRONIUS_HEADERLEN);
    memcpy(&buf[FRONIUS_OF_DATA], pkt->data, pkt->length);
    buf[FRONIUS_OF_DATA + pkt->length] = pkt->checksum;

    /* transmit data stream */
    count = FRONIUS_MIN_PKTLEN + pkt->length;
    do {
        ssize_t rv = write(dev->fd, &buf[bytes_sent], count - bytes_sent);
        if (rv < 0) {
            if (errno == EINTR)
                continue;
            return rv;
        }
        /* update byte counter */
        bytes_sent += rv;
        dev->bytes_sent += rv;
    } while (bytes_sent < count);

    /* update packet counter */
    dev->pkts_sent++;
    gettimeofday(&dev->last_sent, NULL);

    /* dump packet */
    if (dev->debug)
        fronius_pkt_dump(NULL, "fronius_pkt_send", pkt, dev->rawdump);

    return 0;
}
