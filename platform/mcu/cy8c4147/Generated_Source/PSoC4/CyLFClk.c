/***************************************************************************//**
* \file .c
* \version 1.20
*
* \brief
*  This file provides the source code for configuring watchdog timers WDTs, 
*  low frequency clocks (LFCLK) and the Real-time Clock (RTC) component in
*  PSoC Creator for the PSoC 4 families.
*
********************************************************************************
* \copyright
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "CyLFClk.h"
#include "CyLib.h"

#if (CY_IP_WCO && CY_IP_SRSSV2)
    static uint32 CySysClkGetLfclkSource(void);
#endif /* (CY_IP_WCO && CY_IP_SRSSV2) */


#if(CY_IP_SRSSV2 && (!CY_IP_CPUSS))
    /* Default Ilo Trim Register value for ILO trimming*/
    static volatile uint16 defaultIloTrimRegValue = CY_SYS_CLK_ILO_TRIM_DEFAULT_VALUE;
#endif /* (CY_IP_SRSSV2 && (!CY_IP_CPUSS)) */

#if(CY_IP_SRSSV2)
    /* CySysClkLfclkPosedgeCatch() / CySysClkLfclkPosedgeRestore() */
    static uint32 lfclkPosedgeWdtCounter0Enabled = 0u;
    static uint32 lfclkPosedgeWdtCounter0Mode    = CY_SYS_WDT_MODE_NONE;

    static volatile uint32 disableServicedIsr = 0uL;
    static volatile uint32 wdtIsrMask = CY_SYS_WDT_COUNTER0_INT |\
                                        CY_SYS_WDT_COUNTER1_INT |\
                                        CY_SYS_WDT_COUNTER2_INT;

    static const uint32 counterIntMaskTbl[CY_WDT_NUM_OF_WDT] = {CY_SYS_WDT_COUNTER0_INT,
                                                                CY_SYS_WDT_COUNTER1_INT,
                                                                CY_SYS_WDT_COUNTER2_INT};

    static void CySysClkLfclkPosedgeCatch(void);
    static void CySysClkLfclkPosedgeRestore(void);

    static uint32 CySysWdtLocked(void);
    static uint32 CySysClkIloEnabled(void);
#endif /* (CY_IP_SRSSV2) */

#if (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN)
    static uint32 CySysClkGetTimerSource(void);
    static volatile uint32 disableTimerServicedIsr = 0uL;
    static volatile uint32 timerIsrMask = CY_SYS_TIMER0_INT |\
                                          CY_SYS_TIMER1_INT |\
                                          CY_SYS_TIMER2_INT;

    static const uint32 counterTimerIntMaskTbl[CY_SYS_NUM_OF_TIMERS] = {CY_SYS_TIMER0_INT,
                                                                        CY_SYS_TIMER1_INT,
                                                                        CY_SYS_TIMER2_INT};

    static cyTimerCallback cySysTimerCallback[CY_SYS_NUM_OF_TIMERS] = {(void *)0, (void *)0, (void *)0};
#endif /* (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_DWT_EN) */

#if(CY_IP_SRSSV2)
    static cyWdtCallback cySysWdtCallback[CY_WDT_NUM_OF_WDT] = {(void *)0, (void *)0, (void *)0};
#else
    static cyWdtCallback cySysWdtCallback = (void *)0;
#endif /* (CY_IP_SRSSV2) */


/*******************************************************************************
* Function Name: CySysClkIloStart
****************************************************************************//**
* \brief
*  Enables ILO.
*
*  Refer to the device datasheet for the ILO startup time.
*
*******************************************************************************/
void CySysClkIloStart(void)
{
    CY_SYS_CLK_ILO_CONFIG_REG |= CY_SYS_CLK_ILO_CONFIG_ENABLE;
}


/*******************************************************************************
* Function Name: CySysClkIloStop
****************************************************************************//**
* \brief
*  Disables the ILO.
*
*  This function has no effect if WDT is locked (CySysWdtLock() is
*  called). Call CySysWdtUnlock() to unlock WDT and stop ILO.
*
*  PSoC 4100 / PSoC 4200: Note that ILO is required for WDT's operation.
*
*  PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4200L / PSoC 4100M / 
*  PSoC 4200M:
*  Stopping ILO affects the peripheral clocked by LFCLK, if
*  LFCLK is configured to be sourced by ILO.
*
*  If the ILO is disabled, all blocks run by ILO will stop functioning.
*
*******************************************************************************/
void CySysClkIloStop(void)
{
    #if(CY_IP_SRSSV2)
        uint8  interruptState;

        /* Do nothing if WDT is locked or ILO is disabled */
        if (0u == CySysWdtLocked())
        {
            if (0u != CySysClkIloEnabled())
            {

            #if (CY_IP_WCO)
                if (CY_SYS_CLK_LFCLK_SRC_ILO == CySysClkGetLfclkSource())
                {
            #endif /* (CY_IP_WCO) */

                interruptState = CyEnterCriticalSection();
                CySysClkLfclkPosedgeCatch();
                CY_SYS_CLK_ILO_CONFIG_REG &= (uint32) ( ~(uint32)CY_SYS_CLK_ILO_CONFIG_ENABLE);
                CySysClkLfclkPosedgeRestore();
                CyExitCriticalSection(interruptState);

            #if (CY_IP_WCO)
                }
                else /* Safe to disable - shortened pulse does not impact peripheral */
                {
                    CY_SYS_CLK_ILO_CONFIG_REG &= (uint32) ( ~(uint32)CY_SYS_CLK_ILO_CONFIG_ENABLE);
                }
            #endif /* (CY_IP_WCO) */

            }
        }
    #else
        CY_SYS_CLK_ILO_CONFIG_REG &= ( uint32 ) ( ~( uint32 )CY_SYS_CLK_ILO_CONFIG_ENABLE);
    #endif /* (CY_IP_SRSSV2) */
}


/******************************************************************************
* Function Name: CySysClkIloStartMeasurement
***************************************************************************//**
* \brief
* Starts the ILO accuracy measurement.
*
* This function is non-blocking and needs to be called before using the 
* CySysClkIloTrim() and CySysClkIloCompensate() API.
*
* This API configures measurement counters to be sourced by SysClk (Counter 1)
* and ILO (Counter 2). 
*
* \note SysClk should be sourced by IMO. Otherwise CySysClkIloTrim() and 
* CySysClkIloCompensate() API can give incorrect results.
*
* In addition, this API stores the factory ILO trim settings on the first call 
* after reset. This stored factory setting is used by the 
* CySysClkIloRestoreFactoryTrim() API to restore the ILO factory trim. 
* Hence, it is important to call this API before restoring the ILO 
* factory trim settings.
*
******************************************************************************/
void CySysClkIloStartMeasurement(void)
{
#if(CY_IP_SRSSV2 && (!CY_IP_CPUSS))
    static uint8 iloTrimTrig = 0u;

    /* Write default ILO trim value while ILO starting ( Cypress ID 225244 )*/
    if (0u == iloTrimTrig)
    {
        defaultIloTrimRegValue = ((uint8)(CY_SYS_CLK_ILO_TRIM_REG & CY_SYS_CLK_ILO_TRIM_MASK));
        iloTrimTrig = 1u;
    }
#endif /* (CY_IP_SRSSV2 && (!CY_IP_CPUSS)) */

    /* Configure measurement counters to source by SysClk (Counter 1) and ILO (Counter 2)*/
    CY_SYS_CLK_DFT_REG = (CY_SYS_CLK_DFT_REG & (uint32) ~CY_SYS_CLK_DFT_SELECT_DEFAULT_MASK) |
                            CY_SYS_CLK_SEL_ILO_DFT_SOURCE; 

    CY_SYS_TST_DDFT_CTRL_REG = (CY_SYS_TST_DDFT_CTRL_REG & (uint32) ~ CY_SYS_TST_DDFT_CTRL_REG_DEFAULT_MASK) |
                            CY_SYS_TST_DDFT_CTRL_REG_SEL2_CLK1;
}


/******************************************************************************
* Function Name: CySysClkIloStopMeasurement
***************************************************************************//**
* \brief
* Stops the ILO accuracy measurement.
*
* Calling this function immediately stops the the ILO frequency measurement.
* This function should be called before placing the device to deepsleep, if 
* CySysClkIloStartMeasurement() API was called before. 
*
******************************************************************************/
void CySysClkIloStopMeasurement(void)
{
    /* Set default configurations in 11...8 DFT register bits to zero */
    CY_SYS_CLK_DFT_REG &= ~CY_SYS_CLK_DFT_SELECT_DEFAULT_MASK;
    #if(CY_IP_SRSSLT)
        CY_SYS_TST_DDFT_CTRL_REG &= ((uint32) CY_SYS_TST_DDFT_CTRL_REG_DEFAULT_MASK);
    #endif /* (CY_IP_SRSSLT) */
}


/******************************************************************************
* Function Name: CySysClkIloCompensate
***************************************************************************//**
* \brief
* This API measures the current ILO accuracy.
*
* Basing on the measured frequency the required number of ILO cycles for a 
* given delay (in microseconds) is obtained. The desired delay that needs to 
* be compensated is passed through the desiredDelay parameter. The compensated 
* cycle count is returned through the compesatedCycles pointer.
* The compensated ILO cycles can then be used to define the WDT period value, 
* effectively compensating for the ILO inaccuracy and allowing a more 
* accurate WDT interrupt generation.
*
* CySysClkIloStartMeasurement() API should be called prior to calling this API. 
*
* \note SysClk should be sourced by IMO. Otherwise CySysClkIloTrim() and 
* CySysClkIloCompensate() API can give incorrect results.
*
* \note If the System clock frequency is changed in runtime, the CyDelayFreq() 
* with the appropriate parameter (Frequency of bus clock in Hertz) should be 
* called before calling a next CySysClkIloCompensate().
*
* \warning Do not enter deep sleep mode until the function returns CYRET_SUCCESS.
*
* \param desiredDelay Required delay in microseconds.
*
* \param *compensatedCycles The pointer to the variable in which the required
* number of ILO cycles for the given delay will be returned.
*
* \details
* The value returned in *compensatedCycles pointer is not valid until the 
* function returns CYRET_SUCCESS.
*
* The desiredDelay parameter value should be in next range: <br>From 100 to 
* 2 000 000 microseconds for PSoC 4000 / PSoC 4000S / PSoC 4100S / PSoC Analog 
* Coprocessor devices.<br>From 100 to 4 000 000 000 microseconds for 
* PSoC 4100 / PSoC 4200 / PSoC 4100 BLE / PSoC 4200 BLE / PRoC BLE /
* PSoC 4200L / PSoC 4100M /PSoC 4200M devices.
*
* \return CYRET_SUCCESS - The compensation process is complete and the 
* compensated cycles value is returned in the compensatedCycles pointer.
*
* \return CYRET_STARTED - Indicates measurement is in progress. It is 
* strongly recommended to do not make pauses between API calling. The 
* function should be called repeatedly until the API returns CYRET_SUCCESS.
*
* \return CYRET_INVALID_STATE - Indicates that measurement not started. 
* The user should call CySysClkIloStartMeasurement() API before calling 
* this API.
*
* \note For a correct WDT or DeepSleep Timers functioning with ILO compensating 
* the CySysClkIloCompensate() should be called before WDT or DeepSleep Timers 
* enabling.
*
*******************************************************************************/
cystatus CySysClkIloCompensate(uint32 desiredDelay , uint32* compensatedCycles)
{
    uint32 iloCompensatedCycles;
    uint32 desiredDelayInCounts;
    static uint32 compensateRunningStatus = CY_SYS_CLK_TRIM_OR_COMP_FINISHED;
    uint32 checkStatus;
    cystatus returnStatus;

    checkStatus = (uint32) (CY_SYS_CLK_DFT_REG & (uint32) CY_SYS_TST_DDFT_CTRL_REG_DEFAULT_MASK);

    /* Check if CySysStartMeasurement was called before */
    if((checkStatus == CY_SYS_CLK_SEL_ILO_DFT_SOURCE) && 
       (CY_SYS_TST_DDFT_CTRL_REG == CY_SYS_TST_DDFT_CTRL_REG_SEL2_CLK1) &&
       (CY_SYS_CLK_MAX_DELAY_US >= desiredDelay) &&
       (CY_SYS_CLK_MIN_DELAY_US <= desiredDelay) &&
       (compensatedCycles !=  NULL))
    {
        if(CY_SYS_CLK_TRIM_OR_COMP_FINISHED != compensateRunningStatus)
        {
            /* Wait until counter 1 stopped counting and after it calculate compensated cycles */
            if(0u != (CY_SYS_CNT_REG1_REG & CY_SYS_CLK_ILO_CALIBR_COMPLETE_MASK))
            {
                if (0u != CY_SYS_CNT_REG2_REG)
                {
                    /* Calculate required number of ILO cycles for given delay */
                    #if(CY_IP_SRSSV2)
                        if (CY_SYS_CLK_DELAY_COUNTS_LIMIT < desiredDelay)
                        {
                            desiredDelayInCounts = (desiredDelay / CY_SYS_CLK_ILO_PERIOD);
                            iloCompensatedCycles = 
                            (((CY_SYS_CNT_REG2_REG * cydelayFreqHz) / (cydelayFreqHz >> CY_SYS_CLK_SYS_CLK_DEVIDER)) >> 
                                CY_SYS_CLK_ILO_FREQ_2MSB) * (desiredDelayInCounts / CY_SYS_CLK_ILO_FREQ_3LSB);
                        }
                        else
                        {
                            desiredDelayInCounts = ((desiredDelay * CY_SYS_CLK_COEF_PHUNDRED) + 
                                                     CY_SYS_CLK_HALF_OF_CLOCK) / CY_SYS_CLK_ILO_PERIOD_PPH;

                            iloCompensatedCycles = (((CY_SYS_CNT_REG2_REG * cydelayFreqHz) / 
                                                     (cydelayFreqHz >> CY_SYS_CLK_SYS_CLK_DEVIDER)) * 
                                                     desiredDelayInCounts) / CY_SYS_CLK_ILO_DESIRED_FREQ_HZ;
                        }
                    #else /* (CY_IP_SRSSLT) */
                        desiredDelayInCounts = ((desiredDelay * CY_SYS_CLK_COEF_PHUNDRED) + CY_SYS_CLK_HALF_OF_CLOCK) /
                                                                                           CY_SYS_CLK_ILO_PERIOD_PPH;
                        if(CY_SYS_CLK_MAX_LITE_NUMBER < desiredDelayInCounts)
                        {
                            iloCompensatedCycles = (((CY_SYS_CNT_REG2_REG * cydelayFreqHz) / (cydelayFreqHz >> 
                                                   CY_SYS_CLK_SYS_CLK_DEVIDER)) / CY_SYS_CLK_ILO_FREQ_2MSB) * 
                                                   (desiredDelayInCounts / CY_SYS_CLK_ILO_FREQ_3LSB);
                        }
                        else
                        {
                            iloCompensatedCycles = (((CY_SYS_CNT_REG2_REG * cydelayFreqHz) / 
                                                     (cydelayFreqHz >> CY_SYS_CLK_SYS_CLK_DEVIDER)) * 
                                                     desiredDelayInCounts) / CY_SYS_CLK_ILO_DESIRED_FREQ_HZ;
                        }
                    #endif /* (CY_IP_SRSSV2) */

                    *compensatedCycles = iloCompensatedCycles;
                    compensateRunningStatus = CY_SYS_CLK_TRIM_OR_COMP_FINISHED;
                    returnStatus = CYRET_SUCCESS;
                }
                else
                {
                    returnStatus  = CYRET_INVALID_STATE;
                }
            }
            else
            {
                returnStatus = CYRET_STARTED;
            }
        }
        else
        {
            /* Reload CNTR 1 count value for next measurement cycle*/
            CY_SYS_CNT_REG1_REG = (cydelayFreqHz >> CY_SYS_CLK_SYS_CLK_DEVIDER);
            compensateRunningStatus = CY_SYS_CLK_TRIM_OR_COMP_STARTED;
            returnStatus = CYRET_STARTED;
        }
    }
    else
    {
        returnStatus = CYRET_INVALID_STATE;
    }

    return (returnStatus);
}


