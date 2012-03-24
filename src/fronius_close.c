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
