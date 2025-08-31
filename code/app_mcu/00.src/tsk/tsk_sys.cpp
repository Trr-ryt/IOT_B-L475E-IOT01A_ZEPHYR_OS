//! @file tsk_sys.cpp
//! @brief Task SYS source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <zephyr/kernel.h>

// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "tsk/tsk_sys.hpp"
#include "utl/tsk_utl.hpp"
#include "tsk_cfg.hpp"
#include "drv/drv_wdt.hpp"
#include "dt/dt_cfg.hpp"
#include "tsk/tsk_env_sns.hpp"
#include "tsk/tsk_dat_acq.hpp"
#include "tsk/tsk_mqtt.hpp"
#include "tsk/tsk_wifi.hpp"
#include "tsk/tsk_ota.hpp"
#include "tsk/tsk_bt_le.hpp"
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
class sys_func {
    private:
        /* data */
    public:
        #if (TSK_WDT_EN == TSK_ENABLED)
        WDT drv_wdt;
        sys_func() : drv_wdt(wdt) {}
        #endif
        struct que_msg_t in_msg, out_msg;
        static bool tsk_sys_running;
        static void init_thread(void);
        static void start_thread(void);
};

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
bool sys_func::tsk_sys_running = true; //!< Task SYS running flag
// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
void tsk_sys_entry(void* param1, void* param2, void* param3) {
    int rc = 0;
    sys_func tsk_sys;
    #if (TSK_WDT_EN == TSK_ENABLED)
    // Initialize Watchdog
    rc = tsk_sys.drv_wdt.wdt_init();
    if (rc < 0) {
        printk("TSK_SYS : Watchdog initialization failed with error %d\n", rc);
    }
    #endif
    printk("TSK_SYS : Entry\n");
    while (tsk_sys.tsk_sys_running == true) {
        rc = tsk_utl_rcv_que(que_id_t::SYS, tsk_sys.in_msg, K_MSEC(1000));
        if (rc == 0) {
            switch (tsk_sys.in_msg.evt) {
                case que_evt_t::STA_SIG :
                    printk("TSK_SYS : Start signal received\n");
                    tsk_sys.init_thread();
                    tsk_sys.start_thread();
                    break;
                default:
                    break;
            }
        }
        else {
        #if (TSK_WDT_EN == TSK_ENABLED)
            rc = tsk_sys.drv_wdt.wdt_rst_expire(); // Reset the watchdog timer
            if (rc < 0) {
                printk("TSK_SYS : Watchdog reset failed with error %d\n", rc);
            }
        #endif
        }
    }
}

void sys_func::init_thread(void) {

    #if (TSK_ENV_SNS_EN == TSK_ENABLED) 
        tsk_utl_tsk_cre(tsk_id_t::ENV_SNS, tsk_env_sns_entry, nullptr, nullptr, nullptr, "tsk_env_sns");
        tsk_utl_tsk_sta(tsk_id_t::ENV_SNS);
    #endif // TSK_ENV_SNS_EN

    #if (TSK_DAT_ACQ_EN == TSK_ENABLED) 
        tsk_utl_tsk_cre(tsk_id_t::DAT_ACQ, tsk_dat_acq_entry, nullptr, nullptr, nullptr, "tsk_dat_acq");
        tsk_utl_tsk_sta(tsk_id_t::DAT_ACQ);
    #endif // TSK_DAT_ACQ_EN

    #if (TSK_MQTT_EN == TSK_ENABLED)
        tsk_utl_tsk_cre(tsk_id_t::MQTT, tsk_mqtt_entry, nullptr, nullptr, nullptr, "tsk_mqtt");
        tsk_utl_tsk_sta(tsk_id_t::MQTT);
    #endif // TSK_MQTT_EN

    #if (TSK_WIFI_EN == TSK_ENABLED)
        tsk_utl_tsk_cre(tsk_id_t::WIFI, tsk_wifi_entry, nullptr, nullptr, nullptr, "tsk_wifi");
        tsk_utl_tsk_sta(tsk_id_t::WIFI);
    #endif // TSK_WIFI_EN

    #if (TSK_OTA_EN == TSK_ENABLED)
        tsk_utl_tsk_cre(tsk_id_t::OTA, tsk_ota_entry, nullptr, nullptr, nullptr, "tsk_ota");
        tsk_utl_tsk_sta(tsk_id_t::OTA);
    #endif // TSK_OTA_EN

    #if (TSK_BT_LE_EN == TSK_ENABLED)
        tsk_utl_tsk_cre(tsk_id_t::BT, tsk_bt_le_entry, nullptr, nullptr, nullptr, "tsk_bluetooth_main");
        tsk_utl_tsk_sta(tsk_id_t::BT);
    #endif
}

void sys_func::start_thread(void) {
    struct que_msg_t msg;
    msg.evt = que_evt_t::STA_SIG;

    #if (TSK_DAT_ACQ_EN == TSK_ENABLED)
        (int)tsk_utl_snd_que(que_id_t::DAT_ACQ, msg, K_MSEC(20));
    #endif

    #if (TSK_ENV_SNS_EN == TSK_ENABLED)
        (int)tsk_utl_snd_que(que_id_t::ENV_SNS, msg, K_MSEC(20));
    #endif

    #if (TSK_WIFI_EN == TSK_ENABLED)
        (int)tsk_utl_snd_que(que_id_t::WIFI, msg, K_MSEC(20));
    #endif

    #if (TSK_OTA_EN == TSK_ENABLED)
        (int)tsk_utl_snd_que(que_id_t::OTA, msg, K_MSEC(20));
    #endif

    #if (TSK_MQTT_EN == TSK_ENABLED)
        (int)tsk_utl_snd_que(que_id_t::MQTT, msg, K_MSEC(20));
    #endif

    #if (TSK_BT_LE_EN == TSK_ENABLED)
        (int)tsk_utl_snd_que(que_id_t::BT, msg, K_MSEC(20));
    #endif
}

// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
