/***************************************************************************//**
* \file .h
* \version 1.20
*
* \brief
*  This file provides the source code to API for the lfclk and wdt.
*
********************************************************************************
* \copyright
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_LFCLK_CYLIB_H)
#define CY_LFCLK_CYLIB_H

#include "cytypes.h"
#include "cydevice_trm.h"

#define CY_IP_WCO_WDT_EN           (1 == 1)

typedef enum
{
    CY_SYS_TIMER_WAIT = 0u,
    CY_SYS_TIMER_INTERRUPT = 1u
} cy_sys_timer_delaytype_enum;


/***************************************
*    Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
/* Clocks API */
void CySysClkIloStart(void);
void CySysClkIloStop(void);
/** @} general */

/**
* \addtogroup group_compensate
* @{
*/
cystatus CySysClkIloCompensate(uint32 desiredDelay, uint32 *compensatedCycles);
void CySysClkIloStartMeasurement(void);
void CySysClkIloStopMeasurement(void);
/** @} compensate */

#if(CY_IP_SRSSV2 && (!CY_IP_CPUSS))
    /**
    * \addtogroup group_compensate
    * @{
    */
    cystatus CySysClkIloTrim(uint32 mode, int32 *iloAccuracyInPPT);
    cystatus CySysClkIloRestoreFactoryTrim(void);
    /** @} compensate */
    cystatus CySysClkIloUpdateTrimReg(int32* iloAccuracyInPPT);
#endif /* (CY_IP_SRSSV2 && (!CY_IP_CPUSS)) */

#if(CY_IP_SRSSV2 && CY_IP_WCO)
    /**
    * \addtogroup group_general
    * @{
    */
    void CySysClkSetLfclkSource(uint32 source);
    /** @} group_general */
#endif /* (CY_IP_SRSSV2 && CY_IP_WCO) */

#if (CY_IP_WCO)
    /**
    * \addtogroup group_wco
    * @{
    */
    void   CySysClkWcoStart(void);
    void   CySysClkWcoStop(void);
    uint32 CySysClkWcoSetPowerMode(uint32 mode);
    void CySysClkWcoClockOutSelect(uint32 clockSel);
    /** @} wco */

    uint32 CySysClkWcoEnabled(void);

#endif /* (CY_IP_WCO) */

typedef void (*cyWdtCallback)(void);

#if (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN)

    typedef void (*cyTimerCallback)(void);
#endif /* (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN) */

#if(CY_IP_SRSSV2)
    /**
    * \addtogroup group_wdtsrssv2
    * @{
    */
    /* WDT API */
    void   CySysWdtLock(void);
    void   CySysWdtUnlock(void);
    void   CySysWdtSetMode(uint32 counterNum, uint32 mode);
    uint32 CySysWdtGetMode(uint32 counterNum);
    uint32 CySysWdtGetEnabledStatus(uint32 counterNum);
    void   CySysWdtSetClearOnMatch(uint32 counterNum, uint32 enable);
    uint32 CySysWdtGetClearOnMatch(uint32 counterNum);
    void   CySysWdtEnable(uint32 counterMask);
    void   CySysWdtDisable(uint32 counterMask);
    void   CySysWdtSetCascade(uint32 cascadeMask);
    uint32 CySysWdtGetCascade(void);
    void   CySysWdtSetMatch(uint32 counterNum, uint32 match);
    void   CySysWdtSetToggleBit(uint32 bits);
    uint32 CySysWdtGetToggleBit(void);
    uint32 CySysWdtGetMatch(uint32 counterNum);
    uint32 CySysWdtGetCount(uint32 counterNum);
    uint32 CySysWdtGetInterruptSource(void);
    void   CySysWdtClearInterrupt(uint32 counterMask);
    void   CySysWdtResetCounters(uint32 countersMask);
    cyWdtCallback  CySysWdtSetInterruptCallback(uint32 counterNum, cyWdtCallback function);
    cyWdtCallback  CySysWdtGetInterruptCallback(uint32 counterNum);
    void   CySysTimerDelay(uint32 counterNum, cy_sys_timer_delaytype_enum delayType, uint32 delay);
    void   CySysTimerDelayUntilMatch(uint32 counterNum, cy_sys_timer_delaytype_enum delayType, uint32 match);
    void   CySysWatchdogFeed(uint32 counterNum);
    void   CySysWdtEnableCounterIsr(uint32 counterNum);
    void   CySysWdtDisableCounterIsr(uint32 counterNum);
    void   CySysWdtIsr(void);
    /** @} wdtsrssv2 */
#else
    /**
    * \addtogroup group_wdtsrsslite
    * @{
    */
    /* WDT API */
    uint32 CySysWdtGetEnabledStatus(void);
    void   CySysWdtEnable(void);
    void   CySysWdtDisable(void);
    void   CySysWdtSetMatch(uint32 match);
    uint32 CySysWdtGetMatch(void);
    uint32 CySysWdtGetCount(void);
    void   CySysWdtSetIgnoreBits(uint32 bitsNum);
    uint32 CySysWdtGetIgnoreBits(void);
    void   CySysWdtClearInterrupt(void);
    void   CySysWdtMaskInterrupt(void);
    void   CySysWdtUnmaskInterrupt(void);
    cyWdtCallback CySysWdtSetInterruptCallback(cyWdtCallback function);
    cyWdtCallback CySysWdtGetInterruptCallback(void);
    void   CySysWdtIsr(void);
    /** @} wdtsrsslite*/
#endif  /* (CY_IP_SRSSV2) */


