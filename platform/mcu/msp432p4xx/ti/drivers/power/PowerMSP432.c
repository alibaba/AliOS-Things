/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== PowerMSP432.c ========
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * By default disable both asserts and log for this module.
 * This must be done before DebugP.h is included.
 */
#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif
#ifndef DebugP_LOG_ENABLED
#define DebugP_LOG_ENABLED 0
#endif

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>

#include <ti/drivers/utils/List.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

/* driverlib header files */
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/wdt_a.h>
#include <ti/devices/msp432p4xx/driverlib/rtc_c.h>
#include <ti/devices/msp432p4xx/driverlib/pcm.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
/* MSP432P401xx devices */
#include <ti/devices/msp432p4xx/driverlib/flash.h>
#include <ti/devices/msp432p4xx/driverlib/sysctl.h>
#define SET_WAIT_STATES MAP_FlashCtl_setWaitState
#define ENABLE_READ_BUFFERING MAP_FlashCtl_enableReadBuffering
#define DISABLE_READ_BUFFERING MAP_FlashCtl_disableReadBuffering
#define GET_NMI_SOURCESTATUS MAP_SysCtl_getNMISourceStatus()
#define DISABLE_NMI_SOURCE MAP_SysCtl_disableNMISource
#define ENABLE_NMI_SOURCE MAP_SysCtl_enableNMISource
#define FLASH_D_READ FLASH_DATA_READ
#define FLASH_I_FETCH FLASH_INSTRUCTION_FETCH
#define BANK0 FLASH_BANK0
#define BANK1 FLASH_BANK1
#define CSSRC 0x1
#else
/* MSP432P4x1xl devices */
#include <ti/devices/msp432p4xx/driverlib/flash_a.h>
#include <ti/devices/msp432p4xx/driverlib/sysctl_a.h>
#define SET_WAIT_STATES MAP_FlashCtl_A_setWaitState
#define ENABLE_READ_BUFFERING MAP_FlashCtl_A_enableReadBuffering
#define DISABLE_READ_BUFFERING MAP_FlashCtl_A_disableReadBuffering
#define GET_NMI_SOURCESTATUS MAP_SysCtl_A_getNMISourceStatus()
#define DISABLE_NMI_SOURCE MAP_SysCtl_A_disableNMISource
#define ENABLE_NMI_SOURCE MAP_SysCtl_A_enableNMISource
#define FLASH_D_READ FLASH_A_DATA_READ
#define FLASH_I_FETCH FLASH_A_INSTRUCTION_FETCH
#define BANK0 FLASH_A_BANK0
#define BANK1 FLASH_A_BANK1
#define CSSRC 0x1
#endif

/* crystal startup timeout counts to allow for given MCLK speed */
#define MCLK48_HF_TIMEOUT 12214
#define MCLK48_LF_TIMEOUT 4767581
#define MCLK24_HF_TIMEOUT 6107
#define MCLK24_LF_TIMEOUT 2383791
#define MCLK12_HF_TIMEOUT 3054
#define MCLK12_LF_TIMEOUT 1191896
#define MCLK6_HF_TIMEOUT  1527
#define MCLK6_LF_TIMEOUT  595948
#define MCLK3_HF_TIMEOUT  764
#define MCLK3_LF_TIMEOUT  297974
#define MCLK1_HF_TIMEOUT  255
#define MCLK1_LF_TIMEOUT  99325
#define HFXT 0
#define LFXT 1

/* Active states */
#define AM_DCDC_VCORE0    PCM_AM_DCDC_VCORE0
#define AM_DCDC_VCORE1    PCM_AM_DCDC_VCORE1
#define AM_LDO_VCORE0     PCM_AM_LDO_VCORE0
#define AM_LDO_VCORE1     PCM_AM_LDO_VCORE1
#define AM_LF_VCORE0      PCM_AM_LPR_VCORE0
#define AM_LF_VCORE1      PCM_AM_LPR_VCORE1

/* CS defines */
#define REG32(x)          (*(volatile unsigned *)(x))
#define CSKEY             (REG32(0x40010400))
#define CSCTL2            (REG32(0x4001040C))
#define HFXT_EN_BIT       (0x01000000)
#define LFXT_EN_BIT       (0x00000100)

/* externs */
extern const PowerMSP432_ConfigV1 PowerMSP432_config;
void PowerMSP432_schedulerDisable(void)
{
    return ;
}

void PowerMSP432_schedulerRestore(void)
{
    return ;
}

void PowerMSP432_updateFreqs(PowerMSP432_Freqs *freqs)
{
    return;
}

/* internal functions */
static void disableFaultCounter(unsigned int type);
static bool disableLFXT(void);
static bool disableHFXT(void);
static unsigned int getTimeoutXTAL(unsigned int cpuSpeed, unsigned int type);
static bool initPerfControl(unsigned int initLevel);
static bool isEnabledHFXT(void);
static bool isEnabledLFXT(void);
static int_fast16_t notify(uint_fast16_t eventType, uintptr_t eventArg);
static void restartHFXT(void);

/* ModuleState */
PowerMSP432_ModuleState PowerMSP432_module = {
    .notifyList = { NULL, NULL},
    .constraintMask = 0,
    .state = Power_ACTIVE,
    .currentPerfLevel = 0,
    .enablePolicy = false,
    .perfInitialized = false,
    .initialized = false,
    .constraintCounts = { 0 },
    .policyFxn = NULL
};

/*
 * Notes for the implementation of Power_setPerformanceLevel()
 *
 * 1) Depending on the MSP432 device variant, there are three or four
 *    predefined performance levels.  For example, on MSP432P401x devices
 *    the following levels are predefined:
 *
 *    Level    MCLK (MHz)    HSMCLK (MHz)    SMCLK (MHz)    ACLK (Hz)
 *    -----    ----------    ------------    -----------    ---------
 *      0         12              3              3           32768
 *      1         24              6              6           32768
 *      2         48             24             12           32768
 *      3         48             48             24           32768
 *
 *    The levels for all device variants are defined below, via
 *    PowerMSP432_PerfLevel structures.
 *
 * 2) DCDC is assumed to be available
 *
 * 3) Hardware interrupts are disabled during the change of performance level
 *
 */

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x

