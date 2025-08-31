//! @file drv_lps22hb.h
//! @brief LPS22HB driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_LPS22HB_H
#define DRV_LPS22HB_H

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
class LPS22HB {
public:
    LPS22HB(struct device const* const lps22hb_sns_dev) : _lps22hb_sns_dev(lps22hb_sns_dev) {
        // pass
    }

    int init();
    int read_data();
    struct sensor_value temp, pressure;
    double ambient_temp_val;
    double pressure_val;

private:
    struct device const* const _lps22hb_sns_dev;
};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef DRV_LPS22HB_H
