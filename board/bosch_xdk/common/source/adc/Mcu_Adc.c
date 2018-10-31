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

#include "Mcu_Adc_Handle.h"
#include "BCDS_MCU_DMA_Handle.h"
#include "Mcu_Adc.h"
#include "em_adc.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_prs.h"
#include "BSP_Adc.h"
#include "XdkCommonInfo.h"

/* Put the type and macro definitions here */

const ADC_SingleInput_TypeDef singleChannelArray[ADC_ENABLE_CH_MAX] = { adcSingleInpCh4, adcSingleInpCh5, adcSingleInpCh6, adcSingleInpCh7,
        adcSingleInpTemp, adcSingleInpVDDDiv3, adcSingleInpVDD, adcSingleInpVSS, adcSingleInpVrefDiv2 };

static uint16_t ReqNoOfSamplesInScan = 0;
static uint16_t *globalBufferSwitchPtr = NULL;

/* Put constant and variable definitions here */
#define MASK_IEN_ADC_SINGLE             UINT32_C(0x01)
#define MASK_IEN_ADC_SCAN               UINT32_C(0x02)
#define ADC_MIN_SAMPLING_FREQUENCY      UINT32_C(6)
#define ADC_MAX_SAMPLING_FREQUENCY      UINT32_C(45000)

#define PRS_CHANNEL_NO_TIMER0_TO_ADC        (0) //  channel no selected for producing signal from TIMER0 module
#define PRS_CHANNEL_NO_ADC_FROM_TIMER0        (adcPRSSELCh0) //  channel no selected for consumer ADC module

/* Put private function declarations here */
/*Api to find the Number of channels using the given channelmask*/
static uint8_t findNoOfScanChannel(uint32_t channelMask)
{
    uint8_t noOfScanChannels = 0;
    if (channelMask != 0)
    {
        channelMask = channelMask >> 12;
        for (uint8_t i = 0; i < 4; i++)
        {
            if ((channelMask >> i) & (0x01))
            {
                noOfScanChannels++;
            }
        }
    }

    return noOfScanChannels;
}

/*Callback function for ADC scan mode in DMA*/
static void McuAdcDmaScanChannelCb(ADC_T adc)
{
    struct Mcu_Adc_Handle_S* adcPtr = (struct Mcu_Adc_Handle_S*) adc;
    uint32_t currentActiveBuff = 0;
    union Mcu_Adc_Event_U event;
    uint16_t *bufferPtr;
    currentActiveBuff = *((uint32_t*) adcPtr->_DriverCtx.PointerValue);
    event.RegisterValue = 0;
    if (currentActiveBuff)
    {
        bufferPtr = adcPtr->_DriverCtx.ScanBuffer1;
    }
    else
    {
        bufferPtr = adcPtr->_DriverCtx.ScanBuffer2;
    }
    event.Bitfield.ScanComplete = 1;
    adcPtr->_DriverCtx.AppCallback(adc, event.Bitfield, bufferPtr);
}

/*Callback function for ADC single mode in DMA*/
static void McuAdcDmaSingleChannelCb(ADC_T adc)
{
    struct Mcu_Adc_Handle_S* adcPtr = (struct Mcu_Adc_Handle_S*) adc;
    union Mcu_Adc_Event_U event;
    event.RegisterValue = 0;
    event.Bitfield.SingleComplete = 1;
    adcPtr->_DriverCtx.AppCallback(adc, event.Bitfield, adcPtr->_DriverCtx.SingleBuffer);
}

/* Put function implementations here */

/* @brief Configure Timer for ADC PRS trigger
 * topValue = 1 ;results in 2.66usec = translates to Sampling frequency of 375000Hz or 375KHz
 * topValue = 0xFFFF; results in 174323.1 usec = 174.323 msec = 5.7Hz
 */
