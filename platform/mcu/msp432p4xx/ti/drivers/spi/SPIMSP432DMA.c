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

#include <stdint.h>

#include <ti/devices/DeviceFamily.h>
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/pmap.h>
#include <ti/devices/msp432p4xx/driverlib/spi.h>

#include <ti/drivers/dma/UDMAMSP432.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIMSP432DMA.h>

#define MAX_DMA_TRANSFER_AMOUNT (1024)

#define PinConfigValue(config) (((config) >> 10) & 0x1F)
#define PinConfigModuleFunction(config) (((config) >> 8) & 0x3)
#define PinConfigPort(config) (((config) >> 4) & 0xF)
#define PinConfigPin(config) (1 << ((config) & 0x7))

/* SPIMSP432DMA functions */
void SPIMSP432DMA_close(SPI_Handle handle);
int_fast16_t SPIMSP432DMA_control(SPI_Handle handle, uint_fast16_t cmd, void *arg);
void SPIMSP432DMA_init(SPI_Handle handle);
SPI_Handle SPIMSP432DMA_open(SPI_Handle handle, SPI_Params *params);
bool SPIMSP432DMA_transfer(SPI_Handle handle, SPI_Transaction *transaction);
void SPIMSP432DMA_transferCancel(SPI_Handle handle);

/* SPI function table for SPIMSP432DMA implementation */
const SPI_FxnTable SPIMSP432DMA_fxnTable = {
    SPIMSP432DMA_close,
    SPIMSP432DMA_control,
    SPIMSP432DMA_init,
    SPIMSP432DMA_open,
    SPIMSP432DMA_transfer,
    SPIMSP432DMA_transferCancel
};

static const uint16_t frameFormat[] = {
    /* SPI_POL0_PHA0 */
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW |
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,

    /* SPI_POL0_PHA1 */
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW |
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,

    /* SPI_POL1_PHA0 */
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH |
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,

    /* SPI_POL1_PHA1 */
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH |
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
};

/*
 *  ======== blockingTransferCallback ========
 */
static void blockingTransferCallback(SPI_Handle handle,
    SPI_Transaction *transaction)
{
    SPIMSP432DMA_Object *object = handle->object;

    SemaphoreP_post(object->transferComplete);
}

/*
 *  ======== configDMA ========
 *  This functions configures the transmit and receive DMA channels for a given
 *  SPI_Handle and SPI_Transaction
 */
