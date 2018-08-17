/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/I2CSlave.h>
#include <ti/drivers/i2cslave/I2CSlaveMSP432.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>


/* driverlib header files */
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/i2c.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/pmap.h>

#define PinConfigValue(config) (((config) >> 10) & 0x1F)
#define PinConfigModuleFunction(config) (((config) >> 8) & 0x3)
#define PinConfigPort(config) (((config) >> 4) & 0xF)
#define PinConfigPin(config) (1 << ((config) & 0x7))

void I2CSlaveMSP432_close(I2CSlave_Handle handle);
int_fast16_t I2CSlaveMSP432_control(I2CSlave_Handle handle, uint_fast16_t cmd,
                           void *arg);
void I2CSlaveMSP432_init(I2CSlave_Handle handle);
I2CSlave_Handle I2CSlaveMSP432_open(I2CSlave_Handle handle,
                                    I2CSlave_Params *params);
bool I2CSlaveMSP432_write(I2CSlave_Handle handle, const void *buffer,
                          size_t size);
bool I2CSlaveMSP432_read(I2CSlave_Handle handle,void *buffer,
                         size_t size);
static void blockingTransferCallback(I2CSlave_Handle handle,
                                     bool transferStatus);
static void completeTransfer(I2CSlave_Handle handle);
static void initHw(I2CSlaveMSP432_Object *object,
                   I2CSlaveMSP432_HWAttrs const *hwAttrs);
static bool primeTransfer(I2CSlave_Handle handle, const void *writeBuffer,
                          void *readBuffer, size_t size, bool writeOperation);

static void mapPin(uint8_t port, uint8_t pin, uint8_t value);

/* I2CSlave function table for I2CMSP432 implementation */
const I2CSlave_FxnTable I2CSlaveMSP432_fxnTable = {
    I2CSlaveMSP432_close,
    I2CSlaveMSP432_control,
    I2CSlaveMSP432_init,
    I2CSlaveMSP432_open,
    I2CSlaveMSP432_read,
    I2CSlaveMSP432_write
};

#define ERROR_INTERRUPTS (EUSCI_B_I2C_ARBITRATIONLOST_INTERRUPT)

#define ALL_INTERRUPTS (EUSCI_B_I2C_RECEIVE_INTERRUPT0  | \
    EUSCI_B_I2C_TRANSMIT_INTERRUPT0 | ERROR_INTERRUPTS | \
    EUSCI_B_I2C_STOP_INTERRUPT | EUSCI_B_I2C_START_INTERRUPT )

#define ALL_READ_INTERRUPTS (EUSCI_B_I2C_RECEIVE_INTERRUPT0  | \
    ERROR_INTERRUPTS | EUSCI_B_I2C_STOP_INTERRUPT | \
    EUSCI_B_I2C_START_INTERRUPT )

#define ALL_WRITE_INTERRUPTS ( EUSCI_B_I2C_TRANSMIT_INTERRUPT0 | \
    ERROR_INTERRUPTS |     EUSCI_B_I2C_STOP_INTERRUPT | \
    EUSCI_B_I2C_START_INTERRUPT )


/*
 *  ======== blockingTransferCallback ========
 */
static void blockingTransferCallback(I2CSlave_Handle handle,
    bool transferStatus)
{
    I2CSlaveMSP432_Object *object = handle->object;

    DebugP_log1("I2CSlave:(%p) posting transferComplete semaphore",
        ((I2CSlaveMSP432_HWAttrs const *)(handle->hwAttrs))->baseAddr);

    /* Indicate transfer complete */
    SemaphoreP_post(object->transferComplete);
}

/*
 *  ======== completeTransfer =======
 */
static void completeTransfer(I2CSlave_Handle handle)
{
    I2CSlaveMSP432_Object *object = ((I2CSlave_Handle) handle)->object;
    DebugP_log1("I2CSlave:(%p) ISR Transfer Complete",
        ((I2CSlaveMSP432_HWAttrs const *)(handle->hwAttrs))->baseAddr);

    object->transferInProgress = false;
    /*
     * Perform callback in a HWI context, thus any tasks or SWIs
     * made ready to run won't start until the interrupt has
     * finished
     */
    object->transferCallbackFxn(handle, (object->mode == I2CSLAVE_IDLE_MODE));

    /* Remove constraints set during transfer */
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
    Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
}
/*
 *  ======== initHW ========
 *
 *  Configures I2CSlave peripheral
 */
