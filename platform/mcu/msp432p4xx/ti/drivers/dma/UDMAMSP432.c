/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
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
#include <stdbool.h>
#include <stdint.h>

#include <ti/devices/DeviceFamily.h>

#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>

#include <ti/drivers/dma/UDMAMSP432.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#define NUM_DMA_CHANNELS      (8)
#define NUM_DMA_INTERRUPTS    (4)

extern const UDMAMSP432_Config UDMAMSP432_config[];

/* DMA error HwiP handle */
static HwiP_Handle errorHwi = NULL;

/*
 * Lock for exclusive access to hwiFxnArgs, hwiFxnHandlers & hwiHandles arrays
 * during UDMAMSP432_open() & UDMAMSP432_close()
 */
static SemaphoreP_Handle interruptSetupLock = NULL;

/* Bit-mask of DMA channels currently being used */
static uint8_t channelsInUse = 0;

static bool dmaInitialized = false;

/* Bit-mask of DMA interrupts being used */
static uint8_t interruptsInUse = 0;

static uint32_t dmaInt0RefCount = 0;

/* Array of the Hwi handles for all DMA interrupts */
static HwiP_Handle hwiHandles[NUM_DMA_INTERRUPTS];

/* Array of ISR pointers called when a channel causes an interrupt on DMA_INT0 */
static void (*hwiFxnHandlers[NUM_DMA_CHANNELS])(uintptr_t);

/* Array of ISR arguments */
static uintptr_t hwiFxnArgs[NUM_DMA_CHANNELS];

static void UDMAMSP432_hwiFxn(uintptr_t arg);
static bool setupInterrupt(uint32_t channel, uint8_t intNum,
    uint8_t priority, void (*hwiFxn)(uintptr_t), uintptr_t arg);

/*
 *  ======== UDMAMSP432_close ========
 */
void UDMAMSP432_close(UDMAMSP432_Handle handle, uint32_t channel,
    uint8_t intNum)
{
    uintptr_t key;
    uint8_t   channelMask;
    uint8_t   channelNum;
    uint8_t   hwiMask;
    uint8_t   intIndex;

    channelNum = (uint8_t) channel;
    channelMask = 1 << channelNum;

    if (channelNum >= NUM_DMA_CHANNELS) {
        return;
    }

    if ((channelsInUse & channelMask) == 0) {
        return;
    }

    if ((intNum != 0) && (intNum <= DMA_INT0) && (intNum >= DMA_INT3)) {
        /* Delete the Hwi & free the interrupts */
        intIndex = DMA_INT0 - intNum;
        hwiMask = 1 << intIndex;

        /* Acquire exclusive access to interrupt arrays & masks */
        SemaphoreP_pend(interruptSetupLock, SemaphoreP_WAIT_FOREVER);

        if (intNum == DMA_INT0) {
            hwiFxnHandlers[channelNum] = NULL;
            hwiFxnArgs[channelNum] = (uintptr_t) NULL;
            dmaInt0RefCount--;

            if (dmaInt0RefCount == 0) {
                HwiP_delete(hwiHandles[intIndex]);

                interruptsInUse &= ~hwiMask;
            }
        }
        else {
            HwiP_delete(hwiHandles[intIndex]);
            hwiHandles[intIndex] = NULL;
            interruptsInUse &= ~hwiMask;
        }

        SemaphoreP_post(interruptSetupLock);
    }

    /* Free the channel */
    key = HwiP_disable();

    channelsInUse &= ~channelMask;

    HwiP_restore(key);
}

/*
 *  ======== UDMAMSP432_init ========
 */
void UDMAMSP432_init()
{
    uintptr_t                 key;
    HwiP_Params               hwiParams;
    SemaphoreP_Handle         sem;
    UDMAMSP432_Handle         handle = (UDMAMSP432_Handle) &UDMAMSP432_config;
    UDMAMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* speculatively create a binary semaphore for thread safety */
    sem = SemaphoreP_createBinary(1);

    key = HwiP_disable();

    if (interruptSetupLock == NULL) {
        /* use the binary sem created above */
        interruptSetupLock = sem;

        HwiP_restore(key);
    }
    else {
        HwiP_restore(key);

        /* delete unused Semaphore */
        if (sem) {
            SemaphoreP_delete(sem);
        }
    }

    if (interruptSetupLock == NULL) {
        return;
    }

    /* Acquire exclusive access while initializing */
    SemaphoreP_pend(interruptSetupLock, SemaphoreP_WAIT_FOREVER);

    if (!dmaInitialized) {
        /* Enable module & set the control table base address */
        MAP_DMA_enableModule();
        MAP_DMA_setControlBase(hwAttrs->controlBaseAddr);

        /* Create DMA error HWI; it is checked in UDMAMSP432_open() */
        HwiP_Params_init(&hwiParams);
        hwiParams.priority = hwAttrs->intPriority;
        hwiParams.arg = (uintptr_t) handle;
        errorHwi = HwiP_create(DMA_INTERR, hwAttrs->dmaErrorFxn, &hwiParams);

        dmaInitialized = true;
    }

    SemaphoreP_post(interruptSetupLock);
}

