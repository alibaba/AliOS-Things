/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/





#ifndef __SAP_CFG_H__
#define __SAP_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"
#include "sxs_cfg.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "cmn_defs.h"

#ifdef __SAP_CFG_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

/******************************************************************************
       OS TASKS CONFIGURATION
******************************************************************************/
extern const sxs_TaskParse_t l1_TaskParse;
extern const sxs_TaskParse_t sim_TaskParse;
extern const sxs_TaskParse_t lap_TaskParse;
extern const sxs_TaskParse_t rr_TaskParse;
extern const sxs_TaskParse_t rlu_TaskParse;
extern const sxs_TaskParse_t rld_TaskParse;
extern const sxs_TaskParse_t llc_TaskParse;
extern const sxs_TaskParse_t mm_TaskParse;
extern const sxs_TaskParse_t cc_TaskParse;
extern const sxs_TaskParse_t ss_TaskParse;
extern const sxs_TaskParse_t sm_TaskParse;
extern const sxs_TaskParse_t sms_TaskParse;
#ifdef CB_SUPPORT
extern const sxs_TaskParse_t cb_TaskParse;
#endif
extern const sxs_TaskParse_t snd_TaskParse;
extern const sxs_TaskParse_t m2a_TaskParse;
extern const sxs_TaskParse_t api_TaskParse;
extern const sxs_TaskParse_t cfw_TaskParse ;
#ifdef LTE_NBIOT_SUPPORT
extern const sxs_TaskParse_t osi_Timer_Parse;
extern void osi_Timer_Task(sxs_TaskParse_t *TaskParse);
#endif
//extern const sxs_TaskParse_t rlp_TaskParse;
#ifdef CHIP_HAS_WCPU
extern const sxs_TaskParse_t interrat_TaskParse;
#endif

extern void sxs_TaskIdle (void);
#ifdef LTE_NBIOT_SUPPORT
extern void sxs_NBPSMTask (void);
#endif

#ifdef __DUALSIM__

extern const sxs_TaskParse_t mssc_TaskParse;
extern const sxs_TaskParse_t rco_TaskParse;

#endif //__DUALSIM__



#define L1_TASK_STACK_SIZE   3*128
#define L1_TASK_PRIORITY     1

#ifdef CHIP_HAS_WCPU
#define INTERRAT_TASK_STACK_SIZE 3*128
#define INTERRAT_TASK_PRIORITY 3
#endif

#ifdef LTE_NBIOT_SUPPORT
#define SIM_TASK_STACK_SIZE  1024
#define SIM_TASK_PRIORITY    23
#else
#define SIM_TASK_STACK_SIZE  200
#define SIM_TASK_PRIORITY    15
#endif

#define LAP_TASK_STACK_SIZE  200
#define LAP_TASK_PRIORITY    21
/*
#define RLP_TASK_STACK_SIZE  2*128
#define RLP_TASK_PRIORITY    23
*/
#define RR_TASK_STACK_SIZE   8*128    //6*128 before
#define RR_TASK_PRIORITY     25

#define RLD_TASK_STACK_SIZE  200
#define RLD_TASK_PRIORITY    26

#ifdef __DUALSIM__

#define RCO_TASK_STACK_SIZE   300
#define RCO_TASK_PRIORITY     26

#endif /* __DUALSIM__  */



#define RLU_TASK_STACK_SIZE  3*128
#define RLU_TASK_PRIORITY    27

#define LLC_TASK_STACK_SIZE  2*128
#define LLC_TASK_PRIORITY    29


#define MM_TASK_STACK_SIZE   500
#define MM_TASK_PRIORITY     30

#ifdef __DUALSIM__

#define MSSC_TASK_STACK_SIZE   300
#define MSSC_TASK_PRIORITY     31//24

#endif /* __DUALSIM__  */



#define CC_TASK_STACK_SIZE   3*128
#define CC_TASK_PRIORITY     35

#define SS_TASK_STACK_SIZE   2*128
#define SS_TASK_PRIORITY     40

#define SMS_TASK_STACK_SIZE  2*128
#define SMS_TASK_PRIORITY    45

#ifdef CB_SUPPORT
#define CB_TASK_STACK_SIZE   2*128
#define CB_TASK_PRIORITY     50
#endif

#define SM_TASK_STACK_SIZE   256
#define SM_TASK_PRIORITY     120

#define SND_TASK_STACK_SIZE 256
#define SND_TASK_PRIORITY   125

#define API_TASK_STACK_SIZE  2*128
#define API_TASK_PRIORITY    150

