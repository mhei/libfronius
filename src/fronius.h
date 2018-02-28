/*
 * Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#ifndef FRONIUS_H
#define FRONIUS_H

#include <termios.h>
#include <fronius-version.h>

#ifdef  __cplusplus
# define FRONIUS_BEGIN_DECLS  extern "C" {
# define FRONIUS_END_DECLS    }
#else
# define FRONIUS_BEGIN_DECLS
# define FRONIUS_END_DECLS
#endif

FRONIUS_BEGIN_DECLS

typedef enum {
	FRONIUS_IFC_TYPE_PROBE             = 0x00,
	FRONIUS_IFC_TYPE_INTERFACECARD     = 0x01,
	FRONIUS_IFC_TYPE_INTERFACECARDEASY = 0x02,
	FRONIUS_IFC_TYPE_VIRTUALINTERFACE  = 0x03,
	FRONIUS_IFC_TYPE_INTERFACEIGPLUS   = 0x04,
} fronius_ifc_type_t;

typedef enum {
	FRONIUS_BAUDRATE_AUTO   =     B0,
	FRONIUS_BAUDRATE_B2400  =  B2400,
	FRONIUS_BAUDRATE_B4800  =  B4800,
	FRONIUS_BAUDRATE_B9600  =  B9600,
	FRONIUS_BAUDRATE_B14400 =     B0,
	FRONIUS_BAUDRATE_B19200 = B19200,
} fronius_baudrate_t;

typedef enum {
	FRONIUS_ERR_NOERROR            = 0x00,
	/* protocol errors */
	FRONIUS_ERR_UNKNOWN_CMD        = 0x01,
	FRONIUS_ERR_TIMEOUT            = 0x02,
	FRONIUS_ERR_INCORRECT_DATA     = 0x03,
	FRONIUS_ERR_QUEUE_FULL         = 0x04,
	FRONIUS_ERR_NO_DEV             = 0x05,
	FRONIUS_ERR_NO_RESPONSE        = 0x06,
	FRONIUS_ERR_SENSOR_ERROR       = 0x07,
	FRONIUS_ERR_SENSOR_NOT_ACTIVE  = 0x08,
	FRONIUS_ERR_INVALID_CMD        = 0x09,
	FRONIUS_ERR_COLLISION_DETECTED = 0x0A,
	/* library errors */
	FRONIUS_ERR_STARTSEQUENCE      = 0x81,
	FRONIUS_ERR_LENGTH             = 0x82,
	FRONIUS_ERR_CHECKSUM           = 0x83,
	FRONIUS_ERR_INVALID_RESPONSE   = 0x84,
	FRONIUS_ERR_VALUE_OVERFLOW     = 0x85,
	FRONIUS_ERR_VALUE_UNDERFLOW    = 0x86,
} fronius_error_t;


struct fronius_dev;

/*
 * Open the serial device.
 */
struct fronius_dev *fronius_open(const char *,
                                 const fronius_ifc_type_t,
                                 const fronius_baudrate_t);

/*
 * Close the serial device, free all resources.
 */
int fronius_close(struct fronius_dev *);

/*
 * Returns the file descriptor in use for the fronius device.
 */
int fronius_fd(struct fronius_dev *);

/*
 * Probe port for fronius device and return interface card type and baudrate.
 */
int fronius_probe(const char *, fronius_ifc_type_t *, fronius_baudrate_t *);


fronius_error_t fronius_cmd_ic_getversion(struct fronius_dev *, fronius_ifc_type_t *, char *, size_t);
fronius_error_t fronius_cmd_ic_getdevicetype(struct fronius_dev *, uint8_t, uint8_t, const char **, const char **);
fronius_error_t fronius_cmd_ic_getactiveinverters(struct fronius_dev *, char *, size_t);
fronius_error_t fronius_cmd_ic_getactivesensorss(struct fronius_dev *, char *, size_t);

#define FRONIUS_DEVICE_GENERALDATA	0x00
#define FRONIUS_DEVICE_INTERFACECARD	0x00
#define FRONIUS_DEVICE_INVERTER		0x01
#define FRONIUS_DEVICE_SENSORCARD	0x02