#if(CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN)
    /**
    * \addtogroup group_deepsleepwdt
    * @{
    */
    /* WCO WDT APIs */
    void   CySysClkSetTimerSource(uint32 source);
    void   CySysTimerSetMode(uint32 counterNum, uint32 mode);
    uint32 CySysTimerGetMode(uint32 counterNum);
    uint32 CySysTimerGetEnabledStatus(uint32 counterNum);
    void   CySysTimerSetClearOnMatch(uint32 counterNum, uint32 enable);
    uint32 CySysTimerGetClearOnMatch(uint32 counterNum);
    void   CySysTimerEnable(uint32 counterMask);
    void   CySysTimerDisable(uint32 counterMask);
    void   CySysTimerSetCascade(uint32 cascadeMask);
    uint32 CySysTimerGetCascade(void);
    void   CySysTimerSetMatch(uint32 counterNum, uint32 match);
    void   CySysTimerSetToggleBit(uint32 bits);
    uint32 CySysTimerGetToggleBit(void);
    uint32 CySysTimerGetMatch(uint32 counterNum);
    uint32 CySysTimerGetCount(uint32 counterNum);
    uint32 CySysTimerGetInterruptSource(void);
    void   CySysTimerClearInterrupt(uint32 counterMask);
    cyTimerCallback CySysTimerSetInterruptCallback(uint32 counterNum, cyTimerCallback function);
    cyTimerCallback CySysTimerGetInterruptCallback(uint32 counterNum);
    void   CySysTimerDelay(uint32 counterNum, cy_sys_timer_delaytype_enum delayType, uint32 delay);
    void   CySysTimerDelayUntilMatch(uint32 counterNum, cy_sys_timer_delaytype_enum delayType, uint32 match);
    void   CySysTimerResetCounters(uint32 countersMask);
    void   CySysTimerEnableIsr(uint32 counterNum);
    void   CySysTimerDisableIsr(uint32 counterNum);
    void   CySysTimerIsr(void);
    /** @} deepsleepwdt */
#endif /* (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN) */


/*******************************************************************************
* API Constants
*******************************************************************************/
#define CY_SYS_UINT16_MAX_VAL    (0xFFFFu)


/*******************************************************************************
* Clock API Constants
*******************************************************************************/

/* CySysClkIloStart()/CySysClkIloStop() - implementation definitions */
#define CY_SYS_CLK_ILO_CONFIG_ENABLE                    ((uint32)(( uint32 )0x01u << 31u))
#define CY_SYS_CLK_DFT_SELECT_DEFAULT_MASK              ((uint32)(( uint32 )0x0fu << 8u ))

/* CySysClkIloCompensate() - one ILO clock in uS multiplied on thousand */
#if (CY_IP_SRSSV2)
    #define CY_SYS_CLK_ILO_PERIOD_PPH                   ((uint32) (0x0C35u))
#else
    #define CY_SYS_CLK_ILO_PERIOD_PPH                   ((uint32) (0x09C4u))
#endif /* (CY_IP_SRSSV2) */

/* CySysClkIloCompensate() - implementation definitions */
#define CY_SYS_CLK_ILO_CALIBR_COMPLETE_MASK             ((uint32)(( uint32 )0x01u << 31u))
#define CY_SYS_CLK_ILO_DFT_LSB_MASK                     ((uint32)(0x00000FFFu))
#define CY_SYS_CLK_TRIM_OR_COMP_STARTED                 (1u)
#define CY_SYS_CLK_TRIM_OR_COMP_FINISHED                (0u)
#define CY_SYS_CLK_COEF_PHUNDRED                        ((uint32) (0x64u))
#define CY_SYS_CLK_HALF_OF_CLOCK                        ((uint32) ((uint32) CY_SYS_CLK_ILO_PERIOD_PPH >> 2u))

/* CySysClkIloCompensate() - maximum value of desiredDelay argument */
#if (CY_IP_SRSSV2)
    #define CY_SYS_CLK_MAX_DELAY_US                     ((uint32) (0xEE6B2800u))
    #define CY_SYS_CLK_ILO_PERIOD                       ((uint32) (0x1Fu))
    #define CY_SYS_CLK_ILO_FREQ_2MSB                    ((uint32) 5u)
#else
    #define CY_SYS_CLK_MAX_DELAY_US                     ((uint32) (0x1E8480u))
    #define CY_SYS_CLK_ILO_FREQ_2MSB                    ((uint32) (0x28u ))

    /**********************************************************************************
    * CySysClkIloCompensate() - value to walk over oversamling in calculations with 
    * srsslite. The oversample can be obtained when ilo frequency in equal 80 KHz and
    * desired clocks are 80 000 clocks.
    **********************************************************************************/
    #define CY_SYS_CLK_MAX_LITE_NUMBER                     ((uint32) 53600u)
#endif /* (CY_IP_SRSSV2) */

#define CY_SYS_CLK_ILO_FREQ_3LSB                       ((uint32) (0x3E8u))
#define CY_SYS_CLK_DELAY_COUNTS_LIMIT                  ((uint32) (0xD160u))
#define CY_SYS_CLK_MIN_DELAY_US                        ((uint32) (0x64u))

/* CySysClkSetLfclkSource() - parameter definitions */
#if (CY_IP_SRSSV2 && CY_IP_WCO)

    /** Internal Low Frequency (32 kHz) Oscillator (ILO) */
    #define CY_SYS_CLK_LFCLK_SRC_ILO                    (0u)

    /** Low Frequency Watch Crystal Oscillator (WCO) */
    #define CY_SYS_CLK_LFCLK_SRC_WCO                    ((uint32)(( uint32 )0x01u << 30u))