/*
 *  ======== UDMAMSP432_open ========
 */
UDMAMSP432_Handle UDMAMSP432_open(uint32_t channel, uint8_t intNum, uint32_t priority,
    void (*hwiFxn)(uintptr_t), uintptr_t arg)
{
    uintptr_t key;
    uint8_t   channelNum;
    uint8_t   channelMask;
    bool      setupSuccessful;

    if (!dmaInitialized || errorHwi == NULL) {
        /* Driver did not initialize correctly */
        return (NULL);
    }

    channelNum = (uint8_t) channel;
    channelMask = 1 << channelNum;

    if ((channelNum >= NUM_DMA_CHANNELS) || (intNum != 0 && hwiFxn == NULL) ||
        (intNum == 0 && hwiFxn != NULL)) {
        return (NULL);
    }

    key = HwiP_disable();

    /*
     *  If the desired channel is already being used by another peripheral
     *  abort. Otherwise, claim the channel and set it up to transfer data.
     */
    if (channelsInUse & channelMask) {
        HwiP_restore(key);

        return(NULL);
    }

    channelsInUse |= channelMask;

    HwiP_restore(key);

    /* Configure Hwi for DMA channel */
    if (hwiFxn != NULL) {
        setupSuccessful = setupInterrupt(channel, intNum, priority, hwiFxn,
            arg);

        if (!setupSuccessful) {
            key = HwiP_disable();

            channelsInUse &= ~channelMask;

            HwiP_restore(key);

            return (NULL);
        }
    }

    return ((UDMAMSP432_Handle) &(UDMAMSP432_config));
}

/*
 *  ======== UDMAMSP432_setupTransfer ========
 */