/* MSP432P401xx devices */
#define NUMPERFLEVELS    4
PowerMSP432_PerfLevel PowerMSP432_perfLevels[NUMPERFLEVELS] = {
    { .activeState = AM_DCDC_VCORE0,
      .VCORE = 0,
      .DCORESEL = CS_DCO_FREQUENCY_12,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_4,
      .DIVS = CS_CLOCK_DIVIDER_4,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 0,
      .enableFlashBuffer = false,
      .MCLK = 12000000,
      .HSMCLK = 3000000,
      .SMCLK = 3000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
    { .activeState = AM_DCDC_VCORE0,
      .VCORE = 0,
      .DCORESEL = CS_DCO_FREQUENCY_24,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_4,
      .DIVS = CS_CLOCK_DIVIDER_4,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 1,
      .enableFlashBuffer = true,
      .MCLK = 24000000,
      .HSMCLK = 6000000,
      .SMCLK = 6000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
    { .activeState = AM_DCDC_VCORE1,
      .VCORE = 1,
      .DCORESEL = CS_DCO_FREQUENCY_48,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_2,
      .DIVS = CS_CLOCK_DIVIDER_4,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 1,
      .enableFlashBuffer = true,
      .MCLK = 48000000,
      .HSMCLK = 24000000,
      .SMCLK = 12000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
    { .activeState = AM_DCDC_VCORE1,
      .VCORE = 1,
      .DCORESEL = CS_DCO_FREQUENCY_48,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_1,
      .DIVS = CS_CLOCK_DIVIDER_2,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 1,
      .enableFlashBuffer = true,
      .MCLK = 48000000,
      .HSMCLK = 48000000,
      .SMCLK = 24000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
};

#elif DeviceFamily_ID == DeviceFamily_ID_MSP432P4x1xT

/* MSP432Px1xT devices */
#define NUMPERFLEVELS    3
PowerMSP432_PerfLevel PowerMSP432_perfLevels[NUMPERFLEVELS] = {
    { .activeState = AM_DCDC_VCORE0,
      .VCORE = 0,
      .DCORESEL = CS_DCO_FREQUENCY_6,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_2,
      .DIVS = CS_CLOCK_DIVIDER_2,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 0,
      .enableFlashBuffer = false,
      .MCLK = 6000000,
      .HSMCLK = 3000000,
      .SMCLK = 3000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
    { .activeState = AM_DCDC_VCORE0,
      .VCORE = 0,
      .DCORESEL = CS_DCO_FREQUENCY_12,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_1,
      .DIVS = CS_CLOCK_DIVIDER_2,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 1,
      .enableFlashBuffer = true,
      .MCLK = 12000000,
      .HSMCLK = 12000000,
      .SMCLK = 6000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
    { .activeState = AM_DCDC_VCORE0,
      .VCORE = 0,
      .DCORESEL = CS_DCO_FREQUENCY_24,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_1,
      .DIVS = CS_CLOCK_DIVIDER_2,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 2,
      .enableFlashBuffer = true,
      .MCLK = 24000000,
      .HSMCLK = 24000000,
      .SMCLK = 12000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
};

#else

/* MSP432P4x1xl devices */
#define NUMPERFLEVELS    4
PowerMSP432_PerfLevel PowerMSP432_perfLevels[NUMPERFLEVELS] = {
    { .activeState = AM_DCDC_VCORE0,
      .VCORE = 0,
      .DCORESEL = CS_DCO_FREQUENCY_12,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_4,
      .DIVS = CS_CLOCK_DIVIDER_4,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 1,
      .enableFlashBuffer = false,
      .MCLK = 12000000,
      .HSMCLK = 3000000,
      .SMCLK = 3000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
    { .activeState = AM_DCDC_VCORE0,
      .VCORE = 0,
      .DCORESEL = CS_DCO_FREQUENCY_24,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_4,
      .DIVS = CS_CLOCK_DIVIDER_4,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 2,
      .enableFlashBuffer = true,
      .MCLK = 24000000,
      .HSMCLK = 6000000,
      .SMCLK = 6000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
    { .activeState = AM_DCDC_VCORE1,
      .VCORE = 1,
      .DCORESEL = CS_DCO_FREQUENCY_48,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_2,
      .DIVS = CS_CLOCK_DIVIDER_4,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 3,
      .enableFlashBuffer = true,
      .MCLK = 48000000,
      .HSMCLK = 24000000,
      .SMCLK = 12000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
    { .activeState = AM_DCDC_VCORE1,
      .VCORE = 1,
      .DCORESEL = CS_DCO_FREQUENCY_48,
      .SELM = CS_DCOCLK_SELECT,
      .DIVM = CS_CLOCK_DIVIDER_1,
      .SELS = CS_DCOCLK_SELECT,
      .DIVHS = CS_CLOCK_DIVIDER_1,
      .DIVS = CS_CLOCK_DIVIDER_2,
      .SELB = CS_REFOCLK_SELECT,
      .SELA = CS_REFOCLK_SELECT,
      .DIVA = CS_CLOCK_DIVIDER_1,
      .flashWaitStates = 3,
      .enableFlashBuffer = true,
      .MCLK = 48000000,
      .HSMCLK = 48000000,
      .SMCLK = 24000000,
      .BCLK = 32768,
      .ACLK = 32768
     },
};
#endif

/*
 *  ======== Power_disablePolicy ========
 *  Do not run the power policy on each pass through the idle loop
 */
bool Power_disablePolicy(void)
{
    bool enablePolicy = PowerMSP432_module.enablePolicy;
    PowerMSP432_module.enablePolicy = false;

    DebugP_log0("Power: disable policy");
    return (enablePolicy);
}

/*
 *  ======== Power_enablePolicy ========
 *  Enable the power policy to run on each pass through the idle loop
 */
void Power_enablePolicy(void)
{
    PowerMSP432_module.enablePolicy = true;

    DebugP_log0("Power: enable policy");
}

/*
 *  ======== Power_getConstraintMask ========
 *  Get a bitmask indicating the constraints that have been registered with
 *  Power.
 */
uint_fast32_t Power_getConstraintMask(void)
{
    return (PowerMSP432_module.constraintMask);
}

/*
 *  ======== Power_getPerformanceLevel ========
 *  Get the current performance level.
 */
uint_fast16_t Power_getPerformanceLevel(void)
{
    return (PowerMSP432_module.currentPerfLevel);
}

/*
 *  ======== Power_getTransitionLatency ========
 *  Get the transition latency for a sleep state.  The latency is reported
 *  in units of microseconds.
 */
uint_fast32_t Power_getTransitionLatency(uint_fast16_t sleepState,
    uint_fast16_t type)
{
    uint32_t latency = 0;

    if (type == Power_RESUME) {
        if (sleepState == PowerMSP432_SLEEP) {
            latency = PowerMSP432_RESUMETIMESLEEP;
        }
        else if (sleepState == PowerMSP432_DEEPSLEEP_0) {
            latency = PowerMSP432_RESUMETIMEDEEPSLEEP_0;
        }
        else if (sleepState == PowerMSP432_DEEPSLEEP_1) {
            latency = PowerMSP432_RESUMETIMEDEEPSLEEP_1;
        }
    }
    else {
        if (sleepState == PowerMSP432_SLEEP) {
            latency = PowerMSP432_TOTALTIMESLEEP;
        }
        else if (sleepState == PowerMSP432_DEEPSLEEP_0) {
            latency = PowerMSP432_TOTALTIMEDEEPSLEEP_0;
        }
        else if (sleepState == PowerMSP432_DEEPSLEEP_1) {
            latency = PowerMSP432_TOTALTIMEDEEPSLEEP_1;
        }
    }

    return (latency);
}

/*
 *  ======== Power_getTransitionState ========
 *  Get the current sleep transition state.
 */
uint_fast16_t Power_getTransitionState(void)
{
    return (PowerMSP432_module.state);
}

/*
 *  ======== Power_idleFunc ========
 *  Stub function to call the power policy from the OS idle loop.
 */
void Power_idleFunc()
{
    /* only call the policy function if this has been enabled */
    if (PowerMSP432_module.enablePolicy) {
        if (PowerMSP432_module.policyFxn != NULL) {
            DebugP_log1("Power: calling policy function (%p)",
                (uintptr_t) PowerMSP432_module.policyFxn);
            (*(PowerMSP432_module.policyFxn))();
        }
    }
}

/*
 *  ======== Power_init ========
 */
int_fast16_t Power_init()
{
    HwiP_Params hwiParams;

    /* initialize the power manager state */
    if (!PowerMSP432_module.initialized) {

        /* first, if device is resuming from a shutdown state ... */
        if (RSTCTL->PCMRESET_STAT &
               (RSTCTL_PCMRESET_STAT_LPM35 | RSTCTL_PCMRESET_STAT_LPM45)) {

            /* if a shutdown resume hook has been configured, call it now ... */
            if (PowerMSP432_config.resumeShutdownHookFxn != NULL) {
                (*(PowerMSP432_config.resumeShutdownHookFxn))();
            }

            /* now, unlock I/Os and the Backup domain */
            PCM->CTL1 = PCM_CTL1_KEY_VAL;
        }

        /* set module state field 'initialized' to true */
        PowerMSP432_module.initialized = true;

        /* initialize PowerMSP432_module.enablePolicy */
        PowerMSP432_module.enablePolicy = PowerMSP432_config.enablePolicy;

        /* copy the Power policy function to module state */
        PowerMSP432_module.policyFxn = PowerMSP432_config.policyFxn;

        /* if performance level control is enabled, initialize it */
        if (PowerMSP432_config.enablePerf) {

            /* now initialize performance control */
            PowerMSP432_module.perfInitialized =
                initPerfControl(PowerMSP432_config.initialPerfLevel);

            /* if CS interrupts are to be caught, configure interrupt and ISR */
            if ((PowerMSP432_config.useExtendedPerf) &&
                (PowerMSP432_config.enableInterruptsCS)) {
                HwiP_Params_init(&hwiParams);
                hwiParams.priority = PowerMSP432_config.priorityInterruptsCS;
                HwiP_create(INT_CS, (HwiP_Fxn)PowerMSP432_config.isrCS,
                    &hwiParams);
            }
        }

        /* if there is a configured policy init function, call it now ... */
        if (PowerMSP432_config.policyInitFxn != NULL) {
            (*(PowerMSP432_config.policyInitFxn))();
        }

        /*
         * set FORCE_LPM_ENTRY to force LPM3+ transitions, overriding any
         * outstanding peripheral clock requests
         */
        MAP_PCM_enableRudeMode();

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        /* explicitly enable SRAM retention (for CS) */
        SYSCTL->SRAM_BANKRET |= 0xfe;
#endif
    }

    return (Power_SOK);
}

/*
 *  ======== Power_registerNotify ========
 *  Register a function to be called upon a specific power event.
 */
int_fast16_t Power_registerNotify(Power_NotifyObj * pNotifyObj,
    uint_fast16_t eventTypes, Power_NotifyFxn notifyFxn, uintptr_t clientArg)
{
    int_fast16_t status = Power_SOK;

    /* ensure pointers are not NULL */
    if ((pNotifyObj == NULL) || (notifyFxn == NULL)) {
        status = Power_EINVALIDPOINTER;
    }

    else {
        /* fill in notify object elements */
        pNotifyObj->eventTypes = eventTypes;
        pNotifyObj->notifyFxn = notifyFxn;
        pNotifyObj->clientArg = clientArg;

        /* place notify object on event notification queue */
        List_put(&PowerMSP432_module.notifyList, (List_Elem*)pNotifyObj);
    }

    DebugP_log3(
        "Power: register notify (%p), eventTypes (0x%x), notifyFxn (%p)",
        (uintptr_t) pNotifyObj, eventTypes, (uintptr_t) notifyFxn);

    return (status);
}

/*
 *  ======== Power_releaseConstraint ========
 *  Release a previously declared constraint.
 */
int_fast16_t Power_releaseConstraint(uint_fast16_t constraintId)
{
    int_fast16_t status = Power_SOK;
    uintptr_t key;
    uint8_t count;

    /* first ensure constraintId is valid */
    if (constraintId >= PowerMSP432_NUMCONSTRAINTS) {
        status = Power_EINVALIDINPUT;
    }

    /* if constraintId is OK ... */
    else {

        /* disable interrupts */
        key = HwiP_disable();

        /* get the count of the constraint */
        count = PowerMSP432_module.constraintCounts[constraintId];

        /* ensure constraint count is not already zero */
        if (count == 0) {
            status = Power_EFAIL;
        }

        /* if not already zero ... */
        else {

            /* decrement the count */
            count--;

            /* save the updated count */
            PowerMSP432_module.constraintCounts[constraintId] = count;

            /* if constraint count reaches zero, remove constraint from mask */
            if (count == 0) {
                PowerMSP432_module.constraintMask &= ~(1 << constraintId);
            }
        }

        /* restore interrupts */
        HwiP_restore(key);

        DebugP_log1("Power: release constraint (%d)", constraintId);
    }

    return (status);
}

/*
 *  ======== Power_setConstraint ========
 *  Declare an operational constraint.
 */
int_fast16_t Power_setConstraint(uint_fast16_t constraintId)
{
    int_fast16_t status = Power_SOK;
    uintptr_t key;

    /* ensure that constraintId is valid */
    if (constraintId >= PowerMSP432_NUMCONSTRAINTS) {
        status = Power_EINVALIDINPUT;
    }

    else {
        /* disable interrupts */
        key = HwiP_disable();

        /* set the specified constraint in the constraintMask */
        PowerMSP432_module.constraintMask |= 1 << constraintId;

        /* increment the specified constraint count */
        PowerMSP432_module.constraintCounts[constraintId]++;

        /* restore interrupts */
        HwiP_restore(key);

        DebugP_log1("Power: set constraint (%d)", constraintId);
    }

    return (status);
}

/*
 *  ======== Power_setPerformanceLevel ========
 *  Set a new performance level for the device.
 *  This function should only be called from Task context.
 */
int_fast16_t Power_setPerformanceLevel(uint_fast16_t level)
{
    int_fast16_t status = Power_SOK;
    PowerMSP432_PerfLevel perfNow;
    PowerMSP432_PerfLevel perfNew;
    bool changedStateOK = true;
    bool changedState = false;
    bool changedWaits = false;
    PowerMSP432_Freqs freqs;
    unsigned int constraints;
    unsigned int timeout;
    unsigned int xtDrive;
    bool needHFXT = false;
    bool needLFXT = false;
    bool enabledHFXT;
    bool enabledLFXT;
    unsigned int hwiKey;

    /* return immediately if performance control not enabled and initialized */
    if (!PowerMSP432_module.perfInitialized) {
        return (Power_EFAIL);
    }

    /* disable interrupts */
    hwiKey = HwiP_disable();

    /* check to make sure not busy with another transition */
    if (Power_getTransitionState() != Power_ACTIVE) {
        status = Power_EBUSY;
    }

    /* validate the performance level */
    else if (level >= (NUMPERFLEVELS + PowerMSP432_config.numCustom)) {
        status = Power_EINVALIDINPUT;
    }

    else {

        /* check if there is a constraint prohibiting the transition */
        constraints = Power_getConstraintMask();

        /* check if constraint set to prohibit *any* perf changes */
        if (constraints & (1 << PowerMSP432_DISALLOW_PERF_CHANGES)) {
            status = Power_EBUSY;
        }

        /* now check if a constraint is set for this perf level */
        else if (((level == 0) &&
                    (constraints & (1 << PowerMSP432_DISALLOW_PERFLEVEL_0))) ||
                 ((level == 1) &&
                    (constraints & (1 << PowerMSP432_DISALLOW_PERFLEVEL_1))) ||
                 ((level == 2) &&
                    (constraints & (1 << PowerMSP432_DISALLOW_PERFLEVEL_2))) ||
                 ((level == 3) &&
                    (constraints & (1 << PowerMSP432_DISALLOW_PERFLEVEL_3))) ||
                 ((level == 4) &&
                    (constraints & (1 << PowerMSP432_DISALLOW_PERFLEVEL_4))) ||
                 ((level == 5) &&
                    (constraints & (1 << PowerMSP432_DISALLOW_PERFLEVEL_5))) ||
                 ((level == 6) &&
                    (constraints & (1 << PowerMSP432_DISALLOW_PERFLEVEL_6))) ||
                 ((level == 7) &&
                    (constraints & (1 << PowerMSP432_DISALLOW_PERFLEVEL_7)))) {
            status = Power_ECHANGE_NOT_ALLOWED;
         }
    }

    /* if any checks failed, or already at this performance level, exit now */
    if ((status != Power_SOK) ||
        (level == PowerMSP432_module.currentPerfLevel)) {
        HwiP_restore(hwiKey);
        return (status);
    }

    /* else, proceed with the change ... */
    else {

        /* set Power's transition state */
        PowerMSP432_module.state = Power_CHANGING_PERF_LEVEL;

        /* do callout to disable scheduling */
        PowerMSP432_schedulerDisable();

        /* notify any clients registered for a start-change notification */
        status = notify(PowerMSP432_START_CHANGE_PERF_LEVEL, level);

        /* BEGIN PERF CHANGE ... */
        if (status == Power_SOK) {

            /* get perf level structs from either predefined or custom array */
            if (PowerMSP432_module.currentPerfLevel < NUMPERFLEVELS) {
                perfNow =
                    PowerMSP432_perfLevels[PowerMSP432_module.currentPerfLevel];
            }
            else {
                perfNow =
                    PowerMSP432_config.customPerfLevels[
                        PowerMSP432_module.currentPerfLevel - NUMPERFLEVELS];
            }
            if (level < NUMPERFLEVELS) {
                perfNew = PowerMSP432_perfLevels[level];
            }
            else {
                perfNew = PowerMSP432_config.customPerfLevels[level -
                    NUMPERFLEVELS];
            }

            /* if new voltage is higher, change active state now */
            if (perfNew.VCORE > perfNow.VCORE) {
                changedStateOK = MAP_PCM_setPowerState(perfNew.activeState);
                if (!changedStateOK) {
                    status = Power_EFAIL;
                }
                changedState = true;
            }

            /* if all still OK, continue ... */
            if (changedStateOK) {

                /* if new flash wait states are higher, set them now */
                if (perfNew.flashWaitStates > perfNow.flashWaitStates) {
                    SET_WAIT_STATES(BANK0, perfNew.flashWaitStates);
                    SET_WAIT_STATES(BANK1, perfNew.flashWaitStates);
                    changedWaits = true;
                }

                /* configure crystals (if extended perf control is enabled) */
                if (PowerMSP432_config.useExtendedPerf) {

                    /* check if any clocks are to be sourced from HFXT */
                    if ((perfNew.SELM == CS_HFXTCLK_SELECT) ||
                        (perfNew.SELS == CS_HFXTCLK_SELECT) ||
                        (perfNew.SELB == CS_HFXTCLK_SELECT) ||
                        (perfNew.SELA == CS_HFXTCLK_SELECT)) {
                        needHFXT = true;
                    }

                    /* check if any clocks are to be sourced from LFXT ... */
                    if ((perfNew.SELM == CS_LFXTCLK_SELECT) ||
                        (perfNew.SELS == CS_LFXTCLK_SELECT) ||
                        (perfNew.SELB == CS_LFXTCLK_SELECT) ||
                        (perfNew.SELA == CS_LFXTCLK_SELECT)) {
                        needLFXT = true;
                    }

                    enabledHFXT = isEnabledHFXT();
                    enabledLFXT = isEnabledLFXT();

                    /* if need HFXT and not currently enabled, enable it */
                    if (needHFXT && !enabledHFXT) {
                        timeout = getTimeoutXTAL(perfNow.MCLK, HFXT);
                        disableFaultCounter(HFXT);
                        if (!MAP_CS_startHFXTWithTimeout(
                            PowerMSP432_config.bypassHFXT, timeout)) {
                            status = Power_EFAIL;
                        }
                        else {
                            if (PowerMSP432_config.enableInterruptsCS) {
                                MAP_CS_enableInterrupt(CS_HFXT_FAULT);
                            }
                        }
                    }

                    /* if need LFXT and not currently enabled, enable it */
                    if (needLFXT && !enabledLFXT && (status == Power_SOK)) {
                        xtDrive = PowerMSP432_config.bypassLFXT ?
                            CS_LFXT_BYPASS : PowerMSP432_config.LFXTDRIVE;
                        timeout = getTimeoutXTAL(perfNow.MCLK, LFXT);
                        disableFaultCounter(LFXT);
                        if (!MAP_CS_startLFXTWithTimeout(xtDrive, timeout)){
                            status = Power_EFAIL;
                        }
                        else {
                            if (PowerMSP432_config.enableInterruptsCS) {
                                MAP_CS_enableInterrupt(CS_LFXT_FAULT);
                            }
                        }
                    }
                }

                /* now, set new clock sources and dividers */
                if (status == Power_SOK) {

                    /* now change clocks and dividers */
                    MAP_CS_setDCOCenteredFrequency(perfNew.DCORESEL);
                    MAP_CS_initClockSignal(CS_MCLK, perfNew.SELM, perfNew.DIVM);
                    MAP_CS_initClockSignal(CS_HSMCLK, perfNew.SELS,
                        perfNew.DIVHS);
                    MAP_CS_initClockSignal(CS_SMCLK, perfNew.SELS,
                        perfNew.DIVS);
                    MAP_CS_initClockSignal(CS_BCLK, perfNew.SELB, 1);
                    MAP_CS_initClockSignal(CS_ACLK, perfNew.SELA,
                        perfNew.DIVA);

                    /* disable crystals if enabled but no longer needed */
                    if (PowerMSP432_config.useExtendedPerf) {
                        if (!needHFXT && enabledHFXT) {
                            if (PowerMSP432_config.enableInterruptsCS) {
                                MAP_CS_disableInterrupt(CS_HFXT_FAULT);
                            }
                            disableHFXT();
                        }
                        if (!needLFXT && enabledLFXT) {
                            if (PowerMSP432_config.enableInterruptsCS) {
                                MAP_CS_disableInterrupt(CS_LFXT_FAULT);
                            }
                            disableLFXT();
                        }
                    }

                    /* if new waits not changed and different, set them now */
                    if ((changedWaits == false) &&
                        (perfNew.flashWaitStates != perfNow.flashWaitStates)) {
                        SET_WAIT_STATES(BANK0, perfNew.flashWaitStates);
                        SET_WAIT_STATES(BANK1, perfNew.flashWaitStates);
                    }

                    /* setup flash buffering */
                    if(perfNew.enableFlashBuffer) {
                        ENABLE_READ_BUFFERING(BANK0, FLASH_D_READ);
                        ENABLE_READ_BUFFERING(BANK0, FLASH_I_FETCH);
                        ENABLE_READ_BUFFERING(BANK1, FLASH_D_READ);
                        ENABLE_READ_BUFFERING(BANK1, FLASH_I_FETCH);
                    }
                    else {
                        DISABLE_READ_BUFFERING(BANK0, FLASH_D_READ);
                        DISABLE_READ_BUFFERING(BANK0, FLASH_I_FETCH);
                        DISABLE_READ_BUFFERING(BANK1, FLASH_D_READ);
                        DISABLE_READ_BUFFERING(BANK1, FLASH_I_FETCH);
                    }

                    /* if new state not changed and is different, change it */
                    if ((changedState == false) &&
                        (perfNew.activeState != perfNow.activeState)) {
                        changedStateOK =
                            MAP_PCM_setPowerState(perfNew.activeState);
                    }

                    /* if success, update perf level state */
                    if (changedStateOK) {

                        /* success! update Power module state with new level */
                        PowerMSP432_module.currentPerfLevel = level;

                        /* do callout to update frequencies */
                        freqs.MCLK = perfNew.MCLK;
                        freqs.HSMCLK = perfNew.HSMCLK;
                        freqs.SMCLK = perfNew.SMCLK;
                        freqs.BCLK = perfNew.BCLK;
                        freqs.ACLK = perfNew.ACLK;
                        PowerMSP432_updateFreqs(&freqs);

                        /* notify any done-change notification clients */
                        status = notify(PowerMSP432_DONE_CHANGE_PERF_LEVEL,
                            level);

                        DebugP_log1("Power: set performance level (%d)", level);
                    }
                    else {
                        status = Power_EFAIL;
                    }
                }
            }
        }
    }

    /* restore state to active */
    PowerMSP432_module.state = Power_ACTIVE;

    /* re-enable interrupts */
    HwiP_restore(hwiKey);

    /* do callout to restore scheduling */
    PowerMSP432_schedulerRestore();

    return (status);
}

/*
 *  ======== Power_setPolicy ========
 *  Set the Power policy function
 */
void Power_setPolicy(Power_PolicyFxn policy)
{
    PowerMSP432_module.policyFxn = policy;
}

/*
 *  ======== Power_shutdown ========
 *  Put the device into a low power shutdown state.
 */
int_fast16_t Power_shutdown(uint_fast16_t shutdownState,
    uint_fast32_t shutdownTime)
{
    int_fast16_t status = Power_EFAIL;
    unsigned int constraints;
    uintptr_t hwiKey;
    bool restoreHFXT;

    /* first validate the shutdownState */
    if ((shutdownState != PowerMSP432_SHUTDOWN_0) &&
        (shutdownState != PowerMSP432_SHUTDOWN_1)) {
        return (Power_EINVALIDINPUT);
    }

    /* disable interrupts */
    hwiKey = HwiP_disable();

    /* make sure shutdown request doesn't violate a constraint */
    constraints = Power_getConstraintMask();
    if ((shutdownState == PowerMSP432_SHUTDOWN_0) &&
        (constraints & (1 << PowerMSP432_DISALLOW_SHUTDOWN_0))) {
        status = Power_ECHANGE_NOT_ALLOWED;
    }
    else if ((shutdownState == PowerMSP432_SHUTDOWN_1) &&
        (constraints & (1 << PowerMSP432_DISALLOW_SHUTDOWN_1))) {
        status = Power_ECHANGE_NOT_ALLOWED;
    }
    else {
        /* make sure not busy with some other transition */
        if (PowerMSP432_module.state == Power_ACTIVE) {

            /* set new transition state to entering shutdown */
            PowerMSP432_module.state = Power_ENTERING_SHUTDOWN;

            /* signal clients registered for pre-shutdown notification */
            status = notify(PowerMSP432_ENTERING_SHUTDOWN, shutdownState);

            /* if no notification error... proceed to shutdown ... */
            if (status == Power_SOK) {

                DebugP_log2(
                   "Power: entering shutdown state (%d), shutdownTime (%d)",
                   shutdownState, shutdownTime);

                /* sample HFXT enable status; if HFXT enabled, disable it */
                restoreHFXT = disableHFXT();

                if (shutdownState == PowerMSP432_SHUTDOWN_0) {
                    MAP_PCM_setPowerState(PCM_LPM35_VCORE0);
                }
                else {
                    MAP_PCM_setPowerState(PCM_LPM45);
                }

                /* if get here, failed to shutdown, restore HFXT as unwind */
                if (restoreHFXT) {
                    restartHFXT();
                }
            }
        }
        else {
            status = Power_EBUSY;
        }
    }

    /* set state back to Power_ACTIVE */
    PowerMSP432_module.state = Power_ACTIVE;

    /* re-enable interrupts */
    HwiP_restore(hwiKey);

    /* if get here, failed to shutdown, return error code */
    return (status);
}

/*
 *  ======== Power_sleep ========
 *  Put the device into a sleep state.
 */
int_fast16_t Power_sleep(uint_fast16_t sleepState)
{
    int_fast16_t status = Power_SOK;
    uintptr_t eventArg = 0;
    unsigned int preEvent;
    unsigned int postEvent;
    uint8_t powerState;
    uint8_t targetState;
    uint16_t savePAREN;
    uint16_t savePBREN;
    uint16_t savePCREN;
    uint16_t savePDREN;
    uint16_t savePEREN;
    uint16_t savePJREN;
    uint16_t currState;
    uint16_t tempOut;
    bool restoreHFXT;
    uint16_t mask;
    uint8_t holdWDT;
    uint8_t holdRTC;

    /* first validate the requested sleep state */
    if ((sleepState != PowerMSP432_SLEEP) &&
        (sleepState != PowerMSP432_DEEPSLEEP_0) &&
        (sleepState != PowerMSP432_DEEPSLEEP_1)) {
        status = Power_EINVALIDINPUT;
    }

    else if (PowerMSP432_module.state == Power_ACTIVE) {
        /* set transition state to entering sleep */
        PowerMSP432_module.state = Power_ENTERING_SLEEP;

        /* setup sleep vars */
        if (sleepState == PowerMSP432_SLEEP) {
            preEvent = PowerMSP432_ENTERING_SLEEP;
            postEvent = PowerMSP432_AWAKE_SLEEP;
        }
        else if ((sleepState == PowerMSP432_DEEPSLEEP_0) ||
                 (sleepState == PowerMSP432_DEEPSLEEP_1)) {
            preEvent = PowerMSP432_ENTERING_DEEPSLEEP;
            postEvent = PowerMSP432_AWAKE_DEEPSLEEP;
            if (sleepState == PowerMSP432_DEEPSLEEP_1) {
                eventArg = 1;
            }
        }

        /* get current power state */
        powerState = MAP_PCM_getPowerState();

        if (sleepState == PowerMSP432_SLEEP) {
            switch (powerState) {
                case PCM_AM_LDO_VCORE0:
                    targetState = PCM_LPM0_LDO_VCORE0;
                    break;
                case PCM_AM_LDO_VCORE1:
                    targetState = PCM_LPM0_LDO_VCORE1;
                    break;
                case PCM_AM_DCDC_VCORE0:
                    targetState = PCM_LPM0_DCDC_VCORE0;
                    break;
                case PCM_AM_DCDC_VCORE1:
                    targetState = PCM_LPM0_DCDC_VCORE1;
                    break;
                case PCM_AM_LF_VCORE0:
                    targetState = PCM_LPM0_LF_VCORE0;
                    break;
                case PCM_AM_LF_VCORE1:
                    targetState = PCM_LPM0_LF_VCORE1;
                    break;
                default:
                    PowerMSP432_module.state = Power_ACTIVE;
                    return (Power_EFAIL);
            }
        }

        /* signal all clients registered for pre-sleep notification */
        status = notify(preEvent, eventArg);

        /* check for any error */
        if (status != Power_SOK) {
            PowerMSP432_module.state = Power_ACTIVE;
            return (status);
        }

        DebugP_log1("Power: sleep, sleepState (%d)", sleepState);

        /*
         *  Enable pull resistors for input pins in DEEPSLEEP_0 and DEEPSLEEP_1
         *
         *  1. A bitmask is created with a zero in each bit position for each
         *     pin to be explicitly parked.  The conditions to be eligible to
         *     be parked are: the pin is configured as input, with GPIO
         *     function, and currently there is no pull resistor enabled for
         *     the pin:
         *        mask = PxDIR | PxSEL0 | PxSEL1 | PxREN
         *
         *  2. The current PxIN states are sampled:
         *        currState = PxIN
         *
         *  3. The PxOUT bit is cleared for each eligible pin; this is a first
         *     step to enable the appropriate pull resistor:
         *        tempOut = mask & PxOUT
         *
         *  4. The PxOUT bit is set for each eligible pin that was sensed as
         *     logic 1, to enable a PU resistor; bits sensed as zero are left
         *     cleared, to enable a PD resistor:
         *        PxOUT = tempOut | (~mask & currState)
         *
         *  5. The current pull enable bits in PxREN are saved:
         *        savePxREN = PxREN
         *
         *  6. The pull resistors are enabled for all eligile pins:
         *        PxREN = PxREN | ~mask
         *
         *  Note: some register accesses are broken into multiple lines of code
         *  to avoid compiler warnings about undefined order of access to
         *  volatiles.
         */
        if (((sleepState == PowerMSP432_DEEPSLEEP_0) ||
            (sleepState == PowerMSP432_DEEPSLEEP_1)) &&
            (PowerMSP432_config.enableParking == true)) {

            mask = PA->DIR;
            mask |= PA->SEL0;
            mask |= PA->SEL1;
            mask |= PA->REN;
            currState = PA->IN;
            tempOut = mask & PA->OUT;
            PA->OUT = tempOut | (~mask & currState);
            savePAREN = PA->REN;
            PA->REN = savePAREN | ~mask;

            mask = PB->DIR;
            mask |= PB->SEL0;
            mask |= PB->SEL1;
            mask |= PB->REN;
            currState = PB->IN;
            tempOut = mask & PB->OUT;
            PB->OUT = tempOut | (~mask & currState);
            savePBREN = PB->REN;
            PB->REN = savePBREN | ~mask;

            mask = PC->DIR;
            mask |= PC->SEL0;
            mask |= PC->SEL1;
            mask |= PC->REN;
            currState = PC->IN;
            tempOut = mask & PC->OUT;
            PC->OUT = tempOut | (~mask & currState);
            savePCREN = PC->REN;
            PC->REN = savePCREN | ~mask;

            mask = PD->DIR;
            mask |= PD->SEL0;
            mask |= PD->SEL1;
            mask |= PD->REN;
            currState = PD->IN;
            tempOut = mask & PD->OUT;
            PD->OUT = tempOut | (~mask & currState);
            savePDREN = PD->REN;
            PD->REN = savePDREN | ~mask;

            mask = PE->DIR;
            mask |= PE->SEL0;
            mask |= PE->SEL1;
            mask |= PE->REN;
            currState = PE->IN;
            tempOut = mask & PE->OUT;
            PE->OUT = tempOut | (~mask & currState);
            savePEREN = PE->REN;
            PE->REN = savePEREN | ~mask;

            mask = PJ->DIR;
            mask |= PJ->SEL0;
            mask |= PJ->SEL1;
            mask |= PJ->REN;
            currState = PJ->IN;
            tempOut = mask & PJ->OUT;
            PJ->OUT = tempOut | (~mask & currState);
            savePJREN = PJ->REN;
            PJ->REN = (savePJREN | ~mask) & 0xFF;
        }

        /* sample HFXT enable status; if HFXT enabled, disable it now */
        restoreHFXT = disableHFXT();

        /* invoke specific sequences to activate sleep states ... */

        if (sleepState == PowerMSP432_SLEEP) {
            MAP_PCM_setPowerState(targetState);
        }
        else if (sleepState == PowerMSP432_DEEPSLEEP_0) {
            MAP_PCM_setPowerState(PCM_LPM3);
        }
        else if (sleepState == PowerMSP432_DEEPSLEEP_1) {

            /* if RTC is enabled: force hold before sleep */
            holdRTC = RTC_C->CTL13 & RTC_C_CTL13_HOLD;
            if (holdRTC != RTC_C_CTL13_HOLD) {
                MAP_RTC_C_holdClock();
            }

            /* if WDT is enabled: force hold before sleep */
            holdWDT = WDT_A->CTL & WDT_A_CTL_HOLD;
            if (holdWDT != WDT_A_CTL_HOLD) {
                MAP_WDT_A_holdTimer();
            }

            /* now transition to LPM4 */
            MAP_PCM_setPowerState(PCM_LPM3);
        }

        /* set transition state to EXITING_SLEEP */
        PowerMSP432_module.state = Power_EXITING_SLEEP;

        /* if woke from DEEPSLEEP_1: conditionally restart RTC and WDT */
        if (sleepState == PowerMSP432_DEEPSLEEP_1) {

            /* if RTC was held before sleep: start it running again */
            if (holdRTC != RTC_C_CTL13_HOLD) {
                MAP_RTC_C_startClock();
            }

            /* if WDT was held before sleep: start it running again */
            if (holdWDT != WDT_A_CTL_HOLD) {
                MAP_WDT_A_startTimer();
            }
        }

        /* restore pull resistor settings after DEEPSLEEP_0 and DEEPSLEEP_1 */
        if (((sleepState == PowerMSP432_DEEPSLEEP_0) ||
            (sleepState == PowerMSP432_DEEPSLEEP_1)) &&
            (PowerMSP432_config.enableParking == true)) {
            PA->REN = savePAREN;
            PB->REN = savePBREN;
            PC->REN = savePCREN;
            PD->REN = savePDREN;
            PE->REN = savePEREN;
            PJ->REN = savePJREN;
        }

        /* if HFXT was disabled during sleep restart it ... */
        if (restoreHFXT) {
            restartHFXT();
        }

        /* signal clients registered for post-sleep notification */
        status = notify(postEvent, eventArg);

        /* now clear the transition state before re-enabling scheduler */
        PowerMSP432_module.state = Power_ACTIVE;
    }
    else {
        status = Power_EBUSY;
    }

    return (status);
}

/*
 *  ======== Power_unregisterNotify ========
 *  Unregister for a power notification.
 */
void Power_unregisterNotify(Power_NotifyObj * pNotifyObj)
{
    uintptr_t key;

    /* disable interrupts */
    key = HwiP_disable();

    /* remove notify object from its event queue */
    List_remove(&PowerMSP432_module.notifyList, (List_Elem *)pNotifyObj);

    /* re-enable interrupts */
    HwiP_restore(key);

    DebugP_log1("Power: unregister notify (%p)", (uintptr_t) pNotifyObj);
}

/********************* MSP432-named functions **********************/

/*
 *  ======== PowerMSP432_getFreqs ========
 *  Get the frequencies for a performance level.
 */
int_fast16_t PowerMSP432_getFreqs(uint_fast16_t level,
    PowerMSP432_Freqs * freqs)
{
    int_fast16_t status = Power_SOK;
    PowerMSP432_PerfLevel perfLevel;

    /* check that perf level is valid */
    if (level >= (NUMPERFLEVELS + PowerMSP432_config.numCustom)) {
        status = Power_EINVALIDINPUT;
    }

    /* check for NULL pointer for freqs */
    else if (freqs == NULL) {
        status = Power_EINVALIDPOINTER;
    }

    /* else, copy perf level frequency info to structure */
    else {

        if (level < NUMPERFLEVELS) {
            perfLevel = PowerMSP432_perfLevels[level];
        }
        else {
            perfLevel = PowerMSP432_config.customPerfLevels[level -
                NUMPERFLEVELS];
        }
        freqs->MCLK = perfLevel.MCLK;
        freqs->HSMCLK = perfLevel.HSMCLK;
        freqs->SMCLK = perfLevel.SMCLK;
        freqs->BCLK = perfLevel.BCLK;
        freqs->ACLK = perfLevel.ACLK;
    }

    return (status);
}

/*
 *  ======== PowerMSP432_getNumPerfLevels ========
 *  Get the number of configured performance levels.
 */
uint_fast16_t PowerMSP432_getNumPerfLevels(void)
{
    return (NUMPERFLEVELS + PowerMSP432_config.numCustom);
}

/*********************** internal functions **************************/

/*
 *  ======== disableFaultCounter ========
 */
static void disableFaultCounter(unsigned int type)
{
    /* unlock Clock System (CS) registers */
    CSKEY = 0x695A;

    if (type == HFXT) {
        BITBAND_PERI(CS->CTL3, CS_CTL3_FCNTHF_EN_OFS) = 0;
    }
    else {
        BITBAND_PERI(CS->CTL3, CS_CTL3_FCNTLF_EN_OFS) = 0;
    }

    /* re-lock CS register access */
    CSKEY = 0;
}

/*
 *  ======== disableLFXT ========
 */
static bool disableLFXT(void)
{
    bool disabled = false;

    /* unlock Clock System (CS) registers */
    CSKEY = 0x695A;

    /* if LFXT is enabled, disable it */
    if (CSCTL2 & LFXT_EN_BIT) {
        BITBAND_PERI(CS->CTL2, CS_CTL2_LFXT_EN_OFS) = 0;
        disabled = true;
    }

    /* re-lock CS register access */
    CSKEY = 0;

    return (disabled);
}

/*
 *  ======== disableHFXT ========
 */
static bool disableHFXT(void)
{
    bool disabled = false;

    /* unlock Clock System (CS) registers */
    CSKEY = 0x695A;

    /* if HFXT is enabled, disable it */
    if (CSCTL2 & HFXT_EN_BIT) {
        BITBAND_PERI(CS->CTL2, CS_CTL2_HFXT_EN_OFS) = 0;
        disabled = true;
    }

    /* re-lock CS register access */
    CSKEY = 0;

    return (disabled);
}

/*
 *  ======== getTimeoutXTAL ========
 */
static unsigned int getTimeoutXTAL(unsigned int cpuSpeed, unsigned int type)
{
    unsigned int timeoutHFXT;
    unsigned int timeoutLFXT;

    if (cpuSpeed > 24000000) {
        timeoutHFXT = MCLK48_HF_TIMEOUT;
        timeoutLFXT = MCLK48_LF_TIMEOUT;
    }
    else if (cpuSpeed > 12000000) {
        timeoutHFXT = MCLK24_HF_TIMEOUT;
        timeoutLFXT = MCLK24_LF_TIMEOUT;
    }
    else if (cpuSpeed > 6000000) {
        timeoutHFXT = MCLK12_HF_TIMEOUT;
        timeoutLFXT = MCLK12_LF_TIMEOUT;
    }
    else if (cpuSpeed > 3000000) {
        timeoutHFXT = MCLK6_HF_TIMEOUT;
        timeoutLFXT = MCLK6_LF_TIMEOUT;
    }
    else if (cpuSpeed > 1000000) {
        timeoutHFXT = MCLK3_HF_TIMEOUT;
        timeoutLFXT = MCLK3_LF_TIMEOUT;
    }
    else {
        timeoutHFXT = MCLK1_HF_TIMEOUT;
        timeoutLFXT = MCLK1_LF_TIMEOUT;
    }

    if (type == HFXT) {
        return (timeoutHFXT);
    }
    else {
        return (timeoutLFXT);
    }
}

/*
 *  ======== isEnabledHFXT ========
 */
static bool isEnabledHFXT(void)
{
    if (CSCTL2 & HFXT_EN_BIT) {
        return (true);
    }
    else {
        return (false);
    }
}

/*
 *  ======== isEnabledLFXT ========
 */
static bool isEnabledLFXT(void)
{
    if (CSCTL2 & LFXT_EN_BIT) {
        return (true);
    }
    else {
        return (false);
    }
}

/*
 *  ======== initPerfControl ========
 *  Initialize performance control (to be called during Power_init()).
 */
static bool initPerfControl(unsigned int initLevel)
{
    PowerMSP432_PerfLevel perfNew;
    PowerMSP432_Freqs freqs;
    unsigned int xtDrive;
    bool status = false;
    bool changedStateOK;

    /* first validate the specified initial performance level */
    if (initLevel < (NUMPERFLEVELS + PowerMSP432_config.numCustom)) {

        /*
         * since not sure of initial conditions, to be safe, first set a
         * DCO frequency compatible with VCORE0, and requiring no wait
         * states (e.g. 6 MHz), and select this as the source for MCLK,
         * HSMCLK and SMCLK
         */
        MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_6);
        MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);
        MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);

        /* select 32768Hz for REFO */
        MAP_CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ);

        /* get perf level struct from either predefined or custom array */
        if (initLevel < NUMPERFLEVELS) {
            perfNew = PowerMSP432_perfLevels[initLevel];
        }
        else {
            perfNew = PowerMSP432_config.customPerfLevels[initLevel -
                NUMPERFLEVELS];
        }

        /* start crystals if they are needed for the initial perf level  */
        if (PowerMSP432_config.useExtendedPerf) {

            /* configure pin for HFXT function? */
            if (PowerMSP432_config.configurePinHFXT) {
                PJ->SEL0 |= BIT3;
                PJ->SEL1 &= ~BIT3;
            }

            /* configure pins for LFXT function? */
            if (PowerMSP432_config.configurePinLFXT) {
                PJ->SEL0 |= BIT0;
                PJ->SEL1 &= ~BIT0;
            }

            /* inform driverlib of external LF and HF clock frequencies */
            MAP_CS_setExternalClockSourceFrequency(32768,
                PowerMSP432_config.HFXTFREQ);

            /* if any clocks are to be sourced from HFXT... start HFXT now */
            if ((perfNew.SELM == CS_HFXTCLK_SELECT) ||
                (perfNew.SELS == CS_HFXTCLK_SELECT) ||
                (perfNew.SELB == CS_HFXTCLK_SELECT) ||
                (perfNew.SELA == CS_HFXTCLK_SELECT)) {

                disableFaultCounter(HFXT);
                if (!MAP_CS_startHFXTWithTimeout(PowerMSP432_config.bypassHFXT,
                    MCLK6_HF_TIMEOUT)) {
                    return(false); /* report fail if HF crystal did not start */
                }
                if (PowerMSP432_config.enableInterruptsCS) {
                    MAP_CS_enableInterrupt(CS_HFXT_FAULT);
                }
            }

            /* start LFXT now if any clocks are to be sourced from LFXT ... */
            if ((perfNew.SELM == CS_LFXTCLK_SELECT) ||
                (perfNew.SELS == CS_LFXTCLK_SELECT) ||
                (perfNew.SELB == CS_LFXTCLK_SELECT) ||
                (perfNew.SELA == CS_LFXTCLK_SELECT)) {

                xtDrive = PowerMSP432_config.bypassLFXT ?
                    CS_LFXT_BYPASS : PowerMSP432_config.LFXTDRIVE;
                disableFaultCounter(LFXT);
                if (!MAP_CS_startLFXTWithTimeout(xtDrive, MCLK6_LF_TIMEOUT)){
                    return(false); /* report fail if LF crystal did not start */
                }
                if (PowerMSP432_config.enableInterruptsCS) {
                    MAP_CS_enableInterrupt(CS_LFXT_FAULT);
                }
            }
        }

        /* now go the the power state for the initial performance level */
        changedStateOK = MAP_PCM_setPowerState(perfNew.activeState);

        if (changedStateOK) {

            /* setup flash wait states */
            SET_WAIT_STATES(BANK0, perfNew.flashWaitStates);
            SET_WAIT_STATES(BANK1, perfNew.flashWaitStates);

            /* setup flash buffering */
            if(perfNew.enableFlashBuffer) {
                ENABLE_READ_BUFFERING(BANK0, FLASH_D_READ);
                ENABLE_READ_BUFFERING(BANK0, FLASH_I_FETCH);
                ENABLE_READ_BUFFERING(BANK1, FLASH_D_READ);
                ENABLE_READ_BUFFERING(BANK1, FLASH_I_FETCH);
            }
            else {
                DISABLE_READ_BUFFERING(BANK0, FLASH_D_READ);
                DISABLE_READ_BUFFERING(BANK0, FLASH_I_FETCH);
                DISABLE_READ_BUFFERING(BANK1, FLASH_D_READ);
                DISABLE_READ_BUFFERING(BANK1, FLASH_I_FETCH);
            }

            /* now setup clocks */
            MAP_CS_setDCOCenteredFrequency(perfNew.DCORESEL);
            MAP_CS_initClockSignal(CS_MCLK, perfNew.SELM, perfNew.DIVM);
            MAP_CS_initClockSignal(CS_HSMCLK, perfNew.SELS, perfNew.DIVHS);
            MAP_CS_initClockSignal(CS_SMCLK, perfNew.SELS, perfNew.DIVS);
            MAP_CS_initClockSignal(CS_BCLK, perfNew.SELB, 1);
            MAP_CS_initClockSignal(CS_ACLK, perfNew.SELA, perfNew.DIVA);

            /* do callout to update frequencies */
            freqs.MCLK = perfNew.MCLK;
            freqs.HSMCLK = perfNew.HSMCLK;
            freqs.SMCLK = perfNew.SMCLK;
            freqs.BCLK = perfNew.BCLK;
            freqs.ACLK = perfNew.ACLK;
            PowerMSP432_updateFreqs(&freqs);

            /* set perf level in Power module state */
            PowerMSP432_module.currentPerfLevel = initLevel;

            status = true;
        }
    }

    return (status);
}

/*
 *  ======== notify ========
 *  Notify clients registered for eventType notifications.
 *  When this function is called interrupts are disabled.
 */
static int_fast16_t notify(uint_fast16_t eventType, uintptr_t eventArg)
{
    int_fast16_t notifyStatus;
    Power_NotifyFxn notifyFxn;
    List_List *notifyList;
    uintptr_t clientArg;
    List_Elem *elem;

    notifyList = &PowerMSP432_module.notifyList;

    /* point to first notify object */
    elem = List_head(notifyList);

    /* while queue is not empty, service each element */
    while (elem != NULL) {

        /* check if this client wants notification of this event type... */
        if (((Power_NotifyObj *)elem)->eventTypes & eventType) {

            /* yes, pull params from notify object */
            notifyFxn = ((Power_NotifyObj *)elem)->notifyFxn;
            clientArg = ((Power_NotifyObj *)elem)->clientArg;

            /* call the client's notification function */
            notifyStatus = (int_fast16_t) (*(Power_NotifyFxn)notifyFxn)(
                eventType, eventArg, clientArg);

            /* if notification failed: stop all further notifications */
            if (notifyStatus != Power_NOTIFYDONE) {
                return (Power_EFAIL);
            }
        }

        /* get next element in the notification queue */
        elem = List_next(elem);
    }

    return (Power_SOK);
}

/*
 *  ======== restartHFXT ========
 */
static void restartHFXT(void)
{
    uint_fast8_t enableNMI;

    /* unlock CS registers */
    CSKEY = 0x695A;

    /* read and save the SYS_NMI_CTLSTAT:CS_SRC value */
    enableNMI = GET_NMI_SOURCESTATUS & CSSRC;

    /* temporarily disable CS from triggering interrupt or NMI */
    DISABLE_NMI_SOURCE(CSSRC);
    if (PowerMSP432_config.enableInterruptsCS) {
       MAP_CS_disableInterrupt(CS_HFXT_FAULT);
    }

    /* enable HFXT, wait for stabilization */
    BITBAND_PERI(CS->CTL2, CS_CTL2_HFXT_EN_OFS) = 1;
    while (BITBAND_PERI(CS->IFG, CS_IFG_HFXTIFG_OFS))
    {
        BITBAND_PERI(CS->CLRIFG, CS_CLRIFG_CLR_HFXTIFG_OFS) = 1;
    }

    /* re-lock CS register access */
    CSKEY = 0;

    /* restore CS interrupt and SYS_NMI_CTLSTAT enable bits */
    ENABLE_NMI_SOURCE(enableNMI);
    if (PowerMSP432_config.enableInterruptsCS) {
       MAP_CS_enableInterrupt(CS_HFXT_FAULT);
    }

    return;
}
