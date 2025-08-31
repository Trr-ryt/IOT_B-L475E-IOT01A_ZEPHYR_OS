//! @file tsk_env_sns.cpp
//! @brief Task ENV SNS source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include "tsk_cfg.hpp"
#if (TSK_ENV_SNS_EN == TSK_ENABLED)
#include <zephyr/kernel.h>
#include <zephyr/smf.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "tsk_cfg.hpp"
#include "dt/dt_cfg.hpp"
#include "utl/tsk_utl.hpp"
#include "tsk/tsk_env_sns.hpp"
// ---------- (4) EXTERN OBJECT ---------------------------------------------------------------------- //
// N/A

// ---------- (5) PUBLIC INTERFACE FUNCTION ---------------------------------------------------------- //
// N/A

// ---------- (6) CLASS DECLARATION ------------------------------------------------------------------ //
/* User defined object */
struct env_sns_obj_t {
    /* This must be first */
    struct smf_ctx ctx;
    int    ret_code;
    tsk_env_sns* env_oop;
} env_sns_obj;

// ---------- (7) PUBLIC DATA ------------------------------------------------------------------------ //
int    tsk_env_sns::ret_dev              = 0;
int    tsk_env_sns::hts221_sts           = 0;
int    tsk_env_sns::lps22hb_sts          = 0;
int    tsk_env_sns::vl53l0x_sts          = 0;
int    tsk_env_sns::lsm6dsl_sts          = 0;
int    tsk_env_sns::lis3dl_sts           = 0;
bool   tsk_env_sns::tsk_env_sns_running  = true;
struct que_msg_t tsk_env_sns::input_msg  = {};
struct que_msg_t tsk_env_sns::output_msg = {};
const struct smf_state tsk_env_sns::state_machine[] = {
    [tsk_env_sns::env_sns_state::RUN_CFG]    = SMF_CREATE_STATE(tsk_env_cfg_en, tsk_env_cfg_run, tsk_env_cfg_exit, NULL, NULL),
    [tsk_env_sns::env_sns_state::RUN_IDLE]   = SMF_CREATE_STATE(tsk_env_idle_en, tsk_env_idle_run, tsk_env_idle_exit, NULL, NULL),
    [tsk_env_sns::env_sns_state::RUN_FETCH]  = SMF_CREATE_STATE(tsk_env_fetch_en, tsk_env_fetch_run, tsk_env_fetch_exit, NULL, NULL),
    [tsk_env_sns::env_sns_state::RUN_REPORT] = SMF_CREATE_STATE(tsk_env_report_en, tsk_env_report_run, tsk_env_report_exit, NULL, NULL),
};
// ---------- (8) PRIVATE PROGRAMMING DEFINE --------------------------------------------------------- //
// N/A

// ---------- (9) PRIVATE TYPEDEF -------------------------------------------------------------------- //
// N/A

// ---------- (10) PRIVATE FUNCTION ------------------------------------------------------------------- //
// N/A

// ---------- (11) PRIVATE DATA ---------------------------------------------------------------------- //
// N/A

// ---------- (12) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (13) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
void tsk_env_sns_entry(void* param1, void* param2, void* param3) {
    int rc = 0;
    env_sns_obj.ret_code = -1;
    tsk_env_sns tsk_env_sns;
    env_sns_obj.env_oop = &tsk_env_sns; // Initialize the pointer to the tsk_env_sns instance
    smf_set_initial(SMF_CTX(&env_sns_obj), &tsk_env_sns.state_machine[tsk_env_sns::RUN_CFG]);
    printk("TSK_ENV_SNS : Entry\n");
    while (tsk_env_sns.tsk_env_sns_running == true) {
        rc = tsk_utl_rcv_que(que_id_t::ENV_SNS, tsk_env_sns.input_msg, K_MSEC(1000));
        if (rc == 0) {
            switch (tsk_env_sns.input_msg.evt) {
                case que_evt_t::STA_SIG :
                    printk("TSK_ENV_SNS : Start signal received\n");
                    break;
                case que_evt_t::SNS_REQ_SIG :
                    printk("TSK_ENV_SNS : Sensor request data signal received\n");
                    break;
                
            default:
                // If the event is not recognized, set the event to UNKNOWN_SIG
                tsk_env_sns.input_msg.evt = que_evt_t::UNKNOWN_SIG;
                break;
            }
        }
        else {
            // If no message received, set the event to UNKNOWN_SIG
            tsk_env_sns.input_msg.evt = que_evt_t::UNKNOWN_SIG;
        }

        rc = smf_run_state(SMF_CTX(&env_sns_obj));
        if (rc) {
            /* handle return code and terminate state machine */
            break;
        }
    }
}


