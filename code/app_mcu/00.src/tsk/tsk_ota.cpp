//! @file tsk_ota.cpp
//! @brief Task OTA source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <zephyr/kernel.h>

// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "tsk/tsk_ota.hpp"
#include "utl/tsk_utl.hpp"
#include "tsk_cfg.hpp"
#if (TSK_OTA_EN == TSK_ENABLED)
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
static bool tsk_ota_running = true;

// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
void tsk_ota_entry(void* param1, void* param2, void* param3) {
    int rc = 0;
    struct que_msg_t msg;
    printk("TSK_OTA : Entry\n");
    while (tsk_ota_running == true) {
        rc = tsk_utl_rcv_que(que_id_t::OTA, msg, K_MSEC(1000));
        if (rc == 0) {
            switch (msg.evt) {
                case que_evt_t::STA_SIG :
                    break; // Handle task start signal
            default:
                break;
            }
        }
    }
}

// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_OTA_EN