static void configDMA(SPIMSP432DMA_Object *object,
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs, SPI_Transaction *transaction)
{
    uintptr_t key;
    void     *buf;
    uint32_t  channelControlOptions;

    /*
     * The DMA has a max transfer amount of 1024.  If the transaction is
     * greater; we must transfer it in chunks.  object->amtDataXferred has
     * how much data has already been sent.
     */
    if ((transaction->count - object->amtDataXferred) > MAX_DMA_TRANSFER_AMOUNT) {
        object->currentXferAmt = MAX_DMA_TRANSFER_AMOUNT;
    }
    else {
        object->currentXferAmt = (transaction->count - object->amtDataXferred);
    }

    if (transaction->txBuf) {
        channelControlOptions = UDMA_SIZE_8 | UDMA_SRC_INC_8 |
            UDMA_DST_INC_NONE | UDMA_ARB_1;
        buf = (void *) ((uint32_t) transaction->txBuf + object->amtDataXferred);
    }
    else {
        channelControlOptions = UDMA_SIZE_8 | UDMA_SRC_INC_NONE |
            UDMA_DST_INC_NONE | UDMA_ARB_1;
        object->scratchBuffer = hwAttrs->defaultTxBufValue;
        buf = &(object->scratchBuffer);
    }

    /* Setup the TX transfer characteristics & buffers */
    MAP_DMA_setChannelControl(hwAttrs->txDMAChannelIndex | UDMA_PRI_SELECT,
        channelControlOptions);
    MAP_DMA_setChannelTransfer(hwAttrs->txDMAChannelIndex | UDMA_PRI_SELECT,
        UDMA_MODE_BASIC, buf,
        (void *) MAP_SPI_getTransmitBufferAddressForDMA(hwAttrs->baseAddr),
        object->currentXferAmt);

    if (transaction->rxBuf) {
        channelControlOptions = UDMA_SIZE_8 | UDMA_SRC_INC_NONE |
            UDMA_DST_INC_8 | UDMA_ARB_1;
        buf = (void *) ((uint32_t) transaction->rxBuf + object->amtDataXferred);
    }
    else {
        channelControlOptions = UDMA_SIZE_8 | UDMA_SRC_INC_NONE |
            UDMA_DST_INC_NONE | UDMA_ARB_1;
        buf = &(object->scratchBuffer);
    }

    /* Setup the RX transfer characteristics & buffers */
    MAP_DMA_setChannelControl(hwAttrs->rxDMAChannelIndex | UDMA_PRI_SELECT,
        channelControlOptions);
    MAP_DMA_setChannelTransfer(hwAttrs->rxDMAChannelIndex | UDMA_PRI_SELECT,
        UDMA_MODE_BASIC,
        (void *) MAP_SPI_getReceiveBufferAddressForDMA(hwAttrs->baseAddr),
        buf, object->currentXferAmt);

    /* A lock is needed because we are accessing shared DMA memory */
    key = HwiP_disable();

    MAP_DMA_assignChannel(hwAttrs->rxDMAChannelIndex);
    MAP_DMA_assignChannel(hwAttrs->txDMAChannelIndex);

    /* Enable DMA interrupt */
    MAP_DMA_assignInterrupt(hwAttrs->dmaIntNum, hwAttrs->rxDMAChannelIndex & 0x0F);
    MAP_DMA_clearInterruptFlag(hwAttrs->rxDMAChannelIndex & 0x0F);
    MAP_DMA_enableInterrupt(hwAttrs->dmaIntNum);

    MAP_DMA_enableChannel(hwAttrs->rxDMAChannelIndex & 0x0F);
    MAP_DMA_enableChannel(hwAttrs->txDMAChannelIndex & 0x0F);

    HwiP_restore(key);
}

/*
 *  ======== getDmaRemainingXfers ========
 */
static inline uint32_t getDmaRemainingXfers(SPIMSP432DMA_Object *object,
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs) {
    uint32_t          remainingTransfers;
    uint32_t          controlWord;
    DMA_ControlTable *controlTable;

    controlTable = MAP_DMA_getControlBase();
    controlWord = controlTable[(hwAttrs->rxDMAChannelIndex & 0x3f)].control;
    remainingTransfers = ((controlWord & UDMA_CHCTL_XFERSIZE_M) >> 4) + 1;

    return (remainingTransfers);
}

/*
 *  ======== initHW ========
 */
static void initHw(SPIMSP432DMA_Object *object,
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs, uint32_t inputClkFreq)
{
    union {
        eUSCI_SPI_MasterConfig masterCfg;
        eUSCI_SPI_SlaveConfig  slaveCfg;
    } cfgStructs;

    /* Set SW reset bit before reconfiguring */
    MAP_SPI_disableModule(hwAttrs->baseAddr);

    if (object->spiMode == SPI_MASTER) {
        cfgStructs.masterCfg.selectClockSource = hwAttrs->clockSource;
        cfgStructs.masterCfg.clockSourceFrequency = inputClkFreq;
        cfgStructs.masterCfg.desiredSpiClock = object->bitRate;
        cfgStructs.masterCfg.msbFirst = hwAttrs->bitOrder;
        cfgStructs.masterCfg.clockPhase = object->clockPhase;
        cfgStructs.masterCfg.clockPolarity = object->clockPolarity;
        cfgStructs.masterCfg.spiMode = hwAttrs->pinMode;
        MAP_SPI_initMaster(hwAttrs->baseAddr, &(cfgStructs.masterCfg));

        if (hwAttrs->pinMode != EUSCI_SPI_3PIN) {
            MAP_SPI_selectFourPinFunctionality(hwAttrs->baseAddr,
                EUSCI_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);
        }
    }
    else {
        cfgStructs.slaveCfg.msbFirst = hwAttrs->bitOrder;
        cfgStructs.slaveCfg.clockPhase = object->clockPhase;
        cfgStructs.slaveCfg.clockPolarity = object->clockPolarity;
        cfgStructs.slaveCfg.spiMode = hwAttrs->pinMode;
        MAP_SPI_initSlave(hwAttrs->baseAddr, &(cfgStructs.slaveCfg));
    }

    /* Clear SW reset bit to allow peripheral operation */
    MAP_SPI_enableModule(hwAttrs->baseAddr);
}