void tsk_env_sns:: tsk_env_cfg_en(void *obj) {
    struct env_sns_obj_t *env_obj = (struct env_sns_obj_t *)obj;
    printk("TSK_ENV_SNS : CFG entry\n");
    if (env_obj->ret_code != 0 || env_obj->ret_code == -1) {
        env_obj->ret_code = env_obj->env_oop->devices_init();
        // thumbnail the output message to prepare it for sending
        env_obj->env_oop->thumbnail_msg();
    }
}

enum smf_state_result tsk_env_sns:: tsk_env_cfg_run(void *obj) {
    struct env_sns_obj_t *env_obj = (struct env_sns_obj_t *)obj;
    printk("TSK_ENV_SNS : CFG run\n");
    if (env_obj->ret_code == 0) {
        // If the device initialization was successful, transition to the IDLE state
        smf_set_state(SMF_CTX(env_obj), &env_obj->env_oop->state_machine[tsk_env_sns::env_sns_state::RUN_IDLE]);
    } 
    else {
        smf_set_state(SMF_CTX(env_obj), &env_obj->env_oop->state_machine[tsk_env_sns::env_sns_state::RUN_CFG]);
    }
    return SMF_EVENT_HANDLED;
}

void tsk_env_sns:: tsk_env_cfg_exit(void *obj) {
    printk("TSK_ENV_SNS : CFG exit\n");
        /* Do something */
}

void tsk_env_sns:: tsk_env_idle_en(void *obj) {
    printk("TSK_ENV_SNS : IDLE entry\n");
        /* Do something */
}

enum smf_state_result tsk_env_sns:: tsk_env_idle_run(void *obj) {
    struct env_sns_obj_t *env_obj = (struct env_sns_obj_t *)obj;
    printk("TSK_ENV_SNS : IDLE run\n");
    switch (env_obj->env_oop->input_msg.evt)  {
        case que_evt_t::SNS_REQ_SIG :
            smf_set_state(SMF_CTX(env_obj), &env_obj->env_oop->state_machine[tsk_env_sns::env_sns_state::RUN_FETCH]);
            break;
        
        default:
            break;
    }
    return SMF_EVENT_HANDLED;
}

void tsk_env_sns:: tsk_env_idle_exit(void *obj) {
    printk("TSK_ENV_SNS : IDLE exit\n");
        /* Do something */
}

void tsk_env_sns:: tsk_env_fetch_en(void *obj) {
    printk("TSK_ENV_SNS : FETCH entry\n");
        /* Do something */
}

enum smf_state_result tsk_env_sns:: tsk_env_fetch_run(void *obj) {
    struct env_sns_obj_t *env_obj = (struct env_sns_obj_t *)obj;
    printk("TSK_ENV_SNS : FETCH run\n");
    env_obj->env_oop->ret_dev = env_obj->env_oop->fetch_data(*(env_obj->env_oop));
    if (env_obj->env_oop->ret_dev == 0) {
        smf_set_state(SMF_CTX(env_obj), &env_obj->env_oop->state_machine[tsk_env_sns::env_sns_state::RUN_REPORT]);
    } 
    else {
        smf_set_state(SMF_CTX(env_obj), &env_obj->env_oop->state_machine[tsk_env_sns::env_sns_state::RUN_IDLE]);
    }
    return SMF_EVENT_HANDLED;
}

void tsk_env_sns:: tsk_env_fetch_exit(void *obj) {
    printk("TSK_ENV_SNS : FETCH exit\n");
        /* Do something */
}

void tsk_env_sns:: tsk_env_report_en(void *obj) {
    printk("TSK_ENV_SNS : REPORT entry\n");
        /* Do something */
}

