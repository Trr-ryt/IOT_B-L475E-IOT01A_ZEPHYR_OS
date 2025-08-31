//! @file drv_gpio.cpp
//! @brief DRV_GPIO driver source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "drv/drv_gpio.hpp"
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
#define MAX_PERIOD PWM_MSEC(100U)

// ---------- (9) PRIVATE FUNCTION ------------------------------------------------------------------- //
// N/A

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
// N/A

// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
int DRV_GPIO::init() {
    // Initialize the PWM device
	if (!pwm_is_ready_dt(&_green_pwm_led0)) {
		printk("Error: PWM device %s is not ready\n", _green_pwm_led0.dev->name);
		return -ENODEV; // Device not ready
	}

    return 0; // Success
}

int DRV_GPIO::led0_init() {

	led0_sts = gpio_is_ready_dt(&_led0);
	if (!led0_sts) {
		printk("Error: LED on GPIO %s pin %d is not ready",
			_led0.port->name, _led0.pin);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&_led0, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		printk("Error %d: failed to configure GPIO %s pin %d",
			ret, _led0.port->name, _led0.pin);
	}

	return ret;
}

int DRV_GPIO::button_init(gpio_callback_handler_t handler) {

	if (!gpio_is_ready_dt(&_usr_btn)) {
		printk("Error: button GPIO device %s is not ready\n",
			_usr_btn.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&_usr_btn, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: can't configure button on GPIO %s pin %d\n",
			ret, _usr_btn.port->name, _usr_btn.pin);
		return ret;

	}

	gpio_init_callback(&gpio_cb, handler, BIT(_usr_btn.pin));
	gpio_add_callback(_usr_btn.port, &gpio_cb);
	ret = gpio_pin_interrupt_configure_dt(&_usr_btn, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: can't configure button interrupt on \n"
			"GPIO %s pin %d", ret, _usr_btn.port->name, _usr_btn.pin);
		return ret;
	}
	return 0;
}

void DRV_GPIO::run_green_led_pwm() {
    pwm_set_dt(&_green_pwm_led0, MAX_PERIOD, MAX_PERIOD / 2U);
}

void DRV_GPIO::stop_green_led_pwm() {
    pwm_set_dt(&_green_pwm_led0, 0, 0);
}

void DRV_GPIO::led0_update() {
	if (!led0_sts) {
		return;
	}

	led_state = !led_state;
	printk("Turn %s LED\n", led_state ? "on" : "off");
	gpio_pin_set(_led0.port, _led0.pin, led_state);
}

// ---------- (13) END OF SOURCE FILE ---------------------------------------------------------------- //