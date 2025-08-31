//! @file tsk_utl.cpp
//! @brief Task Utility
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <zephyr/kernel.h>

// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "utl/tsk_utl.hpp"
#include "tsk_cfg.hpp"

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
static void tsk_utl_queue_init(void);

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
static K_THREAD_STACK_DEFINE(tsk_sys_stack,            TSK_SYS_STACK_SIZE);
static K_THREAD_STACK_DEFINE(tsk_env_sns_stack,        TSK_ENV_SNS_STACK_SIZE);
static K_THREAD_STACK_DEFINE(tsk_dat_acq_stack,        TSK_DAT_ACQ_STACK_SIZE);
static K_THREAD_STACK_DEFINE(tsk_mqtt_stack,           TSK_MQTT_STACK_SIZE);
static K_THREAD_STACK_DEFINE(tsk_wifi_stack,           TSK_WIFI_STACK_SIZE);
static K_THREAD_STACK_DEFINE(tsk_ota_stack,            TSK_OTA_STACK_SIZE);
static K_THREAD_STACK_DEFINE(tsk_bt_stack,             TSK_BT_LE_STACK_SIZE);

static struct tsk_cfg_t g_tsk_cfg[static_cast<uint8_t>(tsk_id_t::MAX)] {
    {tsk_sys_stack,            TSK_SYS_STACK_SIZE,           TSK_SYS_PRI},
    {tsk_env_sns_stack,        TSK_ENV_SNS_STACK_SIZE,       TSK_ENV_SNS_PRI},
    {tsk_dat_acq_stack,        TSK_DAT_ACQ_STACK_SIZE,       TSK_DAT_ACQ_PRI},
    {tsk_mqtt_stack,           TSK_MQTT_STACK_SIZE,          TSK_MQTT_PRI},
    {tsk_wifi_stack,           TSK_WIFI_STACK_SIZE,          TSK_WIFI_PRI},
    {tsk_ota_stack,            TSK_OTA_STACK_SIZE,           TSK_OTA_PRI},
    {tsk_bt_stack,             TSK_BT_LE_STACK_SIZE,         TSK_BT_LE_PRI},
};

static struct tsk_hndl_t m_tsk_hndl[static_cast<uint8_t>(tsk_id_t::MAX)];

static char __noinit __aligned(4) sys_q_buff[TSK_SYS_Q_LEN * sizeof(struct que_msg_t)];
static char __noinit __aligned(4) env_sns_q_buff[TSK_ENV_SNS_Q_LEN * sizeof(struct que_msg_t)];
static char __noinit __aligned(4) dat_acq_q_buff[TSK_DAT_ACQ_Q_LEN * sizeof(struct que_msg_t)];;
static char __noinit __aligned(4) sender_q_buff[TSK_MQTT_Q_LEN * sizeof(struct que_msg_t)];
static char __noinit __aligned(4) wifi_q_buff[TSK_WIFI_Q_LEN * sizeof(struct que_msg_t)];
static char __noinit __aligned(4) ota_q_buff[TSK_OTA_Q_LEN * sizeof(struct que_msg_t)];
static char __noinit __aligned(4) bt_main_q_buff[TSK_BT_LE_Q_LEN * sizeof(struct que_msg_t)];

static struct que_cfg_t const g_que_cfg[static_cast<uint8_t>(que_id_t::MAX)] {
    {que_id_t::SYS,           TSK_SYS_Q_LEN,          sys_q_buff},
    {que_id_t::ENV_SNS,       TSK_ENV_SNS_Q_LEN,      env_sns_q_buff},
    {que_id_t::DAT_ACQ,       TSK_DAT_ACQ_Q_LEN,      dat_acq_q_buff},
    {que_id_t::MQTT,          TSK_MQTT_Q_LEN,         sender_q_buff},
    {que_id_t::WIFI,          TSK_WIFI_Q_LEN,         wifi_q_buff},
    {que_id_t::OTA,           TSK_OTA_Q_LEN,          ota_q_buff},
    {que_id_t::BT,            TSK_BT_LE_Q_LEN,        bt_main_q_buff},
    
};

static struct k_msgq m_que_hndl[static_cast<uint8_t>(que_id_t::MAX)];

// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
/// @brief Init all task utility internal function
void tsk_utl_init(void) {
    tsk_utl_queue_init();
}

/// @brief Create a task and put it to suspended state wait for user to call tsk_utl_tsk_start
/// @param[in] id Thread's ID
/// @param[in] entry Thread entry function
/// @param[in] p1 1st entry point parameter
/// @param[in] name Thread name
/// @return None
void tsk_utl_tsk_cre(tsk_id_t id, k_thread_entry_t entry, void* p1, void* p2, void* p3, const char* name) {
    k_tid_t tid;

    tid = k_thread_create(&m_tsk_hndl[static_cast<uint8_t>(id)].cb,
                          g_tsk_cfg[static_cast<uint8_t>(id)].stack,
                          g_tsk_cfg[static_cast<uint8_t>(id)].stack_size,
                          entry,
                          p1, p2, p3,
                          g_tsk_cfg[static_cast<uint8_t>(id)].tsk_pri, 0, K_FOREVER);
    m_tsk_hndl[static_cast<uint8_t>(id)].tsk_id = tid;
    if (tid != nullptr) {
        (void) k_thread_name_set(tid, name);
    }
}

/// @brief Start a given Thread's id
/// @param id Id of Thread user want to start
void tsk_utl_tsk_sta(tsk_id_t id) {
    k_thread_start(m_tsk_hndl[static_cast<uint8_t>(id)].tsk_id);
}


/// @brief Init all utility queue struct
static void tsk_utl_queue_init(void) {
    uint_fast16_t indx;

    for (indx = 0U; indx < static_cast<uint_fast16_t>(que_id_t::MAX); indx++) {
        k_msgq_init(&m_que_hndl[indx], g_que_cfg[indx].buffer, sizeof(struct que_msg_t), g_que_cfg[indx].len);
    }
}

/// @brief Send message to target queue
/// @param id Target queue's id to enqueue message
/// @param msg Message struct
/// @param timeout Queue put timeout
/// @retval int 0 Message send
/// @retval int -ENOMSG Returned without waiting or queue purged
/// @retval int -EAGAIN Waiting period timed out
int tsk_utl_snd_que(que_id_t id, que_msg_t& msg, k_timeout_t timeout) {
    int rc;

    rc = k_msgq_put(&m_que_hndl[static_cast<uint8_t>(id)], &msg, timeout);

    return (rc);
}

/// @brief Receive message from target queue
/// @param id Target queue's id to receive message from
/// @param msg Message struct to save message
/// @param timeout Queue get timeout
/// @retval int 0 Message received
/// @retval int -ENOMSG Returned without waiting
/// @retval int -EAGAIN Waiting period timed out
int tsk_utl_rcv_que(que_id_t id, que_msg_t& msg, k_timeout_t timeout) {
    int rc;

    rc = k_msgq_get(&m_que_hndl[static_cast<uint8_t>(id)], &msg, timeout);

    return (rc);
}

/// @brief Flush target queue
/// @param id Target queue's id to flush all message
void tsk_utl_clr_que(que_id_t id) {

    k_msgq_purge(&m_que_hndl[static_cast<uint8_t>(id)]);
}

// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