enum smf_state_result tsk_env_sns:: tsk_env_report_run(void *obj) {
    struct env_sns_obj_t *env_obj = (struct env_sns_obj_t *)obj;
    printk("TSK_ENV_SNS : REPORT run\n");
    // Send the sensor data to the requester
    env_obj->env_oop->ret_dev = tsk_utl_snd_que(que_id_t::DAT_ACQ, env_obj->env_oop->output_msg, K_MSEC(100));
    if (env_obj->env_oop->ret_dev == 0) {
        // Set the state back to idle
        smf_set_state(SMF_CTX(env_obj), &env_obj->env_oop->state_machine[tsk_env_sns::env_sns_state::RUN_IDLE]);
    }
    return SMF_EVENT_HANDLED;
}

void tsk_env_sns:: tsk_env_report_exit(void *obj) {
    printk("TSK_ENV_SNS : REPORT exit\n");
        /* Do something */
}

int tsk_env_sns::devices_init() {
    ret_dev = 0;
    // Initialize the sensor devices
    hts221_sts  = tsk_hts221_drv.init();
    lps22hb_sts = tsk_lps22hb_drv.init();
    vl53l0x_sts = tsk_vl53l0x_drv.init();
    lsm6dsl_sts = tsk_lsm6dsl_drv.init();
    lis3dl_sts  = tsk_lis3dl_drv.init();

    if (hts221_sts!= 0 && lps22hb_sts != 0 && vl53l0x_sts != 0 && lsm6dsl_sts != 0 && lis3dl_sts!= 0) {
        printk("TSK_ENV_SNS : Sensor initialization error HTS221 %d, LPS22HB %d, VL53L0X %d, LSM6DSL %d, LIS3DL %d\n",
               hts221_sts, lps22hb_sts, vl53l0x_sts, lsm6dsl_sts, lis3dl_sts);
        ret_dev = -1;
    }
    else {
        ret_dev = 0;
    }
    return ret_dev;
}

inline void tsk_env_sns::thumbnail_msg() {
    output_msg.evt    = que_evt_t::SNS_DATA_SIG;
    output_msg.res_id = que_id_t::ENV_SNS;
    output_msg.ret    = que_ret_t::RES_OK;
}

