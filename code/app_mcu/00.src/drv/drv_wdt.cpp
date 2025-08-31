//! @file drv_wdt.cpp
//! @brief wdt driver source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "tsk_cfg.hpp"
#if (TSK_WDT_EN == TSK_ENABLED)
#include "drv/drv_wdt.hpp"
#include "utl/tsk_utl.hpp"

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
int WDT::wdt_init() {

	if (!device_is_ready(_wdt)) {
		printk("DRV_WDT : %s device not ready.\n", _wdt->name);
		return  -ENODEV;
	}

    wdt_config.flags      = WDT_FLAG_RESET_SOC,
    wdt_config.window.min = WDT_MIN_WINDOW,
    wdt_config.window.max = WDT_MAX_WINDOW,

	wdt_channel_id = wdt_install_timeout(_wdt, &wdt_config);
	if (wdt_channel_id == -ENOTSUP) {
		/* IWDG driver for STM32 doesn't support callback */
		printk("DRV_WDT : Callback support rejected, continuing anyway\n");
		wdt_config.callback = NULL;
		wdt_channel_id = wdt_install_timeout(_wdt, &wdt_config);
	}

	if (wdt_channel_id < 0) {
		printk("DRV_WDT : Watchdog install error\n");
		return wdt_channel_id;
	}

	err = wdt_setup(_wdt, WDT_OPT);
	if (err < 0) {
		printk("DRV_WDT : Watchdog setup error\n");
		return  err;
	}

    return 0;
}

int WDT::wdt_rst_expire() {

    err = wdt_feed(_wdt, wdt_channel_id);
    return err;
}
#endif
//  ---------- (13) END OF SOURCE FILE ------------------------------------------------------------- //