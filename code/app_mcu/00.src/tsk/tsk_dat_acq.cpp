//! @file tsk_daq_acq.cpp
//! @brief Task DAT ACQ source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <zephyr/kernel.h>

// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "tsk/tsk_dat_acq.hpp"
#include "utl/tsk_utl.hpp"
// ---------- (4) EXTERN OBJECT ---------------------------------------------------------------------- //
// N/A

// ---------- (5) PUBLIC INTERFACE FUNCTION ---------------------------------------------------------- //
// N/A

// ---------- (6) PUBLIC DATA ------------------------------------------------------------------------ //
class tsk_dat_acq {
    private:
        /* data */
    public:
    static bool tsk_daq_acq_running;
    int rc = 0;
    int rc_timer_func = 0;
    static uint8_t timer_cnt;
    struct que_msg_t output_msg, input_msg;
    int  tsk_dat_acq_env_process(env_dat_t& env_dat);
    int  tsk_dat_acq_pack_and_send_data(que_msg_t& input_msg);
    inline int tsk_daq_acq_req_env_dat(tsk_dat_acq& tsk_daq_acq);
};
// ---------- (7) PRIVATE PROGRAMMING DEFINE --------------------------------------------------------- //
// N/A

// ---------- (8) PRIVATE TYPEDEF -------------------------------------------------------------------- //
// N/A

// ---------- (9) PRIVATE FUNCTION ------------------------------------------------------------------- //
// N/A

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
static void tsk_dat_acq_req_data_in_ten_sec(struct k_work *work);
static void timer_handler_ten_sec(struct k_timer* dummy);
static void tsk_dat_acq_req_data(void);
K_WORK_DEFINE(ten_sec_wrk, tsk_dat_acq_req_data_in_ten_sec);
K_TIMER_DEFINE(ten_sec_timer, timer_handler_ten_sec, NULL);
// Initialize the task running state
bool tsk_dat_acq::tsk_daq_acq_running = true;
uint8_t tsk_dat_acq::timer_cnt = 0;

// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
void tsk_dat_acq_entry(void* param1, void* param2, void* param3) {
    int rc = 0;
    tsk_dat_acq tsk_daq_acq;
    printk("TSK_DAQ_ACQ : Entry\n");
    k_timer_start(&ten_sec_timer, K_SECONDS(20), K_SECONDS(10));
    while (tsk_daq_acq.tsk_daq_acq_running == true) {
        rc = tsk_utl_rcv_que(que_id_t::DAT_ACQ, tsk_daq_acq.input_msg, K_MSEC(1000));
        if (rc == 0) {
            switch (tsk_daq_acq.input_msg.evt) {
                case que_evt_t::STA_SIG :
                    printk("TSK_DAQ_ACQ : Start signal received\n");
                    break;
                case que_evt_t::SNS_DATA_SIG :
                    rc = tsk_daq_acq.tsk_dat_acq_pack_and_send_data(tsk_daq_acq.input_msg);
                    if (rc != 0) {
                        printk("TSK_DAQ_ACQ : Error processing sensor data\n");
                    }
                    break;
                default:
                break;
            }
        }
    }
}

static void timer_handler_ten_sec(struct k_timer *dummy) {
    k_work_submit(&ten_sec_wrk);
}

static void tsk_dat_acq_req_data_in_ten_sec(struct k_work *work) {
    tsk_dat_acq_req_data();
}

static void tsk_dat_acq_req_data(void) {
    tsk_dat_acq tsk_daq_acq;
    tsk_daq_acq.timer_cnt++;
    if (tsk_daq_acq.timer_cnt >= 1) {
        tsk_daq_acq.rc = tsk_daq_acq.tsk_daq_acq_req_env_dat(tsk_daq_acq);
        if (tsk_daq_acq.rc == 0) {
            tsk_daq_acq.timer_cnt = 0;
        }
    }
}

inline int tsk_dat_acq::tsk_daq_acq_req_env_dat(tsk_dat_acq& tsk_daq_acq) {
    tsk_daq_acq.output_msg.evt    = que_evt_t::SNS_REQ_SIG;
    tsk_daq_acq.output_msg.res_id = que_id_t::ENV_SNS;
    tsk_daq_acq.output_msg.ret    = que_ret_t::REQ_OK;
    // Send request to the DAT_ACQ queue
    tsk_daq_acq.rc = tsk_utl_snd_que(tsk_daq_acq.output_msg.res_id, tsk_daq_acq.output_msg, K_MSEC(100));
    return tsk_daq_acq.rc;
}

int tsk_dat_acq::tsk_dat_acq_env_process(env_dat_t& env_dat) {
    // Process the environment data
    output_msg.evt    = que_evt_t::SNS_DATA_SIG;
    output_msg.ret    = que_ret_t::RES_OK;
    output_msg.res_id = que_id_t::BT; // Set the response ID to ENV_SNS
    memcpy(&output_msg.env_dat, &env_dat, sizeof(env_dat_t)); // Copy the environment data to the output message
    // Send the processed data back to the requester
    rc = tsk_utl_snd_que(output_msg.res_id, output_msg, K_MSEC(100));
    return rc; // Return the result of sending the message
}
int tsk_dat_acq::tsk_dat_acq_pack_and_send_data(que_msg_t& input_msg) {
    switch (input_msg.res_id) {
        case que_id_t::ENV_SNS:
            rc = tsk_dat_acq_env_process(input_msg.env_dat);
            break;
        
        default:
            break;
    }
    return rc; // Return the result of processing the environment data
}
// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
