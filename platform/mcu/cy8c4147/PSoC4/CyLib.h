/***************************************************************************//**
* \file CyLib.h
* \version 5.70
*
* \brief Provides a system API for the clocking, and interrupts.
*
* \note Documentation of the API's in this file is located in the System
* Reference Guide provided with PSoC Creator.
*
********************************************************************************
* \copyright
* Copyright 2008-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYLIB_H)
#define CY_BOOT_CYLIB_H

#include "cytypes.h"
#include "cydevice_trm.h"
#include "CyLFClk.h"

#include <string.h>
#include <limits.h>
#include <ctype.h>


/**
* \addtogroup group_clocking

PSoC devices supported by PSoC Creator have flexible clocking capabilities. These clocking capabilities are
controlled in PSoC Creator by selections within the Design-Wide Resources settings, connectivity of clocking signals on
the design schematic, and API calls that can modify the clocking at runtime. The clocking API is provided in the CyLib.c
and CyLib.h files.

This section describes how PSoC Creator maps clocks onto the device and provides guidance on clocking methodologies that
are optimized for the PSoC architecture.


\section section_clocking_modes Power Modes
The IMO is available in Active and Sleep modes. It is automatically disabled/enabled for the proper Deep Sleep and
Hibernate mode entry/exit. The IMO is disabled during Deep Sleep and Hibernate modes.

The EXTCLK is available in Active and Sleep modes. The system will enter/exit Deep Sleep and Hibernate using external
clock. The device will re-enable the IMO if it was enabled before entering Deep Sleep or Hibernate, but it does not wait
for the IMO before starting the CPU. After entering Active mode, the IMO may take an additional 2 us to begin toggling.
The IMO will startup cleanly without glitches, but any dependency should account for this extra startup time. If
desired, firmware may increase wakeup hold-off using \ref CySysPmSetWakeupHoldoff() function to include this 2 us and
ensure the IMO is toggling by the time Active mode is reached.

The ILO is available in all modes except Hibernate and Stop.



\section section_clocking_connectivity Clock Connectivity
The PSoC architecture includes flexible clock generation logic. Refer to the Technical Reference Manual for a detailed
description of all the clocking sources available in a particular device. The usage of these various clocking sources
can be categorized by how those clocks are connected to elements of a design.

\section section_clocking_runtime_changing Changing Clocks in Run-time

\subsection section_clocking_runtime_changing_impact Impact on Components Operation
The components with internal clocks are directly impacted by the change of the system clock frequencies or sources. The
components clock frequencies obtained using design-time dividers. The run-time change of components clock source will
correspondingly change the internal component clock.  Refer to the component datasheet for the details.

\subsection section_clocking_runtime_adjust CyDelay APIs
The CyDelay APIs implement simple software-based delay loops. The loops compensate for system clock frequency. The
\ref CyDelayFreq() function must be called in order to adjust \ref CyDelay(), \ref CyDelayUs() and \ref CyDelayCycles()
functions to the new system clock value.

\subsection section_clocking_runtime_cache Cache Configuration
If the CPU clock frequency increases during device operation, the number of clock cycles cache will wait before sampling
data coming back from Flash should be adjusted. If the CPU clock frequency decreases, the number of clock cycles can be
also adjusted to improve CPU performance. See “CySysFlashSetWaitCycles()” for PSoC 4 for more information.

*/


/**
* \addtogroup group_clocking_hfclk High-Frequency Clocking API
* \ingroup group_clocking
* @{
*/
void CySysClkImoStart(void);
void CySysClkImoStop(void);
void CySysClkWriteHfclkDirect(uint32 clkSelect);

#if (CY_IP_IMO_TRIMMABLE_BY_WCO)
    void CySysClkImoEnableWcoLock(void);
    void CySysClkImoDisableWcoLock(void);
    uint32 CySysClkImoGetWcoLock(void);
#endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */

#if (CY_IP_IMO_TRIMMABLE_BY_USB)
    void CySysClkImoEnableUsbLock(void);
    void CySysClkImoDisableUsbLock(void);
    uint32 CySysClkImoGetUsbLock(void);
#endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

#if (CY_IP_SRSSLT)
    void CySysClkWriteHfclkDiv(uint32 divider);
#endif /* (CY_IP_SRSSLT) */

void CySysClkWriteSysclkDiv(uint32 divider);
void CySysClkWriteImoFreq(uint32 freq);
uint32 CySysClkGetSysclkSource(void);
void CySysEnablePumpClock(uint32 enable);

/** @} group_clocking_hfclk */


/**
* \addtogroup group_clocking_lfclk Low-Frequency Clocking API
* \ingroup group_clocking
* \detailed For PSoC 4 devices, the CyLFClk (low-frequency clock) APIs are located in separate files
* (CyLFClk.h/CyLFClk.c). See the CyLFClk Component Datasheet available from the System Reference Guides item of the
* PSoC Creator Help menu.
* @{
*/
/** @} group_clocking_lfclk */


/**
* \addtogroup group_clocking_eco External Crystal Oscillator (ECO) API
* \ingroup group_clocking
* @{
*/
#if (CY_IP_ECO)
    cystatus CySysClkEcoStart(uint32 timeoutUs);
    void     CySysClkEcoStop(void);
    uint32   CySysClkEcoReadStatus(void);

    #if (CY_IP_ECO_BLESS || CY_IP_ECO_BLESSV3)
        void CySysClkWriteEcoDiv(uint32 divider);
    #endif /* (CY_IP_ECO_BLESS || CY_IP_ECO_BLESSV3) */

    #if (CY_IP_ECO_SRSSV2 || CY_IP_ECO_SRSSLT)
        void CySysClkConfigureEcoTrim(uint32 wDTrim, uint32 aTrim, uint32 fTrim, uint32 rTrim, uint32 gTrim);
        cystatus CySysClkConfigureEcoDrive(uint32 freq, uint32 cLoad, uint32 esr, uint32 maxAmplitude);
    #endif /* (CY_IP_ECO_SRSSV2 || CY_IP_ECO_SRSSLT) */
#endif  /* (CY_IP_ECO) */
/** @} group_clocking_eco */


/**
* \addtogroup group_clocking_pll Phase-Locked Loop (PLL) API
* \ingroup group_clocking
* @{
*/
#if (CY_IP_PLL)
    cystatus CySysClkPllStart(uint32 pll, uint32 wait);
    void     CySysClkPllStop(uint32 pll);
    cystatus CySysClkPllSetPQ(uint32 pll, uint32 feedback, uint32 reference, uint32 current);
    cystatus CySysClkPllSetFrequency(uint32 pll, uint32 inputFreq, uint32 pllFreq, uint32 divider, uint32 freqTol);
    void     CySysClkPllSetSource(uint32 pll, uint32 source);
    cystatus CySysClkPllSetOutputDivider(uint32 pll, uint32 divider);
    void CySysClkPllSetBypassMode(uint32 pll, uint32 bypass);
    uint32 CySysClkPllGetUnlockStatus(uint32 pll);
    uint32 CySysClkPllGetLockStatus(uint32 pll);
#endif /* (CY_IP_PLL) */
/** @} group_clocking_pll */


/**
* \addtogroup group_api_lvd_functions Low Voltage Detection API
* @{
*/
#if(CY_IP_SRSSV2)
    void   CySysLvdEnable(uint32 threshold);
    void   CySysLvdDisable(void);
    uint32 CySysLvdGetInterruptSource(void);
    void   CySysLvdClearInterrupt(void);
#endif  /* (CY_IP_SRSSV2) */
/** @} group_api_lvd_functions */


/**
* \addtogroup group_interrupts Interrupt API
* \brief The APIs in this chapter apply to all architectures except as noted. The Interrupts API is provided in the
* CyLib.c and CyLib.h files. Refer also to the Interrupt component datasheet for more information about interrupts.
* @{
*/
cyisraddress CyIntSetSysVector(uint8 number, cyisraddress address);
cyisraddress CyIntGetSysVector(uint8 number);

cyisraddress CyIntSetVector(uint8 number, cyisraddress address);
cyisraddress CyIntGetVector(uint8 number);

void   CyIntSetPriority(uint8 number, uint8 priority);
uint8  CyIntGetPriority(uint8 number);

void   CyIntEnable(uint8 number);
uint8  CyIntGetState(uint8 number);
void   CyIntDisable(uint8 number);

void   CyIntSetPending(uint8 number);
void   CyIntClearPending(uint8 number);

uint32 CyDisableInts(void);
void   CyEnableInts(uint32 mask);
/** @} group_interrupts */


/**
* \addtogroup group_api_delay_functions Delay API
* @{
*/
/* Do not use these definitions directly in your application */
extern uint32 cydelayFreqHz;
extern uint32 cydelayFreqKhz;
extern uint8  cydelayFreqMhz;
extern uint32 cydelay32kMs;

void  CyDelay(uint32 milliseconds);
void  CyDelayUs(uint16 microseconds);
void  CyDelayFreq(uint32 freq);
void  CyDelayCycles(uint32 cycles);
/** @} group_api_delay_functions */


/**
* \addtogroup group_api_system_functions System API
* @{
*/
void  CySoftwareReset(void);
uint8 CyEnterCriticalSection(void);
void  CyExitCriticalSection(uint8 savedIntrStatus);
void  CyHalt(uint8 reason);
uint32 CySysGetResetReason(uint32 reason);
void CyGetUniqueId(uint32* uniqueId);

/* Default interrupt handler */
CY_ISR_PROTO(IntDefaultHandler);
/** @} group_api_system_functions */


/**
* \addtogroup group_api_systick_functions System Timer (SysTick) API
* @{
*/

typedef void (*cySysTickCallback)(void);

void CySysTickStart(void);
void CySysTickInit(void);
void CySysTickEnable(void);
void CySysTickStop(void);
void CySysTickEnableInterrupt(void);
void CySysTickDisableInterrupt(void);
void CySysTickSetReload(uint32 value);
uint32 CySysTickGetReload(void);
uint32 CySysTickGetValue(void);
cySysTickCallback CySysTickSetCallback(uint32 number, cySysTickCallback function);
cySysTickCallback CySysTickGetCallback(uint32 number);

#if(CY_SYSTICK_LFCLK_SOURCE)
    void CySysTickSetClockSource(uint32 clockSource);
    uint32 CySysTickGetClockSource(void);
#endif /* (CY_SYSTICK_LFCLK_SOURCE) */

uint32 CySysTickGetCountFlag(void);
void CySysTickClear(void);
extern uint32 CySysTickInitVar;
/** @} group_api_systick_functions */


#if (CY_IP_DMAC_PRESENT)
    void CySysSetRamAccessArbPriority(uint32 source);
    void CySysSetFlashAccessArbPriority(uint32 source);
    void CySysSetDmacAccessArbPriority(uint32 source);
    void CySysSetPeripheralAccessArbPriority(uint32 interfaceNumber, uint32 source);
#endif /* (CY_IP_DMAC_PRESENT) */


/**
* \addtogroup group_api_pvb_functions Programmable Voltage Block (PVB) API
* @{
*/
#if (CY_IP_PASS)
    void CySysPrbSetGlobalVrefSource(uint32 source);
    void CySysPrbSetBgGain(uint32 gain);
    void CySysPrbSetGlobalVrefVoltage(uint32 voltageTap);
    void CySysPrbEnableDeepsleepVddaRef(void);
    void CySysPrbDisableDeepsleepVddaRef(void);
    void CySysPrbEnableVddaRef(void);
    void CySysPrbDisableVddaRef(void);
    void CySysPrbSetBgBufferTrim(int32 bgTrim);
    int32 CySysPrbGetBgBufferTrim(void);
#endif /* (CY_IP_PASS) */
/** @} group_api_pvb_functions */


/***************************************
* API Constants
***************************************/


/*******************************************************************************
* Clock API Constants
*******************************************************************************/

/* CySysClkWriteHfclkDirect() - implementation definitions */
#if(CY_IP_SRSSV2)
    #define CY_SYS_CLK_SELECT_DIRECT_SEL_MASK           (( uint32 ) 0x07u)
    #define CY_SYS_CLK_SELECT_DIRECT_SEL_PARAM_MASK     (( uint32 ) 0x07u)

    #define CY_SYS_CLK_SELECT_HFCLK_SEL_SHIFT           (( uint32 ) 16u)

    #if (CY_IP_PLL)
        #define CY_SYS_CLK_SELECT_HFCLK_SEL_MASK        (( uint32 ) 3u << CY_SYS_CLK_SELECT_HFCLK_SEL_SHIFT)
    #else
        #define CY_SYS_CLK_SELECT_HFCLK_SEL_MASK        (( uint32 ) 0u )
    #endif /* (CY_IP_PLL) */

