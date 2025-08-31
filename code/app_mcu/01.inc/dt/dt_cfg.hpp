/// @file dt_cfg.h
// ------------------------------------------------------------------------------------------------------------------ //

#ifndef DT_CFG_H
#define DT_CFG_H

// ---------- (01) SYSTEM INCLUDE ----------------------------------------------------------------------------------- //
#include "autoconf.h"
#include "tsk_cfg.hpp"

// ---------- (02) EXTERNAL MODULE INCLUDE -------------------------------------------------------------------------- //
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/eeprom.h>
#include <zephyr/fs/nvs.h>

// ---------- (03) PUBLIC PROGRAMMING DEFINE / CONSTEXPR ------------------------------------------------------------ //
// N/A
// ---------- (04) ENUMERATOR DEFINITION ---------------------------------------------------------------------------- //
// N/A

// ---------- (05) DATA TYPE DEFINITION ----------------------------------------------------------------------------- //
// N/A

// ---------- (06) PUBLIC MACRO DEFINITION -------------------------------------------------------------------------- //
// N/A

// ---------- (07) EXTERN FUNCTION ---------------------------------------------------------------------------------- //
// N/A
// ---------- (08) EXTERN VARIABLE ---------------------------------------------------------------------------------- //
#ifdef __cplusplus
extern "C" {
#endif

extern struct device const* const hts221_sns_dev;
extern struct device const* const lps22hb_sns_dev;
extern struct device const* const vl53l0x_sns_dev;
extern struct device const* const lsm6dsl_sns_dev;
extern struct device const* const lis3dl_sns_dev;
extern struct device const* const spi_hci_dev;

// I/Os
extern const struct device *const wdt;
extern const struct gpio_dt_spec irq;
extern const struct pwm_dt_spec  green_pwm_led0;
extern const struct gpio_dt_spec button;
extern const struct gpio_dt_spec led0;

#ifdef __cplusplus
}
#endif

// ---------- (09) CLASS DECLARATION -------------------------------------------------------------------------------- //
// N/A

// ---------- (10) VERSION INFORMATION ------------------------------------------------------------------------------ //
// N/A

// ---------- END OF FILE ------------------------------------------------------------------------------------------- //
#endif // #ifndef DT_CFG_H