#if(CY_IP_SRSSV2)
    /*******************************************************************************
    * Function Name: CySysClkIloEnabled
    ****************************************************************************//**
    *
    * \internal
    * Reports the ILO enable state.
    *
    * \return
    * 1 if ILO is enabled, and 0 if ILO is disabled.
    *
    * \endinternal
    ********************************************************************************/
    static uint32 CySysClkIloEnabled(void)
    {
        /* Prohibits writing to WDT registers and ILO/WCO registers when not equal to 0 */
        return ((0u != (CY_SYS_CLK_ILO_CONFIG_REG & (uint32)(CY_SYS_CLK_ILO_CONFIG_ENABLE))) ?
            (uint32) 1u :
            (uint32) 0u);
    }
#endif /* (CY_IP_SRSSV2) */


#if(CY_IP_SRSSV2 && (!CY_IP_CPUSS))
/********************************************************************************
* Function Name: CySysClkIloTrim
*****************************************************************************//**
* \brief
* The API trims the ILO frequency to +/- 10% accuracy range using accurate 
* SysClk.
*
* The API can be blocking or non-blocking depending on the value of the mode 
* parameter passed. The accuracy of ILO after trimming in parts per thousand 
* is returned through the iloAccuracyInPPT pointer. A positive number indicates 
* that the ILO is running fast and a negative number indicates that the ILO is 
* running slowly. This error is relative to the error in the reference clock 
* (SysClk), so the absolute error will be higher and depends on the accuracy 
* of the reference.
*
* The CySysClkIloStartMeasurement() API should be called prior to calling this 
* API. Otherwise it will return CYRET_INVALID_STATE as the measurement was not 
* started.
*
* \note SysClk should be sourced by IMO. Otherwise CySysClkIloTrim() and 
* CySysClkIloCompensate() API can give incorrect results.
*
* \note If System clock frequency is changed in runtime, the CyDelayFreq() 
* with the appropriate parameter (Frequency of bus clock in Hertz) should be 
* called before next CySysClkIloCompensate() usage.
*
* \warning Do not enter deep sleep mode until the function returns CYRET_SUCCESS
* or CYRET_TIMEOUT.
*
* Available for all PSoC 4 devices with ILO trim capability. This excludes 
* PSoC 4000 / PSoC 4100 / PSoC 4200 / PSoC 4000S / PSoC 4100S / PSoC
* Analog Coprocessor devices.
*
* \param mode
*  CY_SYS_CLK_BLOCKING -  The function does not return until the ILO is 
* within +/-10% accuracy range or time out has occurred.<br>
* CY_SYS_CLK_NON_BLOCKING - The function returns immediately after 
* performing a single iteration of the trim process. The function should be 
* called repeatedly until the trimming is completed successfully.
*
* \param *iloAccuracyInPPT Pointer to an integer in which the trimmed ILO 
* accuracy will be returned.
*
* \details The value returned in *iloAccuracyInPPT pointer is not valid 
* until the function returns CYRET_SUCCESS. ILO accuracy in PPT is given by:
*
* IloAccuracyInPPT = ((MeasuredIloFreq - DesiredIloFreq) * 
* CY_SYS_CLK_PERTHOUSAND) / DesiredIloFreq);
*
* DesiredIloFreq  = 32000, CY_SYS_CLK_PERTHOUSAND = 1000;
*
* \return CYRET_SUCCESS - Indicates trimming is complete. This value indicates 
* trimming is successful and iloAccuracyInPPT is within +/- 10%.
*
* \return CYRET_STARTED - Indicates measurement is in progress. This is applicable 
* only for non-blocking mode.
*
* \return CYRET_INVALID_STATE - Indicates trimming was unsuccessful. You should 
* call CySysClkIloStartMeasurement() before calling this API.
*
* \return CYRET_TIMEOUT - Indicates trimming was unsuccessful. This is applicable 
* only for blocking mode. Timeout means the trimming was tried 5 times without 
* success (i.e. ILO accuracy > +/- 10%). The user can call the API again for 
* another try or wait for some time before calling it again (to let the system 
* to settle to another operating point change in temperature etc.) and continue 
* using the previous trim value till the next call.
*
**********************************************************************************/
cystatus CySysClkIloTrim(uint32 mode, int32* iloAccuracyInPPT)
{
    uint32 timeOutClocks = CY_SYS_CLK_TIMEOUT;
    uint32 waitUntilCntr1Stops;
    static uint32 trimRunningStatus = CY_SYS_CLK_TRIM_OR_COMP_FINISHED;
    uint32 checkStatus;
    cystatus returnStatus;

    checkStatus = (uint32) (CY_SYS_CLK_DFT_REG & (uint32) CY_SYS_TST_DDFT_CTRL_REG_DEFAULT_MASK);

    /* Check if DFT and CTRL registers were configures in CySysStartMeasurement*/
    if((checkStatus == CY_SYS_CLK_SEL_ILO_DFT_SOURCE) &&
       (CY_SYS_TST_DDFT_CTRL_REG == CY_SYS_TST_DDFT_CTRL_REG_SEL2_CLK1) &&
       (iloAccuracyInPPT != NULL))
    {
        if(CY_SYS_CLK_BLOCKING == mode)
        {
            waitUntilCntr1Stops = cydelayFreqHz >> CY_SYS_CLK_SYS_CLK_DEVIDER;
            do
            {
                /* Reload CNTR 1 count value for measuring cycle*/
                CY_SYS_CNT_REG1_REG = cydelayFreqHz >> CY_SYS_CLK_SYS_CLK_DEVIDER; 

                /* Wait until counter CNTR 1 will finish down-counting */
                while (0u == (CY_SYS_CNT_REG1_REG & CY_SYS_CLK_ILO_CALIBR_COMPLETE_MASK))
                {
                    waitUntilCntr1Stops--;
                    if (0u == waitUntilCntr1Stops)
                    {
                        break;
                    }
                }
                trimRunningStatus = CySysClkIloUpdateTrimReg(iloAccuracyInPPT);
                timeOutClocks--;

            /* Untill ILO accuracy will be in range less than +/- 10% or timeout occurs*/
            } while((CYRET_SUCCESS != trimRunningStatus) && 
              (CYRET_INVALID_STATE != trimRunningStatus) && 
                               (0u != timeOutClocks));

            if (CYRET_SUCCESS == trimRunningStatus)
            {
                returnStatus = CYRET_SUCCESS;
            }
            else
            {
                if(0u == timeOutClocks)
                {
                    trimRunningStatus = CY_SYS_CLK_TRIM_OR_COMP_FINISHED;
                    returnStatus = CYRET_TIMEOUT;
                }
                else
                {
                    returnStatus = CYRET_INVALID_STATE;
                }
            }
        }
        /* Non - blocking mode */
        else
        {
            if (CY_SYS_CLK_TRIM_OR_COMP_FINISHED != trimRunningStatus)
            {
                /* Checking if the counter CNTR 1 finished down-counting */
                if(0u != (CY_SYS_CNT_REG1_REG & CY_SYS_CLK_ILO_CALIBR_COMPLETE_MASK))
                {
                    returnStatus = CySysClkIloUpdateTrimReg(iloAccuracyInPPT);
                    trimRunningStatus = CY_SYS_CLK_TRIM_OR_COMP_FINISHED;
                }
                else
                {
                    returnStatus = CYRET_STARTED;
                }
            }
            else
            {
                /* Reload CNTR 1 count value for next measuring */
                CY_SYS_CNT_REG1_REG = cydelayFreqHz >> CY_SYS_CLK_SYS_CLK_DEVIDER;
                trimRunningStatus = CY_SYS_CLK_TRIM_OR_COMP_STARTED;
                returnStatus = CYRET_STARTED;
            }
        }
    }
    else
    {
        returnStatus = CYRET_INVALID_STATE;
    }

    return (returnStatus);
}


/********************************************************************************
* Function Name: CySysClkIloUpdateTrimReg
*********************************************************************************
*
* \internal
* Function calculates ILO accuracy and check is error range is higher than 
* +/- 10%. If Measured frequency is higher than +/- 10% function updates 
* ILO Trim register.
*
* \param
* iloAccuracyInPPT Pointer to an integer in which the trimmed ILO 
* accuracy will be returned. The value returned in this pointer is not valid 
* until the function returns CYRET_SUCCESS. If ILO frequency error is lower 
* than +/- 10% then the value returned in this pointer will be updated.
*
* \return CYRET_SUCCESS - Indicates that ILO frequency error is lower than 
* +/- 10% and no actions are required.
*
* \return  CYRET_STARTED - Indicates that ILO frequency error is higher than 
* +/- 10% and ILO Trim register was updated.
*
* \return CYRET_INVALID_STATE - Indicates trimming was unsuccessful.
*
* Post #1 - To obtain 10% ILO accuracy the calculated accuracy should be equal 
* CY_SYS_CLK_ERROR_RANGE = 5.6%. Error value should take to account IMO error of 
* +/-2% (+/-0.64kHz), trim step of 2.36kHz (+/-1.18kHz) and error while ILO 
* frequency measuring.
*
* \endinternal
*
**********************************************************************************/
cystatus CySysClkIloUpdateTrimReg(int32* iloAccuracyInPPT)
{
    uint32 measuredIloFreq;
    uint32 currentIloTrimValue;
    int32 iloAccuracyValue;
    int32 trimStep;
    cystatus errorRangeStatus;

    if(0u != CY_SYS_CNT_REG2_REG)
    {
        measuredIloFreq = (CY_SYS_CNT_REG2_REG * cydelayFreqHz) / (cydelayFreqHz >> CY_SYS_CLK_SYS_CLK_DEVIDER);

        /* Calculate value of error in PPT according to formula - 
        *       ((measuredIlofrequency - iloDesired frequency) * 1000 / iloDesired frequency) */
        iloAccuracyValue = (((int32) measuredIloFreq - (int32) CY_SYS_CLK_ILO_DESIRED_FREQ_HZ) * \
                            ((int32) CY_SYS_CLK_PERTHOUSAND)) / ((int32) CY_SYS_CLK_ILO_DESIRED_FREQ_HZ);

        /* Check if ILO accuracy is more than +/- CY_SYS_CLK_ERROR_RANGE. See post #1 of API description.*/
        if(CY_SYS_CLK_ERROR_RANGE < (uint32) (CY_SYS_CLK_ABS_MACRO(iloAccuracyValue)))
        {
            if (0 < iloAccuracyValue)
            {
                trimStep = (int32) (((iloAccuracyValue * (int32) CY_SYS_CLK_ERROR_COEF) + 
                CY_SYS_CLK_HALF_OF_STEP) / CY_SYS_CLK_ERROR_STEP);
            }
            else
            {
                trimStep = (int32) (((iloAccuracyValue * (int32) CY_SYS_CLK_ERROR_COEF) - 
                CY_SYS_CLK_HALF_OF_STEP) / CY_SYS_CLK_ERROR_STEP);
            }
            currentIloTrimValue = (CY_SYS_CLK_ILO_TRIM_REG & CY_SYS_CLK_ILO_TRIM_MASK);
            trimStep = (int32) currentIloTrimValue - trimStep;

            if(trimStep > CY_SYS_CLK_FOURBITS_MAX)
            {
                trimStep = CY_SYS_CLK_FOURBITS_MAX;
            }
            if(trimStep < 0)
            {
                trimStep = 0;
            }
            CY_SYS_CLK_ILO_TRIM_REG = (CY_SYS_CLK_ILO_TRIM_REG & (uint32)(~CY_SYS_CLK_ILO_TRIM_MASK)) |
                                                                 ((uint32) trimStep);
            errorRangeStatus = CYRET_STARTED;
        } /* Else return success because error is in +/- 10% range*/
        else
        {
            /* Write trimmed ILO accuracy through pointer. */
            *iloAccuracyInPPT = iloAccuracyValue;
            errorRangeStatus = CYRET_SUCCESS;
        }
    }
    else
    {
        errorRangeStatus = CYRET_INVALID_STATE;
    }
return (errorRangeStatus);
}


/*******************************************************************************
* Function Name: CySysClkIloRestoreFactoryTrim
****************************************************************************//**
* \brief
* Restores the ILO Trim Register to factory value.
*
* The CySysClkIloStartMeasurement() API should be called prior to 
* calling this API. Otherwise CYRET_UNKNOWN will be returned.
*
* Available for all PSoC 4 devices except for PSoC 4000 / PSoC 4100 / PSoC 4200 
* / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
*
* \return CYRET_SUCCESS - Operation was successful. 
* \return CYRET_UNKNOWN - CySysClkIloStartMeasurement() was not called 
* before this API. Hence the trim value cannot be updated.
*
******************************************************************************/
cystatus CySysClkIloRestoreFactoryTrim(void)
{
    cystatus returnStatus = CYRET_SUCCESS;

    /* Check was defaultIloTrimRegValue modified in CySysClkIloStartMeasurement */
    if(CY_SYS_CLK_ILO_TRIM_DEFAULT_VALUE != defaultIloTrimRegValue)
        {
            CY_SYS_CLK_ILO_TRIM_REG = ((CY_SYS_CLK_ILO_TRIM_REG & (uint32)(~CY_SYS_CLK_ILO_TRIM_MASK)) | 
                                       (defaultIloTrimRegValue & CY_SYS_CLK_ILO_TRIM_MASK));
        }
        else
        {
            returnStatus = CYRET_UNKNOWN;
        }

        return (returnStatus);
}
#endif /* (CY_IP_SRSSV2 && (!CY_IP_CPUSS)) */