#endif /* (CY_IP_SRSSV2 && CY_IP_WCO) */


#if (CY_IP_WCO)

    /* CySysClkSetLfclkSource() - implementation definitions */
    #define CY_SYS_CLK_LFCLK_SEL_MASK                   ((uint32)(( uint32 )0x03u << 30u))
#endif /* (CY_IP_WCO) */

/* CySysClkSetTimerSource() - implementation definitions */
#if (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN)
    #define CY_SYS_CLK_TIMER_SEL_MASK                   ((uint32)(( uint32 )0x03u << 30u))
#endif /* (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN) */

/* CySysClkSetTimerSource() - parameter definitions */
#if (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN)

    /** Internal Low Frequency (32 kHz) Oscillator (ILO) */
    #define CY_SYS_CLK_TIMER_SRC_ILO                (0u)

    /** Low Frequency Watch Crystal Oscillator (WCO) */
    #define CY_SYS_CLK_TIMER_SRC_WCO                ((uint32)(( uint32 )0x01u << 30u))
#endif /* (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN) */

/* CySysClkWcoClockOutSelect() - parameter definitions */
#if (CY_IP_WCO)

    /** Selects External crystal as WCO’s clock source */
    #define CY_SYS_CLK_WCO_SEL_CRYSTAL                  (1u)

    /** Selects External clock input on wco_in pin as WCO’s clock source */
    #define CY_SYS_CLK_WCO_SEL_PIN                      (0u)
#endif /* (CY_IP_WCO) */

/* CySysClkWcoClockOutSelect() - implementation definitions */
#if (CY_IP_WCO)
    #define CY_SYS_CLK_WCO_SELECT_PIN_MASK              ((uint32)(( uint32 )0x01u << 2u))
    #define CY_SYS_CLK_WCO_SELECT_PIN_OFFSET            ((uint32) 0x02u)
#endif /* (CY_IP_WCO) */

/* CySysClkIloRestoreFactoryTrim() - implementation definitions */
#if (CY_IP_SRSSV2 && CY_IP_WCO && (!CY_IP_CPUSS))
    #define CY_SYS_CLK_ILO_TRIM_DEFAULT_VALUE           ((uint8 )(0xF0u))
    #define CY_SYS_CLK_ILO_TRIM_DEFAULT_MASK            ((uint32)((uint32)0x01u << 3u))
    #define CY_SYS_CLK_ILO_TRIM_MASK                    ((uint32)(0x0Fu))
#endif /* (CY_IP_SRSSV2 && CY_IP_WCO && (!CY_IP_CPUSS)) */

/* CySysIloTrim() - parameter definitions and macros*/
#if (CY_IP_SRSSV2 && CY_IP_WCO && (!CY_IP_CPUSS))
    #define CY_SYS_CLK_BLOCKING                         (0u)
    #define CY_SYS_CLK_NON_BLOCKING                     (1u)
    #define CY_SYS_CLK_PERTHOUSAND                      ((uint32) 0x000003E8u )
    #define CY_SYS_CLK_ABS_MACRO(x)                     ((0 > (x)) ? (-(x)) : (x))
    #define CY_SYS_CLK_ERROR_RANGE                      ((uint32) 0x38u)
    #define CY_SYS_CLK_TIMEOUT                          ((uint8 ) 0x05u)

    /* ILO error step is 7,37 % error range */
    #define CY_SYS_CLK_ERROR_STEP                       (( int32) 0x02E1u)
    #define CY_SYS_CLK_ERROR_COEF                       ((uint32) 0x0Au)
#endif /* (CY_IP_SRSSV2 && CY_IP_WCO && (!CY_IP_CPUSS)) */

#if (CY_IP_WCO)

    /* WCO Configuration Register */
    #define CY_SYS_CLK_WCO_CONFIG_LPM_EN                (( uint32 )(( uint32 )0x01u <<  0u))
    #define CY_SYS_CLK_WCO_CONFIG_LPM_AUTO              (( uint32 )(( uint32 )0x01u <<  1u))
    #define CY_SYS_CLK_WCO_CONFIG_LPM_ENABLE            (( uint32 )(( uint32 )0x01u << 31u))

    /* WCO Status Register */
    #define CY_SYS_CLK_WCO_STATUS_OUT_BLNK_A            (( uint32 )(( uint32 )0x01u <<  0u))

    /* WCO Trim Register */
    #define CY_SYS_CLK_WCO_TRIM_XGM_MASK                (( uint32 ) 0x07u)
    #define CY_SYS_CLK_WCO_TRIM_XGM_SHIFT               (( uint32 ) 0x00u)

    #define CY_SYS_CLK_WCO_TRIM_XGM_3370NA              (( uint32 ) 0x00u)
    #define CY_SYS_CLK_WCO_TRIM_XGM_2620NA              (( uint32 ) 0x01u)
    #define CY_SYS_CLK_WCO_TRIM_XGM_2250NA              (( uint32 ) 0x02u)
    #define CY_SYS_CLK_WCO_TRIM_XGM_1500NA              (( uint32 ) 0x03u)
    #define CY_SYS_CLK_WCO_TRIM_XGM_1870NA              (( uint32 ) 0x04u)
    #define CY_SYS_CLK_WCO_TRIM_XGM_1120NA              (( uint32 ) 0x05u)
    #define CY_SYS_CLK_WCO_TRIM_XGM_750NA               (( uint32 ) 0x06u)
    #define CY_SYS_CLK_WCO_TRIM_XGM_0NA                 (( uint32 ) 0x07u)

    #define CY_SYS_CLK_WCO_TRIM_GM_MASK                 (( uint32 )(( uint32 )0x03u << 4u))
    #define CY_SYS_CLK_WCO_TRIM_GM_SHIFT                (( uint32 ) 0x04u)
    #define CY_SYS_CLK_WCO_TRIM_GM_HPM                  (( uint32 ) 0x01u)
    #define CY_SYS_CLK_WCO_TRIM_GM_LPM                  (( uint32 ) 0x02u)