#else
    #if (CY_IP_PLL && CY_IP_SRSSLT)    
        #define CY_SYS_ECO_CLK_SELECT_ECO_PLL_MASK      (( uint32 ) 0x01u )
        #define CY_SYS_CLK_SELECT_HFCLK_SEL_PLL_MASK    (( uint32 ) 0x04u )
        #define CY_SYS_CLK_SELECT_HFCLK_PLL_SHIFT       (( uint32 ) 2u) 
       
        #define CY_SYS_EXCO_PGM_CLK_ENABLE_MASK         (( uint32 ) 0x80000000u)
        #define CY_SYS_EXCO_PGM_CLK_CLK_ECO_MASK        (( uint32 ) 0x2u)
        #define CY_SYS_EXCO_PGM_CLK_SEQ_GENERATOR       (( uint8 ) 0x5u)
    #endif /* (CY_IP_PLL && CY_IP_SRSSLT) */

    #define CY_SYS_CLK_SELECT_HFCLK_SEL_MASK            (( uint32 ) 0u )  
    #define CY_SYS_CLK_SELECT_DIRECT_SEL_MASK           (( uint32 ) 0x03u)
    #define CY_SYS_CLK_SELECT_DIRECT_SEL_PARAM_MASK     (CY_SYS_CLK_SELECT_DIRECT_SEL_MASK)
#endif  /* (CY_IP_SRSSV2) */

/* CySysClkWriteHfclkDirect() - parameter definitions */
#define CY_SYS_CLK_HFCLK_IMO                            (0u)
#define CY_SYS_CLK_HFCLK_EXTCLK                         (1u)
#if (CY_IP_ECO)
    #define CY_SYS_CLK_HFCLK_ECO                        (2u)
#endif  /* (CY_IP_ECO) */

#if (CY_IP_PLL)
    #if (CY_IP_SRSSV2)
        #define CY_SYS_CLK_HFCLK_PLL0                   ((uint32) ((uint32) 2u << CY_SYS_CLK_SELECT_HFCLK_SEL_SHIFT))
        #define CY_SYS_CLK_HFCLK_PLL1                   ((uint32) ((uint32) 1u << CY_SYS_CLK_SELECT_HFCLK_SEL_SHIFT))
    #else
        #define CY_SYS_CLK_HFCLK_PLL0                   (6u)
    #endif /* (CY_IP_SRSSV2) */
#endif  /* (CY_IP_PLL) */

/* CySysClkWriteSysclkDiv() - parameter definitions */
#define CY_SYS_CLK_SYSCLK_DIV1                          (0u)
#define CY_SYS_CLK_SYSCLK_DIV2                          (1u)
#define CY_SYS_CLK_SYSCLK_DIV4                          (2u)
#define CY_SYS_CLK_SYSCLK_DIV8                          (3u)
#if(CY_IP_SRSSV2)
    #define CY_SYS_CLK_SYSCLK_DIV16                     (4u)
    #define CY_SYS_CLK_SYSCLK_DIV32                     (5u)
    #define CY_SYS_CLK_SYSCLK_DIV64                     (6u)
    #define CY_SYS_CLK_SYSCLK_DIV128                    (7u)
#endif  /* (CY_IP_SRSSV2) */


/* CySysClkWriteSysclkDiv() - implementation definitions */
#if(CY_IP_SRSSV2)
    #define CY_SYS_CLK_SELECT_SYSCLK_DIV_SHIFT          (19u)
    #define CY_SYS_CLK_SELECT_SYSCLK_DIV_MASK           (( uint32 )0x07u)
#else
    #define CY_SYS_CLK_SELECT_SYSCLK_DIV_SHIFT          (6u)
    #define CY_SYS_CLK_SELECT_SYSCLK_DIV_MASK           (( uint32 )0x03u)
#endif  /* (CY_IP_SRSSV2) */


/* CySysClkPllSetSource() - implementation definitions */
#if (CY_IP_PLL)
    #if(CY_IP_SRSSV2)
        #define CY_SYS_CLK_SELECT_PLL_SHIFT(x)          (3u + (3u * (x)))
        #define CY_SYS_CLK_SELECT_PLL_MASK(x)           ((uint32) ((uint32) 0x07u << CY_SYS_CLK_SELECT_PLL_SHIFT((x))))
    #else
        #define CY_SYS_ECO_CLK_SELECT_PLL0_SHIFT        (1u)
        #define CY_SYS_ECO_CLK_SELECT_PLL0_MASK         ((uint32) ((uint32) 0x01u << CY_SYS_ECO_CLK_SELECT_PLL0_SHIFT))
    #endif  /* (CY_IP_SRSSV2) */
#endif /* (CY_IP_PLL) */

/* CySysClkPllSetSource() - parameter definitions */
#if (CY_IP_PLL)
    #if(CY_IP_SRSSV2)
        #define CY_SYS_PLL_SOURCE_IMO                   (0u)
        #define CY_SYS_PLL_SOURCE_EXTCLK                (1u)
        #define CY_SYS_PLL_SOURCE_ECO                   (2u)
        #define CY_SYS_PLL_SOURCE_DSI0                  (4u)
        #define CY_SYS_PLL_SOURCE_DSI1                  (5u)
        #define CY_SYS_PLL_SOURCE_DSI2                  (6u)
        #define CY_SYS_PLL_SOURCE_DSI3                  (7u)
    #else
        #define CY_SYS_PLL_SOURCE_ECO                   (0u)
        #define CY_SYS_PLL_SOURCE_IMO                   (1u)
    #endif  /* (CY_IP_SRSSV2) */
#endif /* (CY_IP_PLL) */

/* CySysClkPllSetBypassMode() - parameter definitions */
#if(CY_IP_SRSSV2 || CY_IP_SRSSLT)
    #if (CY_IP_PLL)
        #define CY_SYS_PLL_BYPASS_AUTO         (0u)
        #define CY_SYS_PLL_BYPASS_PLL_REF      (2u)
        #define CY_SYS_PLL_BYPASS_PLL_OUT      (3u)
    #endif /* (CY_IP_PLL) */
#endif  /* (CY_IP_SRSSV2 || CY_IP_SRSSLT)) */

/* CySysClkPllSetOutputDivider()/CySysClkPllSetFrequency() - parameters */
#if(CY_IP_SRSSV2 || CY_IP_SRSSLT)
    #if (CY_IP_PLL)
        #define CY_SYS_PLL_OUTPUT_DIVPASS               (0u)
        #define CY_SYS_PLL_OUTPUT_DIV2                  (1u)
        #define CY_SYS_PLL_OUTPUT_DIV4                  (2u)
        #define CY_SYS_PLL_OUTPUT_DIV8                  (3u)
    #endif /* (CY_IP_PLL) */
#endif  /* (CY_IP_SRSSV2 || CY_IP_SRSSLT) */

/* CySysPumpClock() */
#define CY_SYS_CLK_PUMP_DISABLE                         ((uint32) 0u)
#define CY_SYS_CLK_PUMP_ENABLE                          ((uint32) 1u)

#if (CY_IP_PLL)

    /* Set of the PLL registers */
    typedef struct
    {
        uint32 config;
        uint32 status;
        uint32 test;
    } cy_sys_clk_pll_regs_struct;

    /* Array of the PLL registers */
    typedef struct
    {
        cy_sys_clk_pll_regs_struct pll[2u];
    } cy_sys_clk_pll_struct;


    /* CySysClkPllSetPQ() - implementation definitions */
    #define CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_SHIFT     (0u)
    #define CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_SHIFT    (8u)
    #define CY_SYS_CLK_PLL_CONFIG_OUTPUT_DIV_SHIFT       (14u)
    #define CY_SYS_CLK_PLL_CONFIG_ICP_SEL_SHIFT          (16u)
    #define CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_SHIFT       (20u)

    #define CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_MASK     ((uint32) ((uint32) 0xFFu << CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_SHIFT))
    #define CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_MASK    ((uint32) ((uint32) 0x3Fu << CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_SHIFT))
    #define CY_SYS_CLK_PLL_CONFIG_OUTPUT_DIV_MASK       ((uint32) ((uint32) 0x03u << CY_SYS_CLK_PLL_CONFIG_OUTPUT_DIV_SHIFT))
    #define CY_SYS_CLK_PLL_CONFIG_ICP_SEL_MASK          ((uint32) ((uint32) 0x07u << CY_SYS_CLK_PLL_CONFIG_ICP_SEL_SHIFT))
    #define CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_MASK       ((uint32) ((uint32) 0x03u << CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_SHIFT))

    #define CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_PLL_REF    ((uint32) ((uint32) 2u << CY_SYS_CLK_PLL_CONFIG_BYPASS_SEL_SHIFT))

    #define CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_MIN      (4u)
    #define CY_SYS_CLK_PLL_CONFIG_FEEDBACK_DIV_MAX      (259u)
    #define CY_SYS_CLK_PLL_CONFIG_ICP_SEL_MIN           (2u)
    #define CY_SYS_CLK_PLL_CONFIG_ICP_SEL_MAX           (3u)
    #define CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_MIN     (1u)
    #define CY_SYS_CLK_PLL_CONFIG_REFERENCE_DIV_MAX     (64u)

    /* CySysClkPllGetUnlockStatus() - implementation definitions */
    #define CY_SYS_CLK_PLL_TEST_UNLOCK_OCCURRED_SHIFT   (4u)
    #define CY_SYS_CLK_PLL_TEST_UNLOCK_OCCURRED_MASK    (( uint32 )(( uint32 )0x01u << CY_SYS_CLK_PLL_TEST_UNLOCK_OCCURRED_SHIFT))

    /* CySysClkPllSetFrequency() - implementation definitions */
    #define CY_SYS_CLK_PLL_QMINIP                       (1u)
    #define CY_SYS_CLK_PLL_FPFDMAX                      (3000u)

    #define CY_SYS_CLK_PLL_QMAXIP                       (64u)
    #define CY_SYS_CLK_PLL_FPFDMIN                      (1000u)

    #define CY_SYS_CLK_PLL_INVALID                      (0u)
    #define CY_SYS_CLK_PLL_CURRENT_DEFAULT              (2u)

    #define CY_SYS_CLK_PLL_INPUT_FREQ_MIN               (1000u)
    #define CY_SYS_CLK_PLL_INPUT_FREQ_MAX               (49152u)

    #define CY_SYS_CLK_PLL_OUTPUT_FREQ_MIN              (22500u)
    #define CY_SYS_CLK_PLL_OUTPUT_FREQ_MAX              (49152u)

    /* CySysClkPllStart() / CySysClkPllStop() - implementation definitions */
    #define CY_SYS_CLK_PLL_STATUS_LOCKED                (1u)
    #define CY_SYS_CLK_PLL_MIN_STARTUP_US               (5u)
    #define CY_SYS_CLK_PLL_MAX_STARTUP_US               (255u)

    #define CY_SYS_CLK_PLL_CONFIG_ENABLE                ((uint32) ((uint32) 1u << 31u))
    #define CY_SYS_CLK_PLL_CONFIG_ISOLATE               ((uint32) ((uint32) 1u << 30u))

#endif /* (CY_IP_PLL) */

/* CySysClkWriteImoFreq() - implementation definitions */
#if(CY_IP_SRSSV2)
    #define CY_SYS_CLK_IMO_MAX_FREQ_MHZ                 (48u)
    #define CY_SYS_CLK_IMO_MIN_FREQ_MHZ                 (3u)

    #define CY_SYS_CLK_IMO_TEMP_FREQ_MHZ                (24u)
    #define CY_SYS_CLK_IMO_TEMP_FREQ_TRIM2              (0x19u)     /* Corresponds to 24 MHz */

    #define CY_SYS_CLK_IMO_BOUNDARY_FREQ_MHZ            (43u)
    #define CY_SYS_CLK_IMO_BOUNDARY_FREQ_TRIM2          (0x30u)     /* Corresponds to 43 MHz */

    #define CY_SYS_CLK_IMO_FREQ_TIMEOUT_CYCLES          (5u)
    #define CY_SYS_CLK_IMO_TRIM_TIMEOUT_US              (5u)
    #define CY_SYS_CLK_IMO_FREQ_TABLE_SIZE              (46u)
    #define CY_SYS_CLK_IMO_FREQ_TABLE_OFFSET            (3u)
    #define CY_SYS_CLK_IMO_FREQ_BITS_MASK               (( uint32 )0x3Fu)
    #define CY_SYS_CLK_IMO_FREQ_CLEAR                   (( uint32 )(CY_SYS_CLK_IMO_FREQ_BITS_MASK << 8u))
    #define CY_SYS_CLK_IMO_TRIM4_GAIN_MASK				(( uint32 )0x1Fu)
	#define CY_SYS_CLK_IMO_TRIM4_WCO_GAIN               (( uint32 ) 12u)
    #define CY_SYS_CLK_IMO_TRIM4_USB_GAIN               (( uint32 ) 8u)

    #if(CY_IP_IMO_TRIMMABLE_BY_USB)
        #define CY_SYS_CLK_USBDEVv2_CR1_ENABLE_LOCK     (( uint32 )0x02u)
        #define CY_SFLASH_S1_TESTPGM_REV_MASK           (( uint32 )0x3Fu)
        #define CY_SFLASH_S1_TESTPGM_OLD_REV            (( uint32 )4u)
    #endif /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