fronius_error_t fronius_cmd_iv_getpowernow(struct fronius_dev *, int *);

fronius_error_t fronius_cmd_iv_getvalue(struct fronius_dev *, unsigned char cmd, double *);

typedef enum fronius_unit {
  FRONIUS_UNIT_W,
  FRONIUS_UNIT_WH,
  FRONIUS_UNIT_A,
  FRONIUS_UNIT_V,
  FRONIUS_UNIT_HZ,
  FRONIUS_UNIT_MIN,
  FRONIUS_UNIT_C,
  FRONIUS_UNIT_RPM,
  FRONIUS_UNIT_WPM2,
  FRONIUS_UNIT_CURR,
  FRONIUS_UNIT_TEMP,
} fronius_unit_t;

static const char *fronius_unit_str[] = {
  [FRONIUS_UNIT_W]    = "W",
  [FRONIUS_UNIT_WH]   = "Wh",
  [FRONIUS_UNIT_A]    = "A",
  [FRONIUS_UNIT_V]    = "V",
  [FRONIUS_UNIT_HZ]   = "Hz",
  [FRONIUS_UNIT_MIN]  = "min",
  [FRONIUS_UNIT_C]    = "°C",
  [FRONIUS_UNIT_RPM]  = "rpm",
  [FRONIUS_UNIT_WPM2] = "W/m²",
  [FRONIUS_UNIT_CURR] = "",
  [FRONIUS_UNIT_TEMP] = "",
};

enum fronius_cmd {
  /* commands for inverters */
  FRONIUS_CMD_POWER_NOW    = 0x10,
  FRONIUS_CMD_ENERGY_TOTAL,
  FRONIUS_CMD_ENERGY_DAY,
  FRONIUS_CMD_ENERGY_YEAR,
  FRONIUS_CMD_AC_CURRENT_NOW,
  FRONIUS_CMD_AC_VOLTAGE_NOW,
  FRONIUS_CMD_AC_FREQUENCY_NOW,
  FRONIUS_CMD_DC_CURRENT_NOW,
  FRONIUS_CMD_DC_VOLTAGE_NOW,
  FRONIUS_CMD_YIELD_DAY,
  FRONIUS_CMD_MAX_POWER_DAY,
  FRONIUS_CMD_MAX_AC_VOLTAGE_DAY,
  FRONIUS_CMD_MIN_AC_VOLTAGE_DAY,
  FRONIUS_CMD_MAX_DC_VOLTAGE_DAY,
  FRONIUS_CMD_OPERATING_HOURS_DAY,
  FRONIUS_CMD_YIELD_YEAR,
  FRONIUS_CMD_MAX_POWER_YEAR,
  FRONIUS_CMD_MAX_AC_VOLTAGE_YEAR,
  FRONIUS_CMD_MIN_AC_VOLTAGE_YEAR,
  FRONIUS_CMD_MAX_DC_VOLTAGE_YEAR,
  FRONIUS_CMD_OPERATING_HOURS_YEAR,
  FRONIUS_CMD_YIELD_TOTAL,
  FRONIUS_CMD_MAX_POWER_TOTAL,
  FRONIUS_CMD_MAX_AC_VOLTAGE_TOTAL,
  FRONIUS_CMD_MIN_AC_VOLTAGE_TOTAL,
  FRONIUS_CMD_MAX_DC_VOLTAGE_TOTAL,
  FRONIUS_CMD_OPERATING_HOURS_TOTAL,
  /* commands for 3-phase inverters */
  FRONIUS_CMD_CURRENT_PHASE1 = 0x2b,
  FRONIUS_CMD_CURRENT_PHASE2,
  FRONIUS_CMD_CURRENT_PHASE3,
  FRONIUS_CMD_VOLTAGE_PHASE1,
  FRONIUS_CMD_VOLTAGE_PHASE2,
  FRONIUS_CMD_VOLTAGE_PHASE3,
  FRONIUS_CMD_AMBIENT_TEMPERATUR,
  FRONIUS_CMD_FAN_SPEED_FRONT_LEFT,
  FRONIUS_CMD_FAN_SPEED_FRONT_RIGHT,
  FRONIUS_CMD_FAN_SPEED_REAR_LEFT,
  FRONIUS_CMD_FAN_SPEED_REAR_RIGHT,
  /* commands for sensor card/sensor box */
  FRONIUS_CMD_TEMPERATURE_CHANNEL1_NOW = 0xe0,
  FRONIUS_CMD_TEMPERATURE_CHANNEL2_NOW,
  FRONIUS_CMD_INSOLATION_NOW,
  FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL1_DAY,
  FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL1_DAY,
  FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL1_YEAR,
  FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL1_YEAR,
  FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL1_TOTAL,
  FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL1_TOTAL,
  FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL2_DAY,
  FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL2_DAY,
  FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL2_YEAR,
  FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL2_YEAR,
  FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL2_TOTAL,
  FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL2_TOTAL,
  FRONIUS_CMD_MAX_INSOLATION_DAY,
  FRONIUS_CMD_MAX_INSOLATION_YEAR,
  FRONIUS_CMD_MAX_INSOLATION_TOTAL,
  FRONIUS_CMD_DIGITAL_CHANNEL1_NOW,
  FRONIUS_CMD_DIGITAL_CHANNEL2_NOW,
  FRONIUS_CMD_MAX_DIGITAL_CHANNEL1_DAY,
  FRONIUS_CMD_MAX_DIGITAL_CHANNEL1_YEAR,
  FRONIUS_CMD_MAX_DIGITAL_CHANNEL1_TOTAL,
  FRONIUS_CMD_MAX_DIGITAL_CHANNEL2_DAY,
  FRONIUS_CMD_MAX_DIGITAL_CHANNEL2_YEAR,
  FRONIUS_CMD_MAX_DIGITAL_CHANNEL2_TOTAL,
};