/*
 *  ======== mapPin ========
 *  NOTE: This function may go away when DriverLib supports mapping
 *  an individual pin.
 */
static void mapPin(uint8_t port, uint8_t pin, uint8_t value)
{
    volatile uint8_t pmap;

    pmap = port * 0x8;  // 2 -> 0x10, 3 -> 0x18, 7 -> 0x38

    /* Code from pmap.c: */
    /* Get write-access to port mapping registers: */
    PMAP->KEYID = PMAP_KEYID_VAL;

    /* Enable/Disable reconfiguration during runtime */
    PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | PMAP_ENABLE_RECONFIGURATION;
    HWREG8((uint32_t)PMAP_BASE + pin + pmap) = value;

    /* Disable write-access to port mapping registers: */
    PMAP->KEYID = 0;
}

/*
 *  ======== perfChangeNotifyFxn ========
 */
static int perfChangeNotifyFxn(unsigned int eventType, uintptr_t eventArg,
    uintptr_t clientArg)
{
    uint32_t                      clockFreq;
    PowerMSP432_Freqs             powerFreqs;
    SPIMSP432DMA_Object          *object = ((SPI_Handle) clientArg)->object;
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs = ((SPI_Handle) clientArg)->hwAttrs;

    PowerMSP432_getFreqs((unsigned int) eventArg, &powerFreqs);
    clockFreq = (hwAttrs->clockSource == EUSCI_B_SPI_CLOCKSOURCE_SMCLK) ?
        powerFreqs.SMCLK : powerFreqs.ACLK;

    initHw(object, hwAttrs, clockFreq);

    return (Power_NOTIFYDONE);
}

/*
 *  ======== spiHwiFxn ========
 */
static void spiHwiFxn(uintptr_t arg)
{
    SPI_Transaction              *msg;
    SPIMSP432DMA_Object          *object = ((SPI_Handle) arg)->object;
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs = ((SPI_Handle) arg)->hwAttrs;

    /* RX DMA channel has completed */
    MAP_DMA_disableInterrupt(hwAttrs->dmaIntNum);

    if ((object->transaction->count - object->amtDataXferred) >
        MAX_DMA_TRANSFER_AMOUNT) {
        /* Data still remaining, configure another DMA transfer */
        object->amtDataXferred += object->currentXferAmt;

        configDMA(object, hwAttrs, object->transaction);
    }
    else {
        /* All data sent; set status & perform callback */
        object->transaction->status = SPI_TRANSFER_COMPLETED;

        /*
         * Use a temporary transaction pointer in case the callback function
         * attempts to perform another SPI_transfer call
         */
        msg = object->transaction;

        /* Indicate we are done with this transfer */
        object->transaction = NULL;

        /* Remove constraints set during transfer */
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

        /* Perform callback */
        object->transferCallbackFxn((SPI_Handle) arg, msg);
    }
}

/*
 *  ======== spiPollingTransfer ========
 */
