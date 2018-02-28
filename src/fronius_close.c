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
    int fd;

    /* restore original serial settings if saved */
    if (dev->old_tio) {
        if (tcflush(dev->fd, TCIOFLUSH) == -1)
            return -1;

        if (tcsetattr(dev->fd, TCSANOW, dev->old_tio) == -1)
            return -1;
    }

    fd = dev->fd;
    free(dev);

    return close(fd);
}