#else
    #define CY_SYS_CLK_IMO_MIN_FREQ_MHZ                 (24u)
    #define CY_SYS_CLK_IMO_MAX_FREQ_MHZ                 (48u)
    #define CY_SYS_CLK_IMO_STEP_SIZE_MASK               (0x03u)
    #define CY_SYS_CLK_IMO_TRIM1_OFFSET_MASK            (( uint32 )(0xFFu))
    #define CY_SYS_CLK_IMO_TRIM2_FSOFFSET_MASK          (( uint32 )(0x07u))
    #define CY_SYS_CLK_IMO_TRIM3_VALUES_MASK            (( uint32 )(0x7Fu))
    #define CY_SYS_CLK_IMO_SELECT_FREQ_MASK             (( uint32 )(0x07u))
    #define CY_SYS_CLK_IMO_SELECT_FREQ_SHIFT            (( uint32 )(0x02u))
    #define CY_SYS_CLK_IMO_SELECT_24MHZ                 (( uint32 )(0x00u))

    #define CY_SYS_CLK_IMO_TRIM_DELAY_US                (( uint32 )(50u))
    #define CY_SYS_CLK_IMO_TRIM_DELAY_CYCLES            (( uint32 )(50u))    
#endif  /* (CY_IP_SRSSV2) */

/* CySysClkImoEnableUsbLock(void) -  - implementation definitions */
#if(CY_IP_IMO_TRIMMABLE_BY_USB)
    #define CY_SYS_CLK_USBDEVv2_CR1_ENABLE_LOCK         (( uint32 )0x02u)
#endif /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

#if (CY_IP_IMO_TRIMMABLE_BY_WCO && CY_IP_IMO_TRIMMABLE_BY_USB)
    #define CY_SYS_CLK_OSCINTF_CTL_PORT_SEL_MASK        (( uint32 )0x01u)
    #define CY_SYS_CLK_OSCINTF_CTL_PORT_SEL_USB         (( uint32 )0x00u)
    #define CY_SYS_CLK_OSCINTF_CTL_PORT_SEL_WCO         (( uint32 )0x01u)
#endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO && CY_IP_IMO_TRIMMABLE_BY_USB) */


#if(CY_IP_SRSSV2)
        /* Conversion between CySysClkWriteImoFreq() parameter and register's value */
        extern const uint8 cyImoFreqMhz2Reg[CY_SYS_CLK_IMO_FREQ_TABLE_SIZE];
#endif /* (CY_IP_SRSSV2) */


/* CySysClkImoStart()/CySysClkImoStop() - implementation definitions */
#define CY_SYS_CLK_IMO_CONFIG_ENABLE                    (( uint32 )(( uint32 )0x01u << 31u))


#if(CY_IP_SRSSLT)
    /* CySysClkWriteHfclkDiv() - parameter definitions */
    #define CY_SYS_CLK_HFCLK_DIV_NODIV                  (0u)
    #define CY_SYS_CLK_HFCLK_DIV_2                      (1u)
    #define CY_SYS_CLK_HFCLK_DIV_4                      (2u)
    #define CY_SYS_CLK_HFCLK_DIV_8                      (3u)

    /* CySysClkWriteHfclkDiv() - implementation definitions */
    #define CY_SYS_CLK_SELECT_HFCLK_DIV_SHIFT           (2u)
    #define CY_SYS_CLK_SELECT_HFCLK_DIV_MASK            (( uint32 )0x03u)
#endif  /* (CY_IP_SRSSLT) */


/* Operating source for Pump clock */
#if(CY_IP_SRSSV2)
    #define CY_SYS_CLK_IMO_CONFIG_PUMP_SEL_SHIFT        (25u)
    #define CY_SYS_CLK_IMO_CONFIG_PUMP_SEL_MASK         ((uint32) 0x07u)
    #define CY_SYS_CLK_IMO_CONFIG_PUMP_SEL_IMO          (1u)

	#define CY_SYS_CLK_IMO_CONFIG_PUMP_OSC              (( uint32 )(( uint32 )0x01u << 22u))
#else /* CY_IP_SRSSLT */
    #define CY_SYS_CLK_SELECT_PUMP_SEL_SHIFT            (4u)
    #define CY_SYS_CLK_SELECT_PUMP_SEL_MASK             ((uint32) 0x03u)
    #define CY_SYS_CLK_SELECT_PUMP_SEL_IMO              (1u)
#endif  /* (CY_IP_SRSSLT) */


#if (CY_IP_ECO_BLESS)
    /* Radio configuration register */
    #define CY_SYS_XTAL_BLESS_RF_CONFIG_RF_ENABLE       (( uint32 )0x01u)

    /* RFCTRL mode transition control */
    #define CY_SYS_XTAL_BLERD_DBUS_XTAL_ENABLE          (( uint32 )(( uint32 )0x01u << 15u))

    /* XO is oscillating status */
    #define CY_SYS_XTAL_BLERD_FSM_XO_AMP_DETECT         (( uint32 )0x01u)

    /* BB bump configuration 2 */
    #define CY_SYS_XTAL_BLERD_BB_XO_CAPTRIM_X1_MASK     (( uint32 )(( uint32 )0x7Fu <<  8u))
    #define CY_SYS_XTAL_BLERD_BB_XO_CAPTRIM_X2_MASK     (( uint32 )(( uint32 )0x7Fu <<  0u))
    #define CY_SYS_XTAL_BLERD_BB_XO_CAPTRIM_X1_ADD_CAP  (( uint32 )(( uint32 )0x01u << 15u))
    #define CY_SYS_XTAL_BLERD_BB_XO_CAPTRIM_X2_ADD_CAP  (( uint32 )(( uint32 )0x01u <<  7u))

    /* BB bump configuration 1 */
    #define CY_SYS_XTAL_BLERD_BB_XO_TRIM                ((uint32) 0x2002u)


    /**
    * \addtogroup group_api_eco
    * @{
    */
     #define CY_SYS_CLK_ECO_DIV1    ((uint32) 0x00)     /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 1 */
     #define CY_SYS_CLK_ECO_DIV2    ((uint32) 0x01)     /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 2 */
     #define CY_SYS_CLK_ECO_DIV4    ((uint32) 0x02)     /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 4 */
     #define CY_SYS_CLK_ECO_DIV8    ((uint32) 0x03)     /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 8 */
    /** @} group_api_eco */

    /* CySysClkWriteEcoDiv() - implementation definitions */
    #define CY_SYS_CLK_XTAL_CLK_DIV_MASK                ((uint32) 0x03)
#endif  /* (CY_IP_ECO_BLESS) */

#if (CY_IP_ECO_BLESSV3)
     #define CY_SYS_CLK_ECO_DIV1    ((uint32) 0x00)     /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 1 */
     #define CY_SYS_CLK_ECO_DIV2    ((uint32) 0x01)     /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 2 */
     #define CY_SYS_CLK_ECO_DIV4    ((uint32) 0x02)     /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 4 */
     #define CY_SYS_CLK_ECO_DIV8    ((uint32) 0x03)     /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 8 */
    /** @} group_api_eco */

    #define CY_SYS_BLESS_HVLDO_STARTUP_DELAY                ((uint32) 2u)
    #define CY_SYS_BLESS_ISOLATE_DEASSERT_DELAY             ((uint32) 1u)
    #define CY_SYS_BLESS_ACT_TO_SWITCH_DELAY                ((uint32) 1u)
    #define CY_SYS_BLESS_HVLDO_DISABLE_DELAY                ((uint32) 1u)

    #define CY_SYS_BLESS_OSC_STARTUP_DELAY_LF               ((uint32) 80u)
    #define CY_SYS_BLESS_DSM_OFFSET_TO_WAKEUP_INST_LF       ((uint32) 4u)
    #define CY_SYS_BLESS_ACT_STARTUP_DELAY                  ((uint32) 1u)
    #define CY_SYS_BLESS_DIG_LDO_STARTUP_DELAY              ((uint32) 1u)

    #define CY_SYS_BLESS_XTAL_DISABLE_DELAY                 ((uint32) 1u)
    #define CY_SYS_BLESS_DIG_LDO_DISABLE_DELAY              ((uint32) 1u)

    #define CY_SYS_BLESS_MT_CFG_ACT_LDO                     ((uint32) 1u)
    #define CY_SYS_BLESS_MT_CFG_ENABLE_BLERD                ((uint32) 1u)
    #define CY_SYS_BLESS_MT_CFG_DPSLP_ECO_ON                ((uint32) 1u)
    
    #define CY_SYS_BLESS_MT_STATUS_BLERD_IDLE               ((uint32) 4u)
    #define CY_SYS_BLESS_MT_STATUS_SWITCH_EN                ((uint32) 5u)
    #define CY_SYS_BLESS_MT_STATUS_ACTIVE                   ((uint32) 6u)
    #define CY_SYS_BLESS_MT_STATUS_ISOLATE                  ((uint32) 7u)
    
    #define CY_SYS_BLESS_BLERD_ACTIVE_INTR_MASK             ((uint32) 0x20u)
    #define CY_SYS_BLESS_BLERD_ACTIVE_INTR_STAT             ((uint32) 0x8u)

    #define CY_SYS_BLESS_MT_STATUS_CURR_STATE_MASK          ((uint32) 0x1Eu)
    
    #define CY_SYS_RCB_CTRL_ENABLED                         ((uint32) 1u)
    #define CY_SYS_RCB_CTRL_DIV_ENABLED                     ((uint32) 1u)
    #define CY_SYS_RCB_CTRL_DIV                             ((uint32) 2u)
    #define CY_SYS_RCB_CTRL_LEAD                            ((uint32) 3u)
    #define CY_SYS_RCB_CTRL_LAG                             ((uint32) 3u)
    
    #define CY_SYS_RCB_INTR_RCB_DONE                        ((uint32) 1u)
    #define CY_SYS_RCB_INTR_RCB_RX_FIFO_NOT_EMPTY           ((uint32) ((uint32)0x1u << 17u))
    #define CY_SYS_RCB_INTR_CLEAR                           ((uint32) 0xFFFFFFFFu)
    #define CY_SYS_RCB_RBUS_RD_CMD                          ((uint32) ((uint32)0x1u << 31u))
    #define CY_SYS_RCB_RBUS_DIG_CLK_SET                     ((uint32) 0x1e030400u)
    #define CY_SYS_RCB_RBUS_FREQ_NRST_SET                   ((uint32) 0x1e021800u)
    #define CY_SYS_RCB_RBUS_FREQ_XTAL_DIV_SET               ((uint32) 0x1e090040u)
    #define CY_SYS_RCB_RBUS_FREQ_XTAL_NODIV_SET             ((uint32) 0x1e090000u)
    #define CY_SYS_RCB_RBUS_RF_DCXO_CFG_SET                 ((uint32) 0x1e080000u)
    #define CY_SYS_RCB_RBUS_IB_VAL                          ((uint32) ((uint32)0x1u << 9u))
    #define CY_SYS_RCB_RBUS_IB_MASK                         ((uint32) ((uint32)0x3u << 9u))
    #define CY_SYS_RCB_RBUS_TRIM_VAL                        ((uint32) (CYDEV_RCB_RBUS_RF_DCXO_CAP_TRIM << 1u))
    #define CY_SYS_RCB_RBUS_TRIM_MASK                       ((uint32) ((uint32)0xFFu << 1u))
    #define CY_SYS_RCB_RBUS_VAL_MASK                        ((uint32) 0xFFFFu)

    #define CY_SYS_RCBLL_CPU_ACCESS                         ((uint32) 0u)
    #define CY_SYS_RCBLL_BLELL_ACCESS                       ((uint32) 1u)

    #define CY_SYS_BLELL_CMD_ENTER_DSM                      ((uint32) 0x50u)
    
    #define CY_SYS_BLESS_MT_DELAY_CFG_INIT \
           ((CY_SYS_BLESS_HVLDO_STARTUP_DELAY    <<  CYFLD_BLE_BLESS_HVLDO_STARTUP_DELAY__OFFSET) | \
            (CY_SYS_BLESS_ISOLATE_DEASSERT_DELAY <<  CYFLD_BLE_BLESS_ISOLATE_DEASSERT_DELAY__OFFSET) | \
            (CY_SYS_BLESS_ACT_TO_SWITCH_DELAY    <<  CYFLD_BLE_BLESS_ACT_TO_SWITCH_DELAY__OFFSET) | \
            (CY_SYS_BLESS_HVLDO_DISABLE_DELAY    <<  CYFLD_BLE_BLESS_HVLDO_DISABLE_DELAY__OFFSET))

    #define CY_SYS_BLESS_MT_DELAY_CFG2_INIT \
           ((CY_SYS_BLESS_OSC_STARTUP_DELAY_LF          << CYFLD_BLE_BLESS_OSC_STARTUP_DELAY_LF__OFFSET) | \
            (CY_SYS_BLESS_DSM_OFFSET_TO_WAKEUP_INST_LF  << CYFLD_BLE_BLESS_DSM_OFFSET_TO_WAKEUP_INSTANT_LF__OFFSET) | \
            (CY_SYS_BLESS_ACT_STARTUP_DELAY             << CYFLD_BLE_BLESS_ACT_STARTUP_DELAY__OFFSET) | \
            (CY_SYS_BLESS_DIG_LDO_STARTUP_DELAY         << CYFLD_BLE_BLESS_DIG_LDO_STARTUP_DELAY__OFFSET))
    
    #define CY_SYS_BLESS_MT_DELAY_CFG3_INIT \
           ((CY_SYS_BLESS_XTAL_DISABLE_DELAY    << CYFLD_BLE_BLESS_XTAL_DISABLE_DELAY__OFFSET) | \
            (CY_SYS_BLESS_DIG_LDO_DISABLE_DELAY << CYFLD_BLE_BLESS_DIG_LDO_DISABLE_DELAY__OFFSET))
    
    #define CY_SYS_BLESS_MT_CFG_CLEAR \
                                                  ~(CY_GET_FIELD_MASK(32, CYFLD_BLE_BLESS_ENABLE_BLERD) | \
                                                    CY_GET_FIELD_MASK(32, CYFLD_BLE_BLESS_DPSLP_ECO_ON) | \
                                                    CY_GET_FIELD_MASK(32, CYFLD_BLE_BLESS_ACT_LDO_NOT_BUCK))
    
    #define CY_SYS_BLESS_MT_CFG_INIT \
           ((CY_SYS_BLESS_MT_CFG_ENABLE_BLERD           <<  CYFLD_BLE_BLESS_ENABLE_BLERD__OFFSET) | \
            (CY_SYS_BLESS_MT_CFG_DPSLP_ECO_ON           <<  CYFLD_BLE_BLESS_DPSLP_ECO_ON__OFFSET) | \
            (CY_SYS_BLESS_MT_CFG_ACT_LDO                <<  CYFLD_BLE_BLESS_ACT_LDO_NOT_BUCK__OFFSET))
    
    #define CY_SYS_RCB_CTRL_CLEAR \
                                                  ~(CY_GET_FIELD_MASK(32, CYFLD_BLE_RCB_ENABLED) | \
                                                    CY_GET_FIELD_MASK(32, CYFLD_BLE_RCB_DIV_ENABLED) | \
                                                    CY_GET_FIELD_MASK(32, CYFLD_BLE_RCB_DIV) | \
                                                    CY_GET_FIELD_MASK(32, CYFLD_BLE_RCB_LEAD) | \
                                                    CY_GET_FIELD_MASK(32, CYFLD_BLE_RCB_LAG))
    
    #define CY_SYS_RCB_CTRL_INIT \
           ((CY_SYS_RCB_CTRL_ENABLED            <<  CYFLD_BLE_RCB_ENABLED__OFFSET) | \
            (CY_SYS_RCB_CTRL_DIV_ENABLED        <<  CYFLD_BLE_RCB_DIV_ENABLED__OFFSET) | \
            (CY_SYS_RCB_CTRL_DIV                <<  CYFLD_BLE_RCB_DIV__OFFSET) | \
            (CY_SYS_RCB_CTRL_LEAD               <<  CYFLD_BLE_RCB_LEAD__OFFSET) | \
            (CY_SYS_RCB_CTRL_LAG                <<  CYFLD_BLE_RCB_LAG__OFFSET))

    /* CySysClkWriteEcoDiv() - implementation definitions */
    #define CY_SYS_CLK_XTAL_CLK_DIV_MASK                    ((uint32) 0x03)
    
    #define CY_SYS_BLE_CLK_ECO_FREQ_32MHZ                   (32)