static inline void spiPollingTransfer(SPIMSP432DMA_Object *object,
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs, SPI_Transaction *transaction)
{
    uint8_t *rxBuf;
    uint8_t *txBuf;
    size_t   transferCount;

    if (transaction->rxBuf) {
        rxBuf = transaction->rxBuf;
    }
    else {
        rxBuf = &(object->scratchBuffer);
    }

    if (transaction->txBuf) {
        txBuf = transaction->txBuf;
    }
    else {
        object->scratchBuffer = hwAttrs->defaultTxBufValue;
        txBuf = &(object->scratchBuffer);
    }

    transferCount = transaction->count;

    /* Start the polling transfer */
    while (transferCount--) {
        /* Wait until the transmit flag is set; SPI is ready to transfer */
        while (!(SPI_getInterruptStatus(hwAttrs->baseAddr,
            EUSCI_B_SPI_TRANSMIT_INTERRUPT))) {;}

        MAP_SPI_transmitData(hwAttrs->baseAddr, *txBuf);

        /* Wait until the receive flag is set; SPI has received data */
        while (!(SPI_getInterruptStatus(hwAttrs->baseAddr,
            EUSCI_B_SPI_RECEIVE_INTERRUPT))) {;}

        *rxBuf = MAP_SPI_receiveData(hwAttrs->baseAddr);

        /* Only increment source & destination only if buffers were provided */
        if (transaction->rxBuf) {
            rxBuf++;
        }
        if (transaction->txBuf) {
            txBuf++;
        }
    }
}

/*
 *  ======== SPIMSP432DMA_close ========
 *  @pre    Function assumes that the handle is not NULL
 */
void SPIMSP432DMA_close(SPI_Handle handle)
{
    uint32_t                      i;
    SPIMSP432DMA_Object          *object = handle->object;
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    if (object->hwiHandle) {
        HwiP_delete(object->hwiHandle);
        object->hwiHandle = NULL;
    }

    if (object->transferComplete) {
        SemaphoreP_delete(object->transferComplete);
        object->transferComplete = NULL;
    }

    if (object->dmaHandle) {
        UDMAMSP432_close(object->dmaHandle, hwAttrs->rxDMAChannelIndex,
            hwAttrs->dmaIntNum);
        UDMAMSP432_close(object->dmaHandle, hwAttrs->txDMAChannelIndex, 0);
        object->dmaHandle = NULL;
    }

    for (i = 0; object->perfConstraintMask; i++) {
        if (object->perfConstraintMask & 0x01) {
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERFLEVEL_0 + i);
        }
        object->perfConstraintMask >>= 1;
    }

    Power_unregisterNotify(&object->perfChangeNotify);

    object->isOpen = false;
}

/*
 *  ======== SPIMSP432DMA_control ========
 *  @pre    Function assumes that the handle is not NULL
 */
