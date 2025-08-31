//! @file tsk_wifi.cpp
//! @brief Task WIFI source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include "tsk_cfg.hpp"
#if (TSK_WIFI_EN == TSK_ENABLED)
#include <zephyr/smf.h>
#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/dhcpv4_server.h>
#include <zephyr/drivers/pwm.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "tsk/tsk_wifi.hpp"
#include "drv/drv_gpio.hpp"
#include "utl/tsk_utl.hpp"
#include "dt/dt_cfg.hpp"
// ---------- (4) EXTERN OBJECT ---------------------------------------------------------------------- //
// N/A

// ---------- (5) PUBLIC INTERFACE FUNCTION ---------------------------------------------------------- //
// N/A

// ---------- (6) PUBLIC DATA ------------------------------------------------------------------------ //
// N/A

// ---------- (7) PRIVATE PROGRAMMING DEFINE --------------------------------------------------------- //
#define NET_EVENT_WIFI_MASK                                                                    \
	(NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT |                        \
	 NET_EVENT_WIFI_AP_ENABLE_RESULT | NET_EVENT_WIFI_AP_DISABLE_RESULT |                      \
	 NET_EVENT_WIFI_AP_STA_CONNECTED | NET_EVENT_WIFI_AP_STA_DISCONNECTED)
/* STA Mode Configuration */
#define WIFI_SSID "100G"     /* Replace `SSID` with WiFi ssid. */
#define WIFI_PSK  "moodang15907" /* Replace `PASSWORD` with Router password. */
// ---------- (8) PRIVATE TYPEDEF -------------------------------------------------------------------- //
// N/A

// ---------- (9) PRIVATE FUNCTION ------------------------------------------------------------------- //
static void tsk_wifi_cfg_en(void *obj);
static void tsk_wifi_cfg_run(void *obj);
static void tsk_wifi_cfg_exit(void *obj);
static void tsk_wifi_idle_en(void *obj);
static void tsk_wifi_idle_run(void *obj);
static void tsk_wifi_idle_exit(void *obj);
static void tsk_wifi_conn_en(void *obj);
static void tsk_wifi_conn_run(void *obj);
static void tsk_wifi_conn_exit(void *obj);
// ---------- (10) CLASS DATA ---------------------------------------------------------------------- //
class tsk_wifi {
private:
    /* data */
public:
    static int  ret_wifi;
    static int  ret_conn;
    static bool tsk_wifi_running;
    static bool wifi_connected;
    DRV_GPIO led_pwm; //!< LED PWM driver instance
    static struct net_if *sta_iface;
    static struct wifi_connect_req_params sta_config;
    static struct net_mgmt_event_callback cb;
    static struct que_msg_t in_msg, out_msg;
    static void wifi_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface);
    static int  connect_to_wifi(void);
    static int  wifi_init(void);
    static void thumbnail_msg(que_msg_t& msg_wifi, que_id_t tsk_dest);
    static void tsk_dat_acq_req_data_in_thirty_sec(struct k_work *work);
    static void timer_handler_thirty_sec(struct k_timer* dummy);
    tsk_wifi(): led_pwm(green_pwm_led0) {}
};

/* List of demo states */
enum tsk_wifi_state {RUN_CFG, RUN_IDLE, RUN_CONN};
/* Populate state table */
static const struct smf_state tsk_wifi_states[] = {
    [RUN_CFG]  = SMF_CREATE_STATE(tsk_wifi_cfg_en, tsk_wifi_cfg_run, tsk_wifi_cfg_exit, NULL, NULL),
    [RUN_IDLE] = SMF_CREATE_STATE(tsk_wifi_idle_en, tsk_wifi_idle_run, tsk_wifi_idle_exit, NULL, NULL),
    [RUN_CONN] = SMF_CREATE_STATE(tsk_wifi_conn_en, tsk_wifi_conn_run, tsk_wifi_conn_exit, NULL, NULL),
};

/* User defined object */
struct wifi_obj_t {
        /* This must be first */
        struct smf_ctx ctx;
        tsk_wifi* wifi_oop; //!< Pointer to the task MQTT object
        /* Other state specific data add here */
} wifi_obj;

