/// @file dt_cfg.c
// ------------------------------------------------------------------------------------------------------------------ //

// ---------- (01) LOCAL PREDEFINE ---------------------------------------------------------------------------------- //
// N/A

// ---------- (02) USER INCLUDE ------------------------------------------------------------------------------------- //
#include "dt/dt_cfg.hpp"
#include "tsk_cfg.hpp"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>

// ---------- (03) STANDARD LIBRARY INCLUDE ------------------------------------------------------------------------- //
// N/A

// ---------- (04) EXTERNAL OBJECT ---------------------------------------------------------------------------------- //
// N/A

// ---------- (05) EXTERNAL METHOD ---------------------------------------------------------------------------------- //
// N/A

// ---------- (06) PUBLIC DATA DECLARATION -------------------------------------------------------------------------- //
#define HCI_SPI_NODE           DT_COMPAT_GET_ANY_STATUS_OKAY(st_hci_spi_v1)
//  Devices
struct device const* const hts221_sns_dev  = DEVICE_DT_GET_ONE(st_hts221);
struct device const* const lps22hb_sns_dev = DEVICE_DT_GET_ONE(st_lps22hb_press);
struct device const* const vl53l0x_sns_dev = DEVICE_DT_GET_ONE(st_vl53l0x);
struct device const* const lsm6dsl_sns_dev = DEVICE_DT_GET_ONE(st_lsm6dsl);
struct device const* const lis3dl_sns_dev  = DEVICE_DT_GET_ONE(st_lis3mdl_magn);
struct device const* const spi_hci_dev     = DEVICE_DT_GET(DT_BUS(HCI_SPI_NODE));

// I/Os
const struct device *const wdt             = DEVICE_DT_GET(DT_ALIAS(watchdog0));
const struct gpio_dt_spec irq              = GPIO_DT_SPEC_GET(HCI_SPI_NODE, irq_gpios);
const struct pwm_dt_spec green_pwm_led0    = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
const struct gpio_dt_spec button           = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
const struct gpio_dt_spec led0             = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
// ---------- (07) PRIVATE FUNCTION PROTOTYPE ----------------------------------------------------------------------- //
// N/A

// ---------- (08) PRIVATE DATA DEFINITION -------------------------------------------------------------------------- //
// N/A

// ---------- (09) PRIVATE DATA DECLARATION ------------------------------------------------------------------------- //
// N/A

// ---------- (10) PRIVATE PROGRAMMING DEFINE / CONSTEXPR ----------------------------------------------------------- //
// N/A

// ---------- (11) PRIVATE MACRO DEFINITION ------------------------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION ----------------------------------------------------------------------- //
// N/A
// ---------- (13) END OF SOURCE FILE ---------------------------------------------------------------------------- //