//! @file drv_hci_spi.h
//! @brief HCI_SPI driver header file
// --------------------------------------------------------------------------------------------------- //

#ifndef DRV_HCI_SPI_H
#define DRV_HCI_SPI_H

// ---------- (1) SYSTEM INCLUDE --------------------------------------------------------------------- //
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include "tsk_cfg.hpp"
#if (DRV_BT_HCI_SPI_EN== TSK_ENABLED)

// ---------- (2) EXTERNAL MODULE INCLUDE ------------------------------------------------------------ //
// N/A

// ---------- (3) PUBLIC PROGRAMMING DEFINE ---------------------------------------------------------- //
#define HCI_CMD                0x01
#define HCI_ACL                0x02
#define HCI_SCO                0x03
#define HCI_EVT                0x04
/* Special Values */
#define SPI_WRITE              0x0A
#define SPI_READ               0x0B
#define READY_NOW              0x02
#define SANITY_CHECK           0x02
/* Offsets */
#define STATUS_HEADER_READY    0
#define STATUS_HEADER_TOREAD   3
#define PACKET_TYPE            0
#define EVT_BLUE_INITIALIZED   0x01
/* Needs to be aligned with the SPI master buffer size */
#define SPI_MAX_MSG_LEN        255

// ---------- (4) ENUMERATOR DEFINITION -------------------------------------------------------------- //
// N/A

// ---------- (5) STRUCT OR UNION DATA TYPE DEFINITION ----------------------------------------------- //
// N/A

// ---------- (6) TYPEDEF DATA TYPE DEFINITION ------------------------------------------------------- //
// N/A

// ---------- (7) PUBLIC MACRO DEFINITION ------------------------------------------------------------ //
// N/A

// ---------- (8) EXTERN FUNCTION -------------------------------------------------------------------- //
// N/A

// ---------- (9) EXTERN VARIABLE -------------------------------------------------------------------- //
// N/A

// ---------- (10) CLASS DECLARATION ----------------------------------------------------------------- //
class HCI_SPI {
public:
    HCI_SPI(struct device const* const blt_hci_spi_dev, const struct gpio_dt_spec irq) : _blt_hci_spi_dev(blt_hci_spi_dev), _irq(irq) {
        // pass
    }
    static struct spi_config  spi_cfg;
    static struct spi_buf     tx;
    static struct spi_buf     rx;
    static struct spi_buf_set tx_bufs;
    static struct spi_buf_set rx_bufs;
    static uint8_t rxmsg[SPI_MAX_MSG_LEN];
    static uint8_t txmsg[SPI_MAX_MSG_LEN];
    uint8_t header_master[5] = { 0 };
    uint8_t header_slave[5]  = { READY_NOW, SANITY_CHECK, 0x00, 0x00, 0x00 };
	struct net_buf *buf      = NULL;
	union {
		struct bt_hci_cmd_hdr *cmd_hdr;
		struct bt_hci_acl_hdr *acl_hdr;
	} hci_hdr;
    
    int ret;
    int init();
    void bt_tx();
    inline int spi_send(struct net_buf *buf);
private:
    struct device const* const _blt_hci_spi_dev;
    const struct gpio_dt_spec _irq;
};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #if (TSK_BT_LE_EN == STD_ENABLE)
#endif // #ifndef DRV_HCI_SPI_H
