//! @file tsk_mqtt.cpp
//! @brief Task MQTT source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <zephyr/kernel.h>
#include <zephyr/smf.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/random/random.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "tsk/tsk_mqtt.hpp"
#include "utl/tsk_utl.hpp"
#include "tsk_cfg.hpp"
#if (TSK_MQTT_EN == TSK_ENABLED)
// ---------- (4) EXTERN OBJECT ---------------------------------------------------------------------- //
// N/A

// ---------- (5) PUBLIC INTERFACE FUNCTION ---------------------------------------------------------- //
// N/A

// ---------- (6) PUBLIC DATA ------------------------------------------------------------------------ //
static void tsk_mqtt_cfg_en(void *obj);
static void tsk_mqtt_cfg_run(void *obj);
static void tsk_mqtt_cfg_exit(void *obj);
static void tsk_mqtt_idle_en(void *obj);
static void tsk_mqtt_idle_run(void *obj);
static void tsk_mqtt_idle_exit(void *obj);
static void tsk_mqtt_pack_en(void *obj);
static void tsk_mqtt_pack_run(void *obj);
static void tsk_mqtt_pack_exit(void *obj);
static void tsk_mqtt_pub_en(void *obj);
static void tsk_mqtt_pub_run(void *obj);
static void tsk_mqtt_pub_exit(void *obj);
static void tsk_mqtt_recv_en(void *obj);
static void tsk_mqtt_recv_run(void *obj);
static void tsk_mqtt_recv_exit(void *obj);
/* List of demo states */
enum tsk_mqtt_state {RUN_CFG, RUN_IDLE, RUN_PACK, RUN_PUB, RUN_RECV};
/* Populate state table */
static const struct smf_state tsk_mqtt_states[] = {
    [RUN_CFG]  = SMF_CREATE_STATE(tsk_mqtt_cfg_en, tsk_mqtt_cfg_run, tsk_mqtt_cfg_exit, NULL, NULL),
    [RUN_IDLE] = SMF_CREATE_STATE(tsk_mqtt_idle_en, tsk_mqtt_idle_run, tsk_mqtt_idle_exit, NULL, NULL),
    [RUN_PACK] = SMF_CREATE_STATE(tsk_mqtt_pack_en, tsk_mqtt_pack_run, tsk_mqtt_pack_exit, NULL, NULL),
    [RUN_PUB]  = SMF_CREATE_STATE(tsk_mqtt_pub_en, tsk_mqtt_pub_run, tsk_mqtt_pub_exit, NULL, NULL),
    [RUN_RECV] = SMF_CREATE_STATE(tsk_mqtt_recv_en, tsk_mqtt_recv_run, tsk_mqtt_recv_exit, NULL, NULL),
};

class tsk_mqtt {
    private:
        /* data */
    public:
        static bool tsk_mqtt_running; //!< Task MQTT running flag
        static struct que_msg_t input_msg;   //!< Input message
        static struct que_msg_t output_msg;  //!< Output message
};

/* User defined object */
struct mqtt_obj_t {
        /* This must be first */
        struct smf_ctx ctx;
        tsk_mqtt* mqtt_oop; //!< Pointer to the task MQTT object
        /* Other state specific data add here */
} mqtt_obj;
// ---------- (7) PRIVATE PROGRAMMING DEFINE --------------------------------------------------------- //
// N/A

// ---------- (8) PRIVATE TYPEDEF -------------------------------------------------------------------- //
// N/A

// ---------- (9) PRIVATE FUNCTION ------------------------------------------------------------------- //
// N/A

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
bool tsk_mqtt::tsk_mqtt_running       = true; //!< Task MQTT running flag
struct que_msg_t tsk_mqtt::input_msg  = {};   //!< Input message
struct que_msg_t tsk_mqtt::output_msg = {};   //!< Output message

// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
void tsk_mqtt_entry(void* param1, void* param2, void* param3) {
    int rc = 0;
    tsk_mqtt tsk_mqtt; //!< Create an instance of the tsk_mqtt class
    mqtt_obj.mqtt_oop = &tsk_mqtt; //!< Initialize the user defined object
    smf_set_initial(SMF_CTX(&mqtt_obj), &tsk_mqtt_states[RUN_CFG]);
    printk("TSK_MQTT : Entry\n");
    while (tsk_mqtt.tsk_mqtt_running == true) {
        rc = tsk_utl_rcv_que(que_id_t::MQTT, tsk_mqtt.input_msg, K_MSEC(1000));
        if (rc == 0) {
            switch (tsk_mqtt.input_msg.evt) {
                case que_evt_t::STA_SIG :
                    break;
                case que_evt_t::SNS_DATA_SIG :
                    break;
                case que_evt_t::NET_CON_SIG :
                    break;
            default:
                tsk_mqtt.input_msg.evt = que_evt_t::UNKNOWN_SIG;
                break;
            }
        }
        else {
            tsk_mqtt.input_msg.evt = que_evt_t::UNKNOWN_SIG;
        }
        /* Set the initial state */
        rc = smf_run_state(SMF_CTX(&mqtt_obj));
        if (rc) {
            /* handle return code and terminate state machine */
            break;
        }
    }
}

static void tsk_mqtt_cfg_en(void *obj) {
    printk("TSK_MQTT : Configuration entry\n");
    // Perform configuration tasks here
}

static void tsk_mqtt_cfg_run(void *obj) {
    struct mqtt_obj_t *mqtt_obj = (struct mqtt_obj_t *)obj;
    printk("TSK_MQTT : Configuration run\n");
    // Perform configuration tasks here
    smf_set_state(SMF_CTX(mqtt_obj), &tsk_mqtt_states[RUN_IDLE]);
}

static void tsk_mqtt_cfg_exit(void *obj) {
    printk("TSK_MQTT : Configuration exit\n");
    // Perform cleanup tasks here
}

static void tsk_mqtt_idle_en(void *obj) {
    printk("TSK_MQTT : Idle entry\n");
    // Perform idle tasks here
}
static void tsk_mqtt_idle_run(void *obj) {
    struct mqtt_obj_t *mqtt_obj = (struct mqtt_obj_t *)obj;
    printk("TSK_MQTT : Idle run\n");
    // Perform idle tasks here
    switch (mqtt_obj->mqtt_oop->input_msg.evt) {
        case que_evt_t::STA_SIG :
            smf_set_state(SMF_CTX(mqtt_obj), &tsk_mqtt_states[RUN_PACK]);
            break;
        default:
            break;
    }
}

static void tsk_mqtt_idle_exit(void *obj) {
    printk("TSK_MQTT : Idle exit\n");
    // Perform cleanup tasks here
}

static void tsk_mqtt_pack_en(void *obj) {
    printk("TSK_MQTT : Pack entry\n");
    // Perform pack tasks here
}

static void tsk_mqtt_pack_run(void *obj) {
    struct mqtt_obj_t *mqtt_obj = (struct mqtt_obj_t *)obj;
    printk("TSK_MQTT : Pack run\n");
    // Perform pack tasks here
    smf_set_state(SMF_CTX(mqtt_obj), &tsk_mqtt_states[RUN_PUB]);
}

static void tsk_mqtt_pack_exit(void *obj) {
    printk("TSK_MQTT : Pack exit\n");
    // Perform cleanup tasks here
}

static void tsk_mqtt_pub_en(void *obj) {
    printk("TSK_MQTT : Publish entry\n");
    // Perform publish tasks here
}

static void tsk_mqtt_pub_run(void *obj) {
    struct mqtt_obj_t *mqtt_obj = (struct mqtt_obj_t *)obj;
    printk("TSK_MQTT : Publish run\n");
    // Perform publish tasks here
    smf_set_state(SMF_CTX(mqtt_obj), &tsk_mqtt_states[RUN_RECV]);
}

static void tsk_mqtt_pub_exit(void *obj) {
    printk("TSK_MQTT : Publish exit\n");
    // Perform cleanup tasks here
}

static void tsk_mqtt_recv_en(void *obj) {
    printk("TSK_MQTT : Receive entry\n");
    // Perform receive tasks here
}

static void tsk_mqtt_recv_run(void *obj) {
    struct mqtt_obj_t *mqtt_obj = (struct mqtt_obj_t *)obj;
    printk("TSK_MQTT : Receive run\n");
    // Perform receive tasks here
    smf_set_state(SMF_CTX(mqtt_obj), &tsk_mqtt_states[RUN_IDLE]);
}

static void tsk_mqtt_recv_exit(void *obj) {
    printk("TSK_MQTT : Receive exit\n");
    // Perform cleanup tasks here
}

// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_MQTT_EN