#endif  /* (CY_IP_ECO_BLESSV3) */


/* CySysClkImoEnableWcoLock() / CySysClkImoDisableWcoLock() constants */
#if (CY_IP_IMO_TRIMMABLE_BY_WCO)
    /* Fimo = DPLL_MULT  * Fwco */

    #define CY_SYS_CLK_WCO_CONFIG_DPLL_ENABLE               (( uint32 )(( uint32 )0x01u << 30u))

    /* Rounding integer division: DPLL_MULT = (Fimo_in_khz + Fwco_in_khz / 2) / Fwco_in_khz */
    #define CY_SYS_CLK_WCO_DPLL_MULT_VALUE(frequencyMhz)    ((uint32) (((((frequencyMhz) * 1000000u) + 16384u) / 32768u) - 1u))
    #define CY_SYS_CLK_WCO_DPLL_MULT_MASK                   ((uint32) 0x7FFu)

    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_IGAIN_SHIFT       (16u)
    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_PGAIN_SHIFT       (19u)
    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_SHIFT       (22u)

    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_IGAIN_MASK        (( uint32 )(( uint32 )0x07u << CY_SYS_CLK_WCO_CONFIG_DPLL_LF_IGAIN_SHIFT))
    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_PGAIN_MASK        (( uint32 )(( uint32 )0x07u << CY_SYS_CLK_WCO_CONFIG_DPLL_LF_PGAIN_SHIFT))
    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_MASK        (( uint32 )(( uint32 )0xFFu << CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_SHIFT))

    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_IGAIN             (( uint32 )(( uint32 ) 4u << CY_SYS_CLK_WCO_CONFIG_DPLL_LF_IGAIN_SHIFT))
    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_PGAIN             (( uint32 )(( uint32 ) 2u << CY_SYS_CLK_WCO_CONFIG_DPLL_LF_PGAIN_SHIFT))

    #define CY_SYS_CLK_WCO_CONFIG_DPLL_LF_LIMIT_MAX         ((uint32) 0xFFu)
    #define CY_SYS_CLK_WCO_IMO_TIMEOUT_MS                   ((uint32) 20u)

    #define CY_SYS_CLK_IMO_FREQ_WCO_DPLL_SAFE_POINT         (26u)
    #define CY_SYS_CLK_IMO_FREQ_WCO_DPLL_TABLE_SIZE         (23u)
    #define CY_SYS_CLK_IMO_FREQ_WCO_DPLL_TABLE_OFFSET       (26u)

#endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */


/*******************************************************************************
* System API Constants
*******************************************************************************/

/* CySysGetResetReason() */
#define CY_SYS_RESET_WDT_SHIFT          (0u)
#define CY_SYS_RESET_PROTFAULT_SHIFT    (3u)
#define CY_SYS_RESET_SW_SHIFT           (4u)

#define CY_SYS_RESET_WDT                ((uint32)1u << CY_SYS_RESET_WDT_SHIFT      )
#define CY_SYS_RESET_PROTFAULT          ((uint32)1u << CY_SYS_RESET_PROTFAULT_SHIFT)
#define CY_SYS_RESET_SW                 ((uint32)1u << CY_SYS_RESET_SW_SHIFT       )


/* CySoftwareReset() - implementation definitions */

/* Vector Key */
#define CY_SYS_AIRCR_VECTKEY_SHIFT      (16u)
#define CY_SYS_AIRCR_VECTKEY            ((uint32)((uint32)0x05FAu << CY_SYS_AIRCR_VECTKEY_SHIFT))
#define CY_SYS_AIRCR_VECTKEY_MASK       ((uint32)((uint32)0xFFFFu << CY_SYS_AIRCR_VECTKEY_SHIFT))

/* System Reset Request */
#define CY_SYS_AIRCR_SYSRESETREQ_SHIFT  (2u)
#define CY_SYS_AIRCR_SYSRESETREQ        ((uint32)((uint32)1u << CY_SYS_AIRCR_SYSRESETREQ_SHIFT))


#if defined(__ARMCC_VERSION)

    #define CyGlobalIntEnable           do                      \
                                        {                       \
                                            __enable_irq();     \
                                        } while ( 0 )

    #define CyGlobalIntDisable          do                      \
                                        {                       \
                                            __disable_irq();    \
                                        } while ( 0 )

#elif defined(__GNUC__) || defined (__ICCARM__)

    #define CyGlobalIntEnable           do                      \
                                        {                       \
                                            __asm("CPSIE   i"); \
                                        } while ( 0 )

    #define CyGlobalIntDisable          do                      \
                                        {                       \
                                            __asm("CPSID   i"); \
                                        } while ( 0 )

#else
    #error No compiler toolchain defined
    #define CyGlobalIntEnable
    #define CyGlobalIntDisable
#endif  /* (__ARMCC_VERSION) */

/* System tick timer */
#define CY_SYS_SYST_CSR_ENABLE               ((uint32) (0x01u))
#define CY_SYS_SYST_CSR_ENABLE_INT           ((uint32) (0x02u))
#define CY_SYS_SYST_CSR_CLK_SOURCE_SHIFT     (0x02u)
#define CY_SYS_SYST_CSR_COUNTFLAG_SHIFT      (16u)
#define CY_SYS_SYST_CSR_CLK_SRC_SYSCLK       ((uint32) (1u))
#define CY_SYS_SYST_CSR_CLK_SRC_LFCLK        (0u)
#define CY_SYS_SYST_RVR_CNT_MASK             (0x00FFFFFFu)
#define CY_SYS_SYST_CVR_CNT_MASK             (0x00FFFFFFu)
#define CY_SYS_SYST_NUM_OF_CALLBACKS         (5u)


/*******************************************************************************
* Macro Name: CyAssert
********************************************************************************
* Summary:
*  Macro that evaluates the expression and, if it is false (evaluates to 0),
*  the processor is halted.
*
*  This macro is evaluated unless NDEBUG is defined.
*  If NDEBUG is defined, then no code is generated for this macro.
*  NDEBUG is defined by default for a Release build setting and not defined for
*  a Debug build setting.
*
* Parameters:
*  expr: Logical expression.  Asserts if false.
*
* Return:
*  None
*
*******************************************************************************/
#if !defined(NDEBUG)
    #define CYASSERT(x)                 do                              \
                                        {                               \
                                            if(0u == (uint32)(x))               \
                                            {                           \
                                                CyHalt((uint8) 0u);     \
                                            }                           \
                                        } while ( 0u )
#else
    #define CYASSERT(x)
#endif  /* !defined(NDEBUG) */


/*******************************************************************************
* Interrupt API Constants
*******************************************************************************/
#define CY_NUM_INTERRUPTS                   (CY_IP_INT_NR)

#define CY_MIN_PRIORITY                     (3u)

#define CY_INT_IRQ_BASE                     (16u)
#define CY_INT_CLEAR_DISABLE_ALL            (0xFFFFFFFFu)
#define CY_INT_ENABLE_RANGE_MASK            (0x1Fu)

/* Register n contains priorities for interrupts N=4n .. 4n+3 */
#define CY_INT_PRIORITY_SHIFT(number)       (( uint32 )6u + (8u * (( uint32 )(number) % 4u)))

/* Mask to get valid range of system priority 0-3 */
#define CY_INT_PRIORITY_MASK                (( uint32 ) 0x03u)

/* CyIntSetSysVector()/CyIntGetSysVector() - parameter definitions */
#define CY_INT_NMI_IRQN                      ( 2u)      /* Non Maskable Interrupt      */
#define CY_INT_HARD_FAULT_IRQN               ( 3u)      /* Hard Fault Interrupt        */
#define CY_INT_SVCALL_IRQN                   (11u)      /* SV Call Interrupt           */
#define CY_INT_PEND_SV_IRQN                  (14u)      /* Pend SV Interrupt           */
#define CY_INT_SYSTICK_IRQN                  (15u)      /* System Tick Interrupt       */