static Retcode_T Adc_SamplingTimerSetup(uint32_t samplingFreqInHz)
{
    Retcode_T retVal = RETCODE_OK;
    uint32_t freq = 0;
    uint32_t topValue = 0;
    uint32_t preScaledClkVal = 0;

    /* Use default timer settings */
    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
    if (UINT32_C(0) == samplingFreqInHz)
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        freq = CMU_ClockFreqGet(cmuClock_HFPER);
        /*Note: While developing and testing, this module gets cmuClock_HFPER has 48MHZ, hence the topValue time base is implemented accordingly
         *      If the  cmuClock_HFPER has changed to some other clock value then the requested application sampling rate will not be achieved and the
         *      behavior will be different*/
        preScaledClkVal = (freq / 128);
        /* This value represents the user requested sampling rate*/
        topValue = preScaledClkVal / samplingFreqInHz;
        /*TIMER0 is selected for generating PRS*/
        CMU_ClockEnable(cmuClock_TIMER0, true);
        /* Pre scaler for TIMER0 is selected as 128 */
        timerInit.enable = false;
        /*timerPrescale128 results in frequency = 48MHz/128 = 375000 Hz, and the resolution of one timer tick is 1/375000 Hz = 2.66usec*/
        timerInit.prescale = timerPrescale128;
        TIMER_TopSet(TIMER0, topValue);
        /* Timer is not yet started */
        TIMER_Init(TIMER0, &timerInit);
    }
    return retVal;
}

/* @brief Configure PRS
 *  PRS_CHANNEL_NO_TIMER0_TO_ADC
 * */
static void Adc_PrsChannelSetup(void)
{
    /* Select TIMER0 as source and TIMER0OF (Timer0 overflow) as pass the signal signal (as is) */
    CMU_ClockEnable(cmuClock_PRS, true);
    PRS_SourceSignalSet(PRS_CHANNEL_NO_TIMER0_TO_ADC, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgePos);
}

/*
 * This function will be registered as the ISR for the receive interrupt handler
 */
void Adc_McuIrqCallback(ADC_T adc)
{
    struct Mcu_Adc_Handle_S* adcPtr = (struct Mcu_Adc_Handle_S*) adc;
    union Mcu_Adc_Event_U event;
    static uint8_t scanSwitch = UINT8_C(0);
    uint32_t setCntToZero = UINT32_C(0);
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    event.RegisterValue = 0;
    if ((NULL == adc) || (NULL == adcPtr->_DriverCtx.AppCallback))
    {
        Retcode_RaiseErrorFromIsr (RETCODE( RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
        return;
    }
    if (adcPtr->Instance->IF & ADC_IF_SCAN)
    {
        do
        {
            if ((adcPtr->Instance->STATUS & ADC_STATUS_SCANDV))
            {
                *(globalBufferSwitchPtr) = ADC_DataScanGet(adcPtr->Instance);
                globalBufferSwitchPtr++;
            }

        } while ((adcPtr->Instance->STATUS & ADC_STATUS_SCANACT));
        ADC_IntClear(adcPtr->Instance, ADC_IF_SCAN);

        if (UINT32_C(0) == --ReqNoOfSamplesInScan)
        {
            /*Stop PRS timer*/
            TIMER_Enable(TIMER0, false);
            /*Reset the Cnt register of the Timer to avoid jitter */
            TIMER_CounterSet(TIMER0, setCntToZero);
            ReqNoOfSamplesInScan = (adcPtr->_DriverCtx.NumOfSamples);
            scanSwitch ^= UINT8_C(1);
            if (UINT8_C(1) == scanSwitch)
            {
                globalBufferSwitchPtr = adcPtr->_DriverCtx.ScanBuffer2;
            }
            else
            {
                globalBufferSwitchPtr = adcPtr->_DriverCtx.ScanBuffer1;
            }
            event.Bitfield.ScanComplete = true;
            /*Start PRS timer */
            TIMER_Enable(TIMER0, true);
        }
        adcPtr->_DriverCtx.AppCallback(adc, event.Bitfield, globalBufferSwitchPtr);
    }
    else if (adcPtr->Instance->IF & ADC_IF_SINGLE)
    {
        *(adcPtr->_DriverCtx.SingleBuffer) = ADC_DataSingleGet(adcPtr->Instance);
        ADC_IntClear(adcPtr->Instance, _ADC_IF_MASK);
        event.Bitfield.SingleComplete = true;
        adcPtr->_DriverCtx.AppCallback(adc, event.Bitfield, adcPtr->_DriverCtx.SingleBuffer);
    }
    else if (adcPtr->Instance->IF & ADC_IF_SCANOF)
    {
        event.Bitfield.ScanReadDataOverflow = true;
        ADC_IntClear(adcPtr->Instance, _ADC_IF_MASK);
        adcPtr->_DriverCtx.AppCallback(adc, event.Bitfield, globalBufferSwitchPtr);
    }
    else if (adcPtr->Instance->IF & ADC_IF_SINGLEOF)
    {
        event.Bitfield.SingleReadDataOverflow = true;
        ADC_IntClear(adcPtr->Instance, _ADC_IF_MASK);
        adcPtr->_DriverCtx.AppCallback(adc, event.Bitfield, adcPtr->_DriverCtx.SingleBuffer);
    }
    else
    {
        Retcode_RaiseErrorFromIsr(retcode);
        event.Bitfield.ErrorInConversion = true;
    }
}

/** @brief See interface function description in Mcu_Adc.h */
Retcode_T Mcu_Adc_Initialize(ADC_T adc, Mcu_Adc_Callback_T callback)
{
    Retcode_T retVal = RETCODE_OK;
    struct Mcu_Adc_Handle_S* adcPtr = (struct Mcu_Adc_Handle_S*) adc;
    struct MCU_DMA_Channel_S * adcScanDmaChannel = NULL;
    struct MCU_DMA_Channel_S * adcSingleDmaChannel = NULL;
    if ((NULL == adc) || (NULL == callback))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    if (true == adcPtr->_DriverCtx.Initialized)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_DOPPLE_INITIALIZATION);
    }
    if (BCDS_HAL_TRANSFER_MODE_DMA == adcPtr->TransferMode)
    {
        if ((NULL != adcPtr->Link1RegPtr) && (NULL != adcPtr->Link2RegPtr))
        {
            /*Configure Single Channel DMA channel for ADC */
            adcPtr->DmaScanCallback = McuAdcDmaScanChannelCb;
            adcScanDmaChannel = (struct MCU_DMA_Channel_S *) adcPtr->Link1RegPtr;
            adcScanDmaChannel->Config.cb = &adcScanDmaChannel->CallBack;
            DMA_CfgChannel(adcScanDmaChannel->ChannelId, &adcScanDmaChannel->Config);

            /*Configure Single Channel DMA channel for ADC */
            adcPtr->DmaSingleCallback = McuAdcDmaSingleChannelCb;
            adcSingleDmaChannel = (struct MCU_DMA_Channel_S *) adcPtr->Link2RegPtr;
            adcSingleDmaChannel->Config.cb = &adcSingleDmaChannel->CallBack;
            DMA_CfgChannel(adcSingleDmaChannel->ChannelId, &adcSingleDmaChannel->Config);
        }
        else
        {
            return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_ADC_NULL_DMA_CALLBACK);
        }
    }
    else if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == adcPtr->TransferMode)
    {
        adcPtr->IRQCallback = Adc_McuIrqCallback;
    }
    adcPtr->_DriverCtx.AppCallback = callback;
    adcPtr->_DriverCtx.Initialized = true;
    return retVal;
}