static void initHw(I2CSlaveMSP432_Object *object,
    I2CSlaveMSP432_HWAttrs const *hwAttrs)
{
    /* Disable interrupts      being reconfigured */
    MAP_I2C_disableInterrupt(hwAttrs->baseAddr, ALL_INTERRUPTS);

    /* Configure the I2CSlave module */
    MAP_I2C_initSlave(hwAttrs->baseAddr,hwAttrs->slaveAddress,
        EUSCI_B_I2C_OWN_ADDRESS_OFFSET0, EUSCI_B_I2C_OWN_ADDRESS_ENABLE);

    /* Clear any pending interrupts & enable I2CSlave module */
    MAP_I2C_clearInterruptFlag(hwAttrs->baseAddr, ALL_INTERRUPTS);
    MAP_I2C_enableModule(hwAttrs->baseAddr);
    MAP_I2C_enableInterrupt(hwAttrs->baseAddr, ALL_INTERRUPTS);

    /* Set Slave in Receive mode */
    MAP_I2C_setMode(hwAttrs->baseAddr, EUSCI_B_I2C_RECEIVE_MODE);

    DebugP_log2("I2CSlave:(%p)  Slave Address:(%p) ", hwAttrs->baseAddr,
        hwAttrs->slaveAddress);
}

/*
 *  ======== I2CSlaveMSP432_close ========
 */
void I2CSlaveMSP432_close(I2CSlave_Handle handle)
{
    I2CSlaveMSP432_Object        *object = handle->object;
    I2CSlaveMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* Disable interrupts & the I2CSlave */
    MAP_I2C_disableInterrupt(hwAttrs->baseAddr, ALL_INTERRUPTS);
    MAP_I2C_disableModule(hwAttrs->baseAddr);

    /* Destruct driver resources */
    if (object->hwiHandle) {
        HwiP_delete(object->hwiHandle);
    }
    if (object->mutex) {
        SemaphoreP_delete(object->mutex);
    }
    if (object->transferComplete) {
        SemaphoreP_delete(object->transferComplete);
    }

    object->isOpen = false;

    DebugP_log1("I2CSlave: Object closed 0x%x", hwAttrs->baseAddr);
}

/*
 *  ======== I2CSlaveMSP432_control ========
 *  @pre    Function assumes that the handle is not NULL
 */