int_fast16_t SPIMSP432DMA_control(SPI_Handle handle, uint_fast16_t cmd, void *arg)
{
    return (SPI_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== SPIMSP432DMA_init ========
 *  @pre    Function assumes that the handle is not NULL
 */
void SPIMSP432DMA_init(SPI_Handle handle)
{
    /* Relying on ELF to set object->isOpen to false */

    UDMAMSP432_init();
}

/*
 *  ======== SPIMSP432DMA_open ========
 */
SPI_Handle SPIMSP432DMA_open(SPI_Handle handle, SPI_Params *params)
{
    uintptr_t                     key;
    uint32_t                      clockFreq;
    uint8_t                       i;
    uint8_t                       numPerfLevels;
    uint16_t                      pin;
    uint16_t                      port;
    uint16_t                      value;
    uint16_t                      moduleFunction;
    PowerMSP432_Freqs             powerFreqs;
    SPIMSP432DMA_Object          *object = handle->object;
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    key = HwiP_disable();

    if (object->isOpen) {
        HwiP_restore(key);

        return (NULL);
    }
    object->isOpen = true;

    HwiP_restore(key);

    /* SPI_TI & SPI_MW are not supported */
    if (params->frameFormat == SPI_TI || params->frameFormat == SPI_MW) {
        object->isOpen = false;

        return (NULL);
    }

    /* Ensure a supported clock source is used */
    if (hwAttrs->clockSource != EUSCI_B_SPI_CLOCKSOURCE_ACLK &&
        hwAttrs->clockSource != EUSCI_B_SPI_CLOCKSOURCE_SMCLK) {

        object->isOpen = false;

        return (NULL);
    }

    /* Map the pins */
    /* CLK */
    if (hwAttrs->clkPin == SPIMSP432DMA_PIN_NO_CONFIG) {
        /* SPI clock pin is required */
        object->isOpen = false;

        return (NULL);
    }
    port = PinConfigPort(hwAttrs->clkPin);
    value = PinConfigValue(hwAttrs->clkPin);
    if (value != 0) {
        moduleFunction = GPIO_PRIMARY_MODULE_FUNCTION;
        pin = (hwAttrs->clkPin) & 0x7;
        mapPin(port, pin, value);
    }
    else {
        moduleFunction = PinConfigModuleFunction(hwAttrs->clkPin);
    }
    pin = PinConfigPin(hwAttrs->clkPin);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port, pin, moduleFunction);

    /* STE */
    if (hwAttrs->pinMode != EUSCI_SPI_3PIN &&
        hwAttrs->stePin != SPIMSP432DMA_PIN_NO_CONFIG) {
        port = PinConfigPort(hwAttrs->stePin);
        value = PinConfigValue(hwAttrs->stePin);
        if (value != 0) {
            moduleFunction = GPIO_PRIMARY_MODULE_FUNCTION;
            pin = (hwAttrs->stePin) & 0x7;
            mapPin(port, pin, value);
        }
        else {
            moduleFunction = PinConfigModuleFunction(hwAttrs->stePin);
        }
        pin = PinConfigPin(hwAttrs->stePin);
        MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port, pin,
            moduleFunction);
    }

    /* SIMO */
    if (hwAttrs->simoPin != SPIMSP432DMA_PIN_NO_CONFIG) {
        port = PinConfigPort(hwAttrs->simoPin);
        value = PinConfigValue(hwAttrs->simoPin);
        if (value != 0) {
            moduleFunction = GPIO_PRIMARY_MODULE_FUNCTION;
            pin = (hwAttrs->simoPin) & 0x7;
            mapPin(port, pin, value);
        }
        else {
            moduleFunction = PinConfigModuleFunction(hwAttrs->simoPin);
        }
        pin = PinConfigPin(hwAttrs->simoPin);
        MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port, pin, moduleFunction);
    }

    /* SOMI */
    if (hwAttrs->somiPin != SPIMSP432DMA_PIN_NO_CONFIG) {
        port = PinConfigPort(hwAttrs->somiPin);
        value = PinConfigValue(hwAttrs->somiPin);
        if (value != 0) {
            moduleFunction = GPIO_PRIMARY_MODULE_FUNCTION;
            pin = (hwAttrs->somiPin) & 0x7;
            mapPin(port, pin, value);
        }
        else {
            moduleFunction = PinConfigModuleFunction(hwAttrs->somiPin);
        }
        pin = PinConfigPin(hwAttrs->somiPin);
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(port, pin, moduleFunction);
    }

    /*
     * Add power management support - Disable performance transitions while
     * opening the driver.
     */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /*
     * Verify that the driver can be opened at current performance level and
     * set constraints for other performance levels.
     */
    numPerfLevels = PowerMSP432_getNumPerfLevels();
    PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);
    if (hwAttrs->clockSource == EUSCI_B_SPI_CLOCKSOURCE_ACLK) {
        /*
         * Verify if driver can be opened with ACLK; ACLK does not change
         * in any performance level.
         */
        if (params->bitRate >= powerFreqs.ACLK) {
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

            object->isOpen = false;

            return (NULL);
        }
        clockFreq = powerFreqs.ACLK;
    }
    else {    /* hwAttrs->clockSource == EUSCI_B_SPI_CLOCKSOURCE_SMCLK */
        if (params->bitRate >= powerFreqs.SMCLK) {
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

            object->isOpen = false;

            return (NULL);
        }
        clockFreq = powerFreqs.SMCLK;

        /*
         * SMCLK changes with performance levels.  Set constraints for
         * unsupported performance levels.
         */
        for (i = 0; i < numPerfLevels; i++) {
            PowerMSP432_getFreqs(i, &powerFreqs);
            if (params->bitRate >= powerFreqs.SMCLK) {
                /* Set constraint and keep track of it in perfConstraintMask */
                object->perfConstraintMask |= (1 << i);
                Power_setConstraint(PowerMSP432_DISALLOW_PERFLEVEL_0 + i);
            }
        }
    }

    /* Register function to reconfigure peripheral on perf level changes */
    Power_registerNotify(&object->perfChangeNotify,
        PowerMSP432_DONE_CHANGE_PERF_LEVEL, perfChangeNotifyFxn,
        (uintptr_t) handle);

    /* Claim RX & TX DMA resources */
    object->dmaHandle = UDMAMSP432_open(hwAttrs->rxDMAChannelIndex,
        hwAttrs->dmaIntNum, hwAttrs->intPriority, &spiHwiFxn, (uintptr_t) handle);
    if (object->dmaHandle == NULL) {
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
        SPIMSP432DMA_close(handle);

        return (NULL);
    }

    object->dmaHandle = UDMAMSP432_open(hwAttrs->txDMAChannelIndex, 0, (~0),
        NULL, 0);
    if (object->dmaHandle == NULL) {
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
        SPIMSP432DMA_close(handle);

        return (NULL);
    }

    if (params->transferMode == SPI_MODE_CALLBACK) {
        if (params->transferCallbackFxn == NULL) {
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
            SPIMSP432DMA_close(handle);

            return (NULL);
        }

        object->transferCallbackFxn = params->transferCallbackFxn;
    }
    else {
        /*
         * Create a semaphore to block task execution for the duration of the
         * SPI transfer
         */
        object->transferComplete = SemaphoreP_createBinary(0);
        if (object->transferComplete == NULL) {
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
            SPIMSP432DMA_close(handle);

            return (NULL);
        }

        object->transferCallbackFxn = blockingTransferCallback;
    }

    /*
     * Store SPI parameters & initialize peripheral.  These are used to
     * re/initialize the peripheral when opened or changing performance level.
     */
    object->bitRate = params->bitRate;
    object->clockPhase = frameFormat[params->frameFormat] & EUSCI_A_CTLW0_CKPH;
    object->clockPolarity = frameFormat[params->frameFormat] & EUSCI_A_CTLW0_CKPL;
    object->spiMode = params->mode;
    object->transaction = NULL;
    object->transferMode = params->transferMode;
    object->transferTimeout = params->transferTimeout;

    initHw(object, hwAttrs, clockFreq);

    /* Allow performance level changes */
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    return (handle);
}

