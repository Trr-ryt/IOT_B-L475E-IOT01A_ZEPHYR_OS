//! @file drv_lps22hb.cpp
//! @brief LPS22HB driver
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
#include "drv/drv_lps22hb.hpp"
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
int LPS22HB::init() {
    if (!device_is_ready(_lps22hb_sns_dev)) {
        printk("LPS22HB: Device not ready\n");
        return -ENODEV;
    }
    return (0);
}

int LPS22HB::read_data() {
    int rc = 0;

    rc = sensor_sample_fetch(_lps22hb_sns_dev);
    if (rc < 0) {
        printk("LPS22HB: Failed to fetch sample (%d)\n", rc);
        ambient_temp_val = std::numeric_limits<double>::max();
        pressure_val     = std::numeric_limits<double>::max();
        return rc;
    }

    if (sensor_channel_get(_lps22hb_sns_dev, SENSOR_CHAN_PRESS, &pressure) < 0) {
        printk("LPS22HB: Failed to get pressure (%d)\n", rc);
        pressure_val = std::numeric_limits<double>::max();
    }
    else {
        pressure_val = sensor_value_to_double(&pressure);
    }

    if (sensor_channel_get(_lps22hb_sns_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0) {
        printk("LPS22HB: Failed to get temperature (%d)\n", rc);
        ambient_temp_val = std::numeric_limits<double>::max();
    }
    else {
        ambient_temp_val = sensor_value_to_double(&temp);
    }

    return rc;
}
// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_WIFI_EN