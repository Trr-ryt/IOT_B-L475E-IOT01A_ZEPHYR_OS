//! @file drv_vl53l0x.cpp
//! @brief VL53L0X driver source file
// --------------------------------------------------------------------------------------------------- //

// ---------- (1) LOCAL PREDEFINE -------------------------------------------------------------------- //
// N/A

// ---------- (2) STANDARD LIBRARY INCLUDE ----------------------------------------------------------- //
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#include <zephyr/net_buf.h>
// ---------- (3) USER INCLUDE ----------------------------------------------------------------------- //
#include "drv/drv_hci_spi.hpp"
#include "utl/tsk_utl.hpp"
#include "tsk_cfg.hpp"
#if (DRV_BT_HCI_SPI_EN == TSK_ENABLED)
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/l2cap.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci_raw.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/debug/stack.h>
#include <zephyr/net_buf.h>
// ---------- (4) EXTERN OBJECT ---------------------------------------------------------------------- //
// N/A

// ---------- (5) PUBLIC INTERFACE FUNCTION ---------------------------------------------------------- //
// N/A

// ---------- (6) PUBLIC DATA ------------------------------------------------------------------------ //
struct spi_buf     HCI_SPI::tx      = { 0 };
struct spi_buf     HCI_SPI::rx      = { 0 };
struct spi_config  HCI_SPI::spi_cfg = { .operation = SPI_WORD_SET(8) | SPI_OP_MODE_SLAVE, };
struct spi_buf_set HCI_SPI::tx_bufs = { .buffers = &HCI_SPI::tx, .count = 1 };
struct spi_buf_set HCI_SPI::rx_bufs = { .buffers = &HCI_SPI::rx, .count = 1 };
uint8_t HCI_SPI::rxmsg[SPI_MAX_MSG_LEN] = { 0 };
uint8_t HCI_SPI::txmsg[SPI_MAX_MSG_LEN] = { 0 };


// ---------- (7) PRIVATE PROGRAMMING DEFINE --------------------------------------------------------- //

// ---------- (8) PRIVATE TYPEDEF -------------------------------------------------------------------- //
// N/A

// ---------- (9) PRIVATE FUNCTION ------------------------------------------------------------------- //
static K_SEM_DEFINE(sem_spi_rx, 0, 1);
static K_SEM_DEFINE(sem_spi_tx, 0, 1);

// ---------- (10) PRIVATE DATA ---------------------------------------------------------------------- //
// N/A

// ---------- (11) PRIVATE MACRO DEFINITION ---------------------------------------------------------- //
// N/A

// ---------- (12) SOURCE FILE IMPLEMENTATION -------------------------------------------------------- //
int HCI_SPI::init() {

	if (!device_is_ready(_blt_hci_spi_dev)) {
		printk("DRV_HCI_SPI :SPI bus %s is not ready", _blt_hci_spi_dev->name);
		ret = -EINVAL;
        return ret;
	}

	if (!gpio_is_ready_dt(&_irq)) {
		printk("DRV_HCI_SPI :IRQ GPIO port %s is not ready", _irq.port->name);
		ret = -EINVAL;
        return ret;
	}

	gpio_pin_configure_dt(&_irq, GPIO_OUTPUT_INACTIVE);

	hci_hdr.cmd_hdr = (struct bt_hci_cmd_hdr *)&rxmsg[1];

	(void)memset(txmsg, 0xFF, SPI_MAX_MSG_LEN);

	return 0;
}

