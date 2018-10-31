/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/* Include all headers which are needed by this file in the following order:
 * Own public header
 * Own protected header
 * Own private header
 * System header (if necessary)
 * Other headers
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_ADC

#include "BCDS_HAL.h"

#if BCDS_FEATURE_ADC

#include "BSP_Adc.h"
#include "Mcu_Adc_Handle.h"
#include "BCDS_MCU_SPI_Handle.h"
#include "BCDS_MCU_DMA_Handle.h"
#include "BSP_BoardSettings.h"
#include "BspAdc.h"
#include "em_adc.h"

/* Put the type and macro definitions here */
/* Put constant and variable definitions here */

/*Create the ADC driver handler instance */
static struct Mcu_Adc_Handle_S AdcHandleInstance;

/*Create the pointer to store the address of driver handler instance , which needs to be passed as handle for the ADC middle ware interface */
static struct Mcu_Adc_Handle_S *AdcHandleInstancePtr = NULL;

/*Stores IRQ mcu Callback  function */
#if (BCDS_FEATURE_DMA == 0)
static ADC_IRQ_Callback_T AdcIrqCallBack = NULL;
#endif

#if (BCDS_FEATURE_DMA == 1)
/*DMA channel instance for ADC in Scan Mode */
static struct MCU_DMA_Channel_S AdcMcuScanModeDmaHandle;
/*DMA channel instance for ADC in Single Mode */
static struct MCU_DMA_Channel_S AdcMcuSingleModeDmaHandle;
/* Module scope storage of ADC driver callback for DMA in scan mode */
static ADC_DMA_Callback_T AdcScanDmaCallBack = NULL;
/* Module scope storage of ADC driver callback for DMA in single mode */
static ADC_DMA_Callback_T AdcSingleDmaCallBack = NULL;
#endif

/* Put private function declarations here */

#if (BCDS_FEATURE_DMA == 1)
static void Adc_ScanCompleteDmaCbfunc(uint32_t channel, bool primary, void * user)
{
    BCDS_UNUSED(channel);
    BCDS_UNUSED(user);
    if ((NULL != AdcScanDmaCallBack) && (NULL != AdcHandleInstancePtr))
    {
        /*To indicate in DMA mode at present which buffer holds valid data */
        uint32_t whichBufHasValidData = UINT32_MAX;
        struct MCU_DMA_Channel_S *localDmaHandle = (struct MCU_DMA_Channel_S *) AdcHandleInstancePtr->Link1RegPtr;
        uint32_t noOfDmaSamples = (AdcHandleInstancePtr->_DriverCtx.NumOfSamples) * (AdcHandleInstancePtr->_DriverCtx.NumOfScanChannels);
        DMA_RefreshPingPong(localDmaHandle->ChannelId, primary, false, NULL, NULL, noOfDmaSamples - 1, false);
        whichBufHasValidData = (uint32_t) primary;
        AdcHandleInstancePtr->_DriverCtx.PointerValue = &whichBufHasValidData;
        AdcScanDmaCallBack((ADC_T) AdcHandleInstancePtr);
    }
}

/* Callback function for the Single Complete DMA Callback */
static void Adc_SingleCompleteDmaCbfunc(uint32_t channel, bool primary, void * user)
{
    BCDS_UNUSED(channel);
    BCDS_UNUSED(user);
    BCDS_UNUSED(primary);
    if ((NULL != AdcSingleDmaCallBack) && (NULL != AdcHandleInstancePtr))
    {
        AdcSingleDmaCallBack((ADC_T) AdcHandleInstancePtr);
    }
}
#endif

/* Put function implementations here */
/*
 * Refer to interface header for description
 */
