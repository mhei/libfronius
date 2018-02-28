/*
 * Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <config.h>

#include "fronius-private.h"

#define FRONIUS_MAX_RETRIES (3)

fronius_error_t fronius_cmd_sendrecv(struct fronius_dev * dev,
                                     struct fronius_pkt * command)
{
    struct fronius_pkt response;
    int rv;
    int i = 0;

    /* send/receive */
    do {
        /* leave function if error occurs */
        if ((rv = fronius_pkt_send(dev, command)) < 0)
            return rv;          /* XXX: Todo map return value */

        /* receive may timeout... */
        rv = fronius_pkt_recv(dev, &response);
    } while (rv == -1 && (errno == ETIMEDOUT || errno == EIO)
             && i++ < FRONIUS_MAX_RETRIES);

    if (i == FRONIUS_MAX_RETRIES || rv < 0)
        return FRONIUS_ERR_INVALID_RESPONSE;

    /* validate response */
    rv = fronius_pkt_iserror(&response);
    if (rv != FRONIUS_ERR_NOERROR)
        return rv;
    if (command->device != response.device ||
        command->number != response.number ||
        command->command != response.command)
        return FRONIUS_ERR_INVALID_RESPONSE;

    /* copy response */
    memcpy(command, &response, sizeof(response));
    return FRONIUS_ERR_NOERROR;
}

fronius_error_t fronius_cmd_ic_getversion(struct fronius_dev * dev,
                                          fronius_ifc_type_t * type,
                                          char *version, size_t size)
{
    struct fronius_pkt pkt;
    int rv;

    /* prepare pkt */
    fronius_pkt_init(&pkt);
    pkt.command = FRONIUS_CMD_IFCARD_GETVERSION;

    /* send, recv & validate */
    if (rv = fronius_cmd_sendrecv(dev, &pkt))
        return rv;
    if (pkt.length != 4)
        return FRONIUS_ERR_INVALID_RESPONSE;

    /* process answer */
    if (type)
        *type = pkt.data[0];
    if (version)
        snprintf(version, size, "%d.%d.%d", pkt.data[1], pkt.data[2],
                 pkt.data[3]);

    return FRONIUS_ERR_NOERROR;
}

struct type_map_entry {
    uint8_t identification;
    char *device;
    char *type;
} devicetype_table[] = {
    /* id,  device/option                           type */
    {
    0xfe, "Fronius IG 15", "Inverter"}, {
    0xfd, "Fronius IG 20", "Inverter"}, {
    0xfc, "Fronius IG 30", "Inverter"}, {
    0xfb, "Fronius IG 30 Dummy", "Inverter"}, {
    0xfa, "Fronius IG 40", "Inverter"}, {
    0xf9, "Fronius IG 60 / IG 60 HV", "Inverter"}, {
    0xf6, "Fronius IG 300", "3-phase Inverter"}, {
    0xf5, "Fronius IG 400", "3-phase Inverter"}, {
    0xf4, "Fronius IG 500", "3-phase Inverter"}, {
    0xf3, "Fronius IG 60 / IG 60 HV", "Inverter"}, {
    0xee, "Fronius IG 2000", "Inverter"}, {
    0xed, "Fronius IG 3000", "Inverter"}, {
    0xeb, "Fronius IG 4000", "Inverter"}, {
    0xea, "Fronius IG 5100", "Inverter"}, {
    0xe5, "Fronius IG 2500-LV", "Inverter"}, {
    0xe3, "Fronius IG 4500-LV", "Inverter"}, {
    0xdf, "Fronius IG Plus 11.4-3 Delta", "3-phase Inverter"}, {
    0xde, "Fronius IG Plus 11.4-1 UNI", "Inverter"}, {
    0xdd, "Fronius IG Plus 10.0-1 UNI", "Inverter"}, {
    0xdc, "Fronius IG Plus 7.5-1 UNI", "Inverter"}, {
    0xdb, "Fronius IG Plus 6.0-1 UNI", "Inverter"}, {
    0xda, "Fronius IG Plus 5.0-1 UNI", "Inverter"}, {
    0xd9, "Fronius IG Plus 3.8-1 UNI", "Inverter"}, {
    0xd8, "Fronius IG Plus 3.0-1 UNI", "Inverter"}, {
    0xd7, "Fronius IG Plus 120-3", "3-phase Inverter"}, {
    0xd6, "Fronius IG Plus 70-2", "2-phase Inverter"}, {
    0xd5, "Fronius IG Plus 70-1", "Inverter"}, {
    0xd4, "Fronius IG Plus 35-1", "Inverter"}, {
    0xd3, "Fronius IG Plus 150-3", "3-phase Inverter"}, {
    0xd2, "Fronius IG Plus 100-2", "2-phase Inverter"}, {
    0xd1, "Fronius IG Plus 100-1", "Inverter"}, {
    0xd0, "Fronius IG Plus 50-1", "Inverter"}, {
    0xcf, "Fronius IG Plus 12.0-3 WYE277", "Inverter"}, {
    0xfe, "Sensor card / Sensor box", "DatCom component"}, {
0xff, "Unknown device or option, device or option not active", NULL},};

