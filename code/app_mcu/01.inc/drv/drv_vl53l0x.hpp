//! @file drv_vl53l0x.h
//! @brief VL53L0X driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_VL53L0X_H
#define DRV_VL53L0X_H

// ---------- (1) SYSTEM INCLUDE --------------------------------------------------------------------- //
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

// ---------- (2) EXTERNAL MODULE INCLUDE ------------------------------------------------------------ //
// N/A

// ---------- (3) PUBLIC PROGRAMMING DEFINE ---------------------------------------------------------- //
// N/A

// ---------- (4) ENUMERATOR DEFINITION -------------------------------------------------------------- //
// N/A

// ---------- (5) STRUCT OR UNION DATA TYPE DEFINITION ----------------------------------------------- //
// N/A

// ---------- (6) TYPEDEF DATA TYPE DEFINITION ------------------------------------------------------- //
// N/A

// ---------- (7) PUBLIC MACRO DEFINITION ------------------------------------------------------------ //
// N/A

// ---------- (8) EXTERN FUNCTION -------------------------------------------------------------------- //
// N/A

// ---------- (9) EXTERN VARIABLE -------------------------------------------------------------------- //
// N/A

// ---------- (10) CLASS DECLARATION ----------------------------------------------------------------- //
class VL53L0X {
public:
    VL53L0X(struct device const* const vl53l0x_sns_dev) : _vl53l0x_sns_dev(vl53l0x_sns_dev) {
        // pass
    }

    int init();
    int read_data();
    struct sensor_value value;
    double prox;
    double dist;
    double max_dist;
    double signal_rate;
    double ambient_rate;
    double spad_count;
    double range_status;

private:
    struct device const* const _vl53l0x_sns_dev;
};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef DRV_VL53L0X_H