int tsk_env_sns::fetch_data(tsk_env_sns& dev_drv) {
    int rc = -1;

    hts221_sts = dev_drv.tsk_hts221_drv.read_data();
    if (hts221_sts != 0){
        printk("TSK_ENV_SNS : HTS221 sns error %d\n", hts221_sts);
    }

    lps22hb_sts = dev_drv.tsk_lps22hb_drv.read_data();
    if (lps22hb_sts != 0){
        printk("TSK_ENV_SNS : LPS22HB sns error %d\n", lps22hb_sts);
    }

    vl53l0x_sts = dev_drv.tsk_vl53l0x_drv.read_data();
    if (vl53l0x_sts != 0){
        printk("TSK_ENV_SNS : VL53L0X sns error %d\n", vl53l0x_sts);
    }

    lsm6dsl_sts = dev_drv.tsk_lsm6dsl_drv.read_data();
    if (lsm6dsl_sts != 0){
        printk("TSK_ENV_SNS : LSM6DSL sns error %d\n", lsm6dsl_sts);
    }

    lis3dl_sts = dev_drv.tsk_lis3dl_drv.read_data();
    if (lis3dl_sts != 0){
        printk("TSK_ENV_SNS : LIS3DL sns error %d\n", lis3dl_sts);
    }

    dev_drv.output_msg.env_dat.temp_val         = dev_drv.tsk_hts221_drv.temp_val;
    dev_drv.output_msg.env_dat.humi_val         = dev_drv.tsk_hts221_drv.humi_val;
    dev_drv.output_msg.env_dat.pressure_val     = dev_drv.tsk_lps22hb_drv.pressure_val;
    dev_drv.output_msg.env_dat.ambient_temp_val = dev_drv.tsk_lps22hb_drv.ambient_temp_val;
    dev_drv.output_msg.env_dat.prox             = dev_drv.tsk_vl53l0x_drv.prox;
    dev_drv.output_msg.env_dat.dist             = dev_drv.tsk_vl53l0x_drv.dist;
    dev_drv.output_msg.env_dat.max_dist         = dev_drv.tsk_vl53l0x_drv.max_dist;
    dev_drv.output_msg.env_dat.signal_rate      = dev_drv.tsk_vl53l0x_drv.signal_rate;
    dev_drv.output_msg.env_dat.ambient_rate     = dev_drv.tsk_vl53l0x_drv.ambient_rate;
    dev_drv.output_msg.env_dat.spad_count       = dev_drv.tsk_vl53l0x_drv.spad_count;
    dev_drv.output_msg.env_dat.range_status     = dev_drv.tsk_vl53l0x_drv.range_status;
    dev_drv.output_msg.env_dat.accel_x_val      = dev_drv.tsk_lsm6dsl_drv.accel_x_val;
    dev_drv.output_msg.env_dat.accel_y_val      = dev_drv.tsk_lsm6dsl_drv.accel_y_val;
    dev_drv.output_msg.env_dat.accel_z_val      = dev_drv.tsk_lsm6dsl_drv.accel_z_val;
    dev_drv.output_msg.env_dat.gyro_x_val       = dev_drv.tsk_lsm6dsl_drv.gyro_x_val;
    dev_drv.output_msg.env_dat.gyro_y_val       = dev_drv.tsk_lsm6dsl_drv.gyro_y_val;
    dev_drv.output_msg.env_dat.gyro_z_val       = dev_drv.tsk_lsm6dsl_drv.gyro_z_val;
    dev_drv.output_msg.env_dat.x_val            = dev_drv.tsk_lis3dl_drv.x_val;
    dev_drv.output_msg.env_dat.y_val            = dev_drv.tsk_lis3dl_drv.y_val;
    dev_drv.output_msg.env_dat.z_val            = dev_drv.tsk_lis3dl_drv.z_val; 
    // print sensor data
    // printk("TSK_ENV_SNS : HTS221 temp %.2f C, humi %.2f %%\n",
    //        dev_drv.tsk_hts221_drv.temp_val, dev_drv.tsk_hts221_drv.humi_val);
    // printk("TSK_ENV_SNS : LPS22HB ambient temp %.2f C, pressure %.2f hPa\n",
    //           dev_drv.tsk_lps22hb_drv.ambient_temp_val, dev_drv.tsk_lps22hb_drv.pressure_val);
    // printk("TSK_ENV_SNS : VL53L0X prox %.2f mm, dist %.2f mm, max dist %.2f mm, signal rate %.2f MCPS, ambient rate %.2f MCPS, spad count %.0f, range status %d\n",
    //           dev_drv.tsk_vl53l0x_drv.prox, dev_drv.tsk_vl53l0x_drv.dist,
    //           dev_drv.tsk_vl53l0x_drv.max_dist, dev_drv.tsk_vl53l0x_drv.signal_rate,
    //           dev_drv.tsk_vl53l0x_drv.ambient_rate, dev_drv.tsk_vl53l0x_drv.spad_count,
    //           dev_drv.tsk_vl53l0x_drv.range_status);
    // printk("TSK_ENV_SNS : LSM6DSL accel x %.2f g, y %.2f g, z %.2f g, gyro x %.2f dps, y %.2f dps, z %.2f dps\n",
    //             dev_drv.tsk_lsm6dsl_drv.accel_x_val, dev_drv.tsk_lsm6dsl_drv.accel_y_val,
    //             dev_drv.tsk_lsm6dsl_drv.accel_z_val, dev_drv.tsk_lsm6dsl_drv.gyro_x_val,
    //             dev_drv.tsk_lsm6dsl_drv.gyro_y_val, dev_drv.tsk_lsm6dsl_drv.gyro_z_val);
    // printk("TSK_ENV_SNS : LIS3DL x %.2f g, y %.2f g, z %.2f g\n",
    //             dev_drv.tsk_lis3dl_drv.x_val, dev_drv.tsk_lis3dl_drv.y_val,
    //             dev_drv.tsk_lis3dl_drv.z_val);
    rc = 0;
    return rc;
}


// ---------- (14) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_ENV_SNS_EN
