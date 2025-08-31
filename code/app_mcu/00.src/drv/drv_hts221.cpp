//! @file drv_hts221.cpp
//! @brief HTS221 driver
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
#include "drv/drv_hts221.hpp"
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
int HTS221::init() {
    if (!device_is_ready(_hts221_sns_dev)) {
        printk("HTS221: Device not ready\n");
        return -1;
    }
    return (0);
}

int HTS221::read_data() {
    int rc = 0;

	if (sensor_sample_fetch(_hts221_sns_dev) < 0) {
		printf("Sensor sample update error\n");
        temp_val = std::numeric_limits<double>::max();
        humi_val = std::numeric_limits<double>::max();
        return rc;
	}

	if (sensor_channel_get(_hts221_sns_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0) {
        temp_val = std::numeric_limits<double>::max();
		printf("Cannot read HTS221 temperature channel\n");
	}
    else {
        temp_val = sensor_value_to_double(&temp);
    }

	if (sensor_channel_get(_hts221_sns_dev, SENSOR_CHAN_HUMIDITY, &hum) < 0) {
		humi_val = std::numeric_limits<double>::max();
        printf("Cannot read HTS221 humidity channel\n");
	}
    else {
        humi_val = sensor_value_to_double(&hum);
    }
    
    return rc;
}
// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_WIFI_EN