#if (CY_IP_WCO && CY_IP_SRSSV2)
    /*******************************************************************************
    * Function Name: CySysClkGetLfclkSource
    ********************************************************************************
    *
    * \internal
    *  Gets the clock source for the LFCLK clock.
    *  The function is applicable only for PSoC 4100 BLE / PSoC 4200 BLE / 
    *  PSoC 4100M / PSoC 4200M / PSoC 4200L.
    *
    * \return The LFCLK source:
    * CY_SYS_CLK_LFCLK_SRC_ILO Internal Low Frequency (32 kHz) Oscillator (ILO)
    * CY_SYS_CLK_LFCLK_SRC_WCO Low Frequency Watch Crystal Oscillator (WCO)
    *
    * \endinternal
    *
    *******************************************************************************/
    static uint32 CySysClkGetLfclkSource(void)
    {
        uint32 lfclkSource;
        lfclkSource = CY_SYS_WDT_CONFIG_REG & CY_SYS_CLK_LFCLK_SEL_MASK;
        return (lfclkSource);
    }


    /*******************************************************************************
    * Function Name: CySysClkSetLfclkSource
    ****************************************************************************//**
    * \brief
    *  Sets the clock source for the LFCLK clock.
    *
    * The switch between LFCLK sources must be done between the positive edges of
    * LFCLK, because the glitch risk is around the LFCLK positive edge. To ensure
    * that the switch can be done safely, the WDT counter value is read until it
    * changes.
    *
    * That means that the positive edge just finished and the switch is performed.
    * The enabled WDT counter is used for that purpose. If no counters are enabled,
    * counter 0 is enabled. And after the LFCLK source is switched, counter 0
    * configuration is restored.
    *
    *  The function is applicable only for devices with more than one source for 
    * LFCLK - PSoC 4100 BLE / PSoC 4200 BLE / PRoC BLE / PSoC 4100M / PSoC 4200M / 
    * PSoC 4200L.
    *
    * \note For PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices LFCLK can
    * only be sourced from ILO even though WCO is available.
    *
    * \param
    * source
    * CY_SYS_CLK_LFCLK_SRC_ILO - Internal Low Frequency (32 kHz) 
    * Oscillator (ILO).<br>
    * CY_SYS_CLK_LFCLK_SRC_WCO - Low Frequency Watch Crystal Oscillator (WCO).
    *
    * \details
    * This function has no effect if WDT is locked (CySysWdtLock() is called). 
    * Call CySysWdtUnlock() to unlock WDT.
    *
    * Both the current source and the new source must be running and stable before
    * calling this function.
    *
    * Changing the LFCLK clock source may change the LFCLK clock frequency and
    * affect the functionality that uses this clock. For example, watchdog timer
    * "uses this clock" or "this clock uses" (WDT) is clocked by LFCLK.
    *
    *******************************************************************************/
    void CySysClkSetLfclkSource(uint32 source)
    {
        uint8  interruptState;

        if (CySysClkGetLfclkSource() != source)
        {
            interruptState = CyEnterCriticalSection();
            CySysClkLfclkPosedgeCatch();
            CY_SYS_WDT_CONFIG_REG = (CY_SYS_WDT_CONFIG_REG & (uint32)(~CY_SYS_CLK_LFCLK_SEL_MASK)) |
                                    (source & CY_SYS_CLK_LFCLK_SEL_MASK);
            CySysClkLfclkPosedgeRestore();
            CyExitCriticalSection(interruptState);
        }
    }
#endif /* (CY_IP_WCO && CY_IP_SRSSV2) */


#if (CY_IP_WCO)
    /*******************************************************************************
    * Function Name: CySysClkWcoStart
    ****************************************************************************//**
    * \brief
    *  Enables Watch Crystal Oscillator (WCO).
    * 
    * This API enables WCO which is used as a source for LFCLK. Similar to ILO, 
    * WCO is also available in all modes except Hibernate and Stop modes.
    * \note In PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices
    * WCO cannot be a source for the LFCLK. 
    *
    *  WCO is always enabled in High Power Mode (HPM). Refer to the device
    *  datasheet for the  WCO startup time. Once WCO becomes stable it can be
    *  switched to Low Power Mode (LPM). Note that oscillator can be unstable
    *  during a switch and hence its output should not be used at that moment.
    *
    *  The CySysClkWcoSetPowerMode() function configures the WCO power mode.
    *
    *******************************************************************************/
    void CySysClkWcoStart(void)
    {
        CySysClkWcoSetHighPowerMode();
        CY_SYS_CLK_WCO_CONFIG_REG |= CY_SYS_CLK_WCO_CONFIG_LPM_ENABLE;
    }


    /*******************************************************************************
    * Function Name: CySysClkWcoStop
    ****************************************************************************//**
    * \brief
    * Disables the 32 KHz Crystal Oscillator.
    *
    * API switch of WCO.
    * \note PSoC 4100S / PSoC Analog Coprocessor: WCO is required for DeepSleep 
    * Timer's operation.
    *
    *******************************************************************************/
    void  CySysClkWcoStop(void)
    {
        #if (CY_IP_SRSSV2)
            uint8  interruptState;
        #endif /* (CY_IP_SRSSV2) */

        if (0u != CySysClkWcoEnabled())
        {
            #if (CY_IP_SRSSV2)
                if (CY_SYS_CLK_LFCLK_SRC_WCO == CySysClkGetLfclkSource())
                {
                    interruptState = CyEnterCriticalSection();
                    CySysClkLfclkPosedgeCatch();
                    CY_SYS_CLK_WCO_CONFIG_REG &= (uint32) ~CY_SYS_CLK_WCO_CONFIG_LPM_ENABLE;
                    CySysClkLfclkPosedgeRestore();
                    CyExitCriticalSection(interruptState);
                }
                else /* Safe to disable - shortened pulse does not impact peripheral */
            #endif /* (CY_IP_SRSSV2) */
            {
                CY_SYS_CLK_WCO_CONFIG_REG &= (uint32) ~CY_SYS_CLK_WCO_CONFIG_LPM_ENABLE;
            }
        } /* Otherwise do nothing. WCO configuration cannot be changed. */
    }


    /*******************************************************************************
    * Function Name: CySysClkWcoEnabled
    ****************************************************************************//**
    * \internal Reports the WCO enable state.
    *
    * \return 1 if WCO is enabled
    * \return 0 if WCO is disabled.
    * \endinternal
    *******************************************************************************/
    uint32 CySysClkWcoEnabled(void)
    {
        return ((0u != (CY_SYS_CLK_WCO_CONFIG_REG & (uint32)(CY_SYS_CLK_WCO_CONFIG_LPM_ENABLE))) ?
                (uint32) 1u :
                (uint32) 0u);
    }


    /*******************************************************************************
    * Function Name: CySysClkWcoSetPowerMode
    ****************************************************************************//**
    * \brief
    * Sets the power mode for the 32 KHz WCO.
    *
    * By default (if this function is not called), the WCO is in High power mode 
    * during Active and device's low power modes
    *
    * \param mode
    * CY_SYS_CLK_WCO_HPM - The High Power mode.<br>
    * CY_SYS_CLK_WCO_LPM - The Low Power mode.
    *
    * \return A previous power mode. The same as the parameters.
    *
    * \note
    * The WCO Low Power mode is applicable for PSoC 4100 BLE / PSoC 4200 BLE devices.
    *
    *******************************************************************************/
    uint32 CySysClkWcoSetPowerMode(uint32 mode)
    {
        uint32 powerModeStatus;

        powerModeStatus = CY_SYS_CLK_WCO_CONFIG_REG & CY_SYS_CLK_WCO_CONFIG_LPM_EN;

        switch(mode)
        {
        case CY_SYS_CLK_WCO_HPM:
                CySysClkWcoSetHighPowerMode();
            break;

        #if(CY_IP_BLESS)
            case CY_SYS_CLK_WCO_LPM:
                    CySysClkWcoSetLowPowerMode();
                break;
        #endif /* (CY_IP_BLESS) */

        default:
            CYASSERT(0u != 0u);
            break;
        }

        return (powerModeStatus);
    }


    /*******************************************************************************
    * Function Name: CySysClkWcoClockOutSelect
    ****************************************************************************//**
    * \brief
    * Selects the WCO block output source.
    *
    * In addition to generating 32.768 kHz clock from external crystals, WCO 
    * can be sourced by external clock source using wco_out pin. The API help to
    * lets you select between the sources: External crystal or external pin.
    *
    * If you want to use external pin to drive WCO the next procedure is required:
    * <br> 1) Disable the WCO.
    * <br> 2) Drive the wco_out pin to an external signal source.
    * <br> 3) Call CySysClkWcoClockOutSelect(CY_SYS_CLK_WCO_SEL_PIN).
    * <br> 4) Enable the WCO and wait for 15 us before clocking the XO pad at the high 
    * potential. Let's assume you are using the 1.6v clock amplitude, then the
    * sequence would start at 1.6v, then 0v, then 1.6v etc at a chosen frequency.
    *
    * If you want to use WCO after using an external pin source:
    * <br> 1) Disable the WCO.
    * <br> 2) Drive off wco_out pin with external signal source.
    * <br> 3) Call CySysClkWcoClockOutSelect(CY_SYS_CLK_WCO_SEL_CRYSTAL).
    * <br> 4) Enable the WCO.
    *
    * \warning 
    * Do not use the oscillator output clock prior to a 15uS delay in your system.
    * There are no limitations on the external clock frequency.
    * \warning 
    * When external clock source was selected to drive WCO block the IMO can be 
    * trimmed only when external clock source period is equal to WCO external
    * crystal period. Also external clock source accuracy should be higher 
    * or equal to WCO external crystal accuracy.
    *
    * \param clockSel
    * CY_SYS_CLK_WCO_SEL_CRYSTAL - Selects External crystal as clock 
    * source of WCO.<br>
    * CY_SYS_CLK_WCO_SEL_PIN - Selects External clock input on wco_in pin as 
    * clock source of WCO.
    *
    *******************************************************************************/
    void CySysClkWcoClockOutSelect(uint32 clockSel)
    {
        if (0u != CySysClkWcoEnabled())
        {
            if (1u >= clockSel)
            {
                CY_SYS_CLK_WCO_CONFIG_REG = (CY_SYS_CLK_WCO_CONFIG_REG & (uint32)(~CY_SYS_CLK_WCO_SELECT_PIN_MASK)) |
                                        (clockSel << CY_SYS_CLK_WCO_SELECT_PIN_OFFSET);
            }
            else
            {
                CYASSERT(0u != 0u);
            }
        }
    }
#endif  /* (CY_IP_WCO) */


