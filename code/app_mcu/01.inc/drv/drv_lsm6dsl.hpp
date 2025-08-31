//! @file drv_lsm6dsl.hpp
//! @brief LSM6DSL driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_LSM6DSL_H
#define DRV_LSM6DSL_H

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
class LSM6DSL {
public:
    LSM6DSL(struct device const* const lsm6dsl_sns_dev) : _lsm6dsl_sns_dev(lsm6dsl_sns_dev) {
        // pass
    }

    int init();
    int read_data();
    struct sensor_value odr_attr;
    struct sensor_value accel_x, accel_y, accel_z;
	struct sensor_value gyro_x, gyro_y, gyro_z;
    double accel_x_val;
    double accel_y_val;
    double accel_z_val;
    double gyro_x_val;
    double gyro_y_val;
    double gyro_z_val;

private:
    struct device const* const _lsm6dsl_sns_dev;
};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef DRV_LSM6DSL_H