#endif  /* (CY_IP_WCO) */


/*******************************************************************************
* WDT API Constants
*******************************************************************************/
#if(CY_IP_SRSSV2)

    #define CY_SYS_WDT_MODE_NONE                (0u)
    #define CY_SYS_WDT_MODE_INT                 (1u)
    #define CY_SYS_WDT_MODE_RESET               (2u)
    #define CY_SYS_WDT_MODE_INT_RESET           (3u)

    #define CY_SYS_WDT_COUNTER0_MASK            ((uint32)((uint32)0x01u))           /**< Counter 0 */
    #define CY_SYS_WDT_COUNTER1_MASK            ((uint32)((uint32)0x01u << 8u))     /**< Counter 1 */
    #define CY_SYS_WDT_COUNTER2_MASK            ((uint32)((uint32)0x01u << 16u))    /**< Counter 2 */

    #define CY_SYS_WDT_CASCADE_NONE             ((uint32)0x00u)             /**< Neither */
    #define CY_SYS_WDT_CASCADE_01               ((uint32)0x01u << 3u)       /**< Cascade 01 */
    #define CY_SYS_WDT_CASCADE_12               ((uint32)0x01u << 11u)      /**< Cascade 12 */

    #define CY_SYS_WDT_COUNTER0_INT             ((uint32)0x01u << 2u)
    #define CY_SYS_WDT_COUNTER1_INT             ((uint32)0x01u << 10u)
    #define CY_SYS_WDT_COUNTER2_INT             ((uint32)0x01u << 18u)

    #define CY_SYS_WDT_COUNTER0_RESET           ((uint32)0x01u << 3u)       /**< Counter 0 */
    #define CY_SYS_WDT_COUNTER1_RESET           ((uint32)0x01u << 11u)      /**< Counter 1 */
    #define CY_SYS_WDT_COUNTER2_RESET           ((uint32)0x01u << 19u)      /**< Counter 2 */

    #define CY_SYS_WDT_COUNTERS_RESET           (CY_SYS_WDT_COUNTER0_RESET |\
                                                 CY_SYS_WDT_COUNTER1_RESET |\
                                                 CY_SYS_WDT_COUNTER2_RESET)

    #define CY_SYS_WDT_COUNTER0                 (0x00u)
    #define CY_SYS_WDT_COUNTER1                 (0x01u)
    #define CY_SYS_WDT_COUNTER2                 (0x02u)

    #define CY_SYS_WDT_COUNTER0_OFFSET          (0x00u)
    #define CY_SYS_WDT_COUNTER1_OFFSET          (0x02u)
    #define CY_SYS_WDT_COUNTER2_OFFSET          (0x04u)

    #define CY_SYS_WDT_MODE_MASK                ((uint32)(0x03u))

    #define CY_SYS_WDT_CONFIG_BITS2_MASK        (uint32)(0x1Fu)
    #define CY_SYS_WDT_CONFIG_BITS2_POS         (uint32)(24u)
    #define CY_SYS_WDT_LOWER_16BITS_MASK        (uint32)(0x0000FFFFu)
    #define CY_SYS_WDT_HIGHER_16BITS_MASK       (uint32)(0xFFFF0000u)
    #define CY_SYS_WDT_COUNTERS_MAX             (0x03u)
    #define CY_SYS_WDT_CNT_SHIFT                (0x08u)
    #define CY_SYS_WDT_CNT_MATCH_CLR_SHIFT      (0x02u)
    #define CY_SYS_WDT_CNT_STTS_SHIFT           (0x01u)
    #define CY_SYS_WDT_CNT_MATCH_SHIFT          (0x10u)

    #define CY_SYS_WDT_CLK_LOCK_BITS_MASK       ((uint32)0x03u << 14u)
    #define CY_SYS_WDT_CLK_LOCK_BIT0            ((uint32)0x01u << 14u)
    #define CY_SYS_WDT_CLK_LOCK_BIT1            ((uint32)0x01u << 15u)

    #define CY_WDT_NUM_OF_WDT                   (3u)
    #define CY_WDT_NUM_OF_CALLBACKS             (3u)

    #else
        #define CY_WDT_NUM_OF_WDT               (1u)
        #define CY_WDT_NUM_OF_CALLBACKS         (3u)
        #define CY_SYS_WDT_KEY                  ((uint32)(0xACED8865u))
        #define CY_SYS_WDT_MATCH_MASK           ((uint32)(0x0000FFFFu))
        #define CY_SYS_WDT_IGNORE_BITS_MASK     ((uint32)(0x000F0000u))
        #define CY_SYS_WDT_IGNORE_BITS_SHIFT    ((uint32)(16u))
        #define CY_SYS_WDT_LOWER_BIT_MASK       ((uint32)(0x00000001u))

        #define CY_SYS_WDT_COUNTER0             (0x00u)

#endif /* (CY_IP_SRSSV2) */

