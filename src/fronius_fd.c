/*
 * Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <config.h>

#include "fronius-private.h"

int fronius_fd(struct fronius_dev *dev)
{
    return dev->fd;
}
