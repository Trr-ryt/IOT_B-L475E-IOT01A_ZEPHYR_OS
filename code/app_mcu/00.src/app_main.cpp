/// @file app_main.cpp
// ------------------------------------------------------------------------------------------------------------------ //

// ---------- (1) SYSTEM INCLUDE ---------------------------------------------------------------------------------- //
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/dfu/mcuboot.h>

// ---------- (02) USER INCLUDE ------------------------------------------------------------------------------------- //
#include "utl/tsk_utl.hpp"
#include "tsk/tsk_sys.hpp"
#include "tsk_cfg.hpp"
// ---------- (03) STANDARD LIBRARY INCLUDE ------------------------------------------------------------------------- //
// N/A

// ---------- (04) EXTERNAL OBJECT ---------------------------------------------------------------------------------- //
// N/A

// ---------- (05) EXTERNAL METHOD ---------------------------------------------------------------------------------- //
// N/A

// ---------- (06) PUBLIC DATA DECLARATION -------------------------------------------------------------------------- //
// N/A

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

int main(void) {
    struct que_msg_t msg;
    #if (TSK_WDT_EN == TSK_ENABLED)
    int rc = 0;
    rc = boot_write_img_confirmed();
    if (rc == 0) {
        printk("Success to confirm the image\n");
    }
    else {
        printk("Error to confirm the image\n");
    }
    #endif
	tsk_utl_init();
    #if (CONFIG_TASK_START_ENABLE == TSK_ENABLED)
        tsk_utl_tsk_cre(tsk_id_t::SYS, tsk_sys_entry, nullptr, nullptr, nullptr,"tsk_sys");
        tsk_utl_tsk_sta(tsk_id_t::SYS);
    #endif
	printk("START %s BOARD\n", CONFIG_BOARD_TARGET);
	#if (CONFIG_TASK_START_ENABLE == TSK_ENABLED)
        k_msleep(100);
        msg.evt = que_evt_t::STA_SIG;
        (void) tsk_utl_snd_que(que_id_t::SYS, msg, K_MSEC(100));
    #endif
	return (0);
}
// ---------- END OF FILE ------------------------------------------------------------------------------------------- //