#define M2A_TASK_STACK_SIZE  4*128
#define M2A_TASK_PRIORITY    170

#ifdef LTE_NBIOT_SUPPORT
#define OSI_TIMER_TASK_STACK_SIZE  2048
#define OSI_TIMER_TASK_PRIORITY    16
#endif

#define IDLE_TASK_STACK_SIZE 2*1024
#define IDLE_TASK_PRIORITY   255

#define STT_TASK_STACK_SIZE  300
#define STT_TASK_PRIORITY    0

#define CFW_TASK_STACK_SIZE  1024*5
#define CFW_TASK_PRIORITY    201

#ifdef __SAP_CFG_VAR__
#ifndef __SXS_ONLY__
const sxs_TaskCfg_t sxs_TaskCfg [] =
{
#ifndef __L1_REMOTE__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "Layer 1",
            L1_TASK_STACK_SIZE,
            L1_TASK_PRIORITY
        },
        &l1_TaskParse
    },
#ifndef __L1_SIMU__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "Sim",
            SIM_TASK_STACK_SIZE,
            SIM_TASK_PRIORITY
        },
        &sim_TaskParse
    },
#endif // __L1_SIMU__
#endif // __L1_REMOTE__

#ifndef __PROTO_REMOTE__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "LAPDm",
            LAP_TASK_STACK_SIZE,
            LAP_TASK_PRIORITY
        },
        &lap_TaskParse
    },

    /*{
     {
      (void (*)(void *))sxs_Task,
      0,
      "RLP",
      RLP_TASK_STACK_SIZE,
      RLP_TASK_PRIORITY
     },
     &rlp_TaskParse
    },*/

    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "RR",
            RR_TASK_STACK_SIZE,
            RR_TASK_PRIORITY
        },
        &rr_TaskParse
    },
#ifndef __L1_SIMU__
#ifdef __GPRS__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "RLD",
            RLD_TASK_STACK_SIZE,
            RLD_TASK_PRIORITY
        },
        &rld_TaskParse
    },

    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "RLU",
            RLU_TASK_STACK_SIZE,
            RLU_TASK_PRIORITY
        },
        &rlu_TaskParse
    },

    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "LLC",
            LLC_TASK_STACK_SIZE,
            LLC_TASK_PRIORITY
        },
        &llc_TaskParse
    },
#endif // __GPRS__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "MM",
            MM_TASK_STACK_SIZE,
            MM_TASK_PRIORITY
        },
        &mm_TaskParse
    },
#ifdef __DUALSIM__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "MSSC",
            MSSC_TASK_STACK_SIZE,
            MSSC_TASK_PRIORITY
        },
        &mssc_TaskParse
    },

    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "RCO",
            RCO_TASK_STACK_SIZE,
            RCO_TASK_PRIORITY
        },
        &rco_TaskParse
    },
#endif //__DUALSIM__
#ifdef CHIP_HAS_WCPU
#ifdef __MULTI_RAT__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "InterRAT",
            INTERRAT_TASK_STACK_SIZE,
            INTERRAT_TASK_PRIORITY
        },
        &interrat_TaskParse
    },
#endif
#endif
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "CC",
            CC_TASK_STACK_SIZE,
            CC_TASK_PRIORITY
        },
        &cc_TaskParse
    },

    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "SS",
            SS_TASK_STACK_SIZE,
            SS_TASK_PRIORITY
        },
        &ss_TaskParse
    },

    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "SMS",
            SMS_TASK_STACK_SIZE,
            SMS_TASK_PRIORITY
        },
        &sms_TaskParse
    },
#ifdef CB_SUPPORT
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "CB",
            CB_TASK_STACK_SIZE,
            CB_TASK_PRIORITY
        },
        &cb_TaskParse
    },
#endif
#ifdef __GPRS__
    {
        {
            (void (*)(void *))sxs_Task,
            NIL,
            "SM",
            SM_TASK_STACK_SIZE,
            SM_TASK_PRIORITY
        },
        &sm_TaskParse
    },
    {
        {
            (void (*)(void *))sxs_Task,
            NIL,
            "SNDCP",
            SND_TASK_STACK_SIZE,
            SND_TASK_PRIORITY
        },
        &snd_TaskParse
    },
#endif // __GPRS__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "API",
            API_TASK_STACK_SIZE,
            API_TASK_PRIORITY
        },
        &api_TaskParse
    },
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "CFW",
            CFW_TASK_STACK_SIZE,
            CFW_TASK_PRIORITY
        },
        &cfw_TaskParse
    },
