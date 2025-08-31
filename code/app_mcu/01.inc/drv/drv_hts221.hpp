//! @file drv_hts221.h
//! @brief HTS221 driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_HTS221_H
#define DRV_HTS221_H

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
class HTS221 {
public:
    HTS221(struct device const* const hts221_sns_dev) : _hts221_sns_dev(hts221_sns_dev) {
        // pass
    }

    int init();
    int read_data();
    struct sensor_value temp, hum;
    double temp_val;
    double humi_val;

private:
    struct device const* const _hts221_sns_dev;
};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef DRV_HTS221_H