#if (CY_IP_SRSSV2 && CY_IP_WCO)
    #define CY_SYS_WDT_1LFCLK_ILO_DELAY_US    ((uint16)( 67u))
    #define CY_SYS_WDT_3LFCLK_ILO_DELAY_US    ((uint16)(201u))
    #define CY_SYS_WDT_1LFCLK_WCO_DELAY_US    ((uint16)( 31u))
    #define CY_SYS_WDT_3LFCLK_WCO_DELAY_US    ((uint16)( 93u))

    #define CY_SYS_WDT_1LFCLK_DELAY_US      \
        ((CY_SYS_CLK_LFCLK_SRC_ILO == (CY_SYS_WDT_CONFIG_REG & CY_SYS_CLK_LFCLK_SEL_MASK)) ? \
                                            (CY_SYS_WDT_1LFCLK_ILO_DELAY_US) : \
                                            (CY_SYS_WDT_1LFCLK_WCO_DELAY_US))

    #define CY_SYS_WDT_3LFCLK_DELAY_US      \
        ((CY_SYS_CLK_LFCLK_SRC_ILO == (CY_SYS_WDT_CONFIG_REG & CY_SYS_CLK_LFCLK_SEL_MASK)) ? \
                                            (CY_SYS_WDT_3LFCLK_ILO_DELAY_US) : \
                                            (CY_SYS_WDT_3LFCLK_WCO_DELAY_US))
    #else
        #define CY_SYS_WDT_1LFCLK_DELAY_US      ((uint16)  (67u))
        #define CY_SYS_WDT_3LFCLK_DELAY_US      ((uint16) (201u))
#endif /* (CY_IP_SRSSV2 && CY_IP_WCO) */

#if (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN)

    #define CY_SYS_TIMER_MODE_NONE              (0u)
    #define CY_SYS_TIMER_MODE_INT               (1u)

    #define CY_SYS_TIMER0_MASK                  ((uint32)((uint32)0x01u))           /**< Counter 0 */
    #define CY_SYS_TIMER1_MASK                  ((uint32)((uint32)0x01u << 8u))     /**< Counter 1 */
    #define CY_SYS_TIMER2_MASK                  ((uint32)((uint32)0x01u << 16u))    /**< Counter 2 */

    #define CY_SYS_TIMER0_RESET                 ((uint32)0x01u << 3u)       /**< Counter 0 */
    #define CY_SYS_TIMER1_RESET                 ((uint32)0x01u << 11u)      /**< Counter 1 */
    #define CY_SYS_TIMER2_RESET                 ((uint32)0x01u << 19u)      /**< Counter 2 */

    #define CY_SYS_TIMER_RESET                  (CY_SYS_TIMER0_RESET |\
                                                 CY_SYS_TIMER1_RESET |\
                                                 CY_SYS_TIMER2_RESET)

    #define CY_SYS_TIMER_CASCADE_NONE           ((uint32)0x00u)             /**< Neither */
    #define CY_SYS_TIMER_CASCADE_01             ((uint32)0x01u << 3u)       /**< Cascade 01 */
    #define CY_SYS_TIMER_CASCADE_12             ((uint32)0x01u << 11u)      /**< Cascade 12 */

    #define CY_SYS_TIMER0_INT                   ((uint32)0x01u << 2u)
    #define CY_SYS_TIMER1_INT                   ((uint32)0x01u << 10u)
    #define CY_SYS_TIMER2_INT                   ((uint32)0x01u << 18u)

    #define CY_SYS_TIMER0                       (0x00u)
    #define CY_SYS_TIMER1                       (0x01u)
    #define CY_SYS_TIMER2                       (0x02u)

    #define CY_SYS_TIMER_MODE_MASK              ((uint32)(0x01u))

    #define CY_SYS_TIMER_CONFIG_BITS2_MASK      (uint32)(0x1Fu)
    #define CY_SYS_TIMER_CONFIG_BITS2_POS       (uint32)(24u)
    #define CY_SYS_TIMER_LOWER_16BITS_MASK      (uint32)(0x0000FFFFu)
    #define CY_SYS_TIMER_HIGHER_16BITS_MASK     (uint32)(0xFFFF0000u)
    #define CY_SYS_TIMER_COUNTERS_MAX           (0x03u)
    #define CY_SYS_TIMER_CNT_SHIFT              (0x08u)
    #define CY_SYS_TIMER_CNT_MATCH_CLR_SHIFT    (0x02u)
    #define CY_SYS_TIMER_CNT_STTS_SHIFT         (0x01u)
    #define CY_SYS_TIMER_CNT_MATCH_SHIFT        (0x10u)

    #define CY_SYS_NUM_OF_TIMERS                (3u)

    #define CY_SYS_SET_NEW_TIMER_SOURCE_ILO     ((uint16)(0x02u))
    #define CY_SYS_SET_NEW_TIMER_SOURCE_WCO     ((uint16)(0x01u))
    #define CY_SYS_WCO_WDT_CLKEN_RESET_MASK     ((uint32)(0x03u))

    #define CY_SYS_TIMER_1ILO_DELAY_US          ((uint16)( 67u))
    #define CY_SYS_TIMER_4ILO_DELAY_US          ((uint16)(268u))
    #define CY_SYS_TIMER_3ILO_DELAY_US          ((uint16)(201u))

    #define CY_SYS_TIMER_1WCO_DELAY_US          ((uint16)( 31u))
    #define CY_SYS_TIMER_4WCO_DELAY_US          ((uint16)(124u))
    #define CY_SYS_TIMER_3WCO_DELAY_US          ((uint16)( 93u))

    #define CY_SYS_1TIMER_DELAY_US      \
        ((CY_SYS_CLK_TIMER_SRC_ILO == (CY_SYS_WCO_WDT_CONFIG_REG & CY_SYS_CLK_TIMER_SEL_MASK)) ? \
                                            (CY_SYS_TIMER_1ILO_DELAY_US) : \
                                            (CY_SYS_TIMER_1WCO_DELAY_US))

    #define CY_SYS_4TIMER_DELAY_US      \
        ((CY_SYS_CLK_TIMER_SRC_ILO == (CY_SYS_WCO_WDT_CONFIG_REG & CY_SYS_CLK_TIMER_SEL_MASK)) ? \
                                            (CY_SYS_TIMER_4WCO_DELAY_US) : \
                                            (CY_SYS_TIMER_4ILO_DELAY_US))

    #define CY_SYS_3TIMER_DELAY_US      \
        ((CY_SYS_CLK_TIMER_SRC_ILO == (CY_SYS_WCO_WDT_CONFIG_REG & CY_SYS_CLK_TIMER_SEL_MASK)) ? \
                                            (CY_SYS_TIMER_3ILO_DELAY_US) : \
                                            (CY_SYS_TIMER_3WCO_DELAY_US))

    #define CY_SYS_SET_CURRENT_TIMER_SOURCE_BIT      \
        ((CY_SYS_CLK_TIMER_SRC_ILO == (CY_SYS_WCO_WDT_CONFIG_REG & CY_SYS_CLK_TIMER_SEL_MASK)) ? \
                                            (CY_SYS_SET_NEW_TIMER_SOURCE_ILO) : \
                                            (CY_SYS_SET_NEW_TIMER_SOURCE_WCO))

