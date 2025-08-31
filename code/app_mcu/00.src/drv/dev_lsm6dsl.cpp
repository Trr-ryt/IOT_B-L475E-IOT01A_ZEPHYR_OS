//! @file drv_lsm6dsl.cpp
//! @brief LSM6DSL driver

// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include "tsk_cfg.hpp"
#if (TSK_ENV_SNS_EN == TSK_ENABLED)
#include <stdio.h>
#include <limits>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "drv/drv_lsm6dsl.hpp"
#include "utl/tsk_utl.hpp"
#include "tsk_cfg.hpp"

// ---------- (4) EXTERN OBJECT ---------------------------------------------------------------------- //
// N/A

// ---------- (5) PUBLIC INTERFACE FUNCTION ---------------------------------------------------------- //
// N/A

// ---------- (6) PUBLIC DATA ------------------------------------------------------------------------ //
// N/A

// ---------- (7) PRIVATE PROGRAMMING DEFINE --------------------------------------------------------- //
// N/A

// ---------- (8) PRIVATE TYPEDEF -------------------------------------------------------------------- //
// N/A

// ---------- (9) PRIVATE FUNCTION ------------------------------------------------------------------- //
// N/A

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
// N/A

// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
int LSM6DSL::init() {
    int rc = 0;
    if (!device_is_ready(_lsm6dsl_sns_dev)) {
        printk("LSM6DSL: Device not ready\n");
        return -ENODEV;
    }
    else {
        odr_attr.val1 = 416;
        odr_attr.val2 = 0;

        if (sensor_attr_set(_lsm6dsl_sns_dev, SENSOR_CHAN_ACCEL_XYZ,
                    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
            printk("Cannot set sampling frequency for accelerometer.\n");
            rc = -1;
            return rc;
        }

        if (sensor_attr_set(_lsm6dsl_sns_dev, SENSOR_CHAN_GYRO_XYZ,
                    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
            printk("Cannot set sampling frequency for gyro.\n");
            rc = -1;
            return rc;
        }
    }
    return (0);
}

int LSM6DSL::read_data() {
    int rc      = 0;
    int rc_acc  = 0;
    int rc_gyro = 0;
    /* lsm6dsl accel */
    rc_acc = sensor_sample_fetch_chan(_lsm6dsl_sns_dev, SENSOR_CHAN_ACCEL_XYZ);
	if (rc_acc < 0) {
        printf("Sensor sample update error\n");
        accel_x_val = std::numeric_limits<double>::max();
        accel_y_val = std::numeric_limits<double>::max();
        accel_z_val = std::numeric_limits<double>::max();
        gyro_x_val  = std::numeric_limits<double>::max();
        gyro_y_val  = std::numeric_limits<double>::max();
        gyro_z_val  = std::numeric_limits<double>::max();
        rc_acc = -1;
    }
    else {
        if (sensor_channel_get(_lsm6dsl_sns_dev, SENSOR_CHAN_ACCEL_X, &accel_x) < 0) {
            printf("Cannot read LSM6DSL accelerometer X channel\n");
            accel_x_val = std::numeric_limits<double>::max();
        }
        else {
            accel_x_val = sensor_value_to_double(&accel_x);
        }

        if (sensor_channel_get(_lsm6dsl_sns_dev, SENSOR_CHAN_ACCEL_Y, &accel_y) < 0) {
            printf("Cannot read LSM6DSL accelerometer Y channel\n");
            accel_y_val = std::numeric_limits<double>::max();
        }
        else {
            accel_y_val = sensor_value_to_double(&accel_y);
        }

        if (sensor_channel_get(_lsm6dsl_sns_dev, SENSOR_CHAN_ACCEL_Z, &accel_z) < 0) {
            printf("Cannot read LSM6DSL accelerometer Z channel\n");
            accel_z_val = std::numeric_limits<double>::max();
        }
        else {
            accel_z_val = sensor_value_to_double(&accel_z);
        }
    }

	/* lsm6dsl gyro */
    rc_gyro = sensor_sample_fetch_chan(_lsm6dsl_sns_dev, SENSOR_CHAN_GYRO_XYZ);
	if (rc_gyro < 0) {
        printf("Sensor sample update error\n");
        gyro_x_val = std::numeric_limits<double>::max();
        gyro_y_val = std::numeric_limits<double>::max();
        gyro_z_val = std::numeric_limits<double>::max();
        rc_gyro = -1;
    }
    else {
        /* lsm6dsl gyro */
        if (sensor_channel_get(_lsm6dsl_sns_dev, SENSOR_CHAN_GYRO_X, &gyro_x) < 0) {
            printf("Cannot read LSM6DSL gyro X channel\n");
            gyro_x_val = std::numeric_limits<double>::max();
        }
        else {
            gyro_x_val = sensor_value_to_double(&gyro_x);
        }

        if (sensor_channel_get(_lsm6dsl_sns_dev, SENSOR_CHAN_GYRO_Y, &gyro_y) < 0) {
            printf("Cannot read LSM6DSL gyro Y channel\n");
            gyro_y_val = std::numeric_limits<double>::max();
        }
        else {
            gyro_y_val = sensor_value_to_double(&gyro_y);
        }

        if (sensor_channel_get(_lsm6dsl_sns_dev, SENSOR_CHAN_GYRO_Z, &gyro_z) < 0) {
            printf("Cannot read LSM6DSL gyro Z channel\n");
            gyro_z_val = std::numeric_limits<double>::max();
        }
        else {
            gyro_z_val = sensor_value_to_double(&gyro_z);
        }
    }

    if (rc_acc < 0 || rc_gyro < 0) {
        return -1;
    }
    else {
        rc = 0;
    }

    return rc;
}
// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_LSM6DSL_EN