//! @file utl_msg.hpp
//! @brief Utility message define header
// --------------------------------------------------------------------------------------------------- //

#ifndef UTL_MSG_H
#define UTL_MSG_H

// ---------- (1) SYSTEM INCLUDE --------------------------------------------------------------------- //
#include <zephyr/kernel.h>
#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <zephyr/drivers/sensor.h>


// ---------- (2) EXTERNAL MODULE INCLUDE ------------------------------------------------------------ //
// N/A

// ---------- (3) PUBLIC PROGRAMMING DEFINE ---------------------------------------------------------- //
// N/A

// ---------- (4) ENUMERATOR DEFINITION -------------------------------------------------------------- //
enum class que_evt_t : uint8_t {
    INIT_SIG = 0U,          //!< Initialize signal
    RDY_SIG,                //!< Task/Opt ready signal
    STA_SIG,                //!< Task start signal
    STP_SIG,                //!< Task halt signal
    TICK_SIG,               //!< Time interval tick signal
    TMOUT_SIG,              //!< Timeout signal
    KILL_SIG,               //!< Task kill signal
    RESTART_SIG,            //!< Task restart signal
    ALM_SIG,                //!< Alarm signal
    SNS_REQ_SIG,            //!< Sensor data request signal
    SNS_DATA_SIG,           //!< Sensor data signal
    NET_CON_SIG,            //!< Internet connected signal
    NET_DIS_SIG,            //!< Internet disconnected signal
    UNKNOWN_SIG,            //!< Unknown signal
};

enum class que_ret_t : uint8_t {
    REQ_OK = 0U,
    REQ_NG,
    RES_OK,
    RES_NG
};

enum class tsk_id_t : uint8_t {
    SYS = 0U,
    ENV_SNS,
    DAT_ACQ,
    MQTT,
    WIFI,
    OTA,
    BT,
    MAX,

};

enum class que_id_t : uint8_t {
    SYS = 0U,
    ENV_SNS,
    DAT_ACQ,
    MQTT,
    WIFI,
    OTA,
    BT,
    MAX,
    NONE
};

struct que_cfg_t {
    que_id_t id;
    uint16_t len;
    char* buffer;
};

struct tsk_cfg_t {
    k_thread_stack_t *stack;
    size_t stack_size;
    int tsk_pri;
};

struct tsk_hndl_t {
    k_tid_t tsk_id;
    struct k_thread cb;
};

struct env_dat_t {
    double temp_val;
    double humi_val;
    double pressure_val;
    double ambient_temp_val;
    double prox;
    double dist;
    double max_dist;
    double signal_rate;
    double ambient_rate;
    double spad_count;
    double range_status;
    double accel_x_val;
    double accel_y_val;
    double accel_z_val;
    double gyro_x_val;
    double gyro_y_val;
    double gyro_z_val;
    double x_val;
    double y_val;
    double z_val;
};

struct mqtt_msg_t {
    
};

struct sys_msg_t {

};

struct daq_acq_dat_t {

};
// ---------- (5) STRUCT OR UNION DATA TYPE DEFINITION ----------------------------------------------- //
// N/A

// ---------- (6) TYPEDEF DATA TYPE DEFINITION ------------------------------------------------------- //
/**
 * @brief Message struct use in queue communication
 *
 */
struct que_msg_t {
    // Message header
    que_evt_t evt;
    que_id_t  res_id; //!< Queue ID which want to send reply message back
                      //!< set to QUE_NONE when no need to reply back
    que_ret_t ret;    //!< Queue message return result

    // Message payload
    union {
        uint32_t v32;

        env_dat_t    env_dat;
        mqtt_msg_t   mqtt_dat;
        sys_msg_t    sys_dat;
    };

    void* dp;  //!< Receiver always has responsibility to free
    void* dp2; //!< Receiver always has responsibility to free
};

// ---------- (7) PUBLIC MACRO DEFINITION ------------------------------------------------------------ //
// N/A

// ---------- (8) EXTERN FUNCTION -------------------------------------------------------------------- //

void tsk_utl_init(void);
void tsk_utl_tsk_cre(tsk_id_t id, k_thread_entry_t entry, void* p1, void* p2, void* p3, const char* name = "n/a");
void tsk_utl_tsk_sta(tsk_id_t id);
int  tsk_utl_snd_que(que_id_t id, que_msg_t& msg, k_timeout_t timeout);
int  tsk_utl_rcv_que(que_id_t id, que_msg_t& msg, k_timeout_t timeout);
void tsk_utl_clr_que(que_id_t id);

// ---------- (9) EXTERN VARIABLE -------------------------------------------------------------------- //
// N/A

// ---------- (10) CLASS DECLARATION ----------------------------------------------------------------- //
// N/A

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef UTL_MSG_H
