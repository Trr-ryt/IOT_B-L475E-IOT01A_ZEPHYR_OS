//! @file drv_lis3dl.cpp
//! @brief LIS3DL driver
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
#include "drv/drv_lis3dl.hpp"
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
int LIS3DL::init() {
    if (!device_is_ready(_lis3dl_sns_dev)) {
        printk("LIS3DL: Device not ready\n");
        return -ENODEV;
    }
    return (0);
}

int LIS3DL::read_data() {
    int rc = 0;
    rc = sensor_sample_fetch(_lis3dl_sns_dev);
    if (rc < 0) {
        printk("LIS3DL: Sensor sample fetch failed\n");
        x_val = std::numeric_limits<double>::quiet_NaN();
        y_val = std::numeric_limits<double>::quiet_NaN();
        z_val = std::numeric_limits<double>::quiet_NaN();
        return rc;
    }
    
    rc = sensor_channel_get(_lis3dl_sns_dev, SENSOR_CHAN_MAGN_XYZ, accel);
    if (rc < 0) {
        printk("LIS3DL: Sensor channel get failed\n");
        x_val = std::numeric_limits<double>::quiet_NaN();
        y_val = std::numeric_limits<double>::quiet_NaN();
        z_val = std::numeric_limits<double>::quiet_NaN();
        return rc;
    } 
    else {
        x_val = sensor_value_to_double(&accel[0]);
        y_val = sensor_value_to_double(&accel[1]);
        z_val = sensor_value_to_double(&accel[2]);
    }
    
    return rc;
}
// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_WIFI_EN