#endif /* (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN) */

/* CySysClkWcoSetPowerMode() */
#define CY_SYS_CLK_WCO_HPM                              (0x0u)     /**< WCO High power mode */

#if(CY_IP_BLESS)
    #define CY_SYS_CLK_WCO_LPM                          (0x1u)     /**< WCO Low power mode */
#endif /* (CY_IP_BLESS) */


/*******************************************************************************
*       Trim Registers Constants 
********************************************************************************/
#define CY_SYS_CLK_SYS_CLK_DEVIDER                  ((uint32)0x0Au)
#define CY_SYS_CLK_SEL_ILO_DFT_SOURCE               ((uint32)0x00000100u)
#define CY_SYS_CLK_FOURBITS_MAX                     (( int32)0x0f)
#define CY_SYS_CLK_HALF_OF_STEP                     (( int32)((uint32) CY_SYS_CLK_ERROR_STEP >> 1u))

#if(CY_IP_SRSSV2)
    #define CY_SYS_CLK_ILO_DESIRED_FREQ_HZ          (32000u)
    #define CY_SYS_CLK_DFT_SELSIZE                  ((uint32) 0x3F)
#else
    #define CY_SYS_CLK_ILO_DESIRED_FREQ_HZ          (40000u)
    #define CY_SYS_CLK_DFT_SELSIZE                  ((uint32) 0x0F)
#endif /* (CY_IP_SRSSV2) */

#define CY_SYS_TST_DDFT_CTRL_REG_DEFAULT_MASK    ((uint32)((CY_SYS_CLK_DFT_SELSIZE << 8u) | (CY_SYS_CLK_DFT_SELSIZE )))
#define CY_SYS_TST_DDFT_SELECT_CLK1              ((uint32) ((uint32) CYDEV_DFT_SELECT_CLK1 << 8u))
#define CY_SYS_TST_DDFT_CTRL_REG_SEL2_CLK1       ((uint32) (CY_SYS_TST_DDFT_SELECT_CLK1 | CYDEV_DFT_SELECT_CLK0))


/*******************************************************************************
*       Trim Registers 
********************************************************************************/
/* DFT TST Control Register*/
#define CY_SYS_TST_DDFT_CTRL_REG                         (*(reg32*) CYREG_TST_DDFT_CTRL)
#define CY_SYS_CNT_CTRL_PTR                              ( (reg32*) CYREG_TST_DDFT_CTRL)

/* DFT TST Counter 1 Register*/
#define CY_SYS_CNT_REG1_REG                              (*(reg32*) CYREG_TST_TRIM_CNTR1)
#define CY_SYS_CNT_REG1_PTR                              ( (reg32*) CYREG_TST_TRIM_CNTR1)

/* DFT TST Counter 2 Register*/
#define CY_SYS_CNT_REG2_REG                              (*(reg32*) CYREG_TST_TRIM_CNTR2)
#define CY_SYS_CNT_REG2_PTR                              ( (reg32*) CYREG_TST_TRIM_CNTR2)

/* DFT Muxes Configuration Register*/
#define CY_SYS_CLK_DFT_REG                               (*(reg32*) CYREG_CLK_DFT_SELECT)
#define CY_SYS_CLK_DFT_PTR                               ( (reg32*) CYREG_CLK_DFT_SELECT)

/* ILO Configuration Register*/
#define CY_SYS_CLK_ILO_CONFIG_REG                        (*(reg32 *) CYREG_CLK_ILO_CONFIG)
#define CY_SYS_CLK_ILO_CONFIG_PTR                        ( (reg32 *) CYREG_CLK_ILO_CONFIG)

/* ILO Trim Register*/
#if(CY_IP_SRSSV2 && CY_IP_WCO)
    #define CY_SYS_CLK_ILO_TRIM_REG                      (*(reg32 *) CYREG_CLK_ILO_TRIM)
    #define CY_SYS_CLK_ILO_TRIM_PTR                      ( (reg32 *) CYREG_CLK_ILO_TRIM)
#endif  /* (CY_IP_SRSSV2) && CY_IP_WCO*/