// ---------- (10) STATIC DATA ---------------------------------------------------------------------- //
struct net_if *tsk_wifi::sta_iface                  = nullptr;
struct wifi_connect_req_params tsk_wifi::sta_config = {};
struct net_mgmt_event_callback tsk_wifi::cb         = {};
struct que_msg_t tsk_wifi::in_msg                   = {};
struct que_msg_t tsk_wifi::out_msg                  = {};
bool   tsk_wifi::wifi_connected                     = false;
bool   tsk_wifi::tsk_wifi_running                   = true;
int    tsk_wifi::ret_wifi                           = -1;
int    tsk_wifi::ret_conn                           = -1;

K_WORK_DEFINE(thirty_sec_wrk, tsk_wifi::tsk_dat_acq_req_data_in_thirty_sec);
K_TIMER_DEFINE(thirty_sec_timer, tsk_wifi::timer_handler_thirty_sec, NULL);
// ---------- (12) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (13) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
void tsk_wifi_entry(void* param1, void* param2, void* param3) {
    int rc = 0;
    tsk_wifi tsk_wifi;
    wifi_obj.wifi_oop = &tsk_wifi; //!< Initialize the user defined object
    net_mgmt_init_event_callback(&tsk_wifi.cb, tsk_wifi.wifi_event_handler, NET_EVENT_WIFI_MASK);
    net_mgmt_add_event_callback(&tsk_wifi.cb);
    k_timer_start(&thirty_sec_timer, K_SECONDS(10), K_SECONDS(30));
    smf_set_initial(SMF_CTX(&wifi_obj), &tsk_wifi_states[RUN_CFG]);
    printk("TSK_WIFI : Entry\n");
    while (tsk_wifi.tsk_wifi_running == true) {
        rc = tsk_utl_rcv_que(que_id_t::WIFI, tsk_wifi.in_msg, K_MSEC(1000));
        if (rc == 0) {
            switch (tsk_wifi.in_msg.evt) {
                case que_evt_t::STA_SIG :
                    printk("TSK_WIFI : Task start signal received\n");
                    break;
                case que_evt_t::NET_CON_SIG:
                    printk("TSK_WIFI : request Network connect\n");
                    break;
            default:
                tsk_wifi.in_msg.evt = que_evt_t::UNKNOWN_SIG;
                    break;
            }
        }
        else {
            tsk_wifi.in_msg.evt = que_evt_t::UNKNOWN_SIG; // Set to unknown signal if not handled
        }
        /* Set the initial state */
        rc = smf_run_state(SMF_CTX(&wifi_obj));
        if (rc) {
            break; // handle return code and terminate state machine
        }

        if (tsk_wifi.wifi_connected == true) {
            // If WiFi is connected, run the LED PWM
            tsk_wifi.led_pwm.run_green_led_pwm();
        } 
        else {
            // If WiFi is not connected, stop the LED PWM
            tsk_wifi.led_pwm.stop_green_led_pwm();
        }
    }
}

static void tsk_wifi_cfg_en(void *obj) {
    printk("TSK_WIFI : Configuration entry\n");
    // Perform configuration tasks here
}

static void tsk_wifi_cfg_run(void *obj) {
    struct wifi_obj_t *wifi_obj = (struct wifi_obj_t *)obj;
    printk("TSK_WIFI : Configuration run\n");
    // Perform configuration tasks here
    if (wifi_obj->wifi_oop->ret_wifi != 0) {
        wifi_obj->wifi_oop->ret_wifi = wifi_obj->wifi_oop->wifi_init();
    }

    smf_set_state(SMF_CTX(wifi_obj), &tsk_wifi_states[RUN_IDLE]);
}
static void tsk_wifi_cfg_exit(void *obj) {
    printk("TSK_WIFI : Configuration exit\n");
    // Perform cleanup tasks here
}

static void tsk_wifi_idle_en(void *obj) {
    printk("TSK_WIFI : Idle entry\n");
    // Perform idle tasks here
}

