//! @file drv_vl53l0x.cpp
//! @brief VL53L0X driver source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
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
#include <zephyr/sys/util.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "drv/drv_st_bt_sns.hpp"
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
// N/A

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
DRV_GPIO   ST_BT_SNS::drv_gpio(green_pwm_led0, button, led0);
struct env_dat_t ST_BT_SNS::_env_dat   = {};
int        ST_BT_SNS::err              = 0;
uint16_t   ST_BT_SNS::but_val          = 0;
volatile bool ST_BT_SNS::notify_enable = false;
struct bt_conn *ST_BT_SNS::ble_conn    = NULL;

// --- Bluetooth UUIDs ---
const struct bt_uuid_128 ST_BT_SNS::st_service_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe40, 0xcc7a, 0x482a, 0x984a, 0x7f2ed5b3e58f));
const struct bt_uuid_128 ST_BT_SNS::led_char_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe41, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));
const struct bt_uuid_128 ST_BT_SNS::but_notif_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0000fe42, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));
// --- Humidity Sensor UUID ---
const struct bt_uuid_128 ST_BT_SNS::hum_sns_uuid =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x00080000, 0x0001, 0x11e1, 0xac36, 0x0002a5d5c51b));
// --- Temperature Sensor UUID ---
const struct bt_uuid_128 ST_BT_SNS::temp_sns_uuid =
 BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x00040000,0x0001,0x11e1,0xac36,0x0002a5d5c51b));

// --- Manufacturer Data (for BLE advertising) ---
uint8_t ST_BT_SNS::manuf_data[ADV_LEN] = {
    0x01, // SKD version
    0x83, // STM32WB - P2P Server 1
    0x00, // GROUP A Feature
    0x00, // GROUP A Feature
    0x00, // GROUP B Feature
    0x00, // GROUP B Feature
    0x00, // BLE MAC start - MSB
    0x00,
    0x00,
    0x00,
    0x00,
    0x00  // BLE MAC stop
};

const struct bt_data ST_BT_SNS::ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, ST_BT_SNS::manuf_data, ADV_LEN)
};

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected    = ST_BT_SNS::connected,
	.disconnected = ST_BT_SNS::disconnected,
};
/* ST BLE Sensor GATT services and characteristic */
BT_GATT_SERVICE_DEFINE(stsensor_svc, BT_GATT_PRIMARY_SERVICE(&ST_BT_SNS::st_service_uuid),
BT_GATT_CHARACTERISTIC(&ST_BT_SNS::led_char_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_WRITE, NULL, ST_BT_SNS::recv, (void *)1),
BT_GATT_CHARACTERISTIC(&ST_BT_SNS::but_notif_uuid.uuid, BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ, NULL, NULL, &ST_BT_SNS::but_val),
BT_GATT_DESCRIPTOR(&ST_BT_SNS::hum_sns_uuid.uuid, BT_GATT_PERM_READ, NULL, NULL, (void *)&ST_BT_SNS::_env_dat.humi_val),
BT_GATT_DESCRIPTOR(&ST_BT_SNS::temp_sns_uuid.uuid, BT_GATT_PERM_READ, NULL, NULL, (void *)&ST_BT_SNS::_env_dat.temp_val),
BT_GATT_CCC(ST_BT_SNS::mpu_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
int ST_BT_SNS:: bt_st_report_data() {
	if (ble_conn) {
		if (notify_enable) {
			err = bt_gatt_notify(NULL, &stsensor_svc.attrs[5],
					     &_env_dat.humi_val, sizeof(_env_dat.humi_val));
			if (err) {
				printk("DRV_ST_BT_SNS : Notify error: %d\n", err);
			} 
			else {
				printk("DRV_ST_BT_SNS : Send humidity notify ok\n");
			}
			
			err = bt_gatt_notify(NULL, &stsensor_svc.attrs[6],
					     &_env_dat.temp_val, sizeof(_env_dat.temp_val));
			if (err) {
				printk("DRV_ST_BT_SNS : Notify error: %d\n", err);
			} 
			else {
				printk("DRV_ST_BT_SNS : Send temperature notify ok\n");
			}
		} 
		else {
			printk("DRV_ST_BT_SNS : Notify not enabled\n");
		}
	} 
	else {
		printk("DRV_ST_BT_SNS : BLE not connected\n");
	}
	return 0;
}
void ST_BT_SNS:: button_callback(const struct device *gpiob, struct gpio_callback *cb, uint32_t pins) {

	printk("Button pressed\n");
	if (ble_conn) {
		if (notify_enable) {
			err = bt_gatt_notify(NULL, &stsensor_svc.attrs[4],
					     &but_val, sizeof(but_val));
			if (err) {
				printk("DRV_ST_BT_SNS : Notify error: %d\n", err);
			} 
			else {
				printk("DRV_ST_BT_SNS : Send notify ok\n");
				but_val = (but_val == 0) ? 0x100 : 0;
			}
		} 
		else {
			printk("DRV_ST_BT_SNS : Notify not enabled\n");
		}
	} 
	else {
		printk("DRV_ST_BT_SNS : BLE not connected\n");
	}
}

void ST_BT_SNS:: mpu_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
	
	ARG_UNUSED(attr);
	notify_enable = (value == BT_GATT_CCC_NOTIFY);
	printk("DRV_ST_BT_SNS : Notification %s\n", notify_enable ? "enabled" : "disabled");
}

void ST_BT_SNS:: bt_ready(int err) {

	if (err) {
		printk("DRV_ST_BT_SNS : Bluetooth init failed (err %d)\n", err);
		return;
	}
	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("DRV_ST_BT_SNS : Advertising failed to start (err %d)\n", err);
		return;
	}
}

ssize_t ST_BT_SNS::recv(struct bt_conn *conn, const struct bt_gatt_attr *attr, 
						const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
	drv_gpio.led0_update();

	return 0;
}

void ST_BT_SNS::connected(struct bt_conn *connected, uint8_t err) {
	if (err) {
		printk("DRV_ST_BT_SNS : Connection failed (err %u)\n", err);
	} 
    else {
		printk("DRV_ST_BT_SNS : Connected\n");
		if (!ble_conn) {
			ble_conn = bt_conn_ref(connected);
		}
	}
}

void ST_BT_SNS::disconnected(struct bt_conn *disconn, uint8_t reason) {

	if (ble_conn) {
		bt_conn_unref(ble_conn);
		ble_conn = NULL;
	}

	printk("DRV_ST_BT_SNS : Disconnected, reason %u %s\n", reason, bt_hci_err_to_str(reason));
}

int ST_BT_SNS::st_bt_sns_init(void) {

	err = drv_gpio.button_init(button_callback);
	if (err) {
		return err;
	}

	err = drv_gpio.led0_init();
	if (err) {
		return err;
	}
	
    /* Initialize Bluetooth */
    err = bt_enable(bt_ready);
    if (err) {
        printk("DRV_ST_BT_SNS : Bluetooth enable failed (err %d)\n", err);
        return err;
    }

    return 0;
}
// ------------ (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //