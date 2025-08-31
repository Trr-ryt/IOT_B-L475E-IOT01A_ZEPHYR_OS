//! @file drv_lis3dl.h
//! @brief LIS3DL driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_LIS3DL_H
#define DRV_LIS3DL_H

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
class LIS3DL {
public:
    LIS3DL(struct device const* const lis3dl_sns_dev) : _lis3dl_sns_dev(lis3dl_sns_dev) {
        // pass
    }

    int init();
    int read_data();
    struct sensor_value accel[3];
    double x_val;
    double y_val;
    double z_val;

private:
    struct device const* const _lis3dl_sns_dev;
};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef DRV_LIS3DL_H