/*
 *  ======== SPIMSP432DMA_transfer ========
 */
bool SPIMSP432DMA_transfer(SPI_Handle handle, SPI_Transaction *transaction)
{
    uintptr_t                     key;
    SPIMSP432DMA_Object          *object = handle->object;
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    if ((transaction->count == 0) ||
        ((transaction->rxBuf == NULL) && (transaction->txBuf == NULL))) {
        return (false);
    }

    key = HwiP_disable();

    if (object->transaction) {
        HwiP_restore(key);

        return (false);
    }
    else {
        object->transaction = transaction;
        object->transaction->status = SPI_TRANSFER_STARTED;
        object->amtDataXferred = 0;
        object->currentXferAmt = 0;
    }

    HwiP_restore(key);

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        /*
         * Set power constraints to keep peripheral active during transfer and
         * to prevent a performance level change
         */
        Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
        Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /* Polling transfer if BLOCKING mode & transaction->count < threshold */
    if (object->transferMode == SPI_MODE_BLOCKING &&
        transaction->count < hwAttrs->minDmaTransferSize) {
        spiPollingTransfer(object, hwAttrs, transaction);

        /* Transaction completed; set status & mark SPI ready */
        object->transaction->status = SPI_TRANSFER_COMPLETED;
        object->transaction = NULL;

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
    }
    else {
        /* Perform a DMA backed SPI transfer */
        configDMA(object, hwAttrs, transaction);

        if (object->transferMode == SPI_MODE_BLOCKING) {
            if (SemaphoreP_pend(object->transferComplete,
                object->transferTimeout) != SemaphoreP_OK) {
                /* Timeout occurred; cancel the transfer */
                object->transaction->status = SPI_TRANSFER_FAILED;
                SPIMSP432DMA_transferCancel(handle);

                /*
                 * TransferCancel() performs callback which posts
                 * transferComplete semaphore. This call consumes this extra post.
                 */
                SemaphoreP_pend(object->transferComplete, SemaphoreP_NO_WAIT);

                return (false);
            }
        }
    }

    return (true);
}

