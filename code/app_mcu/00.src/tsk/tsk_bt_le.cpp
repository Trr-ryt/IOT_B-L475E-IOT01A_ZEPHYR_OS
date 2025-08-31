//! @file tsk_bt_main.cpp
//! @brief Task BT_MAIN source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <zephyr/kernel.h>
#include <zephyr/smf.h>
#include <zephyr/bluetooth/hci_raw.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "drv/drv_hci_spi.hpp"
#include "drv/drv_st_bt_sns.hpp"
#include "utl/tsk_utl.hpp"
#include "dt/dt_cfg.hpp"
#include "tsk_cfg.hpp"
#if (TSK_BT_LE_EN == TSK_ENABLED)
// ---------- (4) EXTERN OBJECT ---------------------------------------------------------------------- //
// N/A

// ---------- (5) PUBLIC INTERFACE FUNCTION ---------------------------------------------------------- //
// N/A

// ---------- (6) PUBLIC DATA ------------------------------------------------------------------------ //

// ---------- (7) PRIVATE PROGRAMMING DEFINE --------------------------------------------------------- //
// N/A

// ---------- (8) PRIVATE TYPEDEF -------------------------------------------------------------------- //
// N/A

// ---------- (9) PRIVATE FUNCTION ------------------------------------------------------------------- //
// N/A

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
class tsk_bt
{
    private:
        /* data */
    public:
        static env_dat_t env_dat;
        ST_BT_SNS   tsk_st_bt_sns_drv; //!< ST_BT_SNS driver instance
        static bool tsk_bt_running;
        static struct que_msg_t in_msg, out_msg;
        static const struct smf_state state_machine[]; // State machine states
        enum bt_state {RUN_CFG, RUN_IDLE, RUN_TX, RUN_REPORT};

#if (DRV_BT_HCI_SPI_EN == TSK_ENABLED)
        HCI_SPI     tsk_hci_spi_drv; //!< HCI_SPI driver instance
#endif
        tsk_bt() : 
#if (DRV_BT_HCI_SPI_EN == TSK_ENABLED)
        tsk_hci_spi_drv(spi_hci_dev, irq),
#endif
        tsk_st_bt_sns_drv(&env_dat) {}

        static void tsk_bt_cfg_en(void *obj);
        static enum smf_state_result tsk_bt_cfg_run(void *obj);
        static void tsk_bt_cfg_exit(void *obj);

        static void tsk_bt_idle_en(void *obj);
        static enum smf_state_result tsk_bt_idle_run(void *obj);
        static void tsk_bt_idle_exit(void *obj);

        static void tsk_bt_tx_en(void *obj);
        static enum smf_state_result tsk_bt_tx_run(void *obj);
        static void tsk_bt_tx_exit(void *obj);

        static void tsk_bt_report_en(void *obj);
        static enum smf_state_result tsk_bt_report_run(void *obj);
        static void tsk_bt_report_exit(void *obj);
};
/* User defined object */
struct bt_obj_t {
    /* This must be first */
    int    ret_code;
    int    ret_bt_q;
    struct smf_ctx ctx;
    struct k_fifo* rx_q; //!< Pointer to the RX queue
    class  tsk_bt* bt_oop;
} bt_obj;
// ---------- (7) PUBLIC DATA ------------------------------------------------------------------------ //

bool   tsk_bt::tsk_bt_running   = true; //!< Task Bluetooth running flag
struct env_dat_t tsk_bt::env_dat = {}; //!< Environment data reference
struct que_msg_t tsk_bt::in_msg  = {};   //!< Input message
struct que_msg_t tsk_bt::out_msg = {};   //!< Output message
const struct smf_state tsk_bt::state_machine[] = {
    [tsk_bt::bt_state::RUN_CFG]    = SMF_CREATE_STATE(tsk_bt_cfg_en, tsk_bt_cfg_run, tsk_bt_cfg_exit, NULL, NULL),
    [tsk_bt::bt_state::RUN_IDLE]   = SMF_CREATE_STATE(tsk_bt_idle_en, tsk_bt_idle_run, tsk_bt_idle_exit, NULL, NULL),
    [tsk_bt::bt_state::RUN_TX]     = SMF_CREATE_STATE(tsk_bt_tx_en, tsk_bt_tx_run, tsk_bt_tx_exit, NULL, NULL),
    [tsk_bt::bt_state::RUN_REPORT] = SMF_CREATE_STATE(tsk_bt_report_en, tsk_bt_report_run, tsk_bt_report_exit, NULL, NULL),
};
// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
void tsk_bt_le_entry(void* param1, void* param2, void* param3) {
    tsk_bt tsk_bt;
    static K_FIFO_DEFINE(rx_queue);
    int rc           = 0;
    bt_obj.rx_q     = &rx_queue; //!< Initialize the RX queue
    bt_obj.ret_code = -1;
    bt_obj.ret_code = -1;
    bt_obj.bt_oop  = &tsk_bt; //!< Initialize the user defined object

    smf_set_initial(SMF_CTX(&bt_obj), &tsk_bt::state_machine[tsk_bt::bt_state::RUN_CFG]);
    printk("TSK_BLUETOOTH : Entry\n");
    while (tsk_bt.tsk_bt_running == true) {
        rc = tsk_utl_rcv_que(que_id_t::BT, tsk_bt.in_msg, K_MSEC(1000));
        if (rc == 0) {
            switch (tsk_bt.in_msg.evt) {
                case que_evt_t::STA_SIG :
                    break;
                case que_evt_t::SNS_DATA_SIG :
                    memcpy(&tsk_bt.env_dat, &tsk_bt.in_msg.env_dat, sizeof(env_dat_t));
                    break;
            default:
                tsk_bt.in_msg.evt = que_evt_t::UNKNOWN_SIG; // Set to unknown signal if not handled
                break;
            }
        }
        else {
            tsk_bt.in_msg.evt = que_evt_t::UNKNOWN_SIG; 
        }
        /* Set the initial state */
        rc = smf_run_state(SMF_CTX(&bt_obj));
        if (rc) {
            break;
        }
    }
}