int_fast16_t I2CSlaveMSP432_control(I2CSlave_Handle handle, uint_fast16_t cmd,
        void *arg)
{
    /* No implementation yet */
    return (I2CSLAVE_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== I2CSlaveMSP432_hwiIntFxn ========
 *  Hwi interrupt handler to service the I2CSLAVE peripheral
 *
 *  The handler is a generic handler for a I2CSLAVE object.
 */
void I2CSlaveMSP432_hwiIntFxn(uintptr_t arg)
{
    uint16_t                     intStatus;
    I2CSlaveMSP432_Object        *object = ((I2CSlave_Handle) arg)->object;
    I2CSlaveMSP432_HWAttrs const *hwAttrs = ((I2CSlave_Handle) arg)->hwAttrs;

    /* Get the interrupt status of the I2CSlave controller */
    intStatus = MAP_I2C_getEnabledInterruptStatus(hwAttrs->baseAddr);
    MAP_I2C_clearInterruptFlag(hwAttrs->baseAddr, intStatus);

    /* Filter any spurious interrupts */
    if (!(intStatus & ALL_INTERRUPTS)) {
        return;
    }

    /* Check for I2CSlave Errors */
    if ((intStatus & ERROR_INTERRUPTS)) {
        /* Some sort of error occurred */
        object->mode = I2CSLAVE_ERROR;

        DebugP_log2("I2CSlave:(%p) ISR I2CSlave Bus fault (Status Reg: 0x%x)",
            hwAttrs->baseAddr, intStatus);
    }

    if(intStatus & EUSCI_B_I2C_RECEIVE_INTERRUPT0 )
            object->mode = I2CSLAVE_READ_MODE;
         else if(intStatus & EUSCI_B_I2C_TRANSMIT_INTERRUPT0 )
                object->mode = I2CSLAVE_WRITE_MODE;
              else if(intStatus & EUSCI_B_I2C_START_INTERRUPT )
                           object->mode = I2CSLAVE_START_MODE;
                   else if(intStatus & EUSCI_B_I2C_STOP_INTERRUPT )
                       object->mode = I2CSLAVE_IDLE_MODE;


    /* No errors, now check what we need to do next */
    switch (object->mode) {
        case I2CSLAVE_ERROR:
        case I2CSLAVE_IDLE_MODE:
            completeTransfer((I2CSlave_Handle) arg);
            break;
        case I2CSLAVE_START_MODE:
            if(object->transferInProgress)
            {
                completeTransfer((I2CSlave_Handle) arg);
                object->mode = I2CSLAVE_IDLE_MODE;
            }
                break;

        case I2CSLAVE_WRITE_MODE:
            object->transferInProgress = true;
              if (object->countIdx){
                  /* Write data contents into data register */
                MAP_I2C_clearInterruptFlag(hwAttrs->baseAddr,
                    EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
                MAP_I2C_slavePutData(hwAttrs->baseAddr,
                                     *(object->writeBufferIdx));
                DebugP_log2("I2CSlave:(%p) ISR I2CSLAVE_WRITE_MODE: "
                    "Data to write: 0x%x",
                    hwAttrs->baseAddr, *(object->writeBufferIdx));

                object->writeBufferIdx++;
                object->countIdx--;
            }
            else {
                    DebugP_log1("I2CSlave:(%p) ISR I2CSLAVE_WRITE_MODE: "
                        "No more bytes available to write From slave: 0xFF",
                        hwAttrs->baseAddr);
                    object->mode = I2CSLAVE_ERROR;
                    MAP_I2C_clearInterruptFlag(hwAttrs->baseAddr,
                        EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
                    MAP_I2C_slavePutData(hwAttrs->baseAddr, 0xFF);
                    DebugP_log2("I2CSlave:(%p) ISR I2CSLAVE_WRITE_MODE: "
                        "Data to write: 0x%x",
                        hwAttrs->baseAddr, *(object->writeBufferIdx));
            }
            break; /* I2CSLAVE_WRITE_MODE */

        case I2CSLAVE_READ_MODE:
            object->transferInProgress = true;
            /* Data read from RXBUF and next byte has already been shifted */
            if(object->countIdx) {
                *(object->readBufferIdx) =
                    MAP_I2C_slaveGetData(hwAttrs->baseAddr);

                DebugP_log2("I2CSlave:(%p) ISR I2CSLAVE_READ_MODE: "
                    "Read data byte: 0x%x",
                    hwAttrs->baseAddr, *(object->readBufferIdx));

                if(object->countIdx > 0)
                    object->readBufferIdx++;

                object->countIdx--;
            }
            else {
                DebugP_log1("I2CSlave:(%p) ISR I2CSLAVE_READ_MODE: "
                        "Buffer overrun. Please increase the buffer size",
                        hwAttrs->baseAddr);
                object->mode = I2CSLAVE_ERROR;
                MAP_I2C_slaveSendNAK(hwAttrs->baseAddr);
                MAP_I2C_slaveGetData(hwAttrs->baseAddr);
                completeTransfer((I2CSlave_Handle) arg);
            }
            break; /* I2CSLAVE_READ_MODE */


        default:
            object->mode = I2CSLAVE_ERROR;
            break;
    }
}

/*
 *  ======== I2CSlaveMSP432_init ========
 */
void I2CSlaveMSP432_init(I2CSlave_Handle handle)
{
}

/*
 *  ======== I2CSlaveMSP432_open ========
 */
I2CSlave_Handle I2CSlaveMSP432_open(I2CSlave_Handle handle,
    I2CSlave_Params *params)
{
    uintptr_t                key;
    union {
        SemaphoreP_Params    semParams;
        HwiP_Params          hwiParams;
    } portsParams;
    I2CSlaveMSP432_Object        *object = handle->object;
    I2CSlaveMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint16_t                   pin;
    uint16_t                   port;
    uint16_t                   value;
    uint16_t                   moduleFunction;

    /* Check that a callback is provided if using callback mode */
    DebugP_assert(params->transferMode != I2CSLAVE_MODE_CALLBACK ||
        params->transferCallbackFxn != NULL);

    /* Determine if the driver was already opened */
    key = HwiP_disable();

    if (object->isOpen) {
        HwiP_restore(key);
        DebugP_log1("I2CSlave:(%p) Error! Already in use.", hwAttrs->baseAddr);
        return (NULL);
    }
    object->isOpen = true;

    HwiP_restore(key);

    /* Map the pins */
    /* Clock */
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

    pin  = PinConfigPin(hwAttrs->clkPin);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(port,
            pin, moduleFunction);

    /* Data */
    port = PinConfigPort(hwAttrs->dataPin);
    value = PinConfigValue(hwAttrs->dataPin);

    if (value != 0) {
        moduleFunction = GPIO_PRIMARY_MODULE_FUNCTION;
        pin = (hwAttrs->dataPin) & 0x7;
        mapPin(port, pin, value);
    }
    else {
        moduleFunction = PinConfigModuleFunction(hwAttrs->dataPin);
    }

    pin  = PinConfigPin(hwAttrs->dataPin);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(port,
            pin, moduleFunction);

    /* Create Hwi object for this I2CSlave peripheral */
    HwiP_Params_init(&(portsParams.hwiParams));
    portsParams.hwiParams.arg = (uintptr_t) handle;
    portsParams.hwiParams.priority = hwAttrs->intPriority;
    object->hwiHandle = HwiP_create(hwAttrs->intNum, I2CSlaveMSP432_hwiIntFxn,
        &(portsParams.hwiParams));
    if (!object->hwiHandle) {
        DebugP_log1("I2CSlave:(%p) HwiP_create() failed.", hwAttrs->baseAddr);
        I2CSlaveMSP432_close(handle);
        return (NULL);
    }

    /*
     * Create threadsafe handles for this I2CSlave peripheral. Semaphore ensures
     * exclusive access to the I2CSlave peripheral.
     */
    SemaphoreP_Params_init(&(portsParams.semParams));
    (portsParams.semParams).mode = SemaphoreP_Mode_BINARY;
    object->mutex = SemaphoreP_create(1, &(portsParams.semParams));
    if (!object->mutex) {
        DebugP_log1("I2CSlave:(%p) mutex Semaphore_create() failed: %d.",
            hwAttrs->baseAddr);
        I2CSlaveMSP432_close(handle);
        return (NULL);
    }

    /* Configure driver to Callback or Blocking operating mode */
    if (params->transferMode == I2CSLAVE_MODE_CALLBACK) {
        object->transferCallbackFxn = params->transferCallbackFxn;
        DebugP_log1("I2CSlave:(%p) in I2CSLAVE_MODE_CALLBACK mode",
            hwAttrs->baseAddr);
    }
    else {
        /* Semaphore to block task for the duration of the I2CSlave transfer */
        object->transferComplete = SemaphoreP_create(0,
                                                     &(portsParams.semParams));
        if (!object->transferComplete) {
            DebugP_log1("I2CSlave:(%p) transfer SemaphoreP_create() failed: %d.",
                hwAttrs->baseAddr);
            I2CSlaveMSP432_close(handle);
            return (NULL);
        }
        object->transferCallbackFxn = blockingTransferCallback;
        DebugP_log1("I2CSlave:(%p) in I2CSLAVE_MODE_BLOCKING mode",
            hwAttrs->baseAddr);
    }

    /*
     * Store I2CSlave parameters & initialize peripheral.  These are used to
     * re/initialize the peripheral when opened or changing performance level.
     */
    object->mode = I2CSLAVE_IDLE_MODE;
    object->transferMode = params->transferMode;
    initHw(object, hwAttrs);

    DebugP_log1("I2CSlave: Object opened 0x%x", hwAttrs->baseAddr);

    return (handle);
}

/*
 *  ======== primeTransfer =======
 */
static bool primeTransfer(I2CSlave_Handle handle, const void *writeBuffer,
                          void *readBuffer, size_t size, bool writeOperation)
{
    bool                     ret = false;
    I2CSlaveMSP432_Object        *object = handle->object;
    I2CSlaveMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* Check if anything needs to be written */
    if (!size) {
        return (ret);
    }

    /* Acquire the lock for this particular I2CSlave handle */
    SemaphoreP_pend(object->mutex, SemaphoreP_WAIT_FOREVER);

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
    /*
     * Set power constraints to keep peripheral active during transfer
     */
    Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif

    /*
     * The buffer and flag are being updated and
     * protection is needed from the I2CSlave interrupt
     */
    MAP_I2C_disableInterrupt(hwAttrs->baseAddr, ALL_INTERRUPTS);

    /* Store the new internal counters and pointers */
    object->countIdx = size;
    object->transferInProgress = false;
    object->writeBufferIdx = writeBuffer;
    object->readBufferIdx = readBuffer;

    if(true == writeOperation) {
        MAP_I2C_enableInterrupt(hwAttrs->baseAddr, ALL_WRITE_INTERRUPTS);
    }
    else {
        MAP_I2C_enableInterrupt(hwAttrs->baseAddr, ALL_READ_INTERRUPTS);
    }

    if (object->transferMode == I2CSLAVE_MODE_BLOCKING) {
        DebugP_log1("I2CSlave:(%p) Pending on transferComplete semaphore",
            hwAttrs->baseAddr);
        /*
         * Wait for the transfer to complete here.
         * It's OK to block from here because the I2C's Hwi will unblock
         * upon errors
         */
        SemaphoreP_pend(object->transferComplete, SemaphoreP_WAIT_FOREVER);

        DebugP_log1("I2CSlave:(%p) Transaction completed", hwAttrs->baseAddr);

        /* Hwi handle has posted a 'transferComplete' check for Errors */
        if (object->mode == I2CSLAVE_IDLE_MODE) {
            DebugP_log1("I2CSlave:(%p) Transfer OK", hwAttrs->baseAddr);
            ret = true;
        }
    }
    else {
        /* Always return true if in Asynchronous mode */
        ret = true;
    }

    /* Release the lock for this particular I2CSlave handle */
    SemaphoreP_post(object->mutex);

    object->transferInProgress = false;
    /* Return the number of bytes transfered by the I2CSlave */
    return (ret);
}

/*
 *  ======== I2CSlaveMSP432_write ========
 */
bool I2CSlaveMSP432_write(I2CSlave_Handle handle, const void *buffer,
                          size_t size)
{
    return (primeTransfer(handle, buffer, NULL, size, true));
}

/*
 *  ======== I2CSlaveMSP432_read ========
 */
bool I2CSlaveMSP432_read(I2CSlave_Handle handle, void *buffer,
                         size_t size)
{
    return (primeTransfer(handle, NULL, buffer, size, false));
}

/*
 *  ======== mapPin ========
 *  NOTE: This function may go away when DriverLib supports mapping
 *  an individual pin.
 */
static void mapPin(uint8_t port, uint8_t pin, uint8_t value)
{
    volatile uint8_t                     pmap;

    pmap = port * 0x8;  // 2 -> 0x10, 3 -> 0x18, 7 -> 0x38


    /*  Code from pmap.c: */
    //Get write-access to port mapping registers:
    PMAP->KEYID = PMAP_KEYID_VAL;

    PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | PMAP_ENABLE_RECONFIGURATION;
    HWREG8((uint32_t)PMAP_BASE + pin + pmap) = value;
    //Disable write-access to port mapping registers:

    PMAP->KEYID = 0;
}
