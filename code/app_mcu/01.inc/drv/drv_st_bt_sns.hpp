//! @file drv_st_bt_sns.hpp
//! @brief ST_BT_SNS driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_ST_BT_SNS_H
#define DRV_ST_BT_SNS_H

// ---------- (1) SYSTEM INCLUDE --------------------------------------------------------------------- //
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
// ---------- (2) EXTERNAL MODULE INCLUDE ------------------------------------------------------------ //
#include "drv/drv_gpio.hpp"
#include "dt/dt_cfg.hpp"
#include "utl/tsk_utl.hpp"
// ---------- (3) PUBLIC PROGRAMMING DEFINE ---------------------------------------------------------- //
// N/A

// ---------- (4) ENUMERATOR DEFINITION -------------------------------------------------------------- //
// N/A

// ---------- (5) STRUCT OR UNION DATA TYPE DEFINITION ----------------------------------------------- //
// N/A

// ---------- (6) TYPEDEF DATA TYPE DEFINITION ------------------------------------------------------- //
// N/A

// ---------- (7) PUBLIC MACRO DEFINITION ------------------------------------------------------------ //
#define DEVICE_NAME "TRR_L475_IOT"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define ADV_LEN 12

// ---------- (8) EXTERN FUNCTION -------------------------------------------------------------------- //
// N/A

// ---------- (9) EXTERN VARIABLE -------------------------------------------------------------------- //
// N/A

// ---------- (10) CLASS DECLARATION ----------------------------------------------------------------- //
class ST_BT_SNS {
public:
    ST_BT_SNS(struct env_dat_t *env_dat) {
        _env_dat = *env_dat;
    }
    static DRV_GPIO drv_gpio;
    static int err;
    static struct bt_conn *ble_conn;
    static volatile bool notify_enable;
    static uint16_t but_val;
    static const struct bt_uuid_128 st_service_uuid;
    static const struct bt_uuid_128 led_char_uuid;
    static const struct bt_uuid_128 but_notif_uuid;
    static const struct bt_uuid_128 hum_sns_uuid;
    static const struct bt_uuid_128 temp_sns_uuid;
    static uint8_t manuf_data[ADV_LEN];
    static const struct bt_data ad[];
    static struct env_dat_t _env_dat; //!< Static environment data
    static ssize_t recv(struct bt_conn *conn, const struct bt_gatt_attr *attr, 
                            const void *buf, uint16_t len, uint16_t offset, uint8_t flags);
    static void connected(struct bt_conn *conn, uint8_t err);
    static void disconnected(struct bt_conn *conn, uint8_t reason);
    static void bt_ready(int err);
    static void mpu_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);
    static void button_callback(const struct device *gpiob, struct gpio_callback *cb, uint32_t pins);
    static int st_bt_sns_init();
    int bt_st_report_data();
};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef DRV_ST_BT_SNS_H