#if (CY_IP_WCO)
    #if (CY_IP_BLESS)

        /* WCO Status Register */
        #define CY_SYS_CLK_WCO_STATUS_REG               (*(reg32 *) CYREG_BLE_BLESS_WCO_STATUS)
        #define CY_SYS_CLK_WCO_STATUS_PTR               ( (reg32 *) CYREG_BLE_BLESS_WCO_STATUS)

        /* WCO Configuration Register */
        #define CY_SYS_CLK_WCO_CONFIG_REG               (*(reg32 *) CYREG_BLE_BLESS_WCO_CONFIG)
        #define CY_SYS_CLK_WCO_CONFIG_PTR               ( (reg32 *) CYREG_BLE_BLESS_WCO_CONFIG)

        /* WCO Trim Register */
        #define CY_SYS_CLK_WCO_TRIM_REG                 (*(reg32 *) CYREG_BLE_BLESS_WCO_TRIM)
        #define CY_SYS_CLK_WCO_TRIM_PTR                 ( (reg32 *) CYREG_BLE_BLESS_WCO_TRIM)
    #else

        /* WCO Status Register */
        #define CY_SYS_CLK_WCO_STATUS_REG               (*(reg32 *) CYREG_WCO_STATUS)
        #define CY_SYS_CLK_WCO_STATUS_PTR               ( (reg32 *) CYREG_WCO_STATUS)

        /* WCO Configuration Register */
        #define CY_SYS_CLK_WCO_CONFIG_REG               (*(reg32 *) CYREG_WCO_CONFIG)
        #define CY_SYS_CLK_WCO_CONFIG_PTR               ( (reg32 *) CYREG_WCO_CONFIG)

        /* WCO Trim Register */
        #define CY_SYS_CLK_WCO_TRIM_REG                 (*(reg32 *) CYREG_WCO_TRIM)
        #define CY_SYS_CLK_WCO_TRIM_PTR                 ( (reg32 *) CYREG_WCO_TRIM)
    #endif /* (CY_IP_BLESS) */
#endif  /* (CY_IP_WCO) */


/*******************************************************************************
* WDT API Registers
*******************************************************************************/
#if(CY_IP_SRSSV2)
    #define CY_SYS_WDT_CTRLOW_REG               (*(reg32 *) CYREG_WDT_CTRLOW)
    #define CY_SYS_WDT_CTRLOW_PTR               ( (reg32 *) CYREG_WDT_CTRLOW)

    #define CY_SYS_WDT_CTRHIGH_REG              (*(reg32 *) CYREG_WDT_CTRHIGH)
    #define CY_SYS_WDT_CTRHIGH_PTR              ( (reg32 *) CYREG_WDT_CTRHIGH)

    #define CY_SYS_WDT_MATCH_REG                (*(reg32 *) CYREG_WDT_MATCH)
    #define CY_SYS_WDT_MATCH_PTR                ( (reg32 *) CYREG_WDT_MATCH)

    #define CY_SYS_WDT_CONFIG_REG               (*(reg32 *) CYREG_WDT_CONFIG)
    #define CY_SYS_WDT_CONFIG_PTR               ( (reg32 *) CYREG_WDT_CONFIG)

    #define CY_SYS_WDT_CONTROL_REG              (*(reg32 *) CYREG_WDT_CONTROL)
    #define CY_SYS_WDT_CONTROL_PTR              ( (reg32 *) CYREG_WDT_CONTROL)
#else
    #define CY_SYS_WDT_DISABLE_KEY_REG          (*(reg32 *) CYREG_WDT_DISABLE_KEY)
    #define CY_SYS_WDT_DISABLE_KEY_PTR          ( (reg32 *) CYREG_WDT_DISABLE_KEY)

    #define CY_SYS_WDT_MATCH_REG                (*(reg32 *) CYREG_WDT_MATCH)
    #define CY_SYS_WDT_MATCH_PTR                ( (reg32 *) CYREG_WDT_MATCH)

    #define CY_SYS_WDT_COUNTER_REG              (*(reg32 *) CYREG_WDT_COUNTER)
    #define CY_SYS_WDT_COUNTER_PTR              ( (reg32 *) CYREG_WDT_COUNTER)

    #define CY_SYS_SRSS_INTR_REG                (*(reg32 *) CYREG_SRSS_INTR)
    #define CY_SYS_SRSS_INTR_PTR                ( (reg32 *) CYREG_SRSS_INTR)

    #define CY_SYS_SRSS_INTR_MASK_REG           (*(reg32 *) CYREG_SRSS_INTR_MASK)
    #define CY_SYS_SRSS_INTR_MASK_PTR           ( (reg32 *) CYREG_SRSS_INTR_MASK)
#endif  /* (CY_IP_SRSSV2) */

#if (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN)
    #define CY_SYS_WCO_WDT_CTRLOW_REG           (*(reg32 *) CYREG_WCO_WDT_CTRLOW)
    #define CY_SYS_WCO_WDT_CTRLOW_PTR           ( (reg32 *) CYREG_WCO_WDT_CTRLOW)

    #define CY_SYS_WCO_WDT_CTRHIGH_REG          (*(reg32 *) CYREG_WCO_WDT_CTRHIGH)
    #define CY_SYS_WCO_WDT_CTRHIGH_PTR          ( (reg32 *) CYREG_WCO_WDT_CTRHIGH)

    #define CY_SYS_WCO_WDT_MATCH_REG            (*(reg32 *) CYREG_WCO_WDT_MATCH)
    #define CY_SYS_WCO_WDT_MATCH_PTR            ( (reg32 *) CYREG_WCO_WDT_MATCH)

    #define CY_SYS_WCO_WDT_CONFIG_REG           (*(reg32 *) CYREG_WCO_WDT_CONFIG)
    #define CY_SYS_WCO_WDT_CONFIG_PTR           ( (reg32 *) CYREG_WCO_WDT_CONFIG)

    #define CY_SYS_WCO_WDT_CONTROL_REG          (*(reg32 *) CYREG_WCO_WDT_CONTROL)
    #define CY_SYS_WCO_WDT_CONTROL_PTR          ( (reg32 *) CYREG_WCO_WDT_CONTROL)

    #define CY_SYS_WCO_WDT_CLKEN_REG            (*(reg32 *) CYREG_WCO_WDT_CLKEN)
    #define CY_SYS_WCO_WDT_CLKEN_PTR            ( (reg32 *) CYREG_WCO_WDT_CLKEN)