/** @brief See interface function description in Mcu_Adc.h */
Retcode_T Mcu_Adc_DeInitialize(ADC_T adc)
{
    Retcode_T retVal = RETCODE_OK;
    struct Mcu_Adc_Handle_S* adcPtr = (struct Mcu_Adc_Handle_S*) adc;
    if (NULL == adc)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    if (true == adcPtr->_DriverCtx.Initialized)
    {
        if (BCDS_HAL_TRANSFER_MODE_DMA == adcPtr->TransferMode)
        {
            /* Make the pointer for the DMA callback to NULL */
            adcPtr->DmaScanCallback = NULL;
            adcPtr->DmaSingleCallback = NULL;
        }
        else if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == adcPtr->TransferMode)
        {
            /* Make the pointer for the IRQ callback to NULL */
            adcPtr->IRQCallback = NULL;
        }
        /* Make the pointer for the application callback to NULL */
        adcPtr->_DriverCtx.AppCallback = NULL;
        adcPtr->_DriverCtx.Initialized = false;
    }
    else
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
    }

    return retVal;
}
/** @brief See interface function description in Mcu_Adc.h */
Retcode_T Mcu_Adc_StartSingle(ADC_T adc, Mcu_Adc_ConfigSinglePtr_T configStart)
{
    Retcode_T retVal = RETCODE_OK;
    ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;
    struct Mcu_Adc_Handle_S* adcPtr = (struct Mcu_Adc_Handle_S*) adc;
    if (ADC_ENABLE_CH_MAX <= configStart->Channel)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    }
    else if (NULL == adc)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    adcPtr->_DriverCtx.SingleBuffer = configStart->BufferPtr;
    /* Sets ADC channel to be used. */
    singleInit.input = singleChannelArray[configStart->Channel];
    singleInit.reference = (ADC_Ref_TypeDef) configStart->Reference;
    singleInit.acqTime = (ADC_AcqTime_TypeDef) configStart->AcqTime;
    singleInit.resolution = (ADC_Res_TypeDef) configStart->Resolution;
    /* Configures single acquisition mode. */
    ADC_InitSingle(adcPtr->Instance, &singleInit);
    if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == adcPtr->TransferMode)
    {
        /* Enables the single Interrupt generation  */
        ADC_IntEnable(adcPtr->Instance, ADC_IEN_SINGLE | ADC_IEN_SINGLEOF);
    }
    else if (BCDS_HAL_TRANSFER_MODE_DMA == adcPtr->TransferMode)
    {
        /* Prepare and Call DMA send method */
        DMA_CfgDescr_TypeDef singleAdcDmaDescriptor;
        struct MCU_DMA_Channel_S * adcSingleDmaChannel = (struct MCU_DMA_Channel_S *) adcPtr->Link2RegPtr;
        singleAdcDmaDescriptor.arbRate = dmaArbitrate1;
        singleAdcDmaDescriptor.dstInc = dmaDataIncNone;
        singleAdcDmaDescriptor.hprot = 0;
        singleAdcDmaDescriptor.size = dmaDataSize2;
        singleAdcDmaDescriptor.srcInc = dmaDataIncNone;
        DMA_CfgDescr(adcSingleDmaChannel->ChannelId, true, &singleAdcDmaDescriptor);
        DMA_ActivateBasic(adcSingleDmaChannel->ChannelId, true, false, adcPtr->_DriverCtx.SingleBuffer, (void*) &(adcPtr->Instance->SINGLEDATA), 0);
        DMA_ChannelEnable(adcSingleDmaChannel->ChannelId, true);
    }
    /* Starts ADC conversion. */
    ADC_Start(adcPtr->Instance, adcStartSingle);
    return retVal;
}

