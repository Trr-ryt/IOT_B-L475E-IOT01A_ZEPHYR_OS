//! @file drv_gpio.h
//! @brief DRV GPIO driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_GPIO_H
#define DRV_GPIO_H

// ---------- (1) SYSTEM INCLUDE --------------------------------------------------------------------- //
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>
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
class DRV_GPIO {
public:
    DRV_GPIO(const struct pwm_dt_spec green_pwm_led0, const struct gpio_dt_spec usr_btn, const struct gpio_dt_spec led0): _green_pwm_led0(green_pwm_led0), _usr_btn(usr_btn), _led0(led0) {
        // pass
    }
    int  ret;
    int  led0_sts;
    bool led_state; /* Tracking state here supports GPIO expander-based LEDs. */
    struct gpio_callback gpio_cb;
    int  init();
    int  led0_init();
    int  button_init(gpio_callback_handler_t handler);
    void led0_update();
    void run_green_led_pwm();
    void stop_green_led_pwm();
private:
    const struct pwm_dt_spec _green_pwm_led0;
    const struct gpio_dt_spec _usr_btn;
    const struct gpio_dt_spec _led0;

};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef DRV_GPIO_H