#endif /* (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN) */

#if (CY_IP_WCO)

    /*******************************************************************************
    * Function Name: CySysClkWcoSetHighPowerMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the high power mode for the 32 KHz WCO.
    *
    *******************************************************************************/
    static CY_INLINE void CySysClkWcoSetHighPowerMode(void)
    {
        /* Switch off low power mode for WCO */
        CY_SYS_CLK_WCO_CONFIG_REG &= (uint32) ~CY_SYS_CLK_WCO_CONFIG_LPM_EN;

        /* Switch off auto low power mode in WCO */
        CY_SYS_CLK_WCO_CONFIG_REG &= ((uint32)~CY_SYS_CLK_WCO_CONFIG_LPM_AUTO);

        /* Restore WCO trim register HPM settings */
        CY_SYS_CLK_WCO_TRIM_REG = (CY_SYS_CLK_WCO_TRIM_REG & (uint32)(~CY_SYS_CLK_WCO_TRIM_GM_MASK)) \
                    | (uint32)(CY_SYS_CLK_WCO_TRIM_GM_HPM << CY_SYS_CLK_WCO_TRIM_GM_SHIFT);
        CY_SYS_CLK_WCO_TRIM_REG = (CY_SYS_CLK_WCO_TRIM_REG & (uint32)(~CY_SYS_CLK_WCO_TRIM_XGM_MASK)) \
                    | (uint32)(CY_SYS_CLK_WCO_TRIM_XGM_2620NA << CY_SYS_CLK_WCO_TRIM_XGM_SHIFT);
    }

    #if(CY_IP_BLESS)
        /*******************************************************************************
        * Function Name: CySysClkWcoSetLowPowerMode
        ********************************************************************************
        *
        * Summary:
        *  Sets the low power mode for the 32 KHz WCO.
        *
        *  Note LPM available only for PSoC 4100 BLE / PSoC4 4200 BLE
        *******************************************************************************/
        static CY_INLINE void CySysClkWcoSetLowPowerMode(void)
        {
            /* Switch off auto low power mode in WCO */
            CY_SYS_CLK_WCO_CONFIG_REG &= ((uint32)~CY_SYS_CLK_WCO_CONFIG_LPM_AUTO);

            /* Change WCO trim register settings to LPM */
            CY_SYS_CLK_WCO_TRIM_REG = (CY_SYS_CLK_WCO_TRIM_REG & (uint32)(~CY_SYS_CLK_WCO_TRIM_XGM_MASK)) \
                        | (uint32)(CY_SYS_CLK_WCO_TRIM_XGM_2250NA << CY_SYS_CLK_WCO_TRIM_XGM_SHIFT);
            CY_SYS_CLK_WCO_TRIM_REG = (CY_SYS_CLK_WCO_TRIM_REG & (uint32)(~CY_SYS_CLK_WCO_TRIM_GM_MASK)) \
                        | (uint32)(CY_SYS_CLK_WCO_TRIM_GM_LPM << CY_SYS_CLK_WCO_TRIM_GM_SHIFT);

            /* Switch on low power mode for WCO */
            CY_SYS_CLK_WCO_CONFIG_REG |= CY_SYS_CLK_WCO_CONFIG_LPM_EN;
        }
    #endif /* (CY_IP_BLESS) */

#endif /* (CY_IP_WCO) */


/* These defines are intended to maintain the backward compatibility for
 * projects which use cy_boot_v4_20 or earlier.
*/
#define CySysWdtWriteMode           CySysWdtSetMode
#define CySysWdtReadMode            CySysWdtGetMode
#define CySysWdtWriteClearOnMatch   CySysWdtSetClearOnMatch
#define CySysWdtReadClearOnMatch    CySysWdtGetClearOnMatch
#define CySysWdtReadEnabledStatus   CySysWdtGetEnabledStatus
#define CySysWdtWriteCascade        CySysWdtSetCascade
#define CySysWdtReadCascade         CySysWdtGetCascade
#define CySysWdtWriteMatch          CySysWdtSetMatch
#define CySysWdtWriteToggleBit      CySysWdtSetToggleBit
#define CySysWdtReadToggleBit       CySysWdtGetToggleBit
#define CySysWdtReadMatch           CySysWdtGetMatch
#define CySysWdtReadCount           CySysWdtGetCount
#define CySysWdtWriteIgnoreBits     CySysWdtSetIgnoreBits
#define CySysWdtReadIgnoreBits      CySysWdtGetIgnoreBits
#define CySysWdtSetIsrCallback      CySysWdtSetInterruptCallback
#define CySysWdtGetIsrCallback      CySysWdtGetInterruptCallback

#endif /* (CY_LFCLK_CYLIB_H) */

/* [] END OF FILE */
