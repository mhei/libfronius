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

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <config.h>

#include "fronius-private.h"

void fronius_pkt_init(struct fronius_pkt *pkt)
{
    memset(pkt, 0, sizeof(*pkt));
    memset(&pkt->start, FRONIUS_START_SEQUENCE, sizeof(pkt->start));
}

void fronius_pkt_checksum(struct fronius_pkt *pkt)
{
    uint8_t *p = &pkt->length;
    int i = FRONIUS_HEADERLEN + pkt->length;

    pkt->checksum = 0;

    while (i--)
        pkt->checksum += *p++;
}

fronius_error_t fronius_pkt_validate_header(const struct fronius_pkt *pkt)
{
    int i;

    /* validate start sequence */
    for (i = 0; i < FRONIUS_FL_START; ++i)
        if (pkt->start[i] != FRONIUS_START_SEQUENCE)
            return FRONIUS_ERR_STARTSEQUENCE;

    /* validate max data length */
    if (pkt->length > FRONIUS_MAX_DATALEN)
        return FRONIUS_ERR_LENGTH;

    /* packet header is valid */
    return FRONIUS_ERR_NOERROR;
}

fronius_error_t fronius_pkt_validate(const struct fronius_pkt * pkt)
{
    uint8_t *p = (uint8_t *) & pkt->length;
    uint8_t checksum = 0;
    fronius_error_t err;
    int i;

    err = fronius_pkt_validate_header(pkt);
    if (err)
        return err;

    /* validate checksum */
    i = FRONIUS_HEADERLEN + pkt->length;

    while (i--)
        checksum += *p++;

    if (pkt->checksum != checksum)
        return FRONIUS_ERR_CHECKSUM;

    /* packet is valid */
    return FRONIUS_ERR_NOERROR;
}

fronius_error_t fronius_pkt_iserror(const struct fronius_pkt * pkt)
{
    if ((pkt->length != 2) || (pkt->command != 0x0e))
        /* does not look like an error */
        return FRONIUS_ERR_NOERROR;

    return (fronius_error_t) pkt->data[1];
}

#define FRONIUS_RAWDUMP_FMT1 ": "
#define FRONIUS_RAWDUMP_FMT1_STRLEN (2)
#define FRONIUS_RAWDUMP_FMT2 "0x%02x "
#define FRONIUS_RAWDUMP_FMT2_STRLEN (5)
#define FRONIUS_RAWDUMP_FMT3 "0x%02x\n"
#define FRONIUS_RAWDUMP_FMT3_STRLEN (5)

int fronius_pkt_rawdump(FILE * stream, const char *prefix,
                        const struct fronius_pkt *pkt)
{
    char *buf1;
    char buf2[8];               /* must be at least max(FRONIUS_RAWDUMP_FMT2_STRLEN, FRONIUS_RAWDUMP_FMT3_STRLEN) */
    int i = FRONIUS_MIN_PKTLEN + pkt->length - FRONIUS_FL_CHECKSUM, c = 0;
    uint8_t *p = (uint8_t *) & pkt->start;

    if (prefix)
        c += strlen(prefix) + FRONIUS_RAWDUMP_FMT1_STRLEN;

    c += i * FRONIUS_RAWDUMP_FMT2_STRLEN;       /* i bytes * [strlen] chars */
    c += FRONIUS_RAWDUMP_FMT3_STRLEN + 1;       /* trailing crc + nul byte */

    buf1 = (char *) malloc(c);
    if (!buf1) {
        errno = ENOMEM;
        return -1;
    }

    if (prefix) {
        strcpy(buf1, prefix);
        strcat(buf1, FRONIUS_RAWDUMP_FMT1);
    }

    while (i--) {
        sprintf(buf2, FRONIUS_RAWDUMP_FMT2, *p++);
        strcat(buf1, buf2);
    }

    sprintf(buf2, FRONIUS_RAWDUMP_FMT3, pkt->checksum);
    strcat(buf1, buf2);

    c = fprintf(stream ? : stderr, "%s", buf1);

    free(buf1);

    return c;
}

#define FRONIUS_HEADERDUMP_FMT "%s%slength=%d, device=%d, number=%d, command=0x%02x, checksum=%d\n"

int fronius_pkt_headerdump(FILE * stream, const char *prefix,
                           const struct fronius_pkt *pkt)
{
    return fprintf(stream ? : stderr, FRONIUS_HEADERDUMP_FMT,
                   prefix ? : "", prefix ? ": " : "",
                   pkt->length, pkt->device, pkt->number, pkt->command,
                   pkt->checksum);
}

int fronius_pkt_dump(FILE * stream, const char *prefix,
                     const struct fronius_pkt *pkt, int raw)
{
    if (raw)
        return fronius_pkt_rawdump(stream, prefix, pkt);
    else
        return fronius_pkt_headerdump(stream, prefix, pkt);
}
