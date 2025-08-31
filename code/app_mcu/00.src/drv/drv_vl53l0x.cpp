//! @file drv_vl53l0x.cpp
//! @brief VL53L0X driver source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include "tsk_cfg.hpp"
#if (TSK_ENV_SNS_EN  == TSK_ENABLED)
#include <stdio.h>
#include <limits>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/vl53l0x.h>
#include <zephyr/sys/util.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "drv/drv_vl53l0x.hpp"
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
int VL53L0X::init() {
    if (!device_is_ready(_vl53l0x_sns_dev)) {
        printk("VL530LX: Device not ready\n");
        return -ENODEV;
    }
    return (0);
}

int VL53L0X::read_data() {
    int ret = 0;

    ret = sensor_sample_fetch(_vl53l0x_sns_dev);
    if (ret) {
        printk("sensor_sample_fetch failed ret %d\n", ret);
        prox         = std::numeric_limits<double>::max();
        dist         = std::numeric_limits<double>::max();
        max_dist     = std::numeric_limits<double>::max();
        signal_rate  = std::numeric_limits<double>::max();
        ambient_rate = std::numeric_limits<double>::max();
        spad_count   = std::numeric_limits<double>::max();
        range_status = std::numeric_limits<double>::max();
        return ret;
    }

    if (sensor_channel_get(_vl53l0x_sns_dev, (sensor_channel)SENSOR_CHAN_PROX, &value) < 0) {
        prox = std::numeric_limits<double>::max();
    } else {
        prox = value.val1;
    }

    if (sensor_channel_get(_vl53l0x_sns_dev, (sensor_channel)SENSOR_CHAN_DISTANCE, &value) < 0) {
        dist = std::numeric_limits<double>::max();
    } else {
        dist = sensor_value_to_milli(&value) / 100.0; // Convert to meters
    }

    if (sensor_channel_get(_vl53l0x_sns_dev, (sensor_channel)SENSOR_CHAN_VL53L0X_RANGE_DMAX, &value) < 0) {
        max_dist = std::numeric_limits<double>::max();
    } else {
        max_dist = sensor_value_to_milli(&value) / 100.0; // Convert to meters
    }

    if (sensor_channel_get(_vl53l0x_sns_dev, (sensor_channel)SENSOR_CHAN_VL53L0X_SIGNAL_RATE_RTN_CPS, &value) < 0) {
        signal_rate = std::numeric_limits<double>::max();
    } else {
        signal_rate = value.val1;
    }

    if (sensor_channel_get(_vl53l0x_sns_dev, (sensor_channel)SENSOR_CHAN_VL53L0X_AMBIENT_RATE_RTN_CPS, &value) < 0) {
        ambient_rate = std::numeric_limits<double>::max();
    } else {
        ambient_rate = value.val1;
    }

    if (sensor_channel_get(_vl53l0x_sns_dev, (sensor_channel)SENSOR_CHAN_VL53L0X_EFFECTIVE_SPAD_RTN_COUNT, &value) < 0) {
        spad_count = std::numeric_limits<double>::max();
    } else {
        spad_count = value.val1;
    }

    if (sensor_channel_get(_vl53l0x_sns_dev, (sensor_channel)SENSOR_CHAN_VL53L0X_RANGE_STATUS, &value) < 0) {
        range_status = std::numeric_limits<double>::max();
    } else {
        range_status = value.val1;
    }
    
    return ret;
}
#endif // TSK_WIFI_EN == TSK_ENABLED