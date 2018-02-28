/*
 * Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <time.h>

#include <config.h>

#include "fronius-private.h"

void dump_buf(uint8_t * buffer, size_t size)
{
    fprintf(stderr, "Buffer:");
    while (size--)
        fprintf(stderr, " 0x%02x", *buffer++);
    fprintf(stderr, "\n");
}

#define FRONIUS_RECV_TIMEOUT (3)

int fronius_pkt_recv(struct fronius_dev *dev,
                     const struct fronius_pkt *pkt)
{
    uint8_t buf[FRONIUS_MAX_PKTLEN];
    uint8_t crc;
    size_t pktlen = FRONIUS_MAX_PKTLEN;
    size_t bytes_read = 0;
    fd_set rfds;
    struct timeval tv;
    int rv;

    memset(&buf, 0, sizeof(buf));

    do {
        FD_ZERO(&rfds);
        FD_SET(dev->fd, &rfds);
        tv.tv_sec = FRONIUS_RECV_TIMEOUT;
        tv.tv_usec = 0;

        rv = select(dev->fd + 1, &rfds, NULL, NULL, &tv);

    } while (rv == -1 && errno == EINTR);

    if (rv == -1)
        return rv;

    /* timed out */
    if (rv == 0) {
        errno = ETIMEDOUT;
        return -1;
    }

    do {
        ssize_t c = read(dev->fd, &buf[bytes_read], pktlen - bytes_read);
        if (c <= -1) {
            if (errno == EINTR)
                continue;
            return c;
        }

        /* got some bytes */
        bytes_read += c;
        dev->bytes_received += c;

        /* does the buffer looks like a packet? */
        if (bytes_read >= FRONIUS_MIN_PKTLEN) {
            switch (fronius_pkt_validate_header
                    ((struct fronius_pkt *) buf)) {
            case FRONIUS_ERR_NOERROR:
                /* trim read to real packet length after
                   fronius_pkt_validate_header ensured maximum packet length */
                pktlen = buf[FRONIUS_OF_LENGTH] + FRONIUS_MIN_PKTLEN;
                break;
            default:
                /* read data looks strange */
#if 0
                fprintf(stderr, "strange data\n");
                dump_buf(buf, bytes_read);
#endif
                errno = EIO;
                return -1;
            }
        }

    } while (bytes_read < pktlen);

    /* move crc to end of buffer (use buffer to not override it accidently) */
    crc = buf[pktlen - 1];
    buf[pktlen - 1] = 0;
    buf[FRONIUS_MAX_PKTLEN - 1] = crc;

    /* copy buffer to packet */
    memcpy((void *) pkt, buf, sizeof(*pkt));

    /* update packet counter */
    dev->pkts_received++;
    gettimeofday(&dev->last_received, NULL);

    /* dump packet */
    if (dev->debug)
        fronius_pkt_dump(NULL, "fronius_pkt_recv", pkt, dev->rawdump);

    /* deep packet validation */
    switch (fronius_pkt_validate(pkt)) {
    case FRONIUS_ERR_NOERROR:
        break;
    case FRONIUS_ERR_CHECKSUM:
        dev->checksum_errors++;
        /* fallthrough */
    default:
        errno = EIO;
        return -1;
    }

    return 0;
}