#if(CY_IP_SRSSV2)


    /*******************************************************************************
    * Low Voltage Detection API Constants
    *******************************************************************************/

    /* CySysLvdEnable() - parameter definitions */
    #define CY_LVD_THRESHOLD_1_75_V             (( uint32 )  0u)
    #define CY_LVD_THRESHOLD_1_80_V             (( uint32 )  1u)
    #define CY_LVD_THRESHOLD_1_90_V             (( uint32 )  2u)
    #define CY_LVD_THRESHOLD_2_00_V             (( uint32 )  3u)
    #define CY_LVD_THRESHOLD_2_10_V             (( uint32 )  4u)
    #define CY_LVD_THRESHOLD_2_20_V             (( uint32 )  5u)
    #define CY_LVD_THRESHOLD_2_30_V             (( uint32 )  6u)
    #define CY_LVD_THRESHOLD_2_40_V             (( uint32 )  7u)
    #define CY_LVD_THRESHOLD_2_50_V             (( uint32 )  8u)
    #define CY_LVD_THRESHOLD_2_60_V             (( uint32 )  9u)
    #define CY_LVD_THRESHOLD_2_70_V             (( uint32 ) 10u)
    #define CY_LVD_THRESHOLD_2_80_V             (( uint32 ) 11u)
    #define CY_LVD_THRESHOLD_2_90_V             (( uint32 ) 12u)
    #define CY_LVD_THRESHOLD_3_00_V             (( uint32 ) 13u)
    #define CY_LVD_THRESHOLD_3_20_V             (( uint32 ) 14u)
    #define CY_LVD_THRESHOLD_4_50_V             (( uint32 ) 15u)

    /* CySysLvdEnable()  - implementation definitions */
    #define CY_LVD_PWR_VMON_CONFIG_LVD_EN           (( uint32 ) 0x01u)
    #define CY_LVD_PWR_VMON_CONFIG_LVD_SEL_SHIFT    (1u)
    #define CY_LVD_PWR_VMON_CONFIG_LVD_SEL_MASK     (( uint32 ) (0x0F << CY_LVD_PWR_VMON_CONFIG_LVD_SEL_SHIFT))
    #define CY_LVD_PROPAGATE_INT_TO_CPU             (( uint32 ) 0x02u)
    #define CY_LVD_STABILIZE_TIMEOUT_US             (1000u)

    /* CySysLvdGetInterruptSource()/ CySysLvdClearInterrupt()  - parameter definitions  */
    #define CY_SYS_LVD_INT                          (( uint32 ) 0x02u)
#endif  /* (CY_IP_SRSSV2) */

/* CyDelay()/CyDelayFreq()  - implementation definitions */
#define CY_DELAY_MS_OVERFLOW                    (0x8000u)
#define CY_DELAY_1M_THRESHOLD                   (1000000u)
#define CY_DELAY_1M_MINUS_1_THRESHOLD           (999999u)
#define CY_DELAY_1K_THRESHOLD                   (1000u)
#define CY_DELAY_1K_MINUS_1_THRESHOLD           (999u)


/*******************************************************************************
* ECO
*******************************************************************************/
#if (CY_IP_ECO)
    #if (CY_IP_ECO_SRSSV2 || CY_IP_ECO_SRSSLT)

        /* CySysClkEcoStart() - implementation definitions */
        #define CY_SYS_CLK_ECO_CONFIG_CLK_EN_SHIFT          (0u)
        #define CY_SYS_CLK_ECO_CONFIG_CLK_EN                ((uint32) ((uint32) 1u << CY_SYS_CLK_ECO_CONFIG_CLK_EN_SHIFT))
        #define CY_SYS_CLK_ECO_CONFIG_CLK_EN_TIMEOUT_US     (10u)

        #define CY_SYS_CLK_ECO_CONFIG_ENABLE_SHIFT          (31u)
        #define CY_SYS_CLK_ECO_CONFIG_ENABLE                ((uint32) ((uint32) 1u << CY_SYS_CLK_ECO_CONFIG_ENABLE_SHIFT))

        #define CY_SYS_CLK_ECO_STATUS_WATCHDOG_ERROR_SHIFT  (0u)
        #define CY_SYS_CLK_ECO_STATUS_WATCHDOG_ERROR        ((uint32) ((uint32) 1u << CY_SYS_CLK_ECO_STATUS_WATCHDOG_ERROR_SHIFT))

        #define CY_SYS_CLK_ECO_CONFIG_AGC_EN_SHIFT          (1u)
        #define CY_SYS_CLK_ECO_CONFIG_AGC_EN                ((uint32) ((uint32) 1u << CY_SYS_CLK_ECO_CONFIG_AGC_EN_SHIFT))


        /**
        * \addtogroup group_api_eco
        * @{
        */
        #define CY_SYS_CLK_ECO_WDTRIM0  (0u)    /**< CySysClkWriteEcoDiv(): HFCLK = ECO / 1 */
        #define CY_SYS_CLK_ECO_WDTRIM1  (1u)
        #define CY_SYS_CLK_ECO_WDTRIM2  (2u)
        #define CY_SYS_CLK_ECO_WDTRIM3  (3u)

        #define CY_SYS_CLK_ECO_ATRIM0   (0u)
        #define CY_SYS_CLK_ECO_ATRIM1   (1u)
        #define CY_SYS_CLK_ECO_ATRIM2   (2u)
        #define CY_SYS_CLK_ECO_ATRIM3   (3u)
        #define CY_SYS_CLK_ECO_ATRIM4   (4u)
        #define CY_SYS_CLK_ECO_ATRIM5   (5u)
        #define CY_SYS_CLK_ECO_ATRIM6   (6u)
        #define CY_SYS_CLK_ECO_ATRIM7   (7u)

        #define CY_SYS_CLK_ECO_FTRIM0   (0u)
        #define CY_SYS_CLK_ECO_FTRIM1   (1u)
        #define CY_SYS_CLK_ECO_FTRIM2   (2u)
        #define CY_SYS_CLK_ECO_FTRIM3   (3u)

        #define CY_SYS_CLK_ECO_RTRIM0   (0u)
        #define CY_SYS_CLK_ECO_RTRIM1   (1u)
        #define CY_SYS_CLK_ECO_RTRIM2   (2u)
        #define CY_SYS_CLK_ECO_RTRIM3   (3u)

        #define CY_SYS_CLK_ECO_GTRIM0   (0u)
        #define CY_SYS_CLK_ECO_GTRIM1   (1u)
        #define CY_SYS_CLK_ECO_GTRIM2   (2u)
        #define CY_SYS_CLK_ECO_GTRIM3   (3u)
        /** @} group_api_eco */

        
        /* CySysClkConfigureEcoTrim() - implementation definitions */
        #define CY_SYS_CLK_ECO_TRIM0_WDTRIM_SHIFT       (0u)
        #define CY_SYS_CLK_ECO_TRIM0_WDTRIM_MASK        ((uint32) ((uint32) 3u << CY_SYS_CLK_ECO_TRIM0_WDTRIM_SHIFT))

        #define CY_SYS_CLK_ECO_TRIM0_ATRIM_SHIFT        (2u)
        #define CY_SYS_CLK_ECO_TRIM0_ATRIM_MASK         ((uint32) ((uint32) 7u << CY_SYS_CLK_ECO_TRIM0_ATRIM_SHIFT))

        #define CY_SYS_CLK_ECO_TRIM1_FTRIM_SHIFT        (0u)
        #define CY_SYS_CLK_ECO_TRIM1_FTRIM_MASK         ((uint32) ((uint32) 3u << CY_SYS_CLK_ECO_TRIM1_FTRIM_SHIFT))

        #define CY_SYS_CLK_ECO_TRIM1_RTRIM_SHIFT        (2u)
        #define CY_SYS_CLK_ECO_TRIM1_RTRIM_MASK         ((uint32) ((uint32) 3u << CY_SYS_CLK_ECO_TRIM1_RTRIM_SHIFT))

        #define CY_SYS_CLK_ECO_TRIM1_GTRIM_SHIFT        (4u)
        #define CY_SYS_CLK_ECO_TRIM1_GTRIM_MASK         ((uint32) ((uint32) 3u << CY_SYS_CLK_ECO_TRIM1_GTRIM_SHIFT))


        /* CySysClkConfigureEcoDrive() - implementation definitions */
        #define CY_SYS_CLK_ECO_FREQ_KHZ_MIN     (4000u)
        #define CY_SYS_CLK_ECO_FREQ_KHZ_MAX     (33333u)

        #define CY_SYS_CLK_ECO_MAX_AMPL_MIN_mV  (500u)
        #define CY_SYS_CLK_ECO_TRIM_BOUNDARY    (1200u)

        /* Constant coefficient: 5u * 4u * CY_M_PI * CY_M_PI * 4 / 10 */
        #define CY_SYS_CLK_ECO_GMMIN_COEFFICIENT    (79u)

        #define CY_SYS_CLK_ECO_FREQ_FOR_FTRIM0  (30000u)
        #define CY_SYS_CLK_ECO_FREQ_FOR_FTRIM1  (24000u)
        #define CY_SYS_CLK_ECO_FREQ_FOR_FTRIM2  (17000u)

        #define CY_SYS_CLK_ECO_AMPL_FOR_ATRIM0  (600u)
        #define CY_SYS_CLK_ECO_AMPL_FOR_ATRIM1  (700u)
        #define CY_SYS_CLK_ECO_AMPL_FOR_ATRIM2  (800u)
        #define CY_SYS_CLK_ECO_AMPL_FOR_ATRIM3  (900u)
        #define CY_SYS_CLK_ECO_AMPL_FOR_ATRIM4  (1025u)
        #define CY_SYS_CLK_ECO_AMPL_FOR_ATRIM5  (1150u)
        #define CY_SYS_CLK_ECO_AMPL_FOR_ATRIM6  (1275u)

    #endif  /* (CY_IP_ECO_SRSSV2 || CY_IP_ECO_SRSSLT) */
#endif /* (CY_IP_ECO) */


/*******************************************************************************
* Access Arbitration API Constants
*******************************************************************************/
#if (CY_IP_DMAC_PRESENT)
    #define CY_SYS_CPUSS_RAM_CTL_ARB_SHIFT          (17u)
    #define CY_SYS_CPUSS_RAM_CTL_ARB_MASK           ((uint32) ((uint32) 3u << CY_SYS_CPUSS_RAM_CTL_ARB_SHIFT))

    #define CY_SYS_CPUSS_FLASH_CTL_ARB_SHIFT        (17u)
    #define CY_SYS_CPUSS_FLASH_CTL_ARB_MASK         ((uint32) ((uint32) 3u << CY_SYS_CPUSS_FLASH_CTL_ARB_SHIFT))

    #define CY_SYS_CPUSS_DMAC_CTL_ARB_SHIFT         (17u)
    #define CY_SYS_CPUSS_DMAC_CTL_ARB_MASK          ((uint32) ((uint32) 3u << CY_SYS_CPUSS_DMAC_CTL_ARB_SHIFT))

    #define CY_SYS_CPUSS_SL_CTL_ARB_SHIFT           (17u)
    #define CY_SYS_CPUSS_SL_CTL_ARB_MASK            ((uint32) ((uint32) 3u << CY_SYS_CPUSS_SL_CTL_ARB_SHIFT))

#endif /* (CY_IP_DMAC_PRESENT) */


#if (CY_IP_DMAC_PRESENT)
    #define CY_SYS_RAM_ACCESS_ARB_PRIORITY_CPU              (0u)
    #define CY_SYS_RAM_ACCESS_ARB_PRIORITY_DMA              (1u)
    #define CY_SYS_RAM_ACCESS_ARB_PRIORITY_ROUND            (2u)
    #define CY_SYS_RAM_ACCESS_ARB_PRIORITY_ROUND_STICKY     (3u)
#endif /* (CY_IP_DMAC_PRESENT) */


/*******************************************************************************
* Programmable Voltage Reference API
*******************************************************************************/
#if (CY_IP_PASS)

    #define CYFLD_PASS_VREF_ENABLE__OFFSET     (CYFLD_PASS_VREF0_ENABLE__OFFSET )
    #define CYFLD_PASS_VREF_ENABLE__SIZE       (CYFLD_PASS_VREF0_ENABLE__SIZE   )
    #define CYFLD_PASS_VREF_SUP_SEL__OFFSET    (CYFLD_PASS_VREF0_SUP_SEL__OFFSET)
    #define CYFLD_PASS_VREF_SUP_SEL__SIZE      (CYFLD_PASS_VREF0_SUP_SEL__SIZE  )
    #define CYFLD_PASS_VREF_SEL__OFFSET        (CYFLD_PASS_VREF0_SEL__OFFSET    )
    #define CYFLD_PASS_VREF_SEL__SIZE          (CYFLD_PASS_VREF0_SEL__SIZE      )

    /* CySysSetGlobalVrefSource() */
    #define CY_SYS_VREF_SOURCE_BG           (0u)
    #define CY_SYS_VREF_SOURCE_VDDA         (1u)

    /* CySysSetGlobalVrefBgGain() */
    #define CY_SYS_VREF_BG_GAINx1           (1u)
    #define CY_SYS_VREF_BG_GAINx2           (2u)

    #ifdef CyDesignWideVoltageReference_PRB_REF
        #define CYREG_PASS_PRB_REF          (CyDesignWideVoltageReference_PRB_REF)
    #endif

    #define  CY_SYS_VREF_BG_BUFFER_TRIM_SIGN_BIT    (0x20u)

