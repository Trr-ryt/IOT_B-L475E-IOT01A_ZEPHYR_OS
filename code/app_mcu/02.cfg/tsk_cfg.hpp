//! @file tsk_cfg.hpp
//! @brief Task Configuration
// --------------------------------------------------------------------------------------------------- //

#ifndef TSK_CFG_H
#define TSK_CFG_H

// ---------- (1) SYSTEM INCLUDE --------------------------------------------------------------------- //
#include <zephyr/portability/cmsis_os2.h>

// ---------- (2) EXTERNAL MODULE INCLUDE ------------------------------------------------------------ //

// ---------- (3) PUBLIC PROGRAMMING DEFINE ---------------------------------------------------------- //
#define TSK_ENABLED                   (0x01U)
#define TSK_DISABLED                  (0x00U)

#define CONFIG_TASK_START_ENABLE      (TSK_ENABLED)
#define TSK_WDT_EN                    (TSK_DISABLED)
#define TSK_SYS_EN                    (TSK_ENABLED)
#define TSK_ENV_SNS_EN                (TSK_ENABLED)
#define TSK_DAT_ACQ_EN                (TSK_ENABLED)
#define TSK_MQTT_EN                   (TSK_DISABLED)
#define TSK_OTA_EN                    (TSK_DISABLED)
#define TSK_WIFI_EN                   (TSK_DISABLED)
#define TSK_BT_LE_EN                  (TSK_DISABLED)

#define DRV_BT_HCI_SPI_EN             (TSK_DISABLED)
#define TSK_SYS_PRI                   (osPriorityNormal)
#define TSK_SYS_Q_LEN                 (4U)
#define TSK_SYS_STACK_SIZE            (2048U)

#define TSK_DAT_ACQ_PRI               (osPriorityNormal)
#define TSK_DAT_ACQ_Q_LEN             (4U)
#define TSK_DAT_ACQ_STACK_SIZE        (2048U)

#if (TSK_ENV_SNS_EN == TSK_ENABLED)
#define TSK_ENV_SNS_PRI               (osPriorityNormal)
#define TSK_ENV_SNS_Q_LEN             (4U)
#define TSK_ENV_SNS_STACK_SIZE        (2048U)
#else
#define TSK_ENV_SNS_PRI               (osPriorityIdle)
#define TSK_ENV_SNS_Q_LEN             (0U)
#define TSK_ENV_SNS_STACK_SIZE        (0U)
#endif // TSK_ENV_SNS_EN

#if (TSK_MQTT_EN == TSK_ENABLED)
#define TSK_MQTT_PRI                  (osPriorityNormal)
#define TSK_MQTT_Q_LEN                (4U)
#define TSK_MQTT_STACK_SIZE           (2048U)
#else
#define TSK_MQTT_PRI                  (osPriorityIdle)
#define TSK_MQTT_Q_LEN                (0U)
#define TSK_MQTT_STACK_SIZE           (0U)
#endif // TSK_MQTT_EN

#if (TSK_OTA_EN == TSK_ENABLED)
#define TSK_OTA_PRI                   (osPriorityRealtime)
#define TSK_OTA_Q_LEN                 (4U)
#define TSK_OTA_STACK_SIZE            (2048U)
#else
#define TSK_OTA_PRI                   (osPriorityIdle)
#define TSK_OTA_Q_LEN                 (0U)
#define TSK_OTA_STACK_SIZE            (0U)
#endif // TSK_OTA_EN

#if (TSK_BT_LE_EN == TSK_ENABLED)
#define TSK_BT_LE_PRI                 (osPriorityNormal)
#define TSK_BT_LE_Q_LEN               (4U)
#define TSK_BT_LE_STACK_SIZE          (2048U)
#else
#define TSK_BT_LE_PRI                 (osPriorityIdle)
#define TSK_BT_LE_Q_LEN               (0U)
#define TSK_BT_LE_STACK_SIZE          (0U)
#endif // TSK_BT_LE_EN

#if (TSK_WIFI_EN == TSK_ENABLED)
#define TSK_WIFI_PRI                  (osPriorityNormal)
#define TSK_WIFI_Q_LEN                (4U)
#define TSK_WIFI_STACK_SIZE           (2048U)
#else
#define TSK_WIFI_PRI                  (osPriorityIdle)
#define TSK_WIFI_Q_LEN                (0U)
#define TSK_WIFI_STACK_SIZE           (0U)
#endif // TSK_WIFI_EN


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
// N/A

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef TSK_CFG_H