static void tsk_wifi_idle_run(void *obj) {
    struct wifi_obj_t *wifi_obj = (struct wifi_obj_t *)obj;
    printk("TSK_WIFI : Idle run\n");
    // Perform idle tasks here
    switch (wifi_obj->wifi_oop->in_msg.evt) {
        case que_evt_t::NET_CON_SIG :
            smf_set_state(SMF_CTX(wifi_obj), &tsk_wifi_states[RUN_CONN]);
            break;
        default:
            break;
    }
}

static void tsk_wifi_idle_exit(void *obj) {
    printk("TSK_WIFI : Idle exit\n");
    // Perform cleanup tasks here
}

static void tsk_wifi_conn_en(void *obj) {
    printk("TSK_WIFI : Connection entry\n");
    // Perform connection tasks here
}

static void tsk_wifi_conn_run(void *obj) {
    struct wifi_obj_t *wifi_obj = (struct wifi_obj_t *)obj;
    printk("TSK_WIFI : Connection run\n");
    // Perform connection tasks here
    int rc = wifi_obj->wifi_oop->connect_to_wifi();
    if (rc == 0) {
        smf_set_state(SMF_CTX(wifi_obj), &tsk_wifi_states[RUN_IDLE]);
    } else {
        printk("TSK_WIFI : Failed to connect to WiFi.\n");
        smf_set_state(SMF_CTX(wifi_obj), &tsk_wifi_states[RUN_CFG]);
    }
}

static void tsk_wifi_conn_exit(void *obj) {
    printk("TSK_WIFI : Connection exit\n");
    // Perform cleanup tasks here
}

void tsk_wifi::timer_handler_thirty_sec(struct k_timer *dummy) {
    k_work_submit(&thirty_sec_wrk);
}

void tsk_wifi::tsk_dat_acq_req_data_in_thirty_sec(struct k_work *work) {
    if (wifi_connected != true) {
        thumbnail_msg(tsk_wifi::out_msg, que_id_t::WIFI);
    } 
}

void tsk_wifi::thumbnail_msg(que_msg_t& msg_wifi, que_id_t tsk_dest) {
    // Prepare the output message to send
    out_msg.evt    = que_evt_t::NET_CON_SIG;
    out_msg.res_id = que_id_t::WIFI;
    out_msg.ret    = que_ret_t::REQ_OK;
    tsk_utl_snd_que(tsk_dest, out_msg, K_MSEC(100));
}

void tsk_wifi::wifi_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface) {

	switch (mgmt_event) {
        case NET_EVENT_WIFI_CONNECT_RESULT: {
            wifi_connected = true;
            printk("Connected to %s\n", WIFI_SSID);
            thumbnail_msg(tsk_wifi::out_msg, que_id_t::MQTT);
            break;
        }
        case NET_EVENT_WIFI_DISCONNECT_RESULT: {
            wifi_connected = false;
            printk("Disconnected from %s\n", WIFI_SSID);
            thumbnail_msg(tsk_wifi::out_msg, que_id_t::WIFI);
            break;
        }
        default:
            break;
	}
}

int tsk_wifi::wifi_init(void) {
    // Initialize the WiFi interface
    sta_iface = net_if_get_wifi_sta();
    if (!sta_iface) {
        printk("No default network interface found.\n");
        return -EIO;
    }

    // Set up the WiFi configuration
	sta_config.ssid        = (const uint8_t *)WIFI_SSID;
	sta_config.ssid_length = strlen(WIFI_SSID);
	sta_config.psk         = (const uint8_t *)WIFI_PSK;
	sta_config.psk_length  = strlen(WIFI_PSK);
	sta_config.security    = WIFI_SECURITY_TYPE_PSK;
	sta_config.channel     = WIFI_CHANNEL_ANY;
	sta_config.band        = WIFI_FREQ_BAND_2_4_GHZ;
    
    return 0;
}

int tsk_wifi::connect_to_wifi(void) {
	ret_conn = net_mgmt(NET_REQUEST_WIFI_CONNECT, sta_iface, &sta_config,
			   sizeof(struct wifi_connect_req_params));
	if (ret_conn) {
        wifi_connected = false;
		printk("Unable to Connect to (%s)\n", WIFI_SSID);
	}
    

	return ret_conn;
}

// ---------- (14) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
// SOURCE FORMAT REVISION B
#endif // TSK_WIFI_EN