bool UDMAMSP432_setupTransfer(UDMAMSP432_Transfer *transfer)
{
    uintptr_t key;
    uint8_t   dmaChannelIndex = (uint8_t) transfer->dmaChannel;

    /* The DMA must be open in order to configure a channel */
    if (!dmaInitialized) {
        return(false);
    }

    /* A lock is needed because we are accessing shared DMA registers */
    key = HwiP_disable();


    /* Clear the register before setting attributes */
    MAP_DMA_disableChannelAttribute(transfer->dmaChannel,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

    /* Setting Control Indexes */
    MAP_DMA_setChannelControl(transfer->dmaChannel | transfer->structSelect,
        transfer->ctlOptions);
    MAP_DMA_setChannelTransfer(transfer->dmaChannel | transfer->structSelect,
        transfer->transferMode, transfer->dmaTransferSource, transfer->dmaTransferDestination,
        transfer->transferSize);


    MAP_DMA_clearInterruptFlag(dmaChannelIndex);

    /* Assign and enable DMA channel */
    MAP_DMA_assignChannel(transfer->dmaChannel);
    MAP_DMA_enableChannel(dmaChannelIndex);

    HwiP_restore(key);

    return(true);
}

/*
 *  ======== UDMAMSP432_setupPingPongTransfer ========
 */
bool UDMAMSP432_setupPingPongTransfer(UDMAMSP432_PingPongTransfer *transfer)
{
    uintptr_t key;
    uint8_t   dmaChannelIndex = (uint8_t) transfer->dmaChannel;

    /* The DMA must be open in order to configure a channel */
    if (!dmaInitialized) {
        return(false);
    }

    /* A lock is needed because we are accessing shared DMA registers */
    key = HwiP_disable();

    /*
     *  If the desired channel is already being used by another
     *  peripheral, abort. Otherwise, configure the channel
     *  and set it up to transfer data.
     */

    /* Clear the register before setting attributes */
    MAP_DMA_disableChannelAttribute(transfer->dmaChannel,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

    /* Set primary DMA data structure */
    MAP_DMA_setChannelControl(transfer->dmaChannel | UDMA_PRI_SELECT,
        transfer->ctlOptions);
    MAP_DMA_setChannelTransfer(transfer->dmaChannel | UDMA_PRI_SELECT,
        UDMA_MODE_PINGPONG, transfer->dmaTransferSource, transfer->dmaPrimaryDestination,
        transfer->transferSize);

    /* Set alternate DMA data structure */
    MAP_DMA_setChannelControl(transfer->dmaChannel | UDMA_ALT_SELECT,
        transfer->ctlOptions);
    MAP_DMA_setChannelTransfer(transfer->dmaChannel | UDMA_ALT_SELECT,
        transfer->transferMode, transfer->dmaTransferSource, transfer->dmaAlternateDestination,
        transfer->transferSize);

    MAP_DMA_clearInterruptFlag(dmaChannelIndex);

    /* Assign and enable DMA channel */
    MAP_DMA_assignChannel(transfer->dmaChannel);
    MAP_DMA_enableChannel(dmaChannelIndex);

    HwiP_restore(key);

    return(true);
}

/*
 *  ======== UDMAMSP432_PingPongToggleBuffer ========
 */
void UDMAMSP432_PingPongToggleBuffer(UDMAMSP432_PingPongTransfer *transfer)
{
    /* Switch between primary and alternate buffers with DMA's PingPong mode */
    if (MAP_DMA_getChannelAttribute(transfer->dmaChannel) & UDMA_ATTR_ALTSELECT) {
        MAP_DMA_setChannelControl(transfer->dmaChannel | UDMA_PRI_SELECT,
            transfer->ctlOptions);
        MAP_DMA_setChannelTransfer(transfer->dmaChannel | UDMA_PRI_SELECT,
            UDMA_MODE_PINGPONG, transfer->dmaTransferSource,
            transfer->dmaPrimaryDestination, transfer->transferSize);
    }
    else {
        MAP_DMA_setChannelControl(transfer->dmaChannel | UDMA_ALT_SELECT,
            transfer->ctlOptions);
        MAP_DMA_setChannelTransfer(transfer->dmaChannel | UDMA_ALT_SELECT,
            UDMA_MODE_PINGPONG, transfer->dmaTransferSource,
            transfer->dmaAlternateDestination, transfer->transferSize);
    }
}

/*
 *  ======== setupInterrupt ========
 */
static bool setupInterrupt(uint32_t channel, uint8_t intNum,
    uint8_t priority, void (*hwiFxn)(uintptr_t), uintptr_t arg)
{
    uint8_t                   channelNum;
    uint8_t                   hwiMask;
    uint8_t                   intIndex;
    HwiP_Params               hwiParams;
    HwiP_Fxn                  fxnPtr;
    UDMAMSP432_HWAttrs const *hwAttrs =
        ((UDMAMSP432_Handle) &UDMAMSP432_config)->hwAttrs;

    if (intNum > DMA_INT0 || intNum < DMA_INT3) {
        return (false);
    }

    intIndex = DMA_INT0 - intNum;
    hwiMask = 1 << intIndex;
    channelNum = (uint8_t) channel;

    /* Acquire exclusive access to interrupt arrays & masks */
    SemaphoreP_pend(interruptSetupLock, SemaphoreP_WAIT_FOREVER);

    if ((interruptsInUse & hwiMask) && (intNum != DMA_INT0)) {
        /*
         * Trying to use an independent interrupt (DMA_INT1, DMA_INT2,
         * DMA_INT3) but it is already in use
         */
        SemaphoreP_post(interruptSetupLock);

        return (false);
    }
    else if ((interruptsInUse & hwiMask) && (intNum == DMA_INT0)) {
        /*
         * Using INT_DMA_INT0 & Hwi is already created; need only store
         * fxn & arg
         */
        hwiFxnHandlers[channelNum] = hwiFxn;
        hwiFxnArgs[channelNum] = (uintptr_t) arg;
    }
    else {
        /* HwiP has not been created yet */
        HwiP_Params_init(&hwiParams);

        if (intNum == DMA_INT0) {
            hwiParams.priority = hwAttrs->intPriority;
            fxnPtr = &UDMAMSP432_hwiFxn;
        }
        else {
            hwiParams.arg = arg;
            hwiParams.priority = priority;
            fxnPtr = hwiFxn;
        }

        /* Clear DMA interrupt flags before creating the Hwi */
        MAP_DMA_clearInterruptFlag(channelNum);

        hwiHandles[intIndex] = HwiP_create(intNum, fxnPtr, &hwiParams);
        if (hwiHandles[intIndex] == NULL) {
            SemaphoreP_post(interruptSetupLock);

            return (false);
        }

        /* Mark the interrupt as claimed */
        interruptsInUse |= hwiMask;

        if (intNum != DMA_INT0) {
            MAP_DMA_assignInterrupt(intNum, channelNum);
        }
        else {
            hwiFxnHandlers[channelNum] = hwiFxn;
            hwiFxnArgs[channelNum] = (uintptr_t) arg;
            dmaInt0RefCount++;
        }

        MAP_DMA_enableInterrupt(intNum);
    }

    SemaphoreP_post(interruptSetupLock);

    return (true);
}

/*
 *  ======== UDMAMSP432_hwiFxn ========
 */
static void UDMAMSP432_hwiFxn(uintptr_t arg)
{
    uint8_t  channelIndex;
    uint32_t dmaInterruptStatus;

    dmaInterruptStatus = MAP_DMA_getInterruptStatus();

    /* Find which channel caused the interrupt and call its ISR */
    for (channelIndex = 0; channelIndex < NUM_DMA_CHANNELS; channelIndex++) {
        if ((dmaInterruptStatus >> channelIndex) & 1) {
            MAP_DMA_clearInterruptFlag(channelIndex);
            if (hwiFxnHandlers[channelIndex]) {
                (*hwiFxnHandlers[channelIndex])(hwiFxnArgs[channelIndex]);
            }
        }
    }
}