void tsk_bt::tsk_bt_cfg_en(void *obj) {
    struct bt_obj_t *bt_obj = (struct bt_obj_t *)obj;
    printk("TSK_BT : Configuration entry\n");

    if (bt_obj->ret_code != 0) {
        bt_obj->ret_code = bt_obj->bt_oop->tsk_st_bt_sns_drv.st_bt_sns_init();
    }
}
enum smf_state_result tsk_bt::tsk_bt_cfg_run(void *obj) {
    struct bt_obj_t *bt_obj = (struct bt_obj_t *)obj;
    printk("TSK_BT : Configuration run\n");
    // Perform configuration tasks here
    if (bt_obj->ret_code == 0) {
        // If configuration is successful, transition to idle state
        smf_set_state(SMF_CTX(obj), &bt_obj->bt_oop->state_machine[tsk_bt::bt_state::RUN_IDLE]);
    } 
    else {
        // If configuration fails, go to configuration state again
        smf_set_state(SMF_CTX(obj), &bt_obj->bt_oop->state_machine[tsk_bt::bt_state::RUN_CFG]);
    }
    return SMF_EVENT_HANDLED;
}
void tsk_bt::tsk_bt_cfg_exit(void *obj) {
    printk("TSK_BT : Configuration exit\n");
    // Perform cleanup tasks here
}

void tsk_bt::tsk_bt_idle_en(void *obj) {
    printk("TSK_BT : Idle entry\n");
}

enum smf_state_result tsk_bt::tsk_bt_idle_run(void *obj) {
    struct bt_obj_t *bt_obj = (struct bt_obj_t *)obj;
    printk("TSK_BT : Idle run\n");
    // Perform idle tasks here
    switch (bt_obj->bt_oop->in_msg.evt) {
        case que_evt_t::SNS_DATA_SIG :
            smf_set_state(SMF_CTX(obj), &tsk_bt::state_machine[tsk_bt::RUN_REPORT]);
            break;
        
        default:
            break;
    }
    return SMF_EVENT_HANDLED;
}

void tsk_bt::tsk_bt_idle_exit(void *obj) {
    printk("TSK_BT : Idle exit\n");
    // Perform cleanup tasks here
}

void tsk_bt::tsk_bt_tx_en(void *obj) {
    printk("TSK_BT : Transmit entry\n");
    // Perform transmit tasks here
}
enum smf_state_result tsk_bt::tsk_bt_tx_run(void *obj) {
    printk("TSK_BT : Transmit run\n");
    smf_set_state(SMF_CTX(obj), &tsk_bt::state_machine[tsk_bt::RUN_CFG]);
    return SMF_EVENT_HANDLED;
}
void tsk_bt::tsk_bt_tx_exit(void *obj) {
    printk("TSK_BT : Transmit exit\n");
    // Perform cleanup tasks here
}

void tsk_bt::tsk_bt_report_en(void *obj) {
    printk("TSK_BT : Report entry\n");
    // Perform report tasks here
}
enum smf_state_result tsk_bt::tsk_bt_report_run(void *obj) {
    struct bt_obj_t *bt_obj = (struct bt_obj_t *)obj;
    printk("TSK_BT : Report run\n");
    // Perform report tasks here
    bt_obj->ret_code = bt_obj->bt_oop->tsk_st_bt_sns_drv.bt_st_report_data();
    if (bt_obj->ret_code == 0) {
        // If report is successful, transition to idle state
        smf_set_state(SMF_CTX(obj), &tsk_bt::state_machine[tsk_bt::RUN_CFG]);
    }
    return SMF_EVENT_HANDLED;
}
void tsk_bt::tsk_bt_report_exit(void *obj) {
    printk("TSK_BT : Report exit\n");
    // Perform cleanup tasks here
}

// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_BT_LE_EN
