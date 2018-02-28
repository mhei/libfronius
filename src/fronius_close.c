/*
 * Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <config.h>

#include "fronius-private.h"

int fronius_close(struct fronius_dev *dev)
{
    int fd, rv;


    /* restore original serial settings if saved */
    if (dev->old_tio) {
        if (rv = tcflush(dev->fd, TCIOFLUSH))
            return rv;

        if (rv = tcsetattr(dev->fd, TCSANOW, dev->old_tio))
            return rv;
    }

    fd = dev->fd;
    free(dev);

    return close(fd);
}