void HCI_SPI::bt_tx() {

	tx.buf = header_slave;
	tx.len = 5;
	rx.buf = header_master;
	rx.len = 5;

	do {
		ret = spi_transceive(_blt_hci_spi_dev, &spi_cfg,
						&tx_bufs, &rx_bufs);
		if (ret < 0) {
			printk("DRV_HCI_SPI :SPI transceive error: %d\n", ret);
		}
	} while ((header_master[STATUS_HEADER_READY] != SPI_READ) &&
			(header_master[STATUS_HEADER_READY]  != SPI_WRITE));

	if (header_master[STATUS_HEADER_READY] == SPI_READ) {
		/* Unblock the spi tx thread and wait for it */
		k_sem_give(&sem_spi_tx);
		k_sem_take(&sem_spi_rx, K_FOREVER);
		return;
	}

	tx.buf = txmsg;
	tx.len = SPI_MAX_MSG_LEN;
	rx.buf = rxmsg;
	rx.len = SPI_MAX_MSG_LEN;

	/* Receiving data from the SPI Host */
	ret = spi_transceive(_blt_hci_spi_dev, &spi_cfg,
					&tx_bufs, &rx_bufs);
	if (ret < 0) {
		printk("DRV_HCI_SPI :SPI transceive error: %d\n", ret);
		return;
	}

	switch (rxmsg[PACKET_TYPE]) {
		case HCI_CMD:
			buf = bt_buf_get_tx(BT_BUF_CMD, K_NO_WAIT,
						hci_hdr.cmd_hdr,
						sizeof(*hci_hdr.cmd_hdr));
			if (buf) {
				net_buf_add_mem(buf, &rxmsg[4],
						hci_hdr.cmd_hdr->param_len);
			} 
			else {
				printk("DRV_HCI_SPI :No available command buffers!\n");
				return;
			}
			break;
		case HCI_ACL:
			buf = bt_buf_get_tx(BT_BUF_ACL_OUT, K_NO_WAIT,
						hci_hdr.acl_hdr,
						sizeof(*hci_hdr.acl_hdr));
			if (buf) {
				net_buf_add_mem(buf, &rxmsg[5],
						sys_le16_to_cpu(hci_hdr.acl_hdr->len));
			} 
			else {
				printk("DRV_HCI_SPI :No available ACL buffers!\n");
				return;
			}
			break;
		default:
			printk("Unknown BT HCI buf type\n");
			return;
	}

	printk("DRV_HCI_SPI :buf %p type %u len %u\n", buf, buf->data[0], buf->len);

	ret = bt_send(buf);
	if (ret) {
		printk("DRV_HCI_SPI :Unable to send (ret %d)\n", ret);
		net_buf_unref(buf);
	}

	/* Make sure other threads get a chance to run */
	k_yield();
}

int HCI_SPI::spi_send(struct net_buf *buf) {

	if (buf->len > SPI_MAX_MSG_LEN) {
		printk("DRV_HCI_SPI :TX message too long\n");
		net_buf_unref(buf);
		return -EINVAL;
	}
	header_slave[STATUS_HEADER_TOREAD] = buf->len;

	gpio_pin_set(_irq.port, _irq.pin, 1);

	/* Coordinate transfer lock with the spi rx thread */
	k_sem_take(&sem_spi_tx, K_FOREVER);

	tx.buf = header_slave;
	tx.len = 5;
	rx.buf = header_master;
	rx.len = 5;
	do {
		ret = spi_transceive(_blt_hci_spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);
		if (ret < 0) {
			printk("DRV_HCI_SPI : transceive error: %d", ret);
		}
	} while (header_master[STATUS_HEADER_READY] != SPI_READ);

	tx.buf = buf->data;
	tx.len = buf->len;

	ret = spi_write(_blt_hci_spi_dev, &spi_cfg, &tx_bufs);
	if (ret < 0) {
		printk("DRV_HCI_SPI : transceive error: %d", ret);
	}
	net_buf_unref(buf);

	gpio_pin_set(_irq.port, _irq.pin, 0);
	k_sem_give(&sem_spi_rx);

	return 0;
}
// ---------- (13) END OF SOURCE FILE IMPLEMENTATION ------------------------------------------------- //
#endif //(TSK_BT_LE_EN == STD_ENABLE)