/** @brief See interface function description in Mcu_Adc.h */
Retcode_T Mcu_Adc_StartScan(ADC_T adc, Mcu_Adc_ConfigScanPtr_T configScan)
{
    Retcode_T retVal = RETCODE_OK;
    ADC_InitScan_TypeDef scanInit = ADC_INITSCAN_DEFAULT;
    struct Mcu_Adc_Handle_S* adcPtr = (struct Mcu_Adc_Handle_S*) adc;
    uint16_t totalSamplesInScan = 0;
    if (NULL == adc)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else if ((ADC_MIN_SAMPLING_FREQUENCY > configScan->SamplingRateInHz) || (ADC_MAX_SAMPLING_FREQUENCY < configScan->SamplingRateInHz))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    //Enable TailGatting
    adcPtr->Instance->CTRL |= ADC_CTRL_TAILGATE;
    adcPtr->_DriverCtx.NumOfSamples = configScan->NoOfSamples;
    adcPtr->_DriverCtx.NumOfScanChannels = findNoOfScanChannel(configScan->ChannelScanMask);
    totalSamplesInScan = (adcPtr->_DriverCtx.NumOfSamples) * (adcPtr->_DriverCtx.NumOfScanChannels);
    adcPtr->_DriverCtx.ScanBuffer1 = configScan->BufferPtr;
    adcPtr->_DriverCtx.ScanBuffer2 = configScan->BufferPtr + totalSamplesInScan;
    scanInit.input = configScan->ChannelScanMask;
    scanInit.reference = (ADC_Ref_TypeDef) configScan->Reference;
    scanInit.prsEnable = true;
    scanInit.prsSel = PRS_CHANNEL_NO_ADC_FROM_TIMER0;
    scanInit.acqTime = (ADC_AcqTime_TypeDef) configScan->AcqTime;
    scanInit.resolution = (ADC_Res_TypeDef) configScan->Resolution;

    Adc_PrsChannelSetup();
    if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == adcPtr->TransferMode)
    {
        ReqNoOfSamplesInScan = adcPtr->_DriverCtx.NumOfSamples;
        globalBufferSwitchPtr = adcPtr->_DriverCtx.ScanBuffer1;
        /* Prepare and Call Interrupt send method */
        NVIC_ClearPendingIRQ(ADC0_IRQn);
        ADC_IntClear(adcPtr->Instance, _ADC_IF_MASK);
        /* Enable ADC Interrupt when Scan Conversion Complete */
        ADC_IntEnable(ADC0, ADC_IEN_SCAN | ADC_IEN_SCANOF);
        /*Note: Since we are in the PRS mode we do not need to call ADC_Start() API for scan trigger if not ADC_start() needs to be called */
    }
    else if (BCDS_HAL_TRANSFER_MODE_DMA == adcPtr->TransferMode)
    {
        /* Prepare and Call DMA send method */
        DMA_CfgDescr_TypeDef scanAdcDmaDescriptor;
        struct MCU_DMA_Channel_S * adcScanDmaChannel = (struct MCU_DMA_Channel_S *) adcPtr->Link1RegPtr;
        scanAdcDmaDescriptor.arbRate = dmaArbitrate1;
        scanAdcDmaDescriptor.dstInc = dmaDataInc2;
        scanAdcDmaDescriptor.hprot = 0;
        scanAdcDmaDescriptor.size = dmaDataSize2;
        scanAdcDmaDescriptor.srcInc = dmaDataIncNone;
        DMA_CfgDescr(adcScanDmaChannel->ChannelId, true, &scanAdcDmaDescriptor);
        DMA_CfgDescr(adcScanDmaChannel->ChannelId, false, &scanAdcDmaDescriptor);
        DMA_ActivatePingPong(adcScanDmaChannel->ChannelId, false, adcPtr->_DriverCtx.ScanBuffer1, (void*) &(adcPtr->Instance->SCANDATA), (totalSamplesInScan - 1),
                adcPtr->_DriverCtx.ScanBuffer2, (void*) &(adcPtr->Instance->SCANDATA), (totalSamplesInScan - 1));
        DMA_ChannelEnable(adcScanDmaChannel->ChannelId, true);

    }
    retVal = Adc_SamplingTimerSetup(configScan->SamplingRateInHz);
    if (RETCODE_OK == retVal)
    {
        /* Configures Scan acquisition mode. */
        ADC_InitScan(adcPtr->Instance, &scanInit);
        /*Call the timer after Initialize the mcu Scan channel */
        TIMER_Enable(TIMER0, true);
    }
    return (retVal);
}

/** @brief See interface function description in Mcu_Adc.h */
Retcode_T Mcu_Adc_StopScan(ADC_T adc)
{
    Retcode_T retVal = RETCODE_OK;
    struct Mcu_Adc_Handle_S* adcPtr = (struct Mcu_Adc_Handle_S*) adc;

    struct MCU_DMA_Channel_S * adcScanDmaChannel = (struct MCU_DMA_Channel_S *) adcPtr->Link1RegPtr;

    uint32_t setCntToZero = UINT32_C(0);

    TIMER_Enable(TIMER0, false);
    TIMER_IntDisable(TIMER0, TIMER_IF_OF);
    TIMER_IntClear(TIMER0, TIMER_IF_OF);
    //Disable  TailGatting
    adcPtr->Instance->CTRL &= ~(ADC_CTRL_TAILGATE);
    /*Reset the Cnt register of the Timer to avoid jitter in between Mcu_Adc_StopScan API call and Mcu_Adc_StartScan APIcall */
    TIMER_CounterSet(TIMER0, setCntToZero);
    /*To Lock register access for TIMER0 register to avoid access of TIMER0 ,
     * in between Mcu_Adc_StopScan API call and Mcu_Adc_StartScan API call*/
    CMU_ClockEnable(cmuClock_TIMER0, false);
    adcPtr->Instance->CMD = ADC_CMD_SCANSTOP;
    if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == adcPtr->TransferMode)
    {
        /* disable ADC Interrupt when Scan Conversion Complete */
        ADC_IntDisable(ADC0, ADC_IEN_SCAN | ADC_IEN_SCANOF);
    }
    else if (BCDS_HAL_TRANSFER_MODE_DMA == adcPtr->TransferMode)
    {
        if (true == DMA_ChannelEnabled(adcScanDmaChannel->ChannelId))
        {
            DMA_ChannelEnable(adcScanDmaChannel->ChannelId, false);
        }
    }
    return (retVal);
}

/**@} */
