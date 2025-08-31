//! @file drv_network.h
//! @brief NETWORK driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_NETWORK_H
#define DRV_NETWORK_H

// ---------- (1) SYSTEM INCLUDE --------------------------------------------------------------------- //
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
// ---------- (2) EXTERNAL MODULE INCLUDE ------------------------------------------------------------ //
#include "tsk_cfg.hpp"
#if (TSK_WDT_EN == TSK_ENABLED)

// ---------- (3) PUBLIC PROGRAMMING DEFINE ---------------------------------------------------------- //
#define WDT_MAX_WINDOW  1000U
#define WDT_MIN_WINDOW  0U
#define WDT_OPT WDT_OPT_PAUSE_HALTED_BY_DBG

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
class WDT {
    public:
    WDT(const struct device *const wdt): _wdt(wdt) {
        // pass
    }
	int  err;
	int  wdt_channel_id;
	struct wdt_timeout_cfg wdt_config;
    int wdt_init();
    int wdt_rst_expire();

    private:
        const struct device *const _wdt; //!< WDT device instance

};
#endif
// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef DRV_WDT_H