#ifdef __M2A__
    {
        {
            (void (*)(void *))sxs_Task,
            0,
            "MMI to API",
            M2A_TASK_STACK_SIZE,
            M2A_TASK_PRIORITY
        },
        &m2a_TaskParse
    },
#endif // __M2A__

#endif // __L1_SIMU__
#endif // __PROTO_REMOTE__
#ifdef LTE_NBIOT_SUPPORT
    {
        {
            (void (*)(void *))osi_Timer_Task,
            0,
            "OSI_TIMER",
            OSI_TIMER_TASK_STACK_SIZE,
            OSI_TIMER_TASK_PRIORITY
        },
        &osi_Timer_Parse
    },
#endif
};
#else
const sxs_TaskCfg_t *sxs_TaskCfg = 0;
#endif

const sxr_TaskDesc_t sxs_TaskIdleDesc =
{
    (void(*)(void *))sxs_TaskIdle,
    0,
    "Task Idle",
    IDLE_TASK_STACK_SIZE,
    IDLE_TASK_PRIORITY
};

#ifdef LTE_NBIOT_SUPPORT
const sxr_TaskDesc_t sxs_NBInitDesc =
{
    (void(*)(void *))sxs_NBPSMTask,
    0,
    "NB PSM",
    IDLE_TASK_STACK_SIZE,
    IDLE_TASK_PRIORITY-1
};
u8 sxs_NBInitTaskId;
#endif

#ifndef __SXS_ONLY__
const u8 sxs_NbStaticTasksDesc = sizeof (sxs_TaskCfg) / sizeof (sxs_TaskCfg_t);
#else
const u8 sxs_NbStaticTasksDesc = 0;
#endif

#define USR_INIT_POWER_ON_LOGO_TASK_STACK_SIZE 256
#define USR_INIT_POWER_ON_LOGO_TASK_PRIORITY 1

#define USR_INIT_BAL_INIT_TASK_STACK_SIZE 256*3
#define USR_INIT_BAL_INIT_TASK_PRIORITY 2

#define USR_INIT_DECOMPRESS_TASK_STACK_SIZE 256
#define USR_INIT_DECOMPRESS_TASK_PRIORITY 3

typedef struct
{
    const sxr_TaskDesc_t  TaskDesc;
    void (*UsrInitTask)(void);
} sxs_UsrInitTaskCfg_t;

extern void sxs_UsrInitTask (void(*UsrInitTask)(void));
extern void lcd_present_CS_log(void);
extern BOOL BAL_TaskInit(VOID);
extern VOID BAL_DecompressCode(VOID);

#ifndef __SXS_ONLY__
const sxs_UsrInitTaskCfg_t sxs_UsrInitTaskCfg [] =
{
#ifdef __DISPLAY_POWERON_LOGO__
    {
        {
            (void(*)(void *))sxs_UsrInitTask,
            0,
            "USR INIT: Power-on LOGO",
            USR_INIT_POWER_ON_LOGO_TASK_STACK_SIZE,
            USR_INIT_POWER_ON_LOGO_TASK_PRIORITY
        },
        (void(*)(void))&lcd_present_CS_log
    },
#endif //__DISPLAY_POWERON_LOGO__
    {
        {
            (void(*)(void *))sxs_UsrInitTask,
            0,
            "USR_INIT: BAL Init Task",
            USR_INIT_BAL_INIT_TASK_STACK_SIZE,
            USR_INIT_BAL_INIT_TASK_PRIORITY
        },
        (void(*)(void))&BAL_TaskInit
    },
    {
        {
            (void(*)(void *))sxs_UsrInitTask,
            0,
            "USR_INIT: Decompress Task",
            USR_INIT_DECOMPRESS_TASK_STACK_SIZE,
            USR_INIT_DECOMPRESS_TASK_PRIORITY
        },
        (void(*)(void))&BAL_DecompressCode
    },
};
#define SXS_NB_USR_INIT_TASKS_DESC (sizeof(sxs_UsrInitTaskCfg) / sizeof(sxs_UsrInitTaskCfg_t))
u8 sxs_usrInitTaskId [SXS_NB_USR_INIT_TASKS_DESC];
#else
const sxr_TaskDesc_t *sxs_UsrInitTasksDesc = 0;
#define SXS_NB_USR_INIT_TASKS_DESC (0)
u8 sxs_usrInitTaskId [1];
#endif

#else // !__SAP_CFG_VAR__

extern const u8 sxs_NbStaticTasksDesc;

#endif // !__SAP_CFG_VAR__

DefExtern u8 sxs_CmnTaskId [CMN_LAST];

#undef DefExtern

#ifdef __cplusplus
}
#endif

#endif