fronius_error_t fronius_cmd_ic_getdevicetype(struct fronius_dev *dev,
                                             uint8_t device,
                                             uint8_t number,
                                             char **device_name,
                                             char **device_type)
{
    struct fronius_pkt pkt;
    int rv;
    struct type_map_entry *p = devicetype_table;

    /* prepare pkt */
    fronius_pkt_init(&pkt);
    pkt.device = device;
    pkt.number = number;
    pkt.command = FRONIUS_CMD_IFCARD_GETDEVICETYPE;

    /* send, recv & validate */
    if (rv = fronius_cmd_sendrecv(dev, &pkt))
        return rv;
    if (pkt.length > 1)
        return FRONIUS_ERR_INVALID_RESPONSE;

    /* process answer */
    for (;
         (p->identification != pkt.data[0]) && (p->identification != 0xff);
         p++);

    if (device_name)
        *device_name = p->device;
    if (device_type)
        *device_type = p->type;

    return FRONIUS_ERR_NOERROR;
}

static fronius_error_t fronius_cmd_ic_querylist(struct fronius_dev *dev,
                                                uint8_t cmd,
                                                int maxlistsize,
                                                char *list, size_t size)
{
    struct fronius_pkt pkt;
    int rv;

    /* prepare pkt */
    fronius_pkt_init(&pkt);
    pkt.command = cmd;

    /* send, recv & validate */
    if (rv = fronius_cmd_sendrecv(dev, &pkt))
        return rv;
    if (pkt.length > maxlistsize)
        return FRONIUS_ERR_INVALID_RESPONSE;

    /* process answer */
    if (list)
        memcpy(list, pkt.data, (size >= pkt.length) ? pkt.length : size);

    return FRONIUS_ERR_NOERROR;
}

fronius_error_t fronius_cmd_ic_getactiveinverters(struct fronius_dev * dev,
                                                  char *list, size_t size)
{
    return fronius_cmd_ic_querylist(dev,
                                    FRONIUS_CMD_IFCARD_GETACTIVEINVERTERS,
                                    100, list, size);
}

fronius_error_t fronius_cmd_ic_getactivesensors(struct fronius_dev * dev,
                                                char *list, size_t size)
{
    return fronius_cmd_ic_querylist(dev,
                                    FRONIUS_CMD_IFCARD_GETACTIVESENSORS,
                                    10, list, size);
}

fronius_error_t fronius_cmd_iv_getpowernow(struct fronius_dev * dev,
                                           int *value)
{
    struct fronius_pkt pkt;
    int rv;

    fronius_pkt_init(&pkt);

    pkt.device = FRONIUS_DEVICE_INVERTER;
    pkt.number = 1;
    pkt.command = FRONIUS_CMD_INVERTER_GETPOWERNOW;

    /* send, recv & validate */
    if (rv = fronius_cmd_sendrecv(dev, &pkt))
        return rv;
    if (pkt.length > 3)
        return FRONIUS_ERR_INVALID_RESPONSE;

    /* process answer */
    if (value)
        *value = pkt.data[0] << 8 | pkt.data[1];

    return FRONIUS_ERR_NOERROR;
}

fronius_error_t fronius_get_value(struct fronius_pkt * pkt, double *value)
{
    signed char e = pkt->data[2];

    if (e == FRONIUS_VALUE_OVERFLOW)
        return FRONIUS_ERR_VALUE_OVERFLOW;

    if (e == FRONIUS_VALUE_UNDERFLOW)
        return FRONIUS_ERR_VALUE_UNDERFLOW;

    if (value)
        *value = (pkt->data[0] << 8 | pkt->data[1]) * pow(10, e);

    return FRONIUS_ERR_NOERROR;
}


fronius_error_t fronius_cmd_iv_getvalue(struct fronius_dev * dev,
                                        unsigned char cmd, double *value)
{
    struct fronius_pkt pkt;
    int rv;
    double val;

    fronius_pkt_init(&pkt);

    pkt.device = FRONIUS_DEVICE_INVERTER;
    pkt.number = 1;
    pkt.command = cmd;

    /* send, recv & validate */
    if (rv = fronius_cmd_sendrecv(dev, &pkt))
        return rv;
    if (pkt.length > 3)
        return FRONIUS_ERR_INVALID_RESPONSE;

    return fronius_get_value(&pkt, value);
}