#endif /* (CY_IP_PASS) */


/***************************************
* Registers
***************************************/


/*******************************************************************************
* Clocks API Registers
*******************************************************************************/
#define CY_SYS_CLK_IMO_TRIM1_REG            (*(reg32 *) CYREG_CLK_IMO_TRIM1)
#define CY_SYS_CLK_IMO_TRIM1_PTR            ( (reg32 *) CYREG_CLK_IMO_TRIM1)

#define CY_SYS_CLK_IMO_TRIM2_REG            (*(reg32 *) CYREG_CLK_IMO_TRIM2)
#define CY_SYS_CLK_IMO_TRIM2_PTR            ( (reg32 *) CYREG_CLK_IMO_TRIM2)

#define CY_SYS_CLK_IMO_TRIM3_REG            (*(reg32 *) CYREG_CLK_IMO_TRIM3)
#define CY_SYS_CLK_IMO_TRIM3_PTR            ( (reg32 *) CYREG_CLK_IMO_TRIM3)

#if(CY_IP_SRSSV2)
    #define CY_SYS_CLK_IMO_TRIM4_REG            (*(reg32 *) CYREG_CLK_IMO_TRIM4)
    #define CY_SYS_CLK_IMO_TRIM4_PTR            ( (reg32 *) CYREG_CLK_IMO_TRIM4)
#endif /* (CY_IP_SRSSV2) */

#define CY_SYS_CLK_IMO_CONFIG_REG           (*(reg32 *) CYREG_CLK_IMO_CONFIG)
#define CY_SYS_CLK_IMO_CONFIG_PTR           ( (reg32 *) CYREG_CLK_IMO_CONFIG)


#define CY_SYS_CLK_SELECT_REG               (*(reg32 *) CYREG_CLK_SELECT)
#define CY_SYS_CLK_SELECT_PTR               ( (reg32 *) CYREG_CLK_SELECT)

#if(CY_IP_SRSSV2)

    #if(CY_IP_HOBTO_DEVICE)
        #define CY_SFLASH_IMO_TRIM_REG(number)      ( ((reg8 *) CYREG_SFLASH_IMO_TRIM0)[number])
        #define CY_SFLASH_IMO_TRIM_PTR(number)      (&((reg8 *) CYREG_SFLASH_IMO_TRIM0)[number])
    #else
        #define CY_SFLASH_IMO_TRIM_REG(number)      ( ((reg8 *) CYREG_SFLASH_IMO_TRIM00)[number])
        #define CY_SFLASH_IMO_TRIM_PTR(number)      (&((reg8 *) CYREG_SFLASH_IMO_TRIM00)[number])
    #endif /* (CY_IP_HOBTO_DEVICE) */

    #define CY_SFLASH_USBMODE_IMO_GAIN_TRIM_REG     (*(reg8 *) CYREG_SFLASH_USBMODE_IMO_GAIN_TRIM)
    #define CY_SFLASH_USBMODE_IMO_GAIN_TRIM_PTR     ( (reg8 *) CYREG_SFLASH_USBMODE_IMO_GAIN_TRIM)
    
    #define CY_SFLASH_USBMODE_IMO_TEMPCO_REG        (*(reg8 *) CYREG_SFLASH_USBMODE_IMO_TEMPCO)
    #define CY_SFLASH_USBMODE_IMO_TEMPCO_PTR        ( (reg8 *) CYREG_SFLASH_USBMODE_IMO_TEMPCO)
    
    #define CY_SFLASH_CU_IMO_TRIM_USBMODE_24_REG    (*(reg8 *) CYREG_SFLASH_CU_IMO_TRIM_USBMODE_24)
    #define CY_SFLASH_CU_IMO_TRIM_USBMODE_24_PTR    ( (reg8 *) CYREG_SFLASH_CU_IMO_TRIM_USBMODE_24)

    #define CY_SFLASH_CU_IMO_TRIM_USBMODE_48_REG    (*(reg8 *) CYREG_SFLASH_CU_IMO_TRIM_USBMODE_48)
    #define CY_SFLASH_CU_IMO_TRIM_USBMODE_48_PTR    ( (reg8 *) CYREG_SFLASH_CU_IMO_TRIM_USBMODE_48)
    
    #define CY_SFLASH_S1_TESTPGM_REV_REG        (*(reg8 *) CYSFLASH_S1_testpgm_rev)
    #define CY_SFLASH_S1_TESTPGM_REV_PTR        ( (reg8 *) CYSFLASH_S1_testpgm_rev)

    #define CY_SFLASH_IMO_MAXF0_REG             (*(reg8 *) CYREG_SFLASH_IMO_MAXF0)
    #define CY_SFLASH_IMO_MAXF0_PTR             ( (reg8 *) CYREG_SFLASH_IMO_MAXF0)

    #define CY_SFLASH_IMO_ABS0_REG              (*(reg8 *) CYREG_SFLASH_IMO_ABS0)
    #define CY_SFLASH_IMO_ABS0_PTR              ( (reg8 *) CYREG_SFLASH_IMO_ABS0)

    #define CY_SFLASH_IMO_TMPCO0_REG            (*(reg8 *) CYREG_SFLASH_IMO_TMPCO0)
    #define CY_SFLASH_IMO_TMPCO0_PTR            ( (reg8 *) CYREG_SFLASH_IMO_TMPCO0)

    #define CY_SFLASH_IMO_MAXF1_REG             (*(reg8 *) CYREG_SFLASH_IMO_MAXF1)
    #define CY_SFLASH_IMO_MAXF1_PTR             ( (reg8 *) CYREG_SFLASH_IMO_MAXF1)

    #define CY_SFLASH_IMO_ABS1_REG              (*(reg8 *) CYREG_SFLASH_IMO_ABS1)
    #define CY_SFLASH_IMO_ABS1_PTR              ( (reg8 *) CYREG_SFLASH_IMO_ABS1)

    #define CY_SFLASH_IMO_TMPCO1_REG            (*(reg8 *) CYREG_SFLASH_IMO_TMPCO1)
    #define CY_SFLASH_IMO_TMPCO1_PTR            ( (reg8 *) CYREG_SFLASH_IMO_TMPCO1)

    #define CY_SFLASH_IMO_MAXF2_REG             (*(reg8 *) CYREG_SFLASH_IMO_MAXF2)
    #define CY_SFLASH_IMO_MAXF2_PTR             ( (reg8 *) CYREG_SFLASH_IMO_MAXF2)

    #define CY_SFLASH_IMO_ABS2_REG              (*(reg8 *) CYREG_SFLASH_IMO_ABS2)
    #define CY_SFLASH_IMO_ABS2_PTR              ( (reg8 *) CYREG_SFLASH_IMO_ABS2)

    #define CY_SFLASH_IMO_TMPCO2_REG            (*(reg8 *) CYREG_SFLASH_IMO_TMPCO2)
    #define CY_SFLASH_IMO_TMPCO2_PTR            ( (reg8 *) CYREG_SFLASH_IMO_TMPCO2)

    #define CY_SFLASH_IMO_MAXF3_REG             (*(reg8 *) CYREG_SFLASH_IMO_MAXF3)
    #define CY_SFLASH_IMO_MAXF3_PTR             ( (reg8 *) CYREG_SFLASH_IMO_MAXF3)

    #define CY_SFLASH_IMO_ABS3_REG              (*(reg8 *) CYREG_SFLASH_IMO_ABS3)
    #define CY_SFLASH_IMO_ABS3_PTR              ( (reg8 *) CYREG_SFLASH_IMO_ABS3)

    #define CY_SFLASH_IMO_TMPCO3_REG            (*(reg8 *) CYREG_SFLASH_IMO_TMPCO3)
    #define CY_SFLASH_IMO_TMPCO3_PTR            ( (reg8 *) CYREG_SFLASH_IMO_TMPCO3)

    #define CY_SFLASH_IMO_ABS4_REG              (*(reg8 *) CYREG_SFLASH_IMO_ABS4)
    #define CY_SFLASH_IMO_ABS4_PTR              ( (reg8 *) CYREG_SFLASH_IMO_ABS4)

    #define CY_SFLASH_IMO_TMPCO4_REG            (*(reg8 *) CYREG_SFLASH_IMO_TMPCO4)
    #define CY_SFLASH_IMO_TMPCO4_PTR            ( (reg8 *) CYREG_SFLASH_IMO_TMPCO4)

    #define CY_PWR_BG_TRIM4_REG                 (*(reg32 *) CYREG_PWR_BG_TRIM4)
    #define CY_PWR_BG_TRIM4_PTR                 ( (reg32 *) CYREG_PWR_BG_TRIM4)

    #define CY_PWR_BG_TRIM5_REG                 (*(reg32 *) CYREG_PWR_BG_TRIM5)
    #define CY_PWR_BG_TRIM5_PTR                 ( (reg32 *) CYREG_PWR_BG_TRIM5)

    #if (CY_IP_IMO_TRIMMABLE_BY_USB)

        #define CY_SFLASH_IMO_TRIM_USBMODE_24_REG   (*(reg8 *) CYREG_SFLASH_IMO_TRIM_USBMODE_24)
        #define CY_SFLASH_IMO_TRIM_USBMODE_24_PTR   ( (reg8 *) CYREG_SFLASH_IMO_TRIM_USBMODE_24)

        #define CY_SFLASH_IMO_TRIM_USBMODE_48_REG   (*(reg8 *) CYREG_SFLASH_IMO_TRIM_USBMODE_48)
        #define CY_SFLASH_IMO_TRIM_USBMODE_48_PTR   ( (reg8 *) CYREG_SFLASH_IMO_TRIM_USBMODE_48)

    #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB) */

#else

    #define CY_SYS_CLK_IMO_SELECT_REG           (*(reg32 *) CYREG_CLK_IMO_SELECT)
    #define CY_SYS_CLK_IMO_SELECT_PTR           ( (reg32 *) CYREG_CLK_IMO_SELECT)

    #define CY_SFLASH_IMO_TCTRIM_REG(number)    ( ((reg8 *) CYREG_SFLASH_IMO_TCTRIM_LT0)[number])
    #define CY_SFLASH_IMO_TCTRIM_PTR(number)    (&((reg8 *) CYREG_SFLASH_IMO_TCTRIM_LT0)[number])

    #define CY_SFLASH_IMO_TRIM_REG(number)      ( ((reg8 *) CYREG_SFLASH_IMO_TRIM_LT0)[number])
    #define CY_SFLASH_IMO_TRIM_PTR(number)      (&((reg8 *) CYREG_SFLASH_IMO_TRIM_LT0)[number])

    #if (CY_IP_IMO_TRIMMABLE_BY_USB && CY_IP_SRSSLT)

        #define CY_SFLASH_IMO_TRIM_USBMODE_24_REG   (*(reg8 *) CYREG_SFLASH_IMO_TRIM_USBMODE_24)
        #define CY_SFLASH_IMO_TRIM_USBMODE_24_PTR   ( (reg8 *) CYREG_SFLASH_IMO_TRIM_USBMODE_24)

        #define CY_SFLASH_IMO_TRIM_USBMODE_48_REG   (*(reg8 *) CYREG_SFLASH_IMO_TRIM_USBMODE_48)
        #define CY_SFLASH_IMO_TRIM_USBMODE_48_PTR   ( (reg8 *) CYREG_SFLASH_IMO_TRIM_USBMODE_48)
        
    #endif  /* (CY_IP_IMO_TRIMMABLE_BY_USB && CY_IP_SRSSLT) */

#endif  /* (CY_IP_SRSSV2) */

#if(CY_IP_IMO_TRIMMABLE_BY_USB)
    /* USB control 0 Register */
    #define CY_SYS_CLK_USBDEVv2_CR1_REG           (*(reg32 *) CYREG_USBDEVv2_CR1)
    #define CY_SYS_CLK_USBDEVv2_CR1_PTR           ( (reg32 *) CYREG_USBDEVv2_CR1)
#endif /* (CY_IP_IMO_TRIMMABLE_BY_USB) */