Retcode_T BSP_Adc_Connect(void)
{
    Retcode_T retVal = RETCODE_OK;
    AdcHandleInstance.Instance = ADC_HW_HANDLER;
#if (BCDS_FEATURE_DMA == 1)
    /* DMA Handle configurations for the Scan mode */
    AdcHandleInstance.TransferMode = BCDS_HAL_TRANSFER_MODE_DMA;
    AdcMcuScanModeDmaHandle.ChannelId = ADC_SCAN_CHANNEL_DMA_CHANNEL_ID;
    AdcMcuScanModeDmaHandle.Config.highPri = false;
    AdcMcuScanModeDmaHandle.Config.enableInt = true;
    AdcMcuScanModeDmaHandle.Config.select = DMAREQ_ADC0_SCAN;
    AdcMcuScanModeDmaHandle.CallBack.cbFunc = (DMA_FuncPtr_TypeDef) Adc_ScanCompleteDmaCbfunc;
    AdcMcuScanModeDmaHandle.CallBack.userPtr = &AdcHandleInstance;
    AdcHandleInstance.Link1RegPtr = (void *) &AdcMcuScanModeDmaHandle;

    /* DMA Handle configurations for the Single mode */
    AdcMcuSingleModeDmaHandle.ChannelId = ADC_SINGLE_CHANNEL_DMA_CHANNEL_ID;
    AdcMcuSingleModeDmaHandle.Config.highPri = false;
    AdcMcuSingleModeDmaHandle.Config.enableInt = true;
    AdcMcuSingleModeDmaHandle.Config.select = DMAREQ_ADC0_SINGLE;
    AdcMcuSingleModeDmaHandle.CallBack.cbFunc = (DMA_FuncPtr_TypeDef) Adc_SingleCompleteDmaCbfunc;
    AdcMcuSingleModeDmaHandle.CallBack.userPtr = &AdcHandleInstance;
    AdcHandleInstance.Link2RegPtr = (void *) &AdcMcuSingleModeDmaHandle;
#elif (BCDS_FEATURE_DMA == 0)
    AdcHandleInstance.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
#endif
    AdcHandleInstancePtr = &AdcHandleInstance;
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Adc_Disconnect(void)
{
    Retcode_T retVal = RETCODE_OK;
    if ((NULL == AdcHandleInstancePtr) || (AdcHandleInstancePtr->Instance == NULL))
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
    }
    else
    {
        AdcHandleInstancePtr->Instance = NULL;
        AdcHandleInstancePtr = NULL;
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Adc_Enable(void)
{
    Retcode_T retVal = RETCODE_OK;
    ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
    if ((NULL == AdcHandleInstancePtr->Instance) || (NULL == AdcHandleInstancePtr))
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
    }
    init.timebase = ADC_TimebaseCalc(AUTO_SELECT_CURRENT_ADC_HFPERCLK);
    init.warmUpMode = adcWarmupKeepADCWarm;
    init.tailgate = false;
    init.lpfMode = adcLPFilterDeCap;
    if (BCDS_HAL_TRANSFER_MODE_DMA == AdcHandleInstancePtr->TransferMode)
    {
        init.prescale = ADC_PrescaleCalc(BSP_ADC_CLOCK_FREQ_DMA_MODE, AUTO_SELECT_CURRENT_ADC_HFPERCLK);
        /* Ensure that valid DMA IRQ obtained from the ADC middle ware Initialization routine*/
        if ((NULL == AdcHandleInstancePtr->DmaScanCallback) || (NULL == AdcHandleInstancePtr->DmaSingleCallback))
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        }
        else
        {
#if (BCDS_FEATURE_DMA == 1)
            AdcScanDmaCallBack = AdcHandleInstancePtr->DmaScanCallback;
            AdcSingleDmaCallBack = AdcHandleInstancePtr->DmaSingleCallback;
#endif
        }
    }
    else if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == AdcHandleInstancePtr->TransferMode)
    {
        init.prescale = ADC_PrescaleCalc(BSP_ADC_CLOCK_FREQ_ISR_MODE, AUTO_SELECT_CURRENT_ADC_HFPERCLK);
        /* Ensure that valid Interrupt IRQ obtained from the ADC middle ware Initialization routine*/
        if (NULL == AdcHandleInstancePtr->IRQCallback)
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        }
        else
        {
#if (BCDS_FEATURE_DMA == 0)
            AdcIrqCallBack = AdcHandleInstancePtr->IRQCallback;
#endif
            /* Set interrupt priorities */
            NVIC_SetPriority(ADC_IRQN, ADC_IRQN_PRIORITY);
            /* clear pending interrupts */
            NVIC_ClearPendingIRQ(ADC_IRQN);
            NVIC_EnableIRQ(ADC_IRQN);
        }
    }
    /* Enable ADC clock */
    CMU_ClockEnable(cmuClock_ADC0, true);
    ADC_Init(AdcHandleInstancePtr->Instance, &init);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Adc_Disable(void)
{
    Retcode_T retVal = RETCODE_OK;
    if ((NULL == AdcHandleInstancePtr) || (NULL == AdcHandleInstancePtr->Instance))
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
    }

#if (BCDS_FEATURE_DMA == 1)
    /* Make the internally copied DMA Irq callback address to NULL*/
    AdcScanDmaCallBack = NULL;
    /* Make the internally copied DMA Irq callback address to NULL*/
    AdcSingleDmaCallBack = NULL;
#elif (BCDS_FEATURE_DMA == 0)
    /* Make the internally copied MCU Irq callback address to NULL*/
    NVIC_DisableIRQ(ADC_IRQN);
    AdcIrqCallBack = NULL;
#endif
    ADC_Reset(AdcHandleInstancePtr->Instance);
    return retVal;
}

/*
 * Refer to interface header for description
 */
HWHandle_T BSP_Adc_GetHandle(void)
{
    HWHandle_T handle = (HWHandle_T) AdcHandleInstancePtr;
    return handle;
}

/*
 * Refer to interface header for description
 */
void BSP_Adc_HandlerIsr(void)
{
#if (BCDS_FEATURE_DMA == 0)
    Retcode_T retVal = RETCODE_OK;

    if ((NULL != AdcIrqCallBack) && (NULL != AdcHandleInstancePtr))
    {
        /* All ADC interrupt processing is to be done at the MCU level */
        AdcIrqCallBack((ADC_T) AdcHandleInstancePtr);
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        Retcode_RaiseErrorFromIsr(retVal);
    }
#endif
}
#endif //BCDS_FEATURE_ADC

