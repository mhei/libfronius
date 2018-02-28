/*
 * Copyright © 2009-2012 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: GPL-3.0+
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <config.h>

#include "fronius-private.h"

int main(int argc, char *argv[])
{
    char *device = "/dev/ttyS0";
    struct fronius_dev *dev;
    struct fronius_pkt pkt;
    char buf[255];
    char *devname, *devtype;
    int i;
    double f;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s device\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((dev = fronius_open(argv[1] ? : device, FRONIUS_IFC_TYPE_INTERFACECARDEASY, FRONIUS_BAUDRATE_AUTO)) == NULL) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    dev->debug = 1;
    dev->rawdump = 1;

    memset(buf, 0, sizeof(buf));
    fronius_cmd_ic_getversion(dev, NULL, buf, sizeof(buf));
#if 1
    printf("Version: %s\n", buf);

    memset(buf, 0, sizeof(buf));
    fronius_cmd_ic_getactiveinverters(dev, buf, sizeof(buf));
    for (i = 0; i < (sizeof(buf) - 1) && buf[i] != 0; ++i) {
        devname = NULL;
        devtype = NULL;
        fronius_cmd_ic_getdevicetype(dev, FRONIUS_DEVICE_INVERTER, buf[i], &devname, &devtype);
        printf("Found inverter: %s (ID: %d%s%s)\n", devname, buf[i], devtype != NULL ? ", " : "",
               devtype != NULL ? devtype : "");
    }
#endif
    fronius_cmd_iv_getvalue(dev, FRONIUS_CMD_INVERTER_GETPOWERNOW, &f);
    printf("Power now: %f W\n", f);

    fronius_cmd_iv_getvalue(dev, FRONIUS_CMD_INVERTER_GETENERGYDAY, &f);
    printf("Energy today: %f kWh\n", f);

    fronius_cmd_iv_getvalue(dev, 0x11, &f);
    printf("Energy total: %f kWh\n", f);

    fronius_cmd_iv_getvalue(dev, 0x13, &f);
    printf("Energy year: %f kWh\n", f);

    fronius_cmd_iv_getvalue(dev, 0x14, &f);
    printf("AC current now: %f A\n", f);

    fronius_cmd_iv_getvalue(dev, 0x15, &f);
    printf("AC voltage now: %f V\n", f);

    fronius_cmd_iv_getvalue(dev, 0x19, &f);
    printf("Yield today: %f curr\n", f);

    fronius_cmd_iv_getvalue(dev, FRONIUS_CMD_INVERTER_GETOPERATINGHOURS_DAY, &f);
    printf("Operation hours today: %f min\n", f);

#if 1
    printf("Checksum errors: %d\n", dev->checksum_errors);
#endif
    if (fronius_close(dev) < 0) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