/*******************************************************************************
* ECO
*******************************************************************************/
#if (CY_IP_ECO)
    #if (CY_IP_ECO_BLESS)

        /* Radio configuration register */
        #define CY_SYS_XTAL_BLESS_RF_CONFIG_REG         (*(reg32 *) CYREG_BLE_BLESS_RF_CONFIG)
        #define CY_SYS_XTAL_BLESS_RF_CONFIG_PTR         ( (reg32 *) CYREG_BLE_BLESS_RF_CONFIG)

        /* RFCTRL mode transition control */
        #define CY_SYS_XTAL_BLERD_DBUS_REG              (*(reg32 *) CYREG_BLE_BLERD_DBUS)
        #define CY_SYS_XTAL_BLERD_DBUS_PTR              ( (reg32 *) CYREG_BLE_BLERD_DBUS)

        /* RFCTRL state information */
        #define CY_SYS_XTAL_BLERD_FSM_REG              (*(reg32 *) CYREG_BLE_BLERD_FSM)
        #define CY_SYS_XTAL_BLERD_FSM_PTR              ( (reg32 *) CYREG_BLE_BLERD_FSM)

        /* BB bump configuration 1 */
        #define CY_SYS_XTAL_BLERD_BB_XO_REG             (*(reg32 *) CYREG_BLE_BLERD_BB_XO)
        #define CY_SYS_XTAL_BLERD_BB_XO_PTR             ( (reg32 *) CYREG_BLE_BLERD_BB_XO)

        /* BB bump configuration 2 */
        #define CY_SYS_XTAL_BLERD_BB_XO_CAPTRIM_REG     (*(reg32 *) CYREG_BLE_BLERD_BB_XO_CAPTRIM)
        #define CY_SYS_XTAL_BLERD_BB_XO_CAPTRIM_PTR     ( (reg32 *) CYREG_BLE_BLERD_BB_XO_CAPTRIM)

        /* Crystal clock divider configuration register */
        #define CY_SYS_CLK_XTAL_CLK_DIV_CONFIG_REG      (*(reg32 *) CYREG_BLE_BLESS_XTAL_CLK_DIV_CONFIG)
        #define CY_SYS_CLK_XTAL_CLK_DIV_CONFIG_PTR      ( (reg32 *) CYREG_BLE_BLESS_XTAL_CLK_DIV_CONFIG)

    #elif (CY_IP_ECO_BLESSV3)
        /* Crystal clock divider configuration register */
        #define CY_SYS_CLK_XTAL_CLK_DIV_CONFIG_REG      (*(reg32 *) CYREG_BLE_BLESS_XTAL_CLK_DIV_CONFIG)
        #define CY_SYS_CLK_XTAL_CLK_DIV_CONFIG_PTR      ( (reg32 *) CYREG_BLE_BLESS_XTAL_CLK_DIV_CONFIG)
        
        /* RCB registers */
        #define CY_SYS_RCB_CTRL_REG                     (*(reg32 *) CYREG_BLE_RCB_CTRL)
        #define CY_SYS_RCB_CTRL_PTR                     ( (reg32 *) CYREG_BLE_RCB_CTRL)
        #define CY_SYS_RCB_TX_FIFO_WR_REG               (*(reg32 *) CYREG_BLE_RCB_TX_FIFO_WR)
        #define CY_SYS_RCB_TX_FIFO_WR_PTR               ( (reg32 *) CYREG_BLE_RCB_TX_FIFO_WR)
        #define CY_SYS_RCB_RX_FIFO_RD_REG               (*(reg32 *) CYREG_BLE_RCB_RX_FIFO_RD)
        #define CY_SYS_RCB_RX_FIFO_RD_PTR               ( (reg32 *) CYREG_BLE_RCB_RX_FIFO_RD)
        #define CY_SYS_RCB_INTR_REG                     (*(reg32 *) CYREG_BLE_RCB_INTR)
        #define CY_SYS_RCB_INTR_PTR                     ( (reg32 *) CYREG_BLE_RCB_INTR)
        #define CY_SYS_RCB_INTR_MASK_REG                (*(reg32 *) CYREG_BLE_RCB_INTR_MASK)
        #define CY_SYS_RCB_INTR_MASK_PTR                ( (reg32 *) CYREG_BLE_RCB_INTR_MASK)
        
                                                            
        /* BLESS registers */
        #define CY_SYS_BLESS_MT_CFG_REG                 (*(reg32 *) CYREG_BLE_BLESS_MT_CFG)
        #define CY_SYS_BLESS_MT_CFG_PTR                 ( (reg32 *) CYREG_BLE_BLESS_MT_CFG)
        #define CY_SYS_BLESS_MT_STATUS_REG              (*(reg32 *) CYREG_BLE_BLESS_MT_STATUS)
        #define CY_SYS_BLESS_MT_STATUS_PTR              ( (reg32 *) CYREG_BLE_BLESS_MT_STATUS)
        #define CY_SYS_BLESS_INTR_STAT_REG              (*(reg32 *) CYREG_BLE_BLESS_INTR_STAT)
        #define CY_SYS_BLESS_INTR_STAT_PTR              ( (reg32 *) CYREG_BLE_BLESS_INTR_STAT)
        #define CY_SYS_BLESS_INTR_MASK_REG              (*(reg32 *) CYREG_BLE_BLESS_INTR_MASK)
        #define CY_SYS_BLESS_INTR_MASK_PTR              ( (reg32 *) CYREG_BLE_BLESS_INTR_MASK)
        #define CY_SYS_BLESS_MT_DELAY_CFG_REG           (*(reg32 *) CYREG_BLE_BLESS_MT_DELAY_CFG)
        #define CY_SYS_BLESS_MT_DELAY_CFG_PTR           ( (reg32 *) CYREG_BLE_BLESS_MT_DELAY_CFG)
        #define CY_SYS_BLESS_MT_DELAY_CFG2_REG          (*(reg32 *) CYREG_BLE_BLESS_MT_DELAY_CFG2)
        #define CY_SYS_BLESS_MT_DELAY_CFG2_PTR          ( (reg32 *) CYREG_BLE_BLESS_MT_DELAY_CFG2)
        #define CY_SYS_BLESS_MT_DELAY_CFG3_REG          (*(reg32 *) CYREG_BLE_BLESS_MT_DELAY_CFG3)
        #define CY_SYS_BLESS_MT_DELAY_CFG3_PTR          ( (reg32 *) CYREG_BLE_BLESS_MT_DELAY_CFG3)
        
        /* BLELL registers */
        #define CY_SYS_BLELL_COMMAND_REG                (*(reg32 *) CYREG_BLE_BLELL_COMMAND_REGISTER)
        #define CY_SYS_BLELL_COMMAND_PTR                ( (reg32 *) CYREG_BLE_BLELL_COMMAND_REGISTER)        

    #elif (CY_IP_ECO_SRSSLT)

        /* ECO Clock Select Register */
        #define CY_SYS_ECO_CLK_SELECT_REG       (*(reg32 *) CYREG_EXCO_CLK_SELECT)
        #define CY_SYS_ECO_CLK_SELECT_PTR       ( (reg32 *) CYREG_EXCO_CLK_SELECT)
        
        /* ECO Configuration Register */
        #define CY_SYS_CLK_ECO_CONFIG_REG       (*(reg32 *) CYREG_EXCO_ECO_CONFIG)
        #define CY_SYS_CLK_ECO_CONFIG_PTR       ( (reg32 *) CYREG_EXCO_ECO_CONFIG)

        /* ECO Status Register */
        #define CY_SYS_CLK_ECO_STATUS_REG       (*(reg32 *) CYREG_EXCO_ECO_STATUS)
        #define CY_SYS_CLK_ECO_STATUS_PTR       ( (reg32 *) CYREG_EXCO_ECO_STATUS)

        /* PLL Configuration Register */
        #define CY_SYS_CLK_PLL0_CONFIG_REG      (*(reg32 *) CYREG_EXCO_PLL_CONFIG)
        #define CY_SYS_CLK_PLL0_CONFIG_PTR      ( (reg32 *) CYREG_EXCO_PLL_CONFIG)
        
        /* PLL Status Register */
        #define CY_SYS_CLK_PLL_STATUS_REG       (*(reg32 *) CYREG_EXCO_PLL_STATUS)
        #define CY_SYS_CLK_PLL_STATUS_PTR       ( (reg32 *) CYREG_EXCO_PLL_STATUS)

        #define CY_SYS_CLK_PLL_BASE             (*(volatile cy_sys_clk_pll_struct *) CYREG_EXCO_PLL_CONFIG)        
        
        /* ECO Trim0 Register */
        #define CY_SYS_CLK_ECO_TRIM0_REG        (*(reg32 *) CYREG_EXCO_ECO_TRIM0)
        #define CY_SYS_CLK_ECO_TRIM0_PTR        ( (reg32 *) CYREG_EXCO_ECO_TRIM0)

        /* ECO Trim1 Register */
        #define CY_SYS_CLK_ECO_TRIM1_REG        (*(reg32 *) CYREG_EXCO_ECO_TRIM1)
        #define CY_SYS_CLK_ECO_TRIM1_PTR        ( (reg32 *) CYREG_EXCO_ECO_TRIM1)
        
        /* PLL Trim Register */
        #define CY_SYS_CLK_PLL_TRIM_REG         (*(reg32 *) CYREG_EXCO_PLL_TRIM)
        #define CY_SYS_CLK_PLL_TRIM_PTR         ( (reg32 *) CYREG_EXCO_PLL_TRIM)
        
        #define CY_SYS_EXCO_PGM_CLK_REG         (*(reg32 *) CYREG_EXCO_EXCO_PGM_CLK) 
        #define CY_SYS_EXCO_PGM_CLK_PTR         ( (reg32 *) CYREG_EXCO_EXCO_PGM_CLK)
        
    #else
        /* ECO Configuration Register */
        #define CY_SYS_CLK_ECO_CONFIG_REG        (*(reg32 *) CYREG_CLK_ECO_CONFIG)
        #define CY_SYS_CLK_ECO_CONFIG_PTR        ( (reg32 *) CYREG_CLK_ECO_CONFIG)

        /* ECO Status Register */
        #define CY_SYS_CLK_ECO_STATUS_REG        (*(reg32 *) CYREG_CLK_ECO_STATUS)
        #define CY_SYS_CLK_ECO_STATUS_PTR        ( (reg32 *) CYREG_CLK_ECO_STATUS)

        /* ECO Trim0 Register */
        #define CY_SYS_CLK_ECO_TRIM0_REG         (*(reg32 *) CYREG_CLK_ECO_TRIM0)
        #define CY_SYS_CLK_ECO_TRIM0_PTR         ( (reg32 *) CYREG_CLK_ECO_TRIM0)

        /* ECO Trim1 Register */
        #define CY_SYS_CLK_ECO_TRIM1_REG         (*(reg32 *) CYREG_CLK_ECO_TRIM1)
        #define CY_SYS_CLK_ECO_TRIM1_PTR         ( (reg32 *) CYREG_CLK_ECO_TRIM1)
    #endif  /* (CY_IP_ECO_BLESS) */
#endif /* (CY_IP_ECO) */


/* CySysClkImoEnableWcoLock() / CySysClkImoDisableWcoLock() registers */
#if (CY_IP_IMO_TRIMMABLE_BY_WCO)
    /* WCO DPLL Register */
    #define CY_SYS_CLK_WCO_DPLL_REG                 (*(reg32 *) CYREG_WCO_DPLL)
    #define CY_SYS_CLK_WCO_DPLL_PTR                 ( (reg32 *) CYREG_WCO_DPLL)
#endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO) */


#if (CY_IP_IMO_TRIMMABLE_BY_WCO && CY_IP_IMO_TRIMMABLE_BY_USB)
    /* Oscillator Interface Control */
    #define CY_SYS_CLK_OSCINTF_CTL_REG              (*(reg32 *) CYREG_CLK_OSCINTF_CTL)
    #define CY_SYS_CLK_OSCINTF_CTL_PTR              ( (reg32 *) CYREG_CLK_OSCINTF_CTL)
#endif /* (CY_IP_IMO_TRIMMABLE_BY_WCO && CY_IP_IMO_TRIMMABLE_BY_USB) */


/*******************************************************************************
* PLL
*******************************************************************************/
#if (CY_IP_SRSSV2 && CY_IP_PLL)

    /* PLL #0 Configuration Register */
    #define CY_SYS_CLK_PLL0_CONFIG_REG      (*(reg32 *) CYREG_CLK_PLL0_CONFIG)
    #define CY_SYS_CLK_PLL0_CONFIG_PTR      ( (reg32 *) CYREG_CLK_PLL0_CONFIG)

    /* PLL #0 Status Register */
    #define CY_SYS_CLK_PLL0_STATUS_REG      (*(reg32 *) CYREG_CLK_PLL0_STATUS)
    #define CY_SYS_CLK_PLL0_STATUS_PTR      ( (reg32 *) CYREG_CLK_PLL0_STATUS)


    /* PLL #1 Configuration Register */
    #define CY_SYS_CLK_PLL1_CONFIG_REG      (*(reg32 *) CYREG_CLK_PLL1_CONFIG)
    #define CY_SYS_CLK_PLL1_CONFIG_PTR      ( (reg32 *) CYREG_CLK_PLL1_CONFIG)

    /* PLL #1 Status Register */
    #define CY_SYS_CLK_PLL1_STATUS_REG      (*(reg32 *) CYREG_CLK_PLL1_STATUS)
    #define CY_SYS_CLK_PLL1_STATUS_PTR      ( (reg32 *) CYREG_CLK_PLL1_STATUS)

    #define CY_SYS_CLK_PLL_BASE     (*(volatile cy_sys_clk_pll_struct *) CYREG_CLK_PLL0_CONFIG)