struct fronius_query_value {
  uint8_t        cmd;
  fronius_unit_t unit;
};


static const struct fronius_query_value fronius_cmds[] = {
  /* commands for inverters */
  { FRONIUS_CMD_POWER_NOW,                      FRONIUS_UNIT_W },
  { FRONIUS_CMD_ENERGY_TOTAL,                   FRONIUS_UNIT_WH },
  { FRONIUS_CMD_ENERGY_DAY,                     FRONIUS_UNIT_WH },
  { FRONIUS_CMD_ENERGY_YEAR ,                   FRONIUS_UNIT_WH },
  { FRONIUS_CMD_AC_CURRENT_NOW,                 FRONIUS_UNIT_A },
  { FRONIUS_CMD_AC_VOLTAGE_NOW,                 FRONIUS_UNIT_V },
  { FRONIUS_CMD_AC_FREQUENCY_NOW,               FRONIUS_UNIT_HZ },
  { FRONIUS_CMD_DC_CURRENT_NOW,                 FRONIUS_UNIT_A },
  { FRONIUS_CMD_DC_VOLTAGE_NOW,                 FRONIUS_UNIT_V },
  { FRONIUS_CMD_YIELD_DAY,                      FRONIUS_UNIT_CURR },
  { FRONIUS_CMD_MAX_POWER_DAY,                  FRONIUS_UNIT_W },
  { FRONIUS_CMD_MAX_AC_VOLTAGE_DAY,             FRONIUS_UNIT_V },
  { FRONIUS_CMD_MIN_AC_VOLTAGE_DAY,             FRONIUS_UNIT_V },
  { FRONIUS_CMD_MAX_DC_VOLTAGE_DAY,             FRONIUS_UNIT_V },
  { FRONIUS_CMD_OPERATING_HOURS_DAY,            FRONIUS_UNIT_MIN },
  { FRONIUS_CMD_YIELD_YEAR,                     FRONIUS_UNIT_CURR },
  { FRONIUS_CMD_MAX_POWER_YEAR,                 FRONIUS_UNIT_W },
  { FRONIUS_CMD_MAX_AC_VOLTAGE_YEAR,            FRONIUS_UNIT_V },
  { FRONIUS_CMD_MIN_AC_VOLTAGE_YEAR,            FRONIUS_UNIT_V },
  { FRONIUS_CMD_MAX_DC_VOLTAGE_YEAR,            FRONIUS_UNIT_V },
  { FRONIUS_CMD_OPERATING_HOURS_YEAR,           FRONIUS_UNIT_MIN },
  { FRONIUS_CMD_YIELD_TOTAL,                    FRONIUS_UNIT_CURR },
  { FRONIUS_CMD_MAX_POWER_TOTAL,                FRONIUS_UNIT_W },
  { FRONIUS_CMD_MAX_AC_VOLTAGE_TOTAL,           FRONIUS_UNIT_V },
  { FRONIUS_CMD_MIN_AC_VOLTAGE_TOTAL,           FRONIUS_UNIT_V },
  { FRONIUS_CMD_MAX_DC_VOLTAGE_TOTAL,           FRONIUS_UNIT_V },
  { FRONIUS_CMD_OPERATING_HOURS_TOTAL,          FRONIUS_UNIT_MIN },
    /* commands for 3-phase inverters */
  { FRONIUS_CMD_CURRENT_PHASE1,                 FRONIUS_UNIT_A },
  { FRONIUS_CMD_CURRENT_PHASE2,                 FRONIUS_UNIT_A },
  { FRONIUS_CMD_CURRENT_PHASE3,                 FRONIUS_UNIT_A },
  { FRONIUS_CMD_VOLTAGE_PHASE1,                 FRONIUS_UNIT_V },
  { FRONIUS_CMD_VOLTAGE_PHASE2,                 FRONIUS_UNIT_V },
  { FRONIUS_CMD_VOLTAGE_PHASE3,                 FRONIUS_UNIT_V },
  { FRONIUS_CMD_AMBIENT_TEMPERATUR,             FRONIUS_UNIT_C },
  { FRONIUS_CMD_FAN_SPEED_FRONT_LEFT,           FRONIUS_UNIT_RPM },
  { FRONIUS_CMD_FAN_SPEED_FRONT_RIGHT,          FRONIUS_UNIT_RPM },
  { FRONIUS_CMD_FAN_SPEED_REAR_LEFT,            FRONIUS_UNIT_RPM },
  { FRONIUS_CMD_FAN_SPEED_REAR_RIGHT,           FRONIUS_UNIT_RPM },
  /* commands for sensor card/sensor box */
  { FRONIUS_CMD_TEMPERATURE_CHANNEL1_NOW,       FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_TEMPERATURE_CHANNEL2_NOW,       FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_INSOLATION_NOW,                 FRONIUS_UNIT_WPM2 },
  { FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL1_DAY,   FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL1_DAY,   FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL1_YEAR,  FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL1_YEAR,  FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL1_TOTAL, FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL1_TOTAL, FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL2_DAY,   FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL2_DAY,   FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL2_YEAR,  FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL2_YEAR,  FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MIN_TEMPERATURE_CHANNEL2_TOTAL, FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_TEMPERATURE_CHANNEL2_TOTAL, FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_INSOLATION_DAY,             FRONIUS_UNIT_WPM2 },
  { FRONIUS_CMD_MAX_INSOLATION_YEAR,            FRONIUS_UNIT_WPM2 },
  { FRONIUS_CMD_MAX_INSOLATION_TOTAL,           FRONIUS_UNIT_WPM2 },
  { FRONIUS_CMD_DIGITAL_CHANNEL1_NOW,           FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_DIGITAL_CHANNEL2_NOW,           FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_DIGITAL_CHANNEL1_DAY,       FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_DIGITAL_CHANNEL1_YEAR,      FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_DIGITAL_CHANNEL1_TOTAL,     FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_DIGITAL_CHANNEL2_DAY,       FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_DIGITAL_CHANNEL2_YEAR,      FRONIUS_UNIT_TEMP },
  { FRONIUS_CMD_MAX_DIGITAL_CHANNEL2_TOTAL,     FRONIUS_UNIT_TEMP },
};

FRONIUS_END_DECLS

#endif /* FRONIUS_H */
