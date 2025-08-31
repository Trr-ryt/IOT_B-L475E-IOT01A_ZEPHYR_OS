//! @file cpp_inc_template.h
//! @brief C++ Header File Template
// --------------------------------------------------------------------------------------------------- //

#ifndef TSK_ENV_SNS_H
#define TSK_ENV_SNS_H

// ---------- (1) SYSTEM INCLUDE --------------------------------------------------------------------- //
#include <zephyr/smf.h>

// ---------- (2) EXTERNAL MODULE INCLUDE ------------------------------------------------------------ //
#include "drv/drv_hts221.hpp"
#include "drv/drv_lps22hb.hpp"
#include "drv/drv_vl53l0x.hpp"
#include "drv/drv_lsm6dsl.hpp"
#include "drv/drv_lis3dl.hpp"

// ---------- (3) PUBLIC PROGRAMMING DEFINE ---------------------------------------------------------- //
// N/A

// ---------- (4) ENUMERATOR DEFINITION -------------------------------------------------------------- //
// N/A

// ---------- (5) STRUCT OR UNION DATA TYPE DEFINITION ----------------------------------------------- //
// N/A

// ---------- (6) TYPEDEF DATA TYPE DEFINITION ------------------------------------------------------- //
// N/A

// ---------- (7) PUBLIC MACRO DEFINITION ------------------------------------------------------------ //
// N/A

// ---------- (8) EXTERN FUNCTION -------------------------------------------------------------------- //
void tsk_env_sns_entry(void* param1, void* param2, void* param3);

// ---------- (9) EXTERN VARIABLE -------------------------------------------------------------------- //
// N/A

// ---------- (10) CLASS DECLARATION ----------------------------------------------------------------- //
class tsk_env_sns {
    private:
        /* data */
    public:
        static int  ret_dev;
        static int  hts221_sts;
        static int  lps22hb_sts;
        static int  vl53l0x_sts;
        static int  lsm6dsl_sts;
        static int  lis3dl_sts;
        static bool tsk_env_sns_running;
        static const struct smf_state state_machine[]; // State machine states
        enum env_sns_state {RUN_CFG, RUN_IDLE, RUN_FETCH, RUN_REPORT};
        static struct que_msg_t input_msg, output_msg;
        HTS221    tsk_hts221_drv;  //!< HTS221 sensor driver instance
        LPS22HB   tsk_lps22hb_drv; //!< LPS22HB sensor driver instance
        VL53L0X   tsk_vl53l0x_drv; //!< VL53L0X sensor driver instance
        LSM6DSL   tsk_lsm6dsl_drv; //!< LSM6DSL sensor driver instance
        LIS3DL    tsk_lis3dl_drv;  //!< LIS3DL sensor driver instance
        tsk_env_sns() : tsk_hts221_drv(hts221_sns_dev),
                        tsk_lps22hb_drv(lps22hb_sns_dev),
                        tsk_vl53l0x_drv(vl53l0x_sns_dev),
                        tsk_lsm6dsl_drv(lsm6dsl_sns_dev),
                        tsk_lis3dl_drv(lis3dl_sns_dev) {}
        inline void thumbnail_msg();
        int  devices_init();
        static int fetch_data(tsk_env_sns& dev_drv);

        static void tsk_env_cfg_en(void *obj);
        static enum smf_state_result tsk_env_cfg_run(void *obj);
        static void tsk_env_cfg_exit(void *obj);

        static void tsk_env_idle_en(void *obj);
        static enum smf_state_result tsk_env_idle_run(void *obj);
        static void tsk_env_idle_exit(void *obj);

        static void tsk_env_fetch_en(void *obj);
        static enum smf_state_result tsk_env_fetch_run(void *obj);
        static void tsk_env_fetch_exit(void *obj);

        static void tsk_env_report_en(void *obj);
        static enum smf_state_result tsk_env_report_run(void *obj);
        static void tsk_env_report_exit(void *obj);
};

// ---------- (11) END OF CLASS DECLARATION ---------------------------------------------------------- //
// HEADER FORMAT REVISION A
#endif // #ifndef TSK_ENV_SNS_H