#endif /* (CY_IP_SRSSV2 && CY_IP_PLL) */


/*******************************************************************************
* System API Registers
*******************************************************************************/
#if (CY_IP_CPUSS_CM0)
    #define CY_SYS_AIRCR_REG                (*(reg32 *) CYREG_CM0_AIRCR)
    #define CY_SYS_AIRCR_PTR                ( (reg32 *) CYREG_CM0_AIRCR)
#else /* CY_IP_CPUSS_CM0PLUS */
    #define CY_SYS_AIRCR_REG                (*(reg32 *) CYREG_CM0P_AIRCR)
    #define CY_SYS_AIRCR_PTR                ( (reg32 *) CYREG_CM0P_AIRCR)
#endif /* (CY_IP_CPUSS_CM0) */

/* Reset Cause Observation Register */
#define CY_SYS_RES_CAUSE_REG                (*(reg32 *) CYREG_RES_CAUSE)
#define CY_SYS_RES_CAUSE_PTR                ( (reg32 *) CYREG_RES_CAUSE)

#if(CY_IP_SRSSV2)

    /*******************************************************************************
    * Low Voltage Detection
    *******************************************************************************/

    /* Voltage Monitoring Trim and Configuration */
    #define CY_LVD_PWR_VMON_CONFIG_REG          (*(reg32 *) CYREG_PWR_VMON_CONFIG)
    #define CY_LVD_PWR_VMON_CONFIG_PTR          ( (reg32 *) CYREG_PWR_VMON_CONFIG)

    /* Power System Interrupt Mask Register */
    #define CY_LVD_PWR_INTR_MASK_REG            (*(reg32 *) CYREG_PWR_INTR_MASK)
    #define CY_LVD_PWR_INTR_MASK_PTR            ( (reg32 *) CYREG_PWR_INTR_MASK)

    /* Power System Interrupt Register */
    #define CY_LVD_PWR_INTR_REG                 (*(reg32 *) CYREG_PWR_INTR)
    #define CY_LVD_PWR_INTR_PTR                 ( (reg32 *) CYREG_PWR_INTR)

#endif  /* (CY_IP_SRSSV2) */


/*******************************************************************************
* Interrupt API Registers
*******************************************************************************/
#define CY_INT_VECT_TABLE                   ( (cyisraddress **) CYDEV_SRAM_BASE)

#if (CY_IP_CPUSS_CM0)
    #define CY_INT_PRIORITY_REG(number)         ( ((reg32 *) CYREG_CM0_IPR0)[(number)/4u])
    #define CY_INT_PRIORITY_PTR(number)         (&((reg32 *) CYREG_CM0_IPR0)[(number)/4u])

    #define CY_INT_ENABLE_REG                   (*(reg32 *) CYREG_CM0_ISER)
    #define CY_INT_ENABLE_PTR                   ( (reg32 *) CYREG_CM0_ISER)

    #define CY_INT_CLEAR_REG                    (*(reg32 *) CYREG_CM0_ICER)
    #define CY_INT_CLEAR_PTR                    ( (reg32 *) CYREG_CM0_ICER)

    #define CY_INT_SET_PEND_REG                 (*(reg32 *) CYREG_CM0_ISPR)
    #define CY_INT_SET_PEND_PTR                 ( (reg32 *) CYREG_CM0_ISPR)

    #define CY_INT_CLR_PEND_REG                 (*(reg32 *) CYREG_CM0_ICPR)
    #define CY_INT_CLR_PEND_PTR                 ( (reg32 *) CYREG_CM0_ICPR)
#else /* CY_IP_CPUSS_CM0PLUS */
    #define CY_INT_PRIORITY_REG(number)         ( ((reg32 *) CYREG_CM0P_IPR0)[(number)/4u])
    #define CY_INT_PRIORITY_PTR(number)         (&((reg32 *) CYREG_CM0P_IPR0)[(number)/4u])

    #define CY_INT_ENABLE_REG                   (*(reg32 *) CYREG_CM0P_ISER)
    #define CY_INT_ENABLE_PTR                   ( (reg32 *) CYREG_CM0P_ISER)

    #define CY_INT_CLEAR_REG                    (*(reg32 *) CYREG_CM0P_ICER)
    #define CY_INT_CLEAR_PTR                    ( (reg32 *) CYREG_CM0P_ICER)

    #define CY_INT_SET_PEND_REG                 (*(reg32 *) CYREG_CM0P_ISPR)
    #define CY_INT_SET_PEND_PTR                 ( (reg32 *) CYREG_CM0P_ISPR)

    #define CY_INT_CLR_PEND_REG                 (*(reg32 *) CYREG_CM0P_ICPR)
    #define CY_INT_CLR_PEND_PTR                 ( (reg32 *) CYREG_CM0P_ICPR)
#endif /* (CY_IP_CPUSS_CM0) */

/*******************************************************************************
* System tick API Registers
*******************************************************************************/
#if (CY_IP_CPUSS_CM0)
    #define CY_SYS_SYST_CSR_REG                 (*(reg32 *) CYREG_CM0_SYST_CSR)
    #define CY_SYS_SYST_CSR_PTR                 ( (reg32 *) CYREG_CM0_SYST_CSR)

    #define CY_SYS_SYST_RVR_REG                 (*(reg32 *) CYREG_CM0_SYST_RVR)
    #define CY_SYS_SYST_RVR_PTR                 ( (reg32 *) CYREG_CM0_SYST_RVR)

    #define CY_SYS_SYST_CVR_REG                 (*(reg32 *) CYREG_CM0_SYST_CVR)
    #define CY_SYS_SYST_CVR_PTR                 ( (reg32 *) CYREG_CM0_SYST_CVR)

    #define CY_SYS_SYST_CALIB_REG               (*(reg32 *) CYREG_CM0_SYST_CALIB)
    #define CY_SYS_SYST_CALIB_PTR               ( (reg32 *) CYREG_CM0_SYST_CALIB)
#else /* CY_IP_CPUSS_CM0PLUS */
    #define CY_SYS_SYST_CSR_REG                 (*(reg32 *) CYREG_CM0P_SYST_CSR)
    #define CY_SYS_SYST_CSR_PTR                 ( (reg32 *) CYREG_CM0P_SYST_CSR)

    #define CY_SYS_SYST_RVR_REG                 (*(reg32 *) CYREG_CM0P_SYST_RVR)
    #define CY_SYS_SYST_RVR_PTR                 ( (reg32 *) CYREG_CM0P_SYST_RVR)

    #define CY_SYS_SYST_CVR_REG                 (*(reg32 *) CYREG_CM0P_SYST_CVR)
    #define CY_SYS_SYST_CVR_PTR                 ( (reg32 *) CYREG_CM0P_SYST_CVR)

    #define CY_SYS_SYST_CALIB_REG               (*(reg32 *) CYREG_CM0P_SYST_CALIB)
    #define CY_SYS_SYST_CALIB_PTR               ( (reg32 *) CYREG_CM0P_SYST_CALIB)
#endif /* (CY_IP_CPUSS_CM0) */


/*******************************************************************************
* Access Arbitration API Registers
*******************************************************************************/
#if (CY_IP_DMAC_PRESENT)
    /* RAM control register */
    #define CY_SYS_CPUSS_RAM_CTL_REG            (*(reg32 *) CYREG_CPUSS_RAM_CTL)
    #define CY_SYS_CPUSS_RAM_CTL_PTR            ( (reg32 *) CYREG_CPUSS_RAM_CTL)

    /* FLASH control register */
    #define CY_SYS_CPUSS_FLASH_CTL_REG          (*(reg32 *) CYREG_CPUSS_FLASH_CTL)
    #define CY_SYS_CPUSS_FLASH_CTL_PTR          ( (reg32 *) CYREG_CPUSS_FLASH_CTL)

    /* DMAC control register */
    #define CY_SYS_CPUSS_DMAC_CTL_REG            (*(reg32 *) CYREG_CPUSS_DMAC_CTL)
    #define CY_SYS_CPUSS_DMAC_CTL_PTR            ( (reg32 *) CYREG_CPUSS_DMAC_CTL)

    #if (CY_IP_SL_NR >= 1)
        /* Slave control register # 0 */
        #if (CY_IP_SL_NR == 1)
            #define CY_SYS_CPUSS_SL_CTL0_REG            (*(reg32 *) CYREG_CPUSS_SL_CTL)
            #define CY_SYS_CPUSS_SL_CTL0_PTR            ( (reg32 *) CYREG_CPUSS_SL_CTL)
        #else
            #define CY_SYS_CPUSS_SL_CTL0_REG            (*(reg32 *) CYREG_CPUSS_SL_CTL0)
            #define CY_SYS_CPUSS_SL_CTL0_PTR            ( (reg32 *) CYREG_CPUSS_SL_CTL0)
        #endif /* (CY_IP_SL_NR == 1) */
    #endif /* (CY_IP_SL_NR > 0) */

    #if (CY_IP_SL_NR >= 2)
        /* Slave control register # 1 */
        #define CY_SYS_CPUSS_SL_CTL1_REG            (*(reg32 *) CYREG_CPUSS_SL_CTL1)
        #define CY_SYS_CPUSS_SL_CTL1_PTR            ( (reg32 *) CYREG_CPUSS_SL_CTL1)
    #endif /* (CY_IP_SL_NR >= 1) */

    #if (CY_IP_SL_NR >= 3)
        /* Slave control register # 2 */
        #define CY_SYS_CPUSS_SL_CTL2_REG            (*(reg32 *) CYREG_CPUSS_SL_CTL2)
        #define CY_SYS_CPUSS_SL_CTL2_PTR            ( (reg32 *) CYREG_CPUSS_SL_CTL2)
    #endif /* (CY_IP_SL_NR >= 2) */

#endif /* (CY_IP_DMAC_PRESENT) */


/*******************************************************************************
* The following code is OBSOLETE and must not be used.
*
* If the obsoleted macro definitions are intended for use in the application,
* use the following scheme, redefine your own versions of these definitions:
*    #ifdef <OBSOLETED_DEFINE>
*        #undef  <OBSOLETED_DEFINE>
*        #define <OBSOLETED_DEFINE>      (<New Value>)
*    #endif
*
* Note: Redefine obsoleted macro definitions with caution. They might still be
*       used in the application and their modification might lead to unexpected
*       consequences.
*******************************************************************************/
#define CYINT_IRQ_BASE                     (CY_INT_IRQ_BASE)
#define CY_SYS_CLK_IMO_TRIM4_GAIN          (CY_SYS_CLK_IMO_TRIM4_USB_GAIN)

/* SFLASH0 block has been renamed to SFLASH */
#if (CY_PSOC4_4100 || CY_PSOC4_4200 || CY_PSOC4_4000U)
    #if !defined(CYREG_SFLASH_IMO_TRIM21)
        #define CYREG_SFLASH_IMO_TRIM21         (CYREG_SFLASH0_IMO_TRIM21)
    #endif  /* !defined(CYREG_SFLASH_IMO_TRIM21) */
#endif /* (CY_PSOC4_4100 || CY_PSOC4_4200 || CY_PSOC4_4000U) */

#if (CY_IP_CPUSS_CM0)

    #define CY_SYS_CM0_AIRCR_REG                (CY_SYS_AIRCR_REG)
    #define CY_SYS_CM0_AIRCR_PTR                (CY_SYS_AIRCR_PTR)

    #define CY_SYS_CM0_AIRCR_VECTKEY_SHIFT      (CY_SYS_AIRCR_VECTKEY_SHIFT    )
    #define CY_SYS_CM0_AIRCR_VECTKEY            (CY_SYS_AIRCR_VECTKEY          )
    #define CY_SYS_CM0_AIRCR_VECTKEY_MASK       (CY_SYS_AIRCR_VECTKEY_MASK     )
    #define CY_SYS_CM0_AIRCR_SYSRESETREQ_SHIFT  (CY_SYS_AIRCR_SYSRESETREQ_SHIFT)
    #define CY_SYS_CM0_AIRCR_SYSRESETREQ        (CY_SYS_AIRCR_SYSRESETREQ      )

#endif /* (CY_IP_CPUSS_CM0) */

#endif  /* CY_BOOT_CYLIB_H */


/* [] END OF FILE */