#if(CY_IP_SRSSV2)
    /*******************************************************************************
    * Function Name: CySysWdtLock
    ****************************************************************************//**
    * \brief
    * Locks out configuration changes to the Watchdog timer registers and ILO
    * configuration register.
    *
    * After this function is called, ILO clock can't be disabled until
    * CySysWdtUnlock() is called.
    *
    *******************************************************************************/
    void CySysWdtLock(void)
    {
        uint8 interruptState;
        interruptState = CyEnterCriticalSection();

        CY_SYS_CLK_SELECT_REG = (CY_SYS_CLK_SELECT_REG & (uint32)(~CY_SYS_WDT_CLK_LOCK_BITS_MASK)) |
                 CY_SYS_WDT_CLK_LOCK_BITS_MASK;

        CyExitCriticalSection(interruptState);
    }


    /*******************************************************************************
    * Function Name: CySysWdtLocked
    ****************************************************************************//**
    * \internal
    * Reports the WDT lock state.
    *
    * \return 1 - WDT is locked, and 0 - WDT is unlocked.
    * \endinternal
    *******************************************************************************/
    static uint32 CySysWdtLocked(void)
    {
        /* Prohibits writing to WDT registers and ILO/WCO registers when not equal 0 */
        return ((0u != (CY_SYS_CLK_SELECT_REG & (uint32)(CY_SYS_WDT_CLK_LOCK_BITS_MASK))) ? (uint32) 1u : (uint32) 0u);
    }


    /*******************************************************************************
    * Function Name: CySysWdtUnlock
    ****************************************************************************//**
    * \brief
    *  Unlocks the Watchdog Timer configuration register.
    *
    *******************************************************************************/
    void CySysWdtUnlock(void)
    {
        uint8 interruptState;
        interruptState = CyEnterCriticalSection();

        /* Removing WDT lock requires two writes */
        CY_SYS_CLK_SELECT_REG = ((CY_SYS_CLK_SELECT_REG & (uint32)(~CY_SYS_WDT_CLK_LOCK_BITS_MASK)) |
                CY_SYS_WDT_CLK_LOCK_BIT0);

        CY_SYS_CLK_SELECT_REG = ((CY_SYS_CLK_SELECT_REG & (uint32)(~CY_SYS_WDT_CLK_LOCK_BITS_MASK)) |
                CY_SYS_WDT_CLK_LOCK_BIT1);

        CyExitCriticalSection(interruptState);
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetEnabledStatus
    ****************************************************************************//**
    * \brief
    *  Reads the enabled status of one of the three WDT counters.
    *
    * \param counterNum: Valid range [0-2].  The number of the WDT counter.
    *
    * \return The status of the WDT counter:
    * \return 0 - If the counter is disabled.
    * \return 1 - If the counter is enabled.
    *
    * \details
    * This function returns an actual WDT counter status from the status register. It may
    * take up to 3 LFCLK cycles for the WDT status register to contain actual data
    * after the WDT counter is enabled.
    *
    *******************************************************************************/
    uint32 CySysWdtGetEnabledStatus(uint32 counterNum)
    {
        CYASSERT(counterNum < CY_SYS_WDT_COUNTERS_MAX);
        return ((CY_SYS_WDT_CONTROL_REG >> ((CY_SYS_WDT_CNT_SHIFT * counterNum) + CY_SYS_WDT_CNT_STTS_SHIFT)) & 0x01u);
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetMode
    ****************************************************************************//**
    * \brief
    * Writes the mode of one of the three WDT counters.
    *
    * \param counterNum: Valid range [0-2].  The number of the WDT counter.
    *
    * \param mode
    *  CY_SYS_WDT_MODE_NONE - Free running.<br>
    * CY_SYS_WDT_MODE_INT - The interrupt generated on match for counter 0
    * and 1, and on bit toggle for counter 2.<br>
    * CY_SYS_WDT_MODE_RESET - Reset on match (valid for counter 0 and 1 only).<br>
    * CY_SYS_WDT_MODE_INT_RESET - Generate an interrupt.  Generate a reset on
    * the 3rd non-handled interrupt (valid for counter 0 and counter 1 only).
    *
    * \details
    * WDT counter counterNum should be disabled to set a mode. Otherwise, this
    * function call has no effect. If the specified counter is enabled,
    * call the CySysWdtDisable() function with the corresponding parameter to
    * disable the specified counter and wait for it to stop.
    *
    *******************************************************************************/
    void CySysWdtSetMode(uint32 counterNum, uint32 mode)
    {
        uint32 configRegValue;

        CYASSERT(counterNum < CY_SYS_WDT_COUNTERS_MAX);

        if(0u == CySysWdtGetEnabledStatus(counterNum))
        {
            configRegValue = CY_SYS_WDT_CONFIG_REG &
                                (uint32)~((uint32)(CY_SYS_WDT_MODE_MASK << (counterNum * CY_SYS_WDT_CNT_SHIFT)));
            configRegValue |= (uint32)((mode & CY_SYS_WDT_MODE_MASK) << (counterNum * CY_SYS_WDT_CNT_SHIFT));
            CY_SYS_WDT_CONFIG_REG = configRegValue;
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetMode
    ****************************************************************************//**
    * 
    * \brief Reads the mode of one of the three WDT counters.
    *
    * \param counterNum Valid range [0-2]. The number of the WDT counter.
    *
    * \return The mode of the counter. The same enumerated values as the mode 
    * parameter used in CySysWdtSetMode().
    *
    *******************************************************************************/
    uint32 CySysWdtGetMode(uint32 counterNum)
    {
        return ((CY_SYS_WDT_CONFIG_REG >> (counterNum * CY_SYS_WDT_CNT_SHIFT)) & CY_SYS_WDT_MODE_MASK);
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetClearOnMatch
    ****************************************************************************//**
    * 
    * \brief Configures the WDT counter "clear on match" setting.
    *
    * If configured to "clear on match", the counter counts from 0 to MatchValue
    * giving it a period of (MatchValue + 1).
    *
    * \param counterNum
    * Valid range [0-1]. The number of the WDT counter. The match values are not
    * supported by counter 2.
    *
    * \param enable 0 to disable appropriate counter <br>
    *  1 to enable appropriate counter
    *
    * \details
    * WDT counter counterNum should be disabled. Otherwise this function call
    * has no effect. If the specified counter is enabled, call the CySysWdtDisable()
    * function with the corresponding parameter to disable the specified counter and
    * wait for it to stop. This may take up to three LFCLK cycles.
    *
    *******************************************************************************/
    void CySysWdtSetClearOnMatch(uint32 counterNum, uint32 enable)
    {
        uint32 configRegValue;

        CYASSERT((counterNum == CY_SYS_WDT_COUNTER0) ||
                 (counterNum == CY_SYS_WDT_COUNTER1));

        if(0u == CySysWdtGetEnabledStatus(counterNum))
        {
            configRegValue = CY_SYS_WDT_CONFIG_REG & (uint32)~((uint32)((uint32)1u << 
                                     ((counterNum * CY_SYS_WDT_CNT_SHIFT) + CY_SYS_WDT_CNT_MATCH_CLR_SHIFT)));

            configRegValue
                |= (uint32)(enable << ((counterNum * CY_SYS_WDT_CNT_SHIFT) + CY_SYS_WDT_CNT_MATCH_CLR_SHIFT));

            CY_SYS_WDT_CONFIG_REG = configRegValue;
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetClearOnMatch
    ****************************************************************************//**
    * \brief
    * Reads the "clear on match" setting for the specified counter.
    *
    * \param
    * counterNum Valid range [0-1]. The number of the WDT counter. The match values
    * are not supported by counter 2.
    *
    * \return The "clear on match" status: <br>1 if enabled <br>0 if disabled
    *
    *******************************************************************************/
    uint32 CySysWdtGetClearOnMatch(uint32 counterNum)
    {
        CYASSERT((counterNum == CY_SYS_WDT_COUNTER0) ||
                 (counterNum == CY_SYS_WDT_COUNTER1));

        return (uint32)((CY_SYS_WDT_CONFIG_REG >>
                        ((counterNum * CY_SYS_WDT_CNT_SHIFT) + CY_SYS_WDT_CNT_MATCH_CLR_SHIFT)) & 0x01u);
    }


    /*******************************************************************************
    * Function Name: CySysWdtEnable
    ****************************************************************************//**
    *
    * \brief Enables the specified WDT counters.
    *
    * All the counters specified in the mask are enabled.
    *
    * \param counterMask
    *  CY_SYS_WDT_COUNTER0_MASK - The mask for counter 0 to enable.<br>
    * CY_SYS_WDT_COUNTER1_MASK - The mask for counter 1 to enable.<br>
    * CY_SYS_WDT_COUNTER2_MASK - The mask for counter 2 to enable.
    *
    * \details
    * Enabling or disabling WDT requires 3 LFCLK cycles to come into effect.
    * Therefore, the WDT enable state must not be changed more than once in
    * that period.
    *
    * After WDT is enabled, it is illegal to write WDT configuration (WDT_CONFIG)
    * and control (WDT_CONTROL) registers. This means that all WDT functions that
    * contain 'write' in the name (with the exception of CySysWdtSetMatch()
    * function) are illegal to call if WDT is enabled.
    *
    * PSoC 4100 / PSoC 4200: This function enables ILO.
    *
    * PSoC 4100 BLE / PSoC 4200 BLE / PSoC4200L / PSoC 4100M 
    * / PSoC 4200M:
    * LFLCK should be configured before calling this function. The desired
    * source should be enabled and configured to be the source for LFCLK.
    *
    *******************************************************************************/
    void CySysWdtEnable(uint32 counterMask)
    {
        #if (!CY_IP_WCO)
            CySysClkIloStart();
        #endif /* (!CY_IP_WCO) */

        CY_SYS_WDT_CONTROL_REG |= counterMask;

        if(0u != (counterMask & CY_SYS_WDT_COUNTER0_MASK))
        {
            while (0u == CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER0))
            {
                /* Wait for changes to come into effect */
            }
        }

        if(0u != (counterMask & CY_SYS_WDT_COUNTER1_MASK))
        {
            while (0u == CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER1))
            {
                /* Wait for changes to come into effect */
            }
        }

        if(0u != (counterMask & CY_SYS_WDT_COUNTER2_MASK))
        {
            while (0u == CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER2))
            {
                /* Wait for changes to come into effect */
            }
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtDisable
    ****************************************************************************//**
    *
    * \brief Disables the specified WDT counters.
    * All the counters specified in the mask are disabled. The function waits for 
    * the changes to come into effect.
    *
    * \param counterMask
    * CY_SYS_WDT_COUNTER0_MASK - The mask for counter 0 to disable. <br>
    * CY_SYS_WDT_COUNTER1_MASK - The mask for counter 1 to disable. <br>
    * CY_SYS_WDT_COUNTER2_MASK - The mask for counter 2 to disable.
    *
    *******************************************************************************/
    void CySysWdtDisable(uint32 counterMask)
    {
        if (0uL == CySysWdtLocked())
        {
            CY_SYS_WDT_CONTROL_REG &= ~counterMask;

            if(0u != (counterMask & CY_SYS_WDT_COUNTER0_MASK))
            {
                while (0u != CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER0))
                {
                    /* Wait for changes to come into effect */
                }
            }

            if(0u != (counterMask & CY_SYS_WDT_COUNTER1_MASK))
            {
                while (0u != CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER1))
                {
                    /* Wait for changes to come into effect */
                }
            }

            if(0u != (counterMask & CY_SYS_WDT_COUNTER2_MASK))
            {
                while (0u != CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER2))
                {
                    /* Wait for changes to come into effect */
                }
            }
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetCascade
    ****************************************************************************//**
    * \brief
    * Writes the two WDT cascade values based on the combination of mask values
    * specified.
    *
    * \param cascadeMask The mask value used to set or clear the cascade values:
    * CY_SYS_WDT_CASCADE_NONE - Neither <br>
    * CY_SYS_WDT_CASCADE_01 - Cascade 01 <br>
    * CY_SYS_WDT_CASCADE_12 - Cascade 12
    *
    * If only one cascade mask is specified, the second cascade is disabled.
    * To set both cascade modes, two defines should be ORed:
    * (CY_SYS_TIMER_CASCADE_01 | CY_SYS_TIMER_CASCADE_12).
    * \note If CySysWdtSetCascade() was called with ORed defines it is necessary 
    * to call CySysWdtSetClearOnMatch(1,1). It is needed to make sure that 
    * Counter 2 will be updated in the expected way.
    *
    * WDT counters that are part of the specified cascade should be disabled.
    * Otherwise this function call has no effect. If the specified
    * counter is enabled, call CySysWdtDisable() function with the corresponding
    * parameter to disable the specified counter and wait for it to stop. This may
    * take up to 3 LFCLK cycles.
    *
    *******************************************************************************/
    void CySysWdtSetCascade(uint32 cascadeMask)
    {
        uint32 configRegValue;
        uint32 countersEnableStatus;

        countersEnableStatus = CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER0) |
                            CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER1) |
                            CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER2);

        if (0u == countersEnableStatus)
        {
            configRegValue = CY_SYS_WDT_CONFIG_REG;
            configRegValue &= ((uint32)(~(CY_SYS_WDT_CASCADE_01|CY_SYS_WDT_CASCADE_12)));
            configRegValue |= cascadeMask;
            CY_SYS_WDT_CONFIG_REG = configRegValue;
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetCascade
    ****************************************************************************//**
    *
    * \brief Reads the two WDT cascade values returning a mask of the bits set.
    *
    * \return The mask of the cascade values set.
    * \return CY_SYS_WDT_CASCADE_NONE - Neither
    * \return CY_SYS_WDT_CASCADE_01 - Cascade 01
    * \return CY_SYS_WDT_CASCADE_12 - Cascade 12
    *
    *******************************************************************************/
    uint32 CySysWdtGetCascade(void)
    {
        return (CY_SYS_WDT_CONFIG_REG & (CY_SYS_WDT_CASCADE_01 | CY_SYS_WDT_CASCADE_12));
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetMatch
    ****************************************************************************//**
    *
    * \brief Configures the WDT counter match comparison value.
    *
    * \param counterNum
    * Valid range [0-1]. The number of the WDT counter. The match values are not
    * supported by counter 2.
    *
    * \param match
    * Valid range [0-65535]. The value to be used to match against the counter.
    *
    *******************************************************************************/
    void CySysWdtSetMatch(uint32 counterNum, uint32 match)
    {
        uint32 regValue;

        CYASSERT((counterNum == CY_SYS_WDT_COUNTER0) ||
                 (counterNum == CY_SYS_WDT_COUNTER1));

        /* Wait for previous changes to come into effect */
        CyDelayUs(CY_SYS_WDT_3LFCLK_DELAY_US);

        regValue = CY_SYS_WDT_MATCH_REG;
        regValue &= (uint32)~((uint32)(CY_SYS_WDT_LOWER_16BITS_MASK << (counterNum * CY_SYS_WDT_CNT_MATCH_SHIFT)));
        CY_SYS_WDT_MATCH_REG = (regValue | (match << (counterNum * CY_SYS_WDT_CNT_MATCH_SHIFT)));

        /* Make sure match synchronization has started */
        CyDelayUs(CY_SYS_WDT_1LFCLK_DELAY_US);
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetToggleBit
    ****************************************************************************//**
    * \brief
    * Configures which bit in WDT counter 2 to monitor for a toggle.
    *
    * When that bit toggles, an interrupt is generated if the mode for counter 2 has
    * enabled interrupts.
    *
    * \param bits Valid range [0-31].  Counter 2 bit to monitor for a toggle.
    *
    * \details
    * WDT Counter 2 should be disabled. Otherwise this function call has no
    * effect.
    *
    * If the specified counter is enabled, call the CySysWdtDisable() function with
    * the corresponding parameter to disable the specified counter and wait for it to
    * stop. This may take up to 3 LFCLK cycles.
    *
    *******************************************************************************/
    void CySysWdtSetToggleBit(uint32 bits)
    {
        uint32 configRegValue;

        if (0u == CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER2))
        {
            configRegValue = CY_SYS_WDT_CONFIG_REG;
            configRegValue &= (uint32)(~((uint32)(CY_SYS_WDT_CONFIG_BITS2_MASK << CY_SYS_WDT_CONFIG_BITS2_POS)));
            configRegValue |= ((bits & CY_SYS_WDT_CONFIG_BITS2_MASK) << CY_SYS_WDT_CONFIG_BITS2_POS);
            CY_SYS_WDT_CONFIG_REG = configRegValue;
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetToggleBit
    ****************************************************************************//**
    * \brief
    * Reads which bit in WDT counter 2 is monitored for a toggle.
    *
    * \return The bit that is monitored (range of 0 to 31)
    *
    *******************************************************************************/
    uint32 CySysWdtGetToggleBit(void)
    {
        return ((CY_SYS_WDT_CONFIG_REG >> CY_SYS_WDT_CONFIG_BITS2_POS) & CY_SYS_WDT_CONFIG_BITS2_MASK);
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetMatch
    ****************************************************************************//**
    *
    * \brief Reads the WDT counter match comparison value.
    *
    * \param counterNum Valid range [0-1].  The number of the WDT counter. The match
    * values are not supported by counter 2.
    *
    * \return A 16-bit match value.
    *
    *******************************************************************************/
    uint32 CySysWdtGetMatch(uint32 counterNum)
    {
        CYASSERT((counterNum == CY_SYS_WDT_COUNTER0) ||
                 (counterNum == CY_SYS_WDT_COUNTER1));

        return ((uint32)(CY_SYS_WDT_MATCH_REG >> 
                                            (counterNum * CY_SYS_WDT_CNT_MATCH_SHIFT)) & CY_SYS_WDT_LOWER_16BITS_MASK);
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetCount
    ****************************************************************************//**
    *
    * \brief Reads the current WDT counter value.
    *
    * \param counterNum: Valid range [0-2]. The number of the WDT counter.
    *
    * \return A live counter value.  Counter 0 and Counter 1 are 16 bit counters 
    * and counter 2 is a 32 bit counter.
    *
    *******************************************************************************/
    uint32 CySysWdtGetCount(uint32 counterNum)
    {
        uint32 regValue = 0u;

        switch(counterNum)
        {
            /* WDT Counter 0 */
            case 0u:
                regValue = CY_SYS_WDT_CTRLOW_REG & CY_SYS_WDT_LOWER_16BITS_MASK;
            break;

            /* WDT Counter 1 */
            case 1u:
                regValue = (CY_SYS_WDT_CTRLOW_REG >> CY_SYS_WDT_CNT_MATCH_SHIFT) & CY_SYS_WDT_LOWER_16BITS_MASK;
            break;

            /* WDT Counter 2 */
            case 2u:
                regValue = CY_SYS_WDT_CTRHIGH_REG;
            break;

            default:
                CYASSERT(0u != 0u);
            break;
        }

        return (regValue);
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetInterruptSource
    ****************************************************************************//**
    * \brief
    * Reads a mask containing all the WDT counters interrupts that are currently
    * set by the hardware, if a corresponding mode is selected.
    *
    * \return The mask of interrupts set
    * \return CY_SYS_WDT_COUNTER0_INT - Counter 0
    * \return CY_SYS_WDT_COUNTER1_INT - Counter 1
    * \return CY_SYS_WDT_COUNTER2_INT - Counter 2
    *
    *******************************************************************************/
    uint32 CySysWdtGetInterruptSource(void)
    {
        return (CY_SYS_WDT_CONTROL_REG & (CY_SYS_WDT_COUNTER0_INT | CY_SYS_WDT_COUNTER1_INT | CY_SYS_WDT_COUNTER2_INT));
    }


    /*******************************************************************************
    * Function Name: CySysWdtClearInterrupt
    ****************************************************************************//**
    * \brief
    * Clears all the WDT counter interrupts set in the mask.
    *
    * Calling this function also prevents from Reset when the counter mode is set 
    * to generate 3 interrupts and then the device resets.
    *
    * All the WDT interrupts are to be cleared by the firmware, otherwise
    * interrupts are generated continuously.
    *
    * \param counterMask
    * CY_SYS_WDT_COUNTER0_INT - Clears counter 0 interrupts <br>
    * CY_SYS_WDT_COUNTER1_INT - Clears counter 1 interrupts <br>
    * CY_SYS_WDT_COUNTER2_INT - Clears counter 2 interrupts
    *
    * \details
    * This function temporary removes the watchdog lock, if it was set, and
    * restores the lock state after cleaning the WDT interrupts that are set in
    * a mask.
    *
    *******************************************************************************/
    void CySysWdtClearInterrupt(uint32 counterMask)
    {
        uint8 interruptState;
        uint32 wdtLockState;

        interruptState = CyEnterCriticalSection();

        if (0u != CySysWdtLocked())
        {
            wdtLockState = 1u;
            CySysWdtUnlock();
        }
        else
        {
            wdtLockState = 0u;
        }

        /* Set new WDT control register value */
        counterMask &= (CY_SYS_WDT_COUNTER0_INT |
                        CY_SYS_WDT_COUNTER1_INT |
                        CY_SYS_WDT_COUNTER2_INT);

        CY_SYS_WDT_CONTROL_REG = counterMask | (CY_SYS_WDT_CONTROL_REG & ~(CY_SYS_WDT_COUNTER0_INT |
                                                                           CY_SYS_WDT_COUNTER1_INT |
                                                                           CY_SYS_WDT_COUNTER2_INT));

        /* Read the CY_SYS_WDT_CONTROL_REG to clear the interrupt request.
         * Cypress ID #207093, #206231
        */
        (void)CY_SYS_WDT_CONTROL_REG;

        if (1u == wdtLockState)
        {
            CySysWdtLock();
        }

        CyExitCriticalSection(interruptState);
    }


    /*******************************************************************************
    * Function Name: CySysWdtResetCounters
    ****************************************************************************//**
    * \brief
    * Resets all the WDT counters set in the mask.
    *
    * \param countersMask
    * CY_SYS_WDT_COUNTER0_RESET - Reset counter 0 <br>
    * CY_SYS_WDT_COUNTER1_RESET - Reset counter 1 <br>
    * CY_SYS_WDT_COUNTER2_RESET - Reset counter 2
    *
    * \details
    * This function does not reset counter values if the Watchdog is locked.
    * This function waits while corresponding counters will be reset. This may
    * take up to 3 LFCLK cycles.
    * The LFCLK source must be enabled. Otherwise, the function will never exit.
    *
    *******************************************************************************/
    void CySysWdtResetCounters(uint32 countersMask)
    {
        /* Set new WDT reset value */
        CY_SYS_WDT_CONTROL_REG |= (countersMask & CY_SYS_WDT_COUNTERS_RESET);

        while (0uL != (CY_SYS_WDT_CONTROL_REG & CY_SYS_WDT_COUNTERS_RESET))
        {
            /* Wait for reset to come into effect */
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetInterruptCallback
    ****************************************************************************//**
    * \brief
    * Sets the ISR callback function for the particular WDT counter.
    * These functions are called on the WDT interrupt.
    *
    * \param counterNum The number of the WDT counter.
    * \param function The pointer to the callback function.
    *
    * \return The pointer to the previous callback function.
    * \return NULL is returned if the specified address is not set.
    *
    *******************************************************************************/
    cyWdtCallback CySysWdtSetInterruptCallback(uint32 counterNum, cyWdtCallback function)
    {
        cyWdtCallback prevCallback = (void *)0;

        if(counterNum < CY_WDT_NUM_OF_WDT)
        {
            prevCallback = cySysWdtCallback[counterNum];
            cySysWdtCallback[counterNum] = function;
        }
        else
        {
            CYASSERT(0u != 0u);
        }

        return((cyWdtCallback)prevCallback);
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetInterruptCallback
    ****************************************************************************//**
    * \brief
    * Gets the ISR callback function for the particular WDT counter.
    *
    * \param counterNum The number of the WDT counter.
    *
    * \return The pointer to the callback function registered for a particular WDT by
    * a particular address that are passed through arguments.
    *
    *******************************************************************************/
    cyWdtCallback CySysWdtGetInterruptCallback(uint32 counterNum)
    {
        cyWdtCallback retCallback = (void *)0;

        if(counterNum < CY_WDT_NUM_OF_WDT)
        {
            retCallback = (cyWdtCallback)cySysWdtCallback[counterNum];
        }
        else
        {
            CYASSERT(0u != 0u);
        }

        return(retCallback);
    }


    /*******************************************************************************
    * Function Name: CySysWdtEnableCounterIsr
    ****************************************************************************//**
    * \brief
    * Enables the ISR callback servicing for the particular WDT counter
    *
    * \param counterNum Valid range [0-2]. The number of the WDT counter.
    *
    * Value corresponds to appropriate WDT counter. For example value 1 
    * corresponds to second WDT counter.
    *
    *******************************************************************************/
    void CySysWdtEnableCounterIsr(uint32 counterNum)
    {
        if(counterNum <= CY_SYS_WDT_COUNTER2)
        {
            disableServicedIsr &= ~counterIntMaskTbl[counterNum];
            wdtIsrMask |= counterIntMaskTbl[counterNum];
        }
        else
        {
            CYASSERT(0u != 0u);
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtDisableCounterIsr
    ****************************************************************************//**
    * \brief
    * Disables the ISR callback servicing for the particular WDT counter
    *
    * \param counterNum Valid range [0-2]. The number of the WDT counter.
    *
    *******************************************************************************/
    void CySysWdtDisableCounterIsr(uint32 counterNum)
    {
        if(counterNum <= CY_SYS_WDT_COUNTER2)
        {
            wdtIsrMask &= ~counterIntMaskTbl[counterNum];
        }
        else
        {
            CYASSERT(0u != 0u);
        }
    }


    /*******************************************************************************
    * Function Name: CySysWdtIsr
    ****************************************************************************//**
    * \brief
    * This is the handler of the WDT interrupt in CPU NVIC.
    *
    * The handler checks which WDT triggered in the interrupt and calls the 
    * respective callback functions configured by the user by using 
    * CySysWdtSetIsrCallback() API.
    *
    * The order of the callback execution is incremental. Callback-0 is
    * run as the first one and callback-2 is called as the last one.
    *
    * \details
    * This function clears the WDT interrupt every time when it is called.
    * Reset after the 3rd interrupt does not happen if this function is registered
    * as the interrupt handler even if the "Watchdog with Interrupt" mode is
    * selected on the "Low Frequency Clocks" tab.
    *
    *******************************************************************************/
    void CySysWdtIsr(void)
    {
        if(0u != (CY_SYS_WDT_COUNTER0_INT & CY_SYS_WDT_CONTROL_REG))
        {
            if(0u != (CY_SYS_WDT_COUNTER0_INT & wdtIsrMask))
            {
                wdtIsrMask &= ~(disableServicedIsr & CY_SYS_WDT_COUNTER0_INT);
                disableServicedIsr  &= ~CY_SYS_WDT_COUNTER0_INT;
                if(cySysWdtCallback[CY_SYS_WDT_COUNTER0] != (void *) 0)
                {
                    (void)(cySysWdtCallback[CY_SYS_WDT_COUNTER0])();
                }
            }
            CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
        }

        if(0u != (CY_SYS_WDT_COUNTER1_INT & CY_SYS_WDT_CONTROL_REG))
        {
            if(0u != (CY_SYS_WDT_COUNTER1_INT & wdtIsrMask))
            {
                wdtIsrMask &= ~(disableServicedIsr & CY_SYS_WDT_COUNTER1_INT);
                disableServicedIsr  &= ~CY_SYS_WDT_COUNTER1_INT;
                if(cySysWdtCallback[CY_SYS_WDT_COUNTER1] != (void *) 0)
                {
                    (void)(cySysWdtCallback[CY_SYS_WDT_COUNTER1])();
                }
            }
            CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER1_INT);
        }

        if(0u != (CY_SYS_WDT_COUNTER2_INT & CY_SYS_WDT_CONTROL_REG))
        {
            if(0u != (CY_SYS_WDT_COUNTER2_INT & wdtIsrMask))
            {
                if(cySysWdtCallback[CY_SYS_WDT_COUNTER2] != (void *) 0)
                {
                    (void)(cySysWdtCallback[CY_SYS_WDT_COUNTER2])();
                }
            }
            CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER2_INT);
        }
    }


    /*******************************************************************************
    * Function Name: CySysWatchdogFeed
    ****************************************************************************//**
    * \brief
    * Feeds the corresponded Watchdog Counter before it causes the device reset.
    *
    * Supported only for first WDT0 and second WDT1 counters in the "Watchdog" or 
    * "Watchdog w/ Interrupts" modes.
    *
    * \param counterNum
    * CY_SYS_WDT_COUNTER0 - Feeds the Counter 0 <br>
    * CY_SYS_WDT_COUNTER1 - Feeds the Counter 1
    *
    * Value of counterNum corresponds to appropriate counter. For example value 1 
    * corresponds to second WDT1 Counter.
    *
    * \details
    * Clears the WDT counter in the "Watchdog" mode or clears the WDT interrupt in
    * "Watchdog w/ Interrupts" mode. Does nothing in other modes.
    *
    *******************************************************************************/
    void CySysWatchdogFeed(uint32 counterNum)
    {
        if(counterNum == CY_SYS_WDT_COUNTER0)
        {
            if(CY_SYS_WDT_MODE_INT_RESET == CySysWdtGetMode(counterNum))
            {
                CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
            }
            else if(CY_SYS_WDT_MODE_RESET == CySysWdtGetMode(counterNum))
            {
                CySysWdtResetCounters(CY_SYS_WDT_COUNTER0_RESET);
                CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
            }
            else
            {
                /* Do nothing. */
            }
        }
        else if(counterNum == CY_SYS_WDT_COUNTER1)
        {
            if(CY_SYS_WDT_MODE_INT_RESET == CySysWdtGetMode(counterNum))
            {
                CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER1_INT);
            }
            else if(CY_SYS_WDT_MODE_RESET == CySysWdtGetMode(counterNum))
            {
                CySysWdtResetCounters(CY_SYS_WDT_COUNTER1_RESET);
                CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER1_INT);
            }
            else
            {
                /* Do nothing. */
            }
        }
        else
        {
            /* Do nothing. */
        }
    }


    /*******************************************************************************
    * Function Name: CySysClkLfclkPosedgeCatch
    ****************************************************************************//**
    * \internal
    * Returns once the LFCLK positive edge occurred.
    *
    * CySysClkLfclkPosedgeRestore() should be called after this function
    * to restore the WDT configuration.
    *
    * A pair of the CySysClkLfclkPosedgeCatch() and CySysClkLfclkPosedgeRestore()
    * functions is expected to be called inside a critical section.
    *
    * To ensure that the WDT counter value is read until it changes, the enabled
    * WDT counter is used. If no counter is enabled, counter 0 is enabled.
    * And after the LFCLK source is switched, the counter 0 configuration
    * is restored.
    *
    * Not applicable for the PSoC 4000 / PSoC 4000S / PSoC 4100S / PSoC Analog 
    * Coprocessor devices.
    *
    * \details
    * This function has no effect if WDT is locked (CySysWdtLock() is
    * called). Call CySysWdtUnlock() to unlock WDT.
    * \endinternal
    *******************************************************************************/
    static void CySysClkLfclkPosedgeCatch(void)
    {
        uint32 firstCount;
        static uint32 lfclkPosedgeEnabledWdtCounter = 0u;

        if (0u != CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER0))
        {
            lfclkPosedgeEnabledWdtCounter = CY_SYS_WDT_COUNTER0;
        }
        else if (0u != CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER1))
        {
            lfclkPosedgeEnabledWdtCounter = CY_SYS_WDT_COUNTER1;
        }
        else if (0u != CySysWdtGetEnabledStatus(CY_SYS_WDT_COUNTER2))
        {
            lfclkPosedgeEnabledWdtCounter = CY_SYS_WDT_COUNTER2;
        }
        else /* All WDT counters are disabled */
        {
            /* Configure WDT counter # 0 */
            lfclkPosedgeWdtCounter0Enabled = 1u;
            lfclkPosedgeEnabledWdtCounter = CY_SYS_WDT_COUNTER0;

            lfclkPosedgeWdtCounter0Mode = CySysWdtGetMode(CY_SYS_WDT_COUNTER0);
            CySysWdtSetMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_NONE);
            CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
        }

        firstCount = CySysWdtGetCount(lfclkPosedgeEnabledWdtCounter);
        while (CySysWdtGetCount(lfclkPosedgeEnabledWdtCounter) == firstCount)
        {
            /* Wait for counter to increment */
        }
    }


    /*******************************************************************************
    * Function Name: CySysClkLfclkPosedgeRestore
    ****************************************************************************//**
    * \internal
    *  Restores the WDT configuration after a CySysClkLfclkPosedgeCatch() call.
    *
    *  A pair of the CySysClkLfclkPosedgeCatch() and CySysClkLfclkPosedgeRestore()
    *  functions is expected to be called inside a critical section.
    *
    * Not applicable for the PSoC 4000/PSoC 4000S / PSoC 4100S / PSoC Analog 
    * Coprocessor devices.
    *
    * \details
    *  This function has no effect if WDT is locked (CySysWdtLock() is
    *  called). Call CySysWdtUnlock() to unlock WDT.
    *
    * \endinternal
    *******************************************************************************/
    static void CySysClkLfclkPosedgeRestore(void)
    {
        if (lfclkPosedgeWdtCounter0Enabled != 0u)
        {
            /* Restore counter # 0 configuration and force its shutdown */
            CY_SYS_WDT_CONTROL_REG &= (uint32)(~CY_SYS_WDT_COUNTER0_MASK);
            CySysWdtSetMode(CY_SYS_WDT_COUNTER0, lfclkPosedgeWdtCounter0Mode);
            lfclkPosedgeWdtCounter0Enabled = 0u;
        }
    }

#else

    /*******************************************************************************
    * Function Name: CySysWdtGetEnabledStatus
    ****************************************************************************//**
    *
    * \brief Reads the enabled status of the WDT counter.
    *
    * \return The status of the WDT counter:
    * \return 0 - Counter is disabled
    * \return 1 - Counter is enabled
    *
    *******************************************************************************/
    uint32 CySysWdtGetEnabledStatus(void)
    {
        return ((CY_SYS_WDT_DISABLE_KEY_REG == CY_SYS_WDT_KEY) ? (uint32) 0u : (uint32) 1u);
    }


    /*******************************************************************************
    * Function Name: CySysWdtEnable
    ****************************************************************************//**
    *
    * \brief
    * Enables watchdog timer reset generation.
    *
    * CySysWdtClearInterrupt() feeds the watchdog. Two unserviced interrupts lead 
    * to a system reset (i.e. at the third match).
    *
    * ILO is enabled by the hardware once WDT is started.
    *
    *******************************************************************************/
    void CySysWdtEnable(void)
    {
        CY_SYS_WDT_DISABLE_KEY_REG = 0u;
    }


    /*******************************************************************************
    * Function Name: CySysWdtDisable
    ****************************************************************************//**
    *
    * \brief Disables the WDT reset generation.
    *
    * This function unlocks the ENABLE bit in the CLK_ILO_CONFIG registers and
    * enables the user to disable ILO.
    *
    *******************************************************************************/
    void CySysWdtDisable(void)
    {
        CY_SYS_WDT_DISABLE_KEY_REG = CY_SYS_WDT_KEY;
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetMatch
    ****************************************************************************//**
    *
    * \brief Configures the WDT counter match comparison value.
    *
    * \param match Valid range [0-65535]. The value to be used to match against 
    * the counter.
    *
    *******************************************************************************/
    void CySysWdtSetMatch(uint32 match)
    {
        match &= CY_SYS_WDT_MATCH_MASK;
        CY_SYS_WDT_MATCH_REG = (CY_SYS_WDT_MATCH_REG & (uint32)(~CY_SYS_WDT_MATCH_MASK)) | match;
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetMatch
    ****************************************************************************//**
    *
    * \brief Reads the WDT counter match comparison value.
    *
    * \return The counter match value.
    *
    *******************************************************************************/
    uint32 CySysWdtGetMatch(void)
    {
        return (CY_SYS_WDT_MATCH_REG & CY_SYS_WDT_MATCH_MASK);
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetCount
    ****************************************************************************//**
    *
    * \brief Reads the current WDT counter value.
    *
    * \return A live counter value.
    *
    *******************************************************************************/
    uint32 CySysWdtGetCount(void)
    {
        return ((uint32)CY_SYS_WDT_COUNTER_REG);
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetIgnoreBits
    ****************************************************************************//**
    *
    * \brief
    * Configures the number of the MSB bits of the watchdog timer that are not
    * checked against the match.
    *
    * \param bitsNum Valid range [0-15]. The number of the MSB bits.
    *
    * \details The value of bitsNum controls the time-to-reset of the watchdog
    * (which happens after 3 successive matches).
    *
    *******************************************************************************/
    void CySysWdtSetIgnoreBits(uint32 bitsNum)
    {
        bitsNum = ((uint32)(bitsNum << CY_SYS_WDT_IGNORE_BITS_SHIFT) & CY_SYS_WDT_IGNORE_BITS_MASK);
        CY_SYS_WDT_MATCH_REG = (CY_SYS_WDT_MATCH_REG & (uint32)(~CY_SYS_WDT_IGNORE_BITS_MASK)) | bitsNum;
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetIgnoreBits
    ****************************************************************************//**
    *
    * \brief
    * Reads the number of the  MSB bits of the watchdog timer that are not
    * checked against the match.
    *
    * \return The number of the MSB bits.
    *
    *******************************************************************************/
    uint32 CySysWdtGetIgnoreBits(void)
    {
        return((uint32)((CY_SYS_WDT_MATCH_REG & CY_SYS_WDT_IGNORE_BITS_MASK) >> CY_SYS_WDT_IGNORE_BITS_SHIFT));
    }


    /*******************************************************************************
    * Function Name: CySysWdtClearInterrupt
    ****************************************************************************//**
    *
    * \brief
    * Feeds the watchdog.
    * Cleans the WDT match flag which is set every time the WDT counter reaches a 
    * WDT match value. Two unserviced interrupts lead to a system reset 
    * (i.e. at the third match).
    *
    *******************************************************************************/
    void CySysWdtClearInterrupt(void)
    {
        CY_SYS_SRSS_INTR_REG |= CY_SYS_WDT_LOWER_BIT_MASK;
    }


    /*******************************************************************************
    * Function Name: CySysWdtMaskInterrupt
    ****************************************************************************//**
    *
    * \brief
    * After masking interrupts from WDT, they are not passed to CPU.
    * This function does not disable WDT reset generation.
    *
    *******************************************************************************/
    void CySysWdtMaskInterrupt(void)
    {
        CY_SYS_SRSS_INTR_MASK_REG &= (uint32)(~ (uint32)CY_SYS_WDT_LOWER_BIT_MASK);
    }


    /*******************************************************************************
    * Function Name: CySysWdtUnmaskInterrupt
    ****************************************************************************//**
    *
    * \brief
    * After unmasking interrupts from WDT, they are passed to CPU.
    * This function does not impact the reset generation.
    *
    *******************************************************************************/
    void CySysWdtUnmaskInterrupt(void)
    {
        CY_SYS_SRSS_INTR_MASK_REG |= CY_SYS_WDT_LOWER_BIT_MASK;
    }


    /*******************************************************************************
    * Function Name: CySysWdtSetIsrCallback
    ****************************************************************************//**
    *
    * \brief
    * Sets the ISR callback function for the WDT counter
    *
    * \param function The pointer to the callback function.
    *
    * \return The pointer to a previous callback function.
    *
    *******************************************************************************/
    cyWdtCallback CySysWdtSetInterruptCallback(cyWdtCallback function)
    {
        cyWdtCallback prevCallback = (void *)0;

        prevCallback = cySysWdtCallback;
        cySysWdtCallback = function;

        return(prevCallback);
    }


    /*******************************************************************************
    * Function Name: CySysWdtGetIsrCallback
    ****************************************************************************//**
    *
    * \brief
    * Gets the ISR callback function for the WDT counter
    *
    * \return The pointer to the callback function registered for WDT.
    *
    *******************************************************************************/
    cyWdtCallback CySysWdtGetInterruptCallback(void)
    {
        return(cySysWdtCallback);
    }


    /*******************************************************************************
    * Function Name: CySysWdtIsr
    ****************************************************************************//**
    *
    * \brief
    * This is the handler of the WDT interrupt in CPU NVIC.
    *
    * The handler calls the respective callback functions configured by the user 
    * by using CySysWdtSetIsrCallback() API.
    *
    *
    * \details
    * This function clears the WDT interrupt every time when it is called.
    * Reset after the 3rd interrupt does not happen if this function is registered
    * as the interrupt handler even if the "Watchdog with Interrupt" mode is
    * selected on the "Low Frequency Clocks" tab.
    *
    *******************************************************************************/
    void CySysWdtIsr(void)
    {
        if(cySysWdtCallback != (void *) 0)
        {
            (void)(cySysWdtCallback)();
        }

        CySysWdtClearInterrupt();
    }

#endif /* (CY_IP_SRSSV2) */


#if(CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN)

/*******************************************************************************
* Function Name: CySysClkGetTimerSource
********************************************************************************
*
* \brief Gets the clock source for the DeepSleep Timers.
*
* The function is applicable only for PSoC 4100S / PSoC Analog Coprocessor.
*
* \return The DeepSleep Timer source
* CY_SYS_CLK_TIMER_SRC_ILO Internal Low Frequency (32 kHz) Oscillator (ILO)
* CY_SYS_CLK_TIMER_SRC_WCO Low Frequency Watch Crystal Oscillator (WCO)
*
*******************************************************************************/
    static uint32 CySysClkGetTimerSource(void)
    {
        uint32 timerSource;
        timerSource = CY_SYS_WCO_WDT_CONFIG_REG & CY_SYS_CLK_TIMER_SEL_MASK;
        return (timerSource);
    }    


/*******************************************************************************
* Function Name: CySysClkSetTimerSource
****************************************************************************//**
*
* \brief Sets the clock source for the DeepSleep Timers.
*
* The function is applicable only for PSoC 4100S / PSoC Analog Coprocessor 
* devices.
*
* \param source
*  CY_SYS_CLK_TIMER_SRC_ILO - Internal Low Frequency (32 kHz) Oscillator 
* (ILO).<br>
* CY_SYS_CLK_TIMER_SRC_WCO - Low Frequency Watch Crystal Oscillator 
* (WCO).
*
* \details Both the current source and the new source must be running and stable 
* before calling this function.
*
* \warning DeepSleep Timer reset is required if Timer source was switched while 
* DeepSleep Timers were running. Call CySysTimerResetCounters() API after 
* Timer source switching.
* It is highly recommended to disable DeepSleep Timers before Timer source 
* switching. Changing the Timer source may change the functionality that uses 
* this Timers as clock source.
*******************************************************************************/
    void CySysClkSetTimerSource(uint32 source)
    {
        uint8  interruptState;

        if (CySysClkGetTimerSource() != source)
        {

            /* Reset both _EN bits in WCO_WDT_CLKEN register */
            CY_SYS_WCO_WDT_CLKEN_REG &= ~CY_SYS_WCO_WDT_CLKEN_RESET_MASK;
            
            /* Wait 4 new clock source-cycles for change to come into effect */
            CyDelayUs(CY_SYS_4TIMER_DELAY_US);

            interruptState = CyEnterCriticalSection();
            CY_SYS_WCO_WDT_CONFIG_REG = (CY_SYS_WCO_WDT_CONFIG_REG & (uint32)(~CY_SYS_CLK_TIMER_SEL_MASK)) |
                                    (source & CY_SYS_CLK_TIMER_SEL_MASK);
            CyExitCriticalSection(interruptState);
        }

        CY_SYS_WCO_WDT_CLKEN_REG = (CY_SYS_WCO_WDT_CLKEN_REG & (uint32)(~CY_SYS_WCO_WDT_CLKEN_RESET_MASK)) |
                                    CY_SYS_SET_CURRENT_TIMER_SOURCE_BIT;
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetEnabledStatus
    ****************************************************************************//**
    *
    * \brief Reads the enabled status of one of the three DeepSleep Timer 
    * counters.
    *
    * \param counterNum: Valid range [0-2]. The number of the DeepSleep Timer 
    * counter.
    *
    * \return The status of the Timers counter:
    * \return 0 - If the Counter is disabled.
    * \return 1 - If the Counter is enabled.
    *
    * \details
    * This function returns an actual DeepSleep Timer counter status from the 
    * status register. It may take up to 3 LFCLK cycles for the Timer status 
    * register to contain actual data after the Timer counter is enabled.
    *
    *******************************************************************************/
    uint32 CySysTimerGetEnabledStatus(uint32 counterNum)
    {
        CYASSERT(counterNum < CY_SYS_TIMER_COUNTERS_MAX);
        return ((CY_SYS_WCO_WDT_CONTROL_REG >> ((CY_SYS_TIMER_CNT_SHIFT * counterNum) + 
                                                 CY_SYS_TIMER_CNT_STTS_SHIFT)) & 0x01u);
    }


    /*******************************************************************************
    * Function Name: CySysTimerSetMode
    ****************************************************************************//**
    *
    * \brief Writes the mode of one of the three DeepSleep Timer counters.
    *
    * \param counterNum: Valid range [0-2]. The number of the DeepSleep Timer 
    * counter.
    *
    * \param mode
    *  CY_SYS_TIMER_MODE_NONE - Free running.<br>
    * CY_SYS_TIMER_MODE_INT - The interrupt generated on match for counter 0
    * and 1, and on bit toggle for counter 2.
    *
    * \details
    * DeepSleep Timer counter counterNum should be disabled to set a mode. 
    * Otherwise, this function call has no effect. If the specified counter is 
    * enabled, call the CySysTimerDisable() function with the corresponding 
    * parameter to disable the specified counter and wait for it to stop.
    *
    *******************************************************************************/
    void CySysTimerSetMode(uint32 counterNum, uint32 mode)
    {
        uint32 configRegValue;

        CYASSERT(counterNum < CY_SYS_TIMER_COUNTERS_MAX);

        CYASSERT(mode <= CY_SYS_TIMER_MODE_MASK);

        if(0u == CySysTimerGetEnabledStatus(counterNum))
        {
            configRegValue = CY_SYS_WCO_WDT_CONFIG_REG &
                                (uint32)~((uint32)(CY_SYS_TIMER_MODE_MASK << (counterNum * CY_SYS_TIMER_CNT_SHIFT)));
            configRegValue |= (uint32)((mode & CY_SYS_TIMER_MODE_MASK) << (counterNum * CY_SYS_TIMER_CNT_SHIFT));
            CY_SYS_WCO_WDT_CONFIG_REG = configRegValue;
        }
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetMode
    ****************************************************************************//**
    *
    * \brief Reads the mode of one of the three DeepSleep Timer counters.
    *
    * \param counterNum Valid range [0-2]. The number of the Timer counter.
    *
    * \return The mode of the counter. The same enumerated values as the mode 
    * parameter used in CySysTimerSetMode().
    *
    *******************************************************************************/
    uint32 CySysTimerGetMode(uint32 counterNum)
    {
        return ((CY_SYS_WCO_WDT_CONFIG_REG >> (counterNum * CY_SYS_TIMER_CNT_SHIFT)) & CY_SYS_TIMER_MODE_MASK);
    }


    /*******************************************************************************
    * Function Name: CySysTimerSetClearOnMatch
    ****************************************************************************//**
    *
    * \brief Configures the DeepSleep Timer counter "clear on match" setting.
    *
    * If configured to "clear on match", the counter counts from 0 to MatchValue 
    * giving it a period of (MatchValue + 1).
    *
    * \param counterNum
    * Valid range [0-1]. The number of the Timer counter. The match values are not
    * supported by counter 2.
    * \param enable 0 to disable appropriate counter <br>
    * 1 to enable appropriate counter
    *
    * \details
    * Timer counter counterNum should be disabled. Otherwise this function call
    * has no effect. If the specified counter is enabled, call the CySysTimerDisable()
    * function with the corresponding parameter to disable the specified counter and
    * wait for it to stop. This may take up to three Timer source-cycles.
    *
    *******************************************************************************/
    void CySysTimerSetClearOnMatch(uint32 counterNum, uint32 enable)
    {
        uint32 configRegValue;

        CYASSERT((counterNum == CY_SYS_TIMER0) ||
                 (counterNum == CY_SYS_TIMER1));

        if(0u == CySysTimerGetEnabledStatus(counterNum))
        {
            configRegValue = CY_SYS_WCO_WDT_CONFIG_REG & (uint32)~((uint32)((uint32)1u << 
             ((counterNum * CY_SYS_TIMER_CNT_SHIFT) + CY_SYS_TIMER_CNT_MATCH_CLR_SHIFT)));

            configRegValue
                |= (uint32)(enable << ((counterNum * CY_SYS_TIMER_CNT_SHIFT) + CY_SYS_TIMER_CNT_MATCH_CLR_SHIFT));

            CY_SYS_WCO_WDT_CONFIG_REG = configRegValue;
        }
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetClearOnMatch
    ****************************************************************************//**
    *
    * \brief Reads the "clear on match" setting for the specified DeepSleep Timer 
    * counter.
    *
    * \param counterNum Valid range [0-1].  The number of the Timer counter. The 
    * match values are not supported by counter 2.
    *
    * \return The "clear on match" status: <br>1 if enabled <br>0 if disabled
    *
    *******************************************************************************/
    uint32 CySysTimerGetClearOnMatch(uint32 counterNum)
    {
        CYASSERT((counterNum == CY_SYS_TIMER0) ||
                 (counterNum == CY_SYS_TIMER1));

        return (uint32)((CY_SYS_WCO_WDT_CONFIG_REG >>
                        ((counterNum * CY_SYS_TIMER_CNT_SHIFT) + CY_SYS_TIMER_CNT_MATCH_CLR_SHIFT)) & 0x01u);
    }


    /*******************************************************************************
    * Function Name: CySysTimerEnable
    ****************************************************************************//**
    *
    * \brief Enables the specified DeepSleep Timer counters. All the counters 
    * specified in the mask are enabled.
    *
    * \param counterMask  CY_SYS_TIMER0_MASK - The mask for counter 0 to enable.<br>
    * CY_SYS_TIMER1_MASK - The mask for counter 1 to enable.<br>
    * CY_SYS_TIMER2_MASK - The mask for counter 2 to enable.
    *
    * \details
    * Enabling or disabling Timer requires 3 Timer source-cycles to come into effect.
    * Therefore, the Timer enable state must not be changed more than once in
    * that period.
    *
    * After Timer is enabled, it is illegal to write Timer configuration 
    * (WCO_WDT_CONFIG) and control (WCO_WDT_CONTROL) registers. This means that all
    * Timer functions that contain 'write' in the name (with the exception of 
    * CySysTimerSetMatch() function) are illegal to call once Timer enabled.
    *
    * Timer current source must be running and stable before calling this 
    * function.
    *
    *******************************************************************************/
    void CySysTimerEnable(uint32 counterMask)
    {
        CY_SYS_WCO_WDT_CONTROL_REG |= counterMask;

        if(0u != (counterMask & CY_SYS_TIMER0_MASK))
        {
            while (0u == CySysTimerGetEnabledStatus(CY_SYS_TIMER0))
            {
                /* Wait for changes to come into effect */
            }
        }

        if(0u != (counterMask & CY_SYS_TIMER1_MASK))
        {
            while (0u == CySysTimerGetEnabledStatus(CY_SYS_TIMER1))
            {
                /* Wait for changes to come into effect */
            }
        }

        if(0u != (counterMask & CY_SYS_TIMER2_MASK))
        {
            while (0u == CySysTimerGetEnabledStatus(CY_SYS_TIMER2))
            {
                /* Wait for changes to come into effect */
            }
        }
    }


    /*******************************************************************************
    * Function Name: CySysTimerDisable
    ****************************************************************************//**
    *
    * \brief Disables the specified DeepSleep Timer counters.
    *
    * All the counters specified in the mask are disabled. The function waits for 
    * the changes to come into effect.
    *
    * \param counterMask
    *  CY_SYS_TIMER0_MASK - The mask for Counter 0 to disable.<br>
    * CY_SYS_TIMER1_MASK - The mask for Counter 1 to disable.<br>
    * CY_SYS_TIMER2_MASK - The mask for Counter 2 to disable.
    *
    *******************************************************************************/
    void CySysTimerDisable(uint32 counterMask)
    {

        CY_SYS_WCO_WDT_CONTROL_REG &= ~counterMask;

        if(0u != (counterMask & CY_SYS_TIMER0_MASK))
        {
            while (0u != CySysTimerGetEnabledStatus(CY_SYS_TIMER0))
            {
                /* Wait for changes to come into effect */
            }
        }

        if(0u != (counterMask & CY_SYS_TIMER1_MASK))
        {
            while (0u != CySysTimerGetEnabledStatus(CY_SYS_TIMER1))
            {
                /* Wait for changes to come into effect */
            }
        }

        if(0u != (counterMask & CY_SYS_TIMER2_MASK))
        {
            while (0u != CySysTimerGetEnabledStatus(CY_SYS_TIMER2))
            {
                /* Wait for changes to come into effect */
            }
        }

    }


    /*******************************************************************************
    * Function Name: CySysTimerSetCascade
    ****************************************************************************//**
    *
    * \brief
    * Writes the two DeepSleep Timers cascade values based on the combination of 
    * mask values specified.
    *
    * \param cascadeMask The mask value used to set or clear the cascade values:
    * CY_SYS_TIMER_CASCADE_NONE - Neither<br>
    * CY_SYS_TIMER_CASCADE_01 - Cascade 01<br>
    * CY_SYS_TIMER_CASCADE_12 - Cascade 12
    *
    * If only one cascade mask is specified, the second cascade is disabled.
    * To set both cascade modes, two defines should be ORed:
    * (CY_SYS_TIMER_CASCADE_01 | CY_SYS_TIMER_CASCADE_12).
    * \note If CySysTimerSetCascade() was called with ORed defines it is necessary 
    * to call CySysTimeSetClearOnMatch(1,1). It is needed to make sure that 
    * Counter 2 will be updated in the expected way.
    *
    * Timer counters that are part of the specified cascade should be disabled.
    * Otherwise this function call has no effect. If the specified
    * counter is enabled, call CySysTimerDisable() function with the corresponding
    * parameter to disable the specified counter and wait for it to stop. This may
    * take up to 3 Timers source-cycles.
    *
    *******************************************************************************/
    void CySysTimerSetCascade(uint32 cascadeMask)
    {
        uint32 configRegValue;
        uint32 countersEnableStatus;

        countersEnableStatus = CySysTimerGetEnabledStatus(CY_SYS_TIMER0) |
                            CySysTimerGetEnabledStatus(CY_SYS_TIMER1) |
                            CySysTimerGetEnabledStatus(CY_SYS_TIMER2);

        if (0u == countersEnableStatus)
        {
            configRegValue = CY_SYS_WCO_WDT_CONFIG_REG;
            configRegValue &= ((uint32)(~(CY_SYS_TIMER_CASCADE_01|CY_SYS_TIMER_CASCADE_12)));
            configRegValue |= cascadeMask;
            CY_SYS_WCO_WDT_CONFIG_REG = configRegValue;
        }
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetCascade
    ****************************************************************************//**
    *
    * \brief Reads the two DeepSleep Timer cascade values returning a mask of the 
    * bits set.
    *
    * \return The mask of the cascade values set.
    * \return CY_SYS_TIMER_CASCADE_NONE - Neither
    * \return CY_SYS_TIMER_CASCADE_01 - Cascade 01
    * \return CY_SYS_TIMER_CASCADE_12 - Cascade 12
    *
    *******************************************************************************/
    uint32 CySysTimerGetCascade(void)
    {
        return (CY_SYS_WCO_WDT_CONFIG_REG & (CY_SYS_TIMER_CASCADE_01 | CY_SYS_TIMER_CASCADE_12));
    }


    /*******************************************************************************
    * Function Name: CySysTimerSetMatch
    ****************************************************************************//**
    *
    * \brief Configures the Timer counter match comparison value.
    *
    * \param counterNum Valid range [0-1]. The number of the Timer counter. The 
    * match values are not supported by counter 2.
    *
    * \param match Valid range [0-65535]. The value to be used to match against 
    * the counter.
    *
    *******************************************************************************/
    void CySysTimerSetMatch(uint32 counterNum, uint32 match)
    {
        uint32 regValue;

        CYASSERT((counterNum == CY_SYS_TIMER0) ||
                 (counterNum == CY_SYS_TIMER1));

        /* Wait for previous changes to come into effect */
        CyDelayUs(CY_SYS_3TIMER_DELAY_US);

        regValue = CY_SYS_WCO_WDT_MATCH_REG;
        regValue &= (uint32)~((uint32)(CY_SYS_TIMER_LOWER_16BITS_MASK << (counterNum * CY_SYS_TIMER_CNT_MATCH_SHIFT)));
        CY_SYS_WCO_WDT_MATCH_REG = (regValue | (match << (counterNum * CY_SYS_TIMER_CNT_MATCH_SHIFT)));

        /* Make sure match synchronization has started */
        CyDelayUs(CY_SYS_1TIMER_DELAY_US);
    }


    /*******************************************************************************
    * Function Name: CySysTimerSetToggleBit
    ****************************************************************************//**
    *
    * \brief Configures which bit in Timer counter 2 to monitor for a toggle.
    *
    * When that bit toggles, an interrupt is generated if mode for counter 2 has
    * enabled interrupts.
    *
    * \param bits Valid range [0-31]. Counter 2 bit to monitor for a toggle.
    *
    * \details Timer counter 2 should be disabled. Otherwise this function call has
    * no effect.
    *
    * If the specified counter is enabled, call the CySysTimerDisable() function with
    * the corresponding parameter to disable the specified counter and wait for it to
    * stop. This may take up to three Timer source-cycles.
    *
    *******************************************************************************/
    void CySysTimerSetToggleBit(uint32 bits)
    {
        uint32 configRegValue;

        if (0u == CySysTimerGetEnabledStatus(CY_SYS_TIMER2))
        {
            configRegValue = CY_SYS_WCO_WDT_CONFIG_REG;
            configRegValue &= (uint32)(~((uint32)(CY_SYS_TIMER_CONFIG_BITS2_MASK << CY_SYS_TIMER_CONFIG_BITS2_POS)));
            configRegValue |= ((bits & CY_SYS_TIMER_CONFIG_BITS2_MASK) << CY_SYS_TIMER_CONFIG_BITS2_POS);
            CY_SYS_WCO_WDT_CONFIG_REG = configRegValue;
        }
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetToggleBit
    ****************************************************************************//**
    *
    * \brief Reads which bit in Timer counter 2 is monitored for a toggle.
    *
    * \return The bit that is monitored (range of 0 to 31)
    *
    *******************************************************************************/
    uint32 CySysTimerGetToggleBit(void)
    {
        return ((CY_SYS_WCO_WDT_CONFIG_REG >> CY_SYS_TIMER_CONFIG_BITS2_POS) & CY_SYS_TIMER_CONFIG_BITS2_MASK);
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetMatch
    ****************************************************************************//**
    *
    * \brief Reads the Timer counter match comparison value.
    *
    * \param counterNum Valid range [0-1]. The number of the DeepSleep Timer 
    * counter. The match values are not supported by counter 2.
    *
    * \return A 16-bit match value.
    *
    *******************************************************************************/
    uint32 CySysTimerGetMatch(uint32 counterNum)
    {
        CYASSERT((counterNum == CY_SYS_TIMER0) ||
                 (counterNum == CY_SYS_TIMER1));

        return ((uint32)(CY_SYS_WCO_WDT_MATCH_REG >> (counterNum * CY_SYS_TIMER_CNT_MATCH_SHIFT)) &
                                                                  CY_SYS_TIMER_LOWER_16BITS_MASK);
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetCount
    ****************************************************************************//**
    *
    * \brief Reads the current DeepSleep Timer counter value.
    *
    * \param counterNum Valid range [0-2]. The number of the Timer counter.
    *
    * \return A live counter value. Counter 0 and Counter 1 are 16 bit counters 
    * and counter 2 is a 32 bit counter.
    *
    *******************************************************************************/
    uint32 CySysTimerGetCount(uint32 counterNum)
    {
        uint32 regValue = 0u;

        switch(counterNum)
        {
            /* Timer Counter 0 */
            case 0u:
                regValue = CY_SYS_WCO_WDT_CTRLOW_REG & CY_SYS_TIMER_LOWER_16BITS_MASK;
            break;

            /* Timer Counter 1 */
            case 1u:
                regValue = (CY_SYS_WCO_WDT_CTRLOW_REG >> CY_SYS_TIMER_CNT_MATCH_SHIFT) & CY_SYS_TIMER_LOWER_16BITS_MASK;
            break;

            /* Timer Counter 2 */
            case 2u:
                regValue = CY_SYS_WCO_WDT_CTRHIGH_REG;
            break;

            default:
                CYASSERT(0u != 0u);
            break;
        }

        return (regValue);
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetInterruptSource
    ****************************************************************************//**
    *
    * \brief
    * Reads a mask containing all the DeepSleep Timer counters interrupts that are 
    * currently set by the hardware, if a corresponding mode is selected.
    *
    * \return The mask of interrupts set
    * \return CY_SYS_TIMER0_INT - Set interrupt for Counter 0
    * \return CY_SYS_TIMER1_INT - Set interrupt for Counter 1
    * \return CY_SYS_TIMER2_INT - Set interrupt for Counter 2
    *
    *******************************************************************************/
    uint32 CySysTimerGetInterruptSource(void)
    {
        return (CY_SYS_WCO_WDT_CONTROL_REG & (CY_SYS_TIMER0_INT | CY_SYS_TIMER1_INT | CY_SYS_TIMER2_INT));
    }


    /*******************************************************************************
    * Function Name: CySysTimerClearInterrupt
    ****************************************************************************//**
    *
    * \brief Clears all the DeepSleep Timer counter interrupts set in the mask.
    *
    * All the Timer interrupts are to be cleared by the firmware, otherwise
    * interrupts are generated continuously.
    *
    * \param counterMask
    *  CY_SYS_TIMER0_INT - Clear counter 0<br>
    * CY_SYS_TIMER1_INT - Clear counter 1<br>
    * CY_SYS_TIMER2_INT - Clear counter 2
    *
    *******************************************************************************/
    void CySysTimerClearInterrupt(uint32 counterMask)
    {
        uint8 interruptState;
        interruptState = CyEnterCriticalSection();

        /* Set new WCO_TIMER control register value */
        counterMask &= (CY_SYS_TIMER0_INT |
                        CY_SYS_TIMER1_INT |
                        CY_SYS_TIMER2_INT);

        CY_SYS_WCO_WDT_CONTROL_REG = counterMask | (CY_SYS_WCO_WDT_CONTROL_REG & ~(CY_SYS_TIMER0_INT |
                                                                           CY_SYS_TIMER1_INT |
                                                                           CY_SYS_TIMER2_INT));

        /* Read the CY_SYS_WDT_CONTROL_REG to clear the interrupt request.
         * Cypress ID #207093, #206231
        */
        (void)CY_SYS_WCO_WDT_CONTROL_REG;

        CyExitCriticalSection(interruptState);
    }


    /*******************************************************************************
    * Function Name: CySysTimerSetInterruptCallback
    ****************************************************************************//**
    *
    * \brief
    * Sets the ISR callback function for the particular DeepSleep Timer counter.
    *
    * These functions are called on the Timer interrupt.
    *
    * \param counterNum The number of the Timer counter.
    * \param function The pointer to the callback function.
    *
    * \return  The pointer to the previous callback function. 
    * \return NULL is returned if the specified address is not set.
    *
    *******************************************************************************/
    cyTimerCallback CySysTimerSetInterruptCallback(uint32 counterNum, cyTimerCallback function)
    {
        cyTimerCallback prevCallback = (void *)0;

        if(counterNum < CY_SYS_NUM_OF_TIMERS)
        {
            prevCallback = cySysTimerCallback[counterNum];
            cySysTimerCallback[counterNum] = function;
        }
        else
        {
            CYASSERT(0u != 0u);
        }

        return((cyTimerCallback)prevCallback);
    }


    /*******************************************************************************
    * Function Name: CySysTimerGetInterruptCallback
    ****************************************************************************//**
    *
    * \brief Gets the ISR callback function for the particular DeepSleep Timer 
    * counter.
    *
    * \param counterNum The number of the Timer counter.
    *
    * \return
    * The pointer to the callback function registered for a particular Timer by
    * a particular address that are passed through arguments.
    *
    *******************************************************************************/
    cyTimerCallback CySysTimerGetInterruptCallback(uint32 counterNum)
    {
        cyTimerCallback retCallback = (void *)0;

        if(counterNum < CY_SYS_NUM_OF_TIMERS)
        {
            retCallback = (cyTimerCallback)cySysTimerCallback[counterNum];
        }
        else
        {
            CYASSERT(0u != 0u);
        }

        return(retCallback);
    }


    /*******************************************************************************
    * Function Name: CySysTimerEnableIsr
    ****************************************************************************//**
    *
    * \brief Enables the ISR callback servicing for the particular Timer counter
    *
    * \param counterNum Valid range [0-2]. The number of the Timer counter.
    *
    * Value corresponds to appropriate Timer counter. For example value 1 
    * corresponds to second Timer counter.
    *
    *******************************************************************************/
    void CySysTimerEnableIsr(uint32 counterNum)
    {
        if(counterNum <= CY_SYS_TIMER2)
        {
            disableTimerServicedIsr &= ~counterTimerIntMaskTbl[counterNum];
            timerIsrMask |= counterTimerIntMaskTbl[counterNum];
        }
        else
        {
            CYASSERT(0u != 0u);
        }
    }


    /*******************************************************************************
    * Function Name: CySysTimerDisableIsr
    ****************************************************************************//**
    *
    * \brief Disables the ISR callback servicing for the particular Timer counter
    *
    * \param counterNum Valid range [0-2]. The number of the Timer counter.
    *
    *******************************************************************************/
    void CySysTimerDisableIsr(uint32 counterNum)
    {
        if(counterNum <= CY_SYS_TIMER2)
        {
            timerIsrMask &= ~counterTimerIntMaskTbl[counterNum];
        }
        else
        {
            CYASSERT(0u != 0u);
        }
    }


    /*******************************************************************************
    * Function Name: CySysTimerIsr
    ****************************************************************************//**
    *
    * \brief This is the handler of the DeepSleep Timer interrupt in CPU NVIC.
    *
    * The handler checks which Timer triggered in the interrupt and calls the 
    * respective callback functions configured by the user by using 
    * CySysTimerSetIsrCallback() API.
    *
    * The order of the callback execution is incremental. Callback-0 is
    * run as the first one and callback-2 is called as the last one.
    *
    * \details This function clears the DeepSleep Timer interrupt every time when 
    * it is called.
    *
    *******************************************************************************/
    void CySysTimerIsr(void)
    {
        if(0u != (CY_SYS_TIMER0_INT & CY_SYS_WCO_WDT_CONTROL_REG))
        {
            if(0u != (CY_SYS_TIMER0_INT & timerIsrMask))
            {
                timerIsrMask &= ~(disableTimerServicedIsr & CY_SYS_TIMER0_INT);
                disableTimerServicedIsr  &= ~CY_SYS_TIMER0_INT;
                if(cySysTimerCallback[CY_SYS_TIMER0] != (void *) 0)
                {
                    (void)(cySysTimerCallback[CY_SYS_TIMER0])();
                }
            }
            CySysTimerClearInterrupt(CY_SYS_TIMER0_INT);
        }

        if(0u != (CY_SYS_TIMER1_INT & CY_SYS_WCO_WDT_CONTROL_REG))
        {
            if(0u != (CY_SYS_TIMER1_INT & timerIsrMask))
            {
                timerIsrMask &= ~(disableTimerServicedIsr & CY_SYS_TIMER1_INT);
                disableTimerServicedIsr  &= ~CY_SYS_TIMER1_INT;
                if(cySysTimerCallback[CY_SYS_TIMER1] != (void *) 0)
                {
                    (void)(cySysTimerCallback[CY_SYS_TIMER1])();
                }
            }
            CySysTimerClearInterrupt(CY_SYS_TIMER1_INT);
        }

        if(0u != (CY_SYS_TIMER2_INT & CY_SYS_WCO_WDT_CONTROL_REG))
        {
            if(0u != (CY_SYS_TIMER2_INT & timerIsrMask))
            {
                if(cySysTimerCallback[CY_SYS_TIMER2] != (void *) 0)
                {
                    (void)(cySysTimerCallback[CY_SYS_TIMER2])();
                }
            }
            CySysTimerClearInterrupt(CY_SYS_TIMER2_INT);
        }
    }


    /*******************************************************************************
    * Function Name: CySysTimerResetCounters
    ****************************************************************************//**
    *
    * \brief Resets all the Timer counters set in the mask.
    *
    * \param countersMask
    * CY_SYS_TIMER0_RESET - Reset the Counter 0<br>
    * CY_SYS_TIMER1_RESET - Reset the Counter 1<br>
    * CY_SYS_TIMER2_RESET - Reset the Counter 2
    *
    * \details
    * This function waits while corresponding counters will be reset. This may
    * take up to 3 DeepSleep Timer source-cycles. DeepSleep Timer source must be 
    * enabled. Otherwise, the function will never exit.
    *
    *******************************************************************************/
    void CySysTimerResetCounters(uint32 countersMask)
    {
        /* Set new Timer reset value */
        CY_SYS_WCO_WDT_CONTROL_REG |= (countersMask & CY_SYS_TIMER_RESET);

        while (0uL != (CY_SYS_WCO_WDT_CONTROL_REG & CY_SYS_TIMER_RESET))
        {
            /* Wait for reset to come into effect */
        }
    }
#endif /* (CY_IP_SRSSLT && CY_IP_WCO && CY_IP_WCO_WDT_EN) */


#if(CY_IP_SRSSV2 || (CY_IP_WCO_WDT_EN && CY_IP_SRSSLT))
    /*******************************************************************************
    * Function Name: CySysTimerDelay
    ****************************************************************************//**
    *
    * \brief
    * The function implements the delay specified in the LFCLK clock ticks.
    *
    * This API is applicable for PSoC 4100 / PSoC 4200 / PSoC 4100 BLE / 
    * PSoC 4200 BLE / PRoC BLE / PSoC 4200L / PSoC 4100M / PSoC 4200M devices to 
    * use WDT. Also this API is available to use for PSoC4100S and / PSoC Analog 
    * Coprocessor devices to use DeepSleep Timers.
    *
    * For PSoC 4100 / PSoC 4200 / PSoC 4100 BLE / PSoC 4200 BLE / PRoC BLE / PSoC 
    * 4200L / PSoC 4100M / PSoC 4200M devices:
    * The specified WDT counter should be configured as described below and started.
    *
    * For PSoC 4100S / PSoC Analog Coprocessor devices:
    * The specified DeepSleep Timer counter should be configured as described below
    * and started.
    *
    * This function can operate in two modes: the "WAIT" and "INTERRUPT" modes. In
    * the "WAIT" mode, the function waits for the specified number of ticks. In the
    * "INTERRUPT" mode, the interrupt is generated after the specified number of
    * ticks.
    *
    * For the correct function operation, the "Clear On Match" option should be
    * disabled for the specified WDT or DeepSleep Timer counter. Use 
    * CySysWdtSetClearOnMatch() for WDT or CySysTimerSetClearOnMatch() for DeepSleep
    * Timer function with the "enable" parameter equal to zero for the used WDT 
    * counter or DeepSleep Timer counter.
    *
    * The corresponding WDT counter should be configured to match the selected
    * mode: "Free running Timer" for the "WAIT" mode, and
    * "Periodic Timer" / "Watchdog (w/Interrupt)" for the "INTERRUPT" mode.
    *
    * Or the corresponding DeepSleep Timer counter should be configured to match the
    * selected mode: "Free running Timer" for the "WAIT" mode, and
    * "Periodic Timer" for the "INTERRUPT" mode.
    *
    * This can be configured in two ways:
    * - Through the DWR page. Open the "Clocks" tab, click the "Edit Clocks..."
    * button, in the "Configure System Clocks" window click on the
    * "Low Frequency Clocks" tab and choose the appropriate option for the used
    * WDT or DeepSleep Timer counter.
    *
    * - Through the CySysWdtSetMode() for WDT or CySysTimerSetMode() for DeepSleep
    * Timer function. Call it with the appropriate "mode" parameter for the 
    * used WDT or DeepSleep Timer counter.
    *
    * For the "INTERRUPT" mode, the recommended sequence is the following:
    * - Call the CySysWdtDisableCounterIsr() for WDT or 
    * CySysTimerDisableIsr() for DeepSleep Timer function to disable servicing
    * interrupts of the specified WDT or DeepSleep Timer counter.
    *
    * - Call the CySysWdtSetInterruptCallback() for WDT or 
    * CySysTimerSetIsrCallback() for DeepSleep Timer function to register 
    * the callback function for the corresponding WDT or DeepSleep Timer counter.
    *
    * - Call the CySysTimerDelay() function.
    *
    * \param counterNum Valid range [0-1]. The number of the counter 
    * (Timer0 or Timer1).
    * \param delayType
    * CY_SYS_TIMER_WAIT - "WAIT" mode. <br>
    * CY_SYS_TIMER_INTERRUPT - "INTERRUPT" mode.
    * \param delay The delay value in the LFCLK ticks 
    * (allowable range - 16-bit value).
    *
    * \details
    * In the "INTERRUPT" mode, this function enables ISR callback servicing
    * from the corresponding WDT or DeepSleep Timer counter. Servicing of this ISR 
    * callback will be disabled after the expiration of the delay time.
    *
    *******************************************************************************/
    void CySysTimerDelay(uint32 counterNum, cy_sys_timer_delaytype_enum delayType, uint32 delay)
    {
        uint32 regValue;
        uint32 matchValue;

        #if(CY_IP_SRSSV2)
            if((counterNum < CY_SYS_WDT_COUNTER2) && (0uL == CySysWdtGetClearOnMatch(counterNum)) && 
                                                                (delay <= CY_SYS_UINT16_MAX_VAL))
            {
                regValue   = CySysWdtGetCount(counterNum);
                matchValue = (regValue + delay) & (uint32)CY_SYS_UINT16_MAX_VAL;

                CySysTimerDelayUntilMatch(counterNum, delayType, matchValue);
            }
            else
            {
                CYASSERT(0u != 0u);
            }
        #endif /* (CY_IP_SRSSV2) */

        #if(CY_IP_WCO_WDT_EN && CY_IP_SRSSLT)
            if((counterNum < CY_SYS_TIMER2) && (0uL == CySysTimerGetClearOnMatch(counterNum)) && 
                                                            (delay <= CY_SYS_UINT16_MAX_VAL))
            {
                regValue   = CySysTimerGetCount(counterNum);
                matchValue = (regValue + delay) & (uint32)CY_SYS_UINT16_MAX_VAL;

                CySysTimerDelayUntilMatch(counterNum, delayType, matchValue);
            }
            else
            {
                CYASSERT(0u != 0u);
            }
        #endif /* (CY_IP_WCO_WDT_EN) */
    }


    /*******************************************************************************
    * Function Name: CySysTimerDelayUntilMatch
    ****************************************************************************//**
    *
    * \brief
    * The function implements the delay specified as the number of WDT or DeepSleep
    * Timer clock source ticks between WDT or DeepSleep Timer current value and 
    * match" value.
    *
    * This API is applicable for PSoC 4100 / PSoC 4200 / PRoC BLE / PSoC 4100 BLE /
    * PSoC 4200 BLE / PSoC 4200L / PSoC 4100M / PSoC 4200M devices to use WDT.
    * Also this API is available to use for PSoC4100S / Analog Coprocessor devices
    * to use DeepSleep Timers.
    * 
    * For PSoC 4100 / PSoC 4200 / PSoC 4100 BLE / PSoC 4200 BLE / PRoC BLE / PSoC 
    * 4200L / PSoC 4100M / PSoC 4200M devices:
    * The function implements the delay specified as the number of LFCLK ticks
    * between the specified WDT counter's current value and the "match"
    * passed as the parameter to this function. The current WDT counter value can 
    * be obtained using the CySysWdtGetCount() function.
    *
    * For PSoC4100 S and Analog Coprocessor devices:
    * The function implements the delay specified as the number of DeepSleep Timer 
    * input clock ticks for Timer0/Timer1 counter's current value and the "match" 
    * passed as the parameter to this function. The current DeepSleep Timer counter
    * value can be obtained using the CySysWdtGetCount() function.
    *
    * For PSoC 4100 / PSoC 4200 / PSoC 4100 BLE / PSoC 4200 BLE / PRoC BLE / PSoC 
    * 4200L / PSoC 4100M / PSoC 4200M devices:
    * The specified WDT counter should be configured as described below and started.
    *
    * For PSoC PSoC 4100S / PSoC Analog Coprocessor devices:
    * The specified DeepSleep Timer counter should be configured as described below
    * and started.
    *
    * This function can operate in two modes: the "WAIT" and "INTERRUPT" modes. In
    * the "WAIT" mode, the function waits for the specified number of ticks. In the
    * "INTERRUPT" mode, the interrupt is generated after the specified number of
    * ticks.
    *
    * For the correct function operation, the "Clear On Match" option should be
    * disabled for the specified WDT or DeepSleep Timer counter. Use 
    * CySysWdtSetClearOnMatch() for WDT or CySysTimerSetClearOnMatch() for DeepSleep
    * Timer function with the "enable" parameter equal to zero for the used WDT 
    * or DeepSleep Timer counter.
    *
    * For PSoC 4100 / PSoC 4200 / PSoC 4100 BLE / PSoC 4200 BLE / PRoC BLE / PSoC 
    * 4200L / PSoC 4100M/PSoC 4200M devices:
    * The corresponding WDT counter should be configured to match the selected
    * mode: "Free running Timer" for the "WAIT" mode, and
    * "Periodic Timer" / "Watchdog (w/Interrupt)" for the "INTERRUPT" mode.
    *
    * For PSoC 4100S / PSoC Analog Coprocessor devices:
    * Corresponding DeepSleep Timer counter should be configured to match the 
    * selected mode: "Free running Timer" for the "WAIT" mode, and
    * "Periodic Timer" for the "INTERRUPT" mode.
    *
    * This can be configured in two ways:
    * - Through the DWR page. Open the "Clocks" tab, click the "Edit Clocks..."
    * button, in the "Configure System Clocks" window click on the
    * "Low Frequency Clocks" tab and choose the appropriate option for the used
    * WDT or DeepSleep Timer counter.
    *
    * - Through the CySysWdtSetMode() for WDT or CySysTimerSetMode() for DeepSleep
    * Timer function. Call it with the appropriate "mode" parameter for the 
    * used WDT or DeepSleep Timer counter.
    *
    * For the "INTERRUPT" mode, the recommended sequence is the following:
    * - Call the CySysWdtDisableCounterIsr() for WDT or 
    * CySysTimerDisableIsr() for DeepSleep Timer function to disable servicing
    * interrupts of the specified WDT or DeepSleep Timer counter.
    *
    * - Call the CySysWdtSetInterruptCallback() for WDT or 
    * CySysTimerSetInterruptCallback() for DeepSleep Timer function to register 
    * the callback function for the corresponding WDT or DeepSleep Timer counter.
    *
    * - Call the CySysTimerDelay() function.
    *
    * \param counterNum Valid range [0-1]. The number of the WDT or DeepSleep 
    * Timer.
    * counter (Timer0 or Timer1).
    * \param delayType  CY_SYS_TIMER_WAIT - "WAIT" mode.<br>
    * CY_SYS_TIMER_INTERRUPT - "INTERRUPT" mode.
    * \param delay The delay value in the LFCLK ticks
    * (allowable range - 16-bit value).
    *
    * \details
    * In the "INTERRUPT" mode, this function enables ISR callback servicing
    * from the corresponding WDT counter. Servicing of this ISR callback will be
    * disabled after the expiration of the delay time.
    *
    *******************************************************************************/
    void CySysTimerDelayUntilMatch(uint32 counterNum, cy_sys_timer_delaytype_enum delayType, uint32 match)
    {
        uint32 tmpValue;

        #if(CY_IP_SRSSV2)
            if((counterNum < CY_SYS_WDT_COUNTER2) && (0uL == CySysWdtGetClearOnMatch(counterNum)) && 
                                                                (match <= CY_SYS_UINT16_MAX_VAL))
            {
                if(delayType == CY_SYS_TIMER_WAIT)
                {
                    do
                    {
                        tmpValue = CySysWdtGetCount(counterNum);
                    }while(tmpValue > match);

                    do
                    {
                        tmpValue = CySysWdtGetCount(counterNum);
                    }while(tmpValue < match);
                }
                else
                {
                    tmpValue = counterIntMaskTbl[counterNum];
                    CySysWdtSetMatch(counterNum, match);

                    disableServicedIsr  |= tmpValue;
                    wdtIsrMask |= tmpValue;
                }
            }
            else
            {
                CYASSERT(0u != 0u);
            }

        #endif /* (CY_IP_SRSSV2) */  

        #if(CY_IP_WCO_WDT_EN && CY_IP_SRSSLT)
        if((counterNum < CY_SYS_TIMER2) && (0uL == CySysTimerGetClearOnMatch(counterNum)) &&
                                                        (match <= CY_SYS_UINT16_MAX_VAL))
        {
            if(delayType == CY_SYS_TIMER_WAIT)
            {
                do
                {
                    tmpValue = CySysTimerGetCount(counterNum);
                }while(tmpValue > match);

                do
                {
                    tmpValue = CySysTimerGetCount(counterNum);
                }while(tmpValue < match);
            }
            else
            {
                tmpValue = counterTimerIntMaskTbl[counterNum];
                CySysTimerSetMatch(counterNum, match);

                disableTimerServicedIsr  |= tmpValue;
                timerIsrMask |= tmpValue;
            }
        }
        else
        {
            CYASSERT(0u != 0u);
        }
        #endif /* (CY_IP_WCO_WDT_EN && CY_IP_SRSSLT) */
    }

#endif /* (CY_IP_SRSSV2 || (CY_IP_WCO_WDT_EN && CY_IP_SRSSLT) */

/* [] END OF FILE */