/*
 *  ======== SPIMSP432DMA_transferCancel ========
 */
void SPIMSP432DMA_transferCancel(SPI_Handle handle)
{
    uintptr_t                     key;
    SPI_Transaction              *msg;
    uint32_t                      clockFreq;
    PowerMSP432_Freqs             powerFreqs;
    SPIMSP432DMA_Object          *object = handle->object;
    SPIMSP432DMA_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    /*
     * There are 2 use cases in which to call transferCancel():
     *   1.  The driver is in CALLBACK mode.
     *   2.  The driver is in BLOCKING mode & there has been a transfer timeout.
     */
    if (object->transferMode == SPI_MODE_CALLBACK ||
        object->transaction->status == SPI_TRANSFER_FAILED) {

        key = HwiP_disable();

        if (object->transaction == NULL || object->cancelInProgress) {
            HwiP_restore(key);

            return;
        }
        object->cancelInProgress = true;

        /* Prevent DMA interrupt from occurring while canceling the transfer */
        HwiP_disableInterrupt(hwAttrs->dmaIntNum);
        HwiP_clearInterrupt(hwAttrs->dmaIntNum);

        /* Clear DMA configuration */
        MAP_DMA_disableInterrupt(hwAttrs->dmaIntNum);
        MAP_DMA_disableChannel(hwAttrs->rxDMAChannelIndex & 0x0F);
        MAP_DMA_disableChannel(hwAttrs->txDMAChannelIndex & 0x0F);
        MAP_DMA_clearInterruptFlag(hwAttrs->rxDMAChannelIndex & 0x0F);

        HwiP_restore(key);

        /* Make sure performance level does not change while resetting SPI */
        PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);
        clockFreq = (hwAttrs->clockSource == EUSCI_B_SPI_CLOCKSOURCE_SMCLK) ?
            powerFreqs.SMCLK : powerFreqs.ACLK;

        initHw(object, hwAttrs, clockFreq);

        HwiP_enableInterrupt(hwAttrs->dmaIntNum);

        /*
         * Calculate amount of data which has already been sent & store
         * it in transaction->count
         */
        object->transaction->count = object->amtDataXferred +
            (object->currentXferAmt - getDmaRemainingXfers(object, hwAttrs));

        /* Set status CANCELED if we did not cancel due to timeout  */
        if (object->transaction->status == SPI_TRANSFER_STARTED) {
            object->transaction->status = SPI_TRANSFER_CANCELED;
        }

        /* Remove constraints set during transfer */
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

        /*
         * Use a temporary transaction pointer in case the callback function
         * attempts to perform another SPI_transfer call
         */
        msg = object->transaction;

        /* Indicate we are done with this transfer */
        object->transaction = NULL;
        object->cancelInProgress = false;
        object->transferCallbackFxn(handle, msg);

    }
}
