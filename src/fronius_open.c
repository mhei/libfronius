/*
 * Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netdb.h>

#include <config.h>

#include "fronius-private.h"

static int fronius_setserialmode(struct fronius_dev *dev)
{
    struct termios new_tio;
    int rv;

    /* allocate memory for saving old settings */
    dev->old_tio = malloc(sizeof(struct termios));
    if (!dev->old_tio)
        return -1;

    if (rv = tcgetattr(dev->fd, dev->old_tio))
        goto free;

    memcpy(&new_tio, dev->old_tio, sizeof(new_tio));

    /* varios bits for raw mode */
    new_tio.c_iflag &= ~(IGNBRK | IGNPAR);
    new_tio.c_iflag |= BRKINT;
    new_tio.c_oflag &= ~OPOST;
    new_tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    new_tio.c_cflag |= CLOCAL | CREAD;

    /* disable software flow control */
    new_tio.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* select protocol 8N1 */
    new_tio.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
    new_tio.c_cflag |= CS8;

    /* set baudrate */
    if (rv = cfsetispeed(&new_tio, dev->baudrate))
        goto free;
    if (rv = cfsetospeed(&new_tio, dev->baudrate))
        goto free;

    if (rv = tcflush(dev->fd, TCIOFLUSH))
        goto free;

    rv = tcsetattr(dev->fd, TCSANOW, &new_tio);

free:
    /* free memory on error */
    if (rv) {
        free(dev->old_tio);
        dev->old_tio = NULL;
    }
    return rv;
}

static int fronius_opensocket(struct fronius_dev *dev,
                              const char *hostname, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;        /* allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;

    s = getaddrinfo(hostname, port, &hints, &result);
    if (s != 0) {
        /* set a reasonable errno for compatibility */
        if (s != EAI_SYSTEM)
            errno = EINVAL;
        return -1;
    }

    /* getaddrinfo() returns a list of address structures. Try each address until
       we successfully connect. If socket or connect fails, we (close the socket
       and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        dev->fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (dev->fd == -1)
            continue;

        if (connect(dev->fd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;              /* success */

        close(dev->fd);
    }

    if (rp == NULL) {           /* no address succeeded */
        errno = EINVAL;
        return -1;
    }

    freeaddrinfo(result);       /* no longer needed */

    return 0;
}

#define SOCKET_PREFIX "raw://"

struct fronius_dev *fronius_open(const char *dev_name,
                                 const fronius_ifc_type_t ifc_type,
                                 const fronius_baudrate_t baud)
{
    struct fronius_dev *dev;

    /* require baudrate setting for Interface Card; Interface Card Easy will
       auto-detect baudrate */
    if (ifc_type == FRONIUS_IFC_TYPE_PROBE ||
        (ifc_type == FRONIUS_IFC_TYPE_INTERFACECARD
         && baud == FRONIUS_BAUDRATE_AUTO)) {
        errno = EINVAL;
        return NULL;
    }

    dev = malloc(sizeof(struct fronius_dev));
    if (dev) {
        memset(dev, 0, sizeof(struct fronius_dev));

        dev->ifc_type = ifc_type;
        dev->baudrate = baud;

        /* use highest supported baudrate for Interface Card Easy if not preset */
        if (dev->ifc_type == FRONIUS_IFC_TYPE_INTERFACECARDEASY &&
            baud == FRONIUS_BAUDRATE_AUTO)
            dev->baudrate = FRONIUS_BAUDRATE_B19200;

        /* dev_name can contain an inet addr ala
           raw://hostname:port */
        if (strstr(dev_name, SOCKET_PREFIX) == dev_name) {
            /* use a socket for device communication */
            char hostname[255];
            char *port = rindex(dev_name, ':');

            /* find port number */
            if (port)
                port++;

            if (!port || (port && !*port)) {
                errno = EINVAL;
                goto free;
            }

            /* fetch hostname */
            memset(hostname, 0, sizeof(hostname));
            strncpy(hostname, &dev_name[strlen(SOCKET_PREFIX)],
                    strlen(dev_name) - strlen(SOCKET_PREFIX) -
                    strlen(":") - strlen(port));
#if 0
            fprintf(stderr, "Got hostname: %s, port %s\n", hostname, port);
#endif
            if (fronius_opensocket(dev, hostname, port))
                goto free;

        } else {
            /* use a serial port for device communication */
            dev->fd =
                open(dev_name, O_RDWR | O_CLOEXEC | O_NOCTTY | O_NDELAY);
            if (dev->fd == -1)
                goto free;

            /* configurate the serial port */
            if (fronius_setserialmode(dev))
                goto close;
        }

        /* blocking i/o */
        if (fcntl(dev->fd, F_SETFL, 0))
            goto close;
    }

    return dev;

close:
    close(dev->fd);

free:
    free(dev);
    return NULL;
}
