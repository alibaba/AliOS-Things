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

/*-----------------------------------------------------------------------*/
/* MMC/SDC (in SPI mode) control module  (C)ChaN, 2007                   */
/*-----------------------------------------------------------------------*/

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

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>
#include <ti/drivers/SDSPI.h>
#include <ti/drivers/sdspi/SDSPIMSP432.h>

/* driverlib header files */
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/spi.h>
#include <ti/devices/msp432p4xx/driverlib/pmap.h>

/* Definitions for MMC/SDC command */
#define CMD0                        (0x40+0)    /* GO_IDLE_STATE */
#define CMD1                        (0x40+1)    /* SEND_OP_COND */
#define CMD8                        (0x40+8)    /* SEND_IF_COND */
#define CMD9                        (0x40+9)    /* SEND_CSD */
#define CMD10                       (0x40+10)   /* SEND_CID */
#define CMD12                       (0x40+12)   /* STOP_TRANSMISSION */
#define CMD16                       (0x40+16)   /* SET_BLOCKLEN */
#define CMD17                       (0x40+17)   /* READ_SINGLE_BLOCK */
#define CMD18                       (0x40+18)   /* READ_MULTIPLE_BLOCK */
#define CMD23                       (0x40+23)   /* SET_BLOCK_COUNT */
#define CMD24                       (0x40+24)   /* WRITE_BLOCK */
#define CMD25                       (0x40+25)   /* WRITE_MULTIPLE_BLOCK */
#define CMD41                       (0x40+41)   /* SEND_OP_COND (ACMD) */
#define CMD55                       (0x40+55)   /* APP_CMD */
#define CMD58                       (0x40+58)   /* READ_OCR */

#define SD_SECTOR_SIZE              (512)

#define START_BLOCK_TOKEN           (0xFE)
#define START_MULTIBLOCK_TOKEN      (0xFC)
#define STOP_MULTIBLOCK_TOKEN       (0xFD)

#define DRIVE_NOT_MOUNTED           ((uint16_t) ~0)

#define PinConfigPin(config)              (1 << ((config) & 0x7))
#define PinConfigPort(config)             ((config >> 4) & 0xF)
#define PinConfigValue(config)            (((config) >> 10) & 0x1F)
#define PinConfigModuleFunction(config)   (((config) >> 8) & 0x3)

static void mapPin(uint8_t port, uint8_t pin, uint8_t value);

/*
 * Array of SDSPI_Handles to determine the association of the FatFs drive number
 * with a SDSPI_Handle
 * FF_VOLUMES is defined in <third_party/fatfs/ffconf.h>
 */
static SDSPI_Handle sdspiHandles[FF_VOLUMES];

/* uS scaling to function timeouts */
static uint32_t       uSClockPeriod = 0;

/* FatFs disk I/O functions */
DSTATUS SDSPIMSP432_diskInitialize(BYTE drv);
DRESULT SDSPIMSP432_diskIOctrl(BYTE drv, BYTE ctrl, void *buf);
DRESULT SDSPIMSP432_diskRead(BYTE drv, BYTE *buf, DWORD sector, UINT count);
DSTATUS SDSPIMSP432_diskStatus(BYTE drv);
DRESULT SDSPIMSP432_diskWrite(BYTE drv, const BYTE *buf, DWORD sector,
    UINT count);

/* SDSPIMSP432 functions */
void SDSPIMSP432_close(SDSPI_Handle handle);
int_fast16_t  SDSPIMSP432_control(SDSPI_Handle handle, uint_fast16_t cmd,\
        void *arg);
void SDSPIMSP432_init(SDSPI_Handle handle);
SDSPI_Handle SDSPIMSP432_open(SDSPI_Handle handle, uint_least8_t drv,
    SDSPI_Params *params);
static void initHw(SDSPIMSP432_Object *object,
    SDSPIMSP432_HWAttrsV1 const *hwAttrs, uint32_t inputClkFreq);
static int perfChangeNotifyFxn(unsigned int eventType, uintptr_t eventArg,
    uintptr_t clientArg);
static uint32_t rcvr_datablock(SDSPIMSP432_HWAttrsV1 const *hwAttrs,
        uint8_t *buf, uint32_t btr);
static inline void releaseSPIBus(SDSPIMSP432_HWAttrsV1 const *hwAttrs);
static inline uint8_t rxSPI(SDSPIMSP432_HWAttrsV1 const *hwAttrs);
static uint8_t send_cmd(SDSPIMSP432_HWAttrsV1 const *hwAttrs, uint8_t cmd,
    uint32_t arg);
static void send_initial_clock_train(SDSPIMSP432_HWAttrsV1 const *hwAttrs);
static inline void takeSPIBus(SDSPIMSP432_HWAttrsV1 const *hwAttrs);
static inline void txSPI(SDSPIMSP432_HWAttrsV1 const *hwAttrs, uint8_t dat);
static uint8_t wait_ready(SDSPIMSP432_HWAttrsV1 const *hwAttrs);
static bool xmit_datablock(SDSPIMSP432_HWAttrsV1 const *hwAttrs,
    const uint8_t *buf, uint8_t token);

/* SDSPI function table for SDSPIMSP432 implementation */
const SDSPI_FxnTable SDSPIMSP432_fxnTable = {
    SDSPIMSP432_init,
    SDSPIMSP432_open,
    SDSPIMSP432_close,
    SDSPIMSP432_control
};

/*
 *  ======== initHW ========
 *
 *  Configures SDSPI peripheral
 */
static void initHw(SDSPIMSP432_Object *object,
    SDSPIMSP432_HWAttrsV1 const *hwAttrs, uint32_t inputClkFreq)
{
    uint32_t               bitRate;
    eUSCI_SPI_MasterConfig masterCfg;

    /*
     * If disk is not initialized configure the SPI bus to 400 kHz as
     * required per SD specs. Otherwise configure SPI with operating bit rate.
     */
    bitRate = (object->diskState == STA_NOINIT) ? 400000 : object->bitRate;

    masterCfg.selectClockSource = hwAttrs->clockSource;
    masterCfg.clockSourceFrequency = inputClkFreq;
    masterCfg.desiredSpiClock = bitRate;
    masterCfg.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    masterCfg.clockPhase =
        EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    masterCfg.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    masterCfg.spiMode = EUSCI_B_SPI_3PIN;

    MAP_SPI_initMaster(hwAttrs->baseAddr, &masterCfg);

    MAP_SPI_enableModule(hwAttrs->baseAddr);

    DebugP_log3("SDSPI:(%p) CPU freq: %d; SDSPI freq to %d Hz",
        hwAttrs->baseAddr, inputClkFreq, bitRate);
}

/*
 *  ======== perfChangeNotifyFxn ========
 *
 *  Called by Power module before and after performance level is changed.
 */
static int perfChangeNotifyFxn(unsigned int eventType, uintptr_t eventArg,
        uintptr_t clientArg)
{
    uint32_t                     clockFreq;
    PowerMSP432_Freqs            powerFreqs;
    SDSPIMSP432_Object          *object = ((SDSPI_Handle) clientArg)->object;
    SDSPIMSP432_HWAttrsV1 const *hwAttrs = ((SDSPI_Handle) clientArg)->hwAttrs;

    if (eventType == PowerMSP432_START_CHANGE_PERF_LEVEL) {
        /* Disable peripheral before performance level change */
        MAP_SPI_disableModule(hwAttrs->baseAddr);
    }
    else { /* eventType == PowerMSP432_DONE_CHANGE_PERF_LEVEL */
        /* Get new performance level clock frequencies */
        PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);
        clockFreq = (hwAttrs->clockSource == EUSCI_B_SPI_CLOCKSOURCE_SMCLK) ?
            powerFreqs.SMCLK : powerFreqs.ACLK;

        /* Reconfigure SPI peripheral */
        initHw(object, hwAttrs, clockFreq);
    }

    return (Power_NOTIFYDONE);
}

/*
 *  ======== rcvr_datablock ========
 *  Function to receive a block of data from the SDCard
 *
 *  btr count must be an even number
 */
static uint32_t rcvr_datablock(SDSPIMSP432_HWAttrsV1 const *hwAttrs,
        uint8_t *buf, uint32_t btr)
{
    uint8_t  token;
    uint32_t clockTimeout;
    uint32_t clockStart;
    uint32_t clockCurrent;

    /* Wait for data packet in timeout of 1000 ms */
    clockStart = ClockP_getSystemTicks();
    clockTimeout = clockStart + (1000 * 1000/uSClockPeriod);
    if (clockTimeout > clockStart) {
        clockStart = ~0;
    }
    do {
        token = rxSPI(hwAttrs);
        clockCurrent = ClockP_getSystemTicks();
    } while ((token == 0xFF) && ((clockCurrent <= clockTimeout) ||
        (clockCurrent >= clockStart)));

    if (token != START_BLOCK_TOKEN) {
        /* If not valid data token, return error */
        return (0);
    }

    /* Receive the data block into buffer */
    do {
        *(buf++) = rxSPI(hwAttrs);
    } while (--btr);

    /* Read the CRC, but discard it */
    rxSPI(hwAttrs);
    rxSPI(hwAttrs);

    /* Return with success */
    return (1);
}

/*
 *  ======== releaseSPIBus ========
 *  Function to release the SPI bus
 *
 *  @param  hwAttrs     Pointer to hardware attributes
 */
static inline void releaseSPIBus(SDSPIMSP432_HWAttrsV1 const *hwAttrs)
{
    /* Deselect the SD card. */
    MAP_GPIO_setOutputHighOnPin(PinConfigPort(hwAttrs->csPin),
            PinConfigPin(hwAttrs->csPin));
}

/*
 *  ======== rxSPI ========
 *  Function to receive one byte onto the SPI bus. Polling (Blocked)
 *
 *  @param  hwAttrs     Pointer to hardware attributes
 */
static inline uint8_t rxSPI(SDSPIMSP432_HWAttrsV1 const *hwAttrs)
{
    uint8_t rcvdat;

    /* Wait for all TX/RX to finish */
    while (!(MAP_SPI_getInterruptStatus(hwAttrs->baseAddr,
        EUSCI_A_SPI_TRANSMIT_INTERRUPT) & EUSCI_A_SPI_TRANSMIT_INTERRUPT)) {}

    /* write dummy data */
    MAP_SPI_transmitData(hwAttrs->baseAddr, 0xFF);

    /* Wait while not ready for RX */
    while (!(MAP_SPI_getInterruptStatus(hwAttrs->baseAddr,
        EUSCI_A_SPI_RECEIVE_INTERRUPT) & EUSCI_A_SPI_RECEIVE_INTERRUPT)) {}

    /* Read data frm RX */
    rcvdat = MAP_SPI_receiveData(hwAttrs->baseAddr);

    return (rcvdat);
}

/*
 *  ======== send_cmd ========
 *  Function that will transmit an command to the SDCard
 *
 *  @param  hwAttrs     Pointer to hardware attributes
 *
 *  @param  cmd         SD command
 *
 *  @param  arg         SD command argument
 */
static uint8_t send_cmd(SDSPIMSP432_HWAttrsV1 const *hwAttrs, uint8_t cmd,
    uint32_t arg)
{
    uint8_t n;
    uint8_t res;

    if (wait_ready(hwAttrs) != 0xFF) {
        DebugP_log1("SDSPI:(%p) send_cmd: SD card wait time expired",
            hwAttrs->baseAddr);
        return (0xFF);
    }

    /* Send command packet */
    txSPI(hwAttrs, cmd);                    /* Command */
    txSPI(hwAttrs, (uint8_t)(arg >> 24));   /* Argument[31..24] */
    txSPI(hwAttrs, (uint8_t)(arg >> 16));   /* Argument[23..16] */
    txSPI(hwAttrs, (uint8_t)(arg >> 8));    /* Argument[15..8] */
    txSPI(hwAttrs, (uint8_t)arg);           /* Argument[7..0] */

    if (cmd == CMD0) {
        /* CRC for CMD0(0) */
        n = 0x95;
    }
    else if (cmd == CMD8) {
        /* CRC for CMD8(0x1AA) */
        n = 0x87;
    }
    else {
        /* Default CRC should be at least 0x01 */
        n = 0x01;
    }

    /* Future enhancement to add CRC support */
    txSPI(hwAttrs, n);

    /* Receive command response */
    if (cmd == CMD12) {
        /* Skip a stuff byte when stop reading */
        rxSPI(hwAttrs);
    }

    /* Wait for a valid response in timeout; 10 attempts */
    n = 10;
    do {
        res = rxSPI(hwAttrs);
    } while ((res & 0x80) && --n);

    /* Return with the response value */
    return (res);
}

/*
 *  ======== send_initial_clock_train ========
 *  Function to get the SDCard into SPI mode
 *
 *  @param  hwAttrs     Pointer to hardware attributes
 */
static void send_initial_clock_train(SDSPIMSP432_HWAttrsV1 const *hwAttrs)
{
    unsigned char i;
    uint16_t      pin;
    uint8_t       port;
    uint8_t       mode;
    uint8_t       value;

    /* Deselect the SD card. */
    MAP_GPIO_setOutputHighOnPin(PinConfigPort(hwAttrs->csPin),
            PinConfigPin(hwAttrs->csPin));

    pin = PinConfigPin(hwAttrs->simoPin);
    port = PinConfigPort(hwAttrs->simoPin);
    mode = PinConfigModuleFunction(hwAttrs->simoPin);

    /* Switch the SPI TX line to a GPIO and drive it high too. */
    MAP_GPIO_setAsOutputPin(port, pin);
    MAP_GPIO_setOutputHighOnPin(port, pin);

    /*
     * Send 10 bytes over the SPI bus. This causes the clock to toggle several
     * times to get the SD Card into SPI mode.
     */
    for (i = 0; i < 10; i++) {
        rxSPI(hwAttrs);
    }

    /* Revert to hardware control of the SPITX line. */
    mode = PinConfigModuleFunction(hwAttrs->simoPin);
    value = PinConfigValue(hwAttrs->simoPin);

    if (value != 0) {
        mode = GPIO_PRIMARY_MODULE_FUNCTION;
        mapPin(port, (hwAttrs->simoPin) & 0x7 /* pin */, value);
    }

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port, pin, mode);

    DebugP_log1("SDSPI:(%p) initialized SD card to SPI mode",
        hwAttrs->baseAddr);
}

/*
 *  ======== takeSPIBus ========
 *  Function to take the SPI bus
 *
 *  @param  hwAttrs     Pointer to hardware attributes
 */
static inline void takeSPIBus(SDSPIMSP432_HWAttrsV1 const *hwAttrs)
{
    /* Select the SD card. */
    MAP_GPIO_setOutputLowOnPin(PinConfigPort(hwAttrs->csPin),
            PinConfigPin(hwAttrs->csPin));
}

/*
 *  ======== txSPI ========
 *  Function to transmit one byte onto the SPI bus. Polling (Blocked)
 *
 *  @param  hwAttrs     Pointer to hardware attributes
 *
 *  @param  dat         Data to be sent onto the SPI bus
 */
static inline void txSPI(SDSPIMSP432_HWAttrsV1 const *hwAttrs, uint8_t dat)
{
    uintptr_t key;

    key = HwiP_disable();

    /* Wait for all TX/RX to finish */
    while (!(MAP_SPI_getInterruptStatus(hwAttrs->baseAddr,
        EUSCI_A_SPI_TRANSMIT_INTERRUPT) & EUSCI_A_SPI_TRANSMIT_INTERRUPT)) {}

    /* Write the data to the TX */
    MAP_SPI_transmitData(hwAttrs->baseAddr, dat);

    /* Wait for all TX/RX to finish */
    while (!(MAP_SPI_getInterruptStatus(hwAttrs->baseAddr,
        EUSCI_A_SPI_RECEIVE_INTERRUPT) & EUSCI_A_SPI_RECEIVE_INTERRUPT)) {}

    /* flush data read during the write */
    MAP_SPI_receiveData(hwAttrs->baseAddr);

    HwiP_restore(key);
}

/*
 *  ======== wait_ready ========
 *  Function to check if the SDCard is busy
 *
 *  This function queries the SDCard to see if it is in a busy state or ready
 *  state
 *
 *  @param  hwAttrs     Pointer to hardware attributes
 */
static uint8_t wait_ready(SDSPIMSP432_HWAttrsV1 const *hwAttrs)
{
    uint8_t   res;
    uint32_t  clockTimeout;
    uint32_t  clockStart;
    uint32_t  clockCurrent;

    /* Wait for data packet in timeout of 1s */
    clockStart = ClockP_getSystemTicks();
    clockTimeout = clockStart + (1000 * 1000/uSClockPeriod);
    if (clockTimeout > clockStart) {
        clockStart = ~0;
    }
    rxSPI(hwAttrs);
    do {
        res = rxSPI(hwAttrs);
        clockCurrent = ClockP_getSystemTicks();
    } while ((res != 0xFF) && ((clockCurrent <= clockTimeout) ||
        (clockCurrent >= clockStart)));

    return (res);
}

/* _READONLY is defined in <third_party/fatfs/diskio.h> */
#if _READONLY == 0
/*
 *  ======== xmit_datablock ========
 *  Function to transmit a block of data to the SDCard
 *
 *  @param  hwAttrs     Pointer to hardware attributes
 *
 *  @param  params      SDSPIMSP432 hardware attributes
 *
 *  @param  buf         pointer to const data buffer
 *
 *  @param  token       command token to be sent to the SD card prior to
 *                      sending the data block. The available tokens are:
 *                      START_BLOCK_TOKEN
 *                      START_MULTIBLOCK_TOKEN
 *                      STOP_MULTIBLOCK_TOKEN
 */
static bool xmit_datablock(SDSPIMSP432_HWAttrsV1 const *hwAttrs,
    const uint8_t *buf, uint8_t token)
{
    uint8_t resp;
    uint8_t wc;

    if (wait_ready(hwAttrs) != 0xFF) {
        /* Return with error */
        return (false);
    }

    /* Xmit data token */
    txSPI(hwAttrs, token);

    /* Send data only when token != STOP_MULTIBLOCK_TOKEN */
    if (token != STOP_MULTIBLOCK_TOKEN) {
        /* Is data token */
        wc = 0;
        /* Transferring 512 byte blocks using a 8 bit counter */
        do {
            /* Xmit the SD_SECTOR_SIZE byte data block */
            txSPI(hwAttrs, *buf++);
            txSPI(hwAttrs, *buf++);
        } while (--wc);

        /* Future enhancement to add CRC support */
        txSPI(hwAttrs, 0xFF);
        txSPI(hwAttrs, 0xFF);

        /* Reveive data response */
        resp = rxSPI(hwAttrs);

        /* If not accepted, return error */
        if ((resp & 0x1F) != 0x05) {
            return (false);
        }
    }

    /* Return with success */
    return (true);
}
#endif /* _READONLY */

/*
 *  ======== SDSPIMSP432_close ========
 *  Function to unmount the FatFs filesystem and unregister the SDSPIMSP432
 *  disk I/O functions from SYS/BIOS' FatFS module.
 *
 *  @param  handle      SDSPI_Handle returned by SDSPI_open()
 */
void SDSPIMSP432_close(SDSPI_Handle handle)
{
    unsigned int                 i;
    DRESULT                      dresult;
    FRESULT                      fresult;
    SDSPIMSP432_Object          *object = handle->object;
    SDSPIMSP432_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    TCHAR                        path[3];

    path[0] = '0' + object->driveNumber;
    path[1] = ':';
    path[2] = '\0';

    /* Unmount the FatFs drive */
    fresult = f_mount(NULL, path, 0);
    if (fresult != FR_OK) {
        DebugP_log2("SDSPI:(%p) Could not unmount FatFs volume @ drive number %d",
            hwAttrs->baseAddr, object->driveNumber);
    }

    /* Unregister the disk_*() functions */
    dresult = disk_unregister(object->driveNumber);
    if (dresult != RES_OK) {
        DebugP_log2("SDSPI:(%p) Error unregistering disk functions @ drive number %d",
            hwAttrs->baseAddr, object->driveNumber);
    }

    MAP_SPI_disableModule(hwAttrs->baseAddr);

    /* Remove power constraints */
    for (i = 0; object->perfConstraintMask; ++i) {
        if (object->perfConstraintMask & 0x01) {
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERFLEVEL_0 + i);
        }
        object->perfConstraintMask >>= 1;
    }
    Power_unregisterNotify(&object->perfChangeNotify);

    object->diskState = STA_NOINIT;
    object->driveNumber = DRIVE_NOT_MOUNTED;

    DebugP_log1("SDSPI:(%p) closed", hwAttrs->baseAddr);
}

/*
 *  ======== SDSPIMSP432_control ========
 *  @pre    Function assumes that the handle is not NULL
 */
int_fast16_t SDSPIMSP432_control(SDSPI_Handle handle, uint_fast16_t cmd,
        void *arg)
{
    /* No implementation yet */
    return (SDSPI_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== SDSPIMSP432_diskInitialize ========
 *  Function to initialize the SD Card.  This function is called by the FatFs
 *  module and must not be called by the application!
 *
 *  @param  drv         Drive Number
 */
DSTATUS SDSPIMSP432_diskInitialize(BYTE drv)
{
    uint8_t                      n;
    uint8_t                      ocr[4];
    SDSPIMSP432_CardType         cardType;
    uint32_t                     clockFreq;
    uint32_t                     clockTimeout;
    uint32_t                     clockStart;
    uint32_t                     clockCurrent;
    PowerMSP432_Freqs            powerFreqs;
    SDSPIMSP432_Object          *object = sdspiHandles[drv]->object;
    SDSPIMSP432_HWAttrsV1 const *hwAttrs = sdspiHandles[drv]->hwAttrs;

    /* No card in the socket */
    if (object->diskState & STA_NODISK) {
        DebugP_log1("SDSPI:(%p) Error! Disk initialization failed: No disk",
            hwAttrs->baseAddr);

        return (object->diskState);
    }

    /* Disallow performance transitions while initializing disk. */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /* Initialize the SD Card for SPI mode */
    send_initial_clock_train(hwAttrs);

    /* Select the SD Card's chip select */
    takeSPIBus(hwAttrs);
    cardType = SDSPIMSP432_NOCARD;

    /* Send the CMD0 to put the SD Card in "Idle" state */
    if (send_cmd(hwAttrs, CMD0, 0) == 1) {
        /*
         * Determine what SD Card version we are dealing with
         * Depending on which SD Card version, we need to send different SD
         * commands to the SD Card, which will have different response fields.
         */
        if (send_cmd(hwAttrs, CMD8, 0x1AA) == 1) {
            /* SDC Ver2+ */
            for (n = 0; n < 4; n++) {
                ocr[n] = rxSPI(hwAttrs);
            }

            /*
             * Ensure that the card's voltage range is valid
             * The card can work at vdd range of 2.7-3.6V
             */
            if ((ocr[2] == 0x01) && (ocr[3] == 0xAA)) {
                /* Wait for data packet in timeout of 1s */
                clockStart = ClockP_getSystemTicks();
                clockTimeout = clockStart + (1000 * 1000/uSClockPeriod);
                if (clockTimeout > clockStart) {
                    clockStart = ~0;
                }
                do {
                    /* ACMD41 with HCS bit */
                    if (send_cmd(hwAttrs, CMD55, 0) <= 1 &&
                        send_cmd(hwAttrs, CMD41, 1UL << 30) == 0) {
                        clockTimeout = 0;
                        break;
                    }
                    clockCurrent = ClockP_getSystemTicks();
                } while ((clockCurrent <= clockTimeout) ||
                    (clockCurrent >= clockStart));

                /*
                 * Check CCS bit to determine which type of capacity we are
                 * dealing with
                 */
                if ((!clockTimeout) && send_cmd(hwAttrs, CMD58, 0) == 0) {
                    for (n = 0; n < 4; n++) {
                        ocr[n] = rxSPI(hwAttrs);
                    }
                    cardType = (ocr[0] & 0x40) ? SDSPIMSP432_SDHC : SDSPIMSP432_SDSC;
                }
            }
        }

        /* SDC Ver1 or MMC */
        else {
            /*
             * The card version is not SDC V2+ so check if we are dealing with a
             * SDC or MMC card
             */
            if ((send_cmd(hwAttrs, CMD55, 0) <= 1 &&
                 send_cmd(hwAttrs, CMD41, 0) <= 1)) {
                cardType = SDSPIMSP432_SDSC;
            }
            else {
                cardType = SDSPIMSP432_MMC;
            }

            /* Wait for data packet in timeout of 1s */
            clockStart = ClockP_getSystemTicks();
            clockTimeout = clockStart + (1000 * 1000/uSClockPeriod);
            if (clockTimeout > clockStart) {
                clockStart = ~0;
            }
            do {
                if (cardType == SDSPIMSP432_SDSC) {
                    /* ACMD41 */
                    if (send_cmd(hwAttrs, CMD55, 0) <= 1 &&
                        send_cmd(hwAttrs, CMD41, 0) == 0) {
                        clockTimeout = 0;
                        break;
                    }
                }
                else {
                    /* CMD1 */
                    if (send_cmd(hwAttrs, CMD1, 0) == 0) {
                        clockTimeout = 0;
                        break;
                    }
                }
                clockCurrent = ClockP_getSystemTicks();
            } while ((clockCurrent <= clockTimeout) ||
                     (clockCurrent >= clockStart));

            /* Select R/W block length */
            if ((clockTimeout) ||
                send_cmd(hwAttrs, CMD16, SD_SECTOR_SIZE) != 0) {
                cardType = SDSPIMSP432_NOCARD;
            }
        }
    }

    object->cardType = cardType;

    /* Deselect the SD Card's chip select */
    releaseSPIBus(hwAttrs);

    /* Idle (Release DO) */
    rxSPI(hwAttrs);

    /* Check to see if a card type was determined */
    if (cardType != SDSPIMSP432_NOCARD) {
        /* Get clock frequencies off the current performance level */
        PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);

        /*
         * Reconfigure the SPI bus at the new frequency rate.
         * Since we've gotten this far, we know that hwAttrs->clockSource
         * and current performance level are valid.
         */
        clockFreq = (hwAttrs->clockSource == EUSCI_SPI_CLOCKSOURCE_SMCLK) ?
            powerFreqs.SMCLK : powerFreqs.ACLK;

        MAP_SPI_disableModule(hwAttrs->baseAddr);
        MAP_SPI_changeMasterClock(hwAttrs->baseAddr, clockFreq, object->bitRate);
        MAP_SPI_enableModule(hwAttrs->baseAddr);

        DebugP_log3("SDSPI:(%p) CPU freq: %d; Reconfiguring SDSPI freq to %d",
            hwAttrs->baseAddr, clockFreq, object->bitRate);

        /* Initialization succeeded */
        object->diskState &= ~STA_NOINIT;
    }
    else {
        DebugP_log1("SDSPI:(%p) disk initialization failed",
            hwAttrs->baseAddr);
    }

    /* Allow performance level changes  */
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    return (object->diskState);
}

/*
 *  ======== SDSPIMSP432_diskIOctrl ========
 *  Function to perform specifed disk operations. This function is called by the
 *  FatFs module and must not be called by the application!
 *
 *  @param  drv         Drive Number
 *
 *  @param  ctrl        Control code
 *
 *  @param  buf         Buffer to send/receive control data
 */
DRESULT SDSPIMSP432_diskIOctrl(BYTE drv, BYTE ctrl, void *buf)
{
    DRESULT                      res = RES_ERROR;
    uint8_t                      n;
    uint8_t                      csd[16];
    WORD                         csize;
    SDSPIMSP432_Object          *object = sdspiHandles[drv]->object;
    SDSPIMSP432_HWAttrsV1 const *hwAttrs = sdspiHandles[drv]->hwAttrs;

    if (object->diskState & STA_NOINIT) {
        DebugP_log1("SDSPI:(%p) disk IO control: disk not initialized",
            hwAttrs->baseAddr);

        return (RES_NOTRDY);
    }

    /* Disallow performance transitions while accessing disk. */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /* Select the SD Card's chip select */
    takeSPIBus(hwAttrs);

    switch (ctrl) {
        case GET_SECTOR_COUNT:
            /* Get number of sectors on the disk (uint32_t) */
            if ((send_cmd(hwAttrs, CMD9, 0) == 0) &&
                 rcvr_datablock(hwAttrs, csd, 16)) {

                /* SDC ver 2.00 */
                if ((csd[0] >> 6) == 1) {
                    csize = csd[9] + ((WORD)csd[8] << 8) + 1;
                    *(uint32_t*)buf = (uint32_t)csize << 10;
                }
                /* MMC or SDC ver 1.XX */
                else {
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) +
                        ((csd[9] & 3) << 1) + 2;

                    csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) +
                            ((WORD)(csd[6] & 3) << 10) + 1;

                    *(uint32_t*) buf = (uint32_t)csize << (n - 9);
                }
                DebugP_log2("SDSPI:(%p) disk IO control: sector count: %d",
                    hwAttrs->baseAddr, *(uint32_t*)buf);
                res = RES_OK;
            }
            break;

        case GET_SECTOR_SIZE:
            /* Get sectors on the disk (WORD) */
            *(WORD*)buf = SD_SECTOR_SIZE;
            DebugP_log2("SDSPI:(%p) disk IO control: sector size: %d",
                hwAttrs->baseAddr, *(WORD*)buf);
            res = RES_OK;
            break;

        case CTRL_SYNC:
            /* Make sure that data has been written */
            if (wait_ready(hwAttrs) == 0xFF) {
                DebugP_log1("SDSPI:(%p) disk IO control: control sync: ready",
                    hwAttrs->baseAddr);
                res = RES_OK;
            }
            else {
                DebugP_log1("SDSPI:(%p) disk IO control: control sync: not ready",
                    hwAttrs->baseAddr);
                res = RES_NOTRDY;
            }
            break;

        default:
            DebugP_log1("SDSPI:(%p) disk IO control: parameter error",
                hwAttrs->baseAddr);
            res = RES_PARERR;
            break;
    }

    /* Deselect the SD Card's chip select */
    releaseSPIBus(hwAttrs);

    /* Idle (Release DO) */
    rxSPI(hwAttrs);

    /* Allow performance level changes  */
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    return (res);
}

/*
 *  ======== SDSPIMSP432_diskRead ========
 *  Function to perform a disk read from the SDCard. This function is called by
 *  the FatFs module and must not be called by the application!
 *
 *  @param  drv         Drive Number
 *
 *  @param  buf         Pointer to a buffer on which to store data
 *
 *  @param  sector      Starting sector number (LBA)
 *
 *  @param  count       Sector count (1...255)
 */
DRESULT SDSPIMSP432_diskRead(BYTE drv, BYTE *buf, DWORD sector, UINT count)
{
    SDSPIMSP432_Object          *object = sdspiHandles[drv]->object;
    SDSPIMSP432_HWAttrsV1 const *hwAttrs = sdspiHandles[drv]->hwAttrs;

    if (!count) {
        DebugP_log1("SDSPI:(%p) disk read: 0 sectors to read",
            hwAttrs->baseAddr);

        return (RES_PARERR);
    }

    if (object->diskState & STA_NOINIT) {
        DebugP_log1("SDSPI:(%p) Error! Disk read: disk not initialized",
            hwAttrs->baseAddr);

        return (RES_NOTRDY);
    }

    /* Disallow performance transitions while reading. */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /*
     * On a SDSC card, the sector address is a byte address on the SD Card
     * On a SDHC card, the sector address is address by sector blocks
     */
    if (object->cardType != SDSPIMSP432_SDHC) {
        /* Convert to byte address */
        sector *= SD_SECTOR_SIZE;
    }

    /* Select the SD Card's chip select */
    takeSPIBus(hwAttrs);

    /* Single block read */
    if (count == 1) {
        if ((send_cmd(hwAttrs, CMD17, sector) == 0) &&
            rcvr_datablock(hwAttrs, buf, SD_SECTOR_SIZE)) {
            count = 0;
        }
    }
    /* Multiple block read */
    else {
        if (send_cmd(hwAttrs, CMD18, sector) == 0) {
            do {
                if (!rcvr_datablock(hwAttrs, buf, SD_SECTOR_SIZE)) {
                    break;
                }
                buf += SD_SECTOR_SIZE;
            } while (--count);

            /* STOP_TRANSMISSION */
            send_cmd(hwAttrs, CMD12, 0);
        }
    }

    /* Deselect the SD Card's chip select */
    releaseSPIBus(hwAttrs);

    /* Idle (Release DO) */
    rxSPI(hwAttrs);

    /* Allow performance level changes  */
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    return (count ? RES_ERROR : RES_OK);
}

/*
 *  ======== SDSPIMSP432_diskStatus ========
 *  Function to return the current disk status. This function is called by
 *  the FatFs module and must not be called by the application!
 *
 *  @param(drv)         Drive Number
 */
DSTATUS SDSPIMSP432_diskStatus(BYTE drv)
{
    /* Get the pointer to the object */
    SDSPIMSP432_Object *object = sdspiHandles[drv]->object;

    /* Use Diags_USER1 to reduce noise in the logs */
    DebugP_log2("SDSPI:(%p) disk status: diskState: %d",
        ((SDSPIMSP432_HWAttrsV1 const *)(sdspiHandles[drv]->hwAttrs))->baseAddr,
        object->diskState);

    return (object->diskState);
}

#if _READONLY == 0
/*
 *  ======== SDSPIMSP432_diskWrite ========
 *  Function to perform a disk write from the SDCard. This function is called by
 *  the FatFs module and must not be called by the application!
 *
 *  @param  drv         Drive Number
 *
 *  @param  buf         Pointer to a buffer from which to read data
 *
 *  @param  sector      Starting sector number (LBA)
 *
 *  @param  count       Sector count (1...255)
 */
DRESULT SDSPIMSP432_diskWrite(BYTE drv, const BYTE *buf, DWORD sector,
    UINT count)
{
    SDSPIMSP432_Object          *object = sdspiHandles[drv]->object;
    SDSPIMSP432_HWAttrsV1 const *hwAttrs = sdspiHandles[drv]->hwAttrs;

    if (!count) {
        DebugP_log1("SDSPI:(%p) disk write: 0 sectors to write",
            hwAttrs->baseAddr);

        return (RES_PARERR);
    }
    if (object->diskState & STA_NOINIT) {
        DebugP_log1("SDSPI:(%p) Error! Disk write: disk not initialized",
            hwAttrs->baseAddr);

        return (RES_NOTRDY);
    }
    if (object->diskState & STA_PROTECT) {
        DebugP_log1("SDSPI:(%p) Error! Disk write: disk protected",
            hwAttrs->baseAddr);

        return (RES_WRPRT);
    }

    /* Disallow performance transitions while writing. */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /*
     * On a SDSC card, the sector address is a byte address on the SD Card
     * On a SDHC card, the sector address is address by sector blocks
     */
    if (object->cardType != SDSPIMSP432_SDHC) {
        /* Convert to byte address if needed */
        sector *= SD_SECTOR_SIZE;
    }

    /* Select the SD Card's chip select */
    takeSPIBus(hwAttrs);

    /* Single block write */
    if (count == 1) {
        if ((send_cmd(hwAttrs, CMD24, sector) == 0) &&
             xmit_datablock(hwAttrs, buf, START_BLOCK_TOKEN)) {
            count = 0;
        }
    }
    /* Multiple block write */
    else {
        if ((object->cardType == SDSPIMSP432_SDSC) ||
            (object->cardType == SDSPIMSP432_SDHC)) {
            send_cmd(hwAttrs, CMD55, 0);
            send_cmd(hwAttrs, CMD23, count);    /* ACMD23 */
        }
        /* WRITE_MULTIPLE_BLOCK */
        if (send_cmd(hwAttrs, CMD25, sector) == 0) {
            do {
                if (!xmit_datablock(hwAttrs, buf, START_MULTIBLOCK_TOKEN)) {
                    break;
                }
                buf += SD_SECTOR_SIZE;
            } while (--count);

            /* STOP_TRAN token */
            if (!xmit_datablock(hwAttrs, 0, STOP_MULTIBLOCK_TOKEN)) {
                count = 1;
            }
        }
    }

    /* Deselect the SD Card's chip select */
    releaseSPIBus(hwAttrs);

    /* Idle (Release DO) */
    rxSPI(hwAttrs);

    /* Allow performance level changes  */
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    return (count ? RES_ERROR : RES_OK);
}
#endif /* _READONLY */

/*
 *  ======== SDSPIMSP432_init ========
 *  Function to initialize SDSPI module
 */
void SDSPIMSP432_init(SDSPI_Handle handle)
{
    SDSPIMSP432_Object *object = handle->object;

    /* Mark the object as available */
    object->driveNumber = DRIVE_NOT_MOUNTED;
    object->diskState = STA_NOINIT;
    object->cardType = SDSPIMSP432_NOCARD;
}

/*
 *  ======== SDSPIMSP432_open ========
 *  Function to mount the FatFs filesystem and register the SDSPIMSP432 disk
 *  I/O functions with SYS/BIOS' FatFS module.
 *
 *  This function also configures some basic GPIO settings needed for the
 *  software chip select with the SDCard.
 *
 *  @param  handle      SDSPI handle
 *  @param  drv         Drive Number
 *  @param  params      SDSPI parameters
 */
SDSPI_Handle SDSPIMSP432_open(SDSPI_Handle handle, uint_least8_t drv,
    SDSPI_Params  *params)
{
    unsigned int                 i;
    uintptr_t                    key;
    DRESULT                      dresult;
    FRESULT                      fresult;
    uint32_t                     clockFreq;
    uint8_t                      numPerfLevels;
    PowerMSP432_Freqs            powerFreqs;
    SDSPIMSP432_Object          *object = handle->object;
    SDSPIMSP432_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    TCHAR                        path[3];
    uint8_t                      value;
    uint8_t                      mode;

    /* Determine if the device was already opened */
    key = HwiP_disable();

    if (object->driveNumber != DRIVE_NOT_MOUNTED) {
        HwiP_restore(key);
        return (NULL);
    }
    /* Mark as being used */
    object->driveNumber = drv;

    HwiP_restore(key);

    /* Ensure a supported clock source is used */
    if (hwAttrs->clockSource != EUSCI_B_SPI_CLOCKSOURCE_ACLK &&
        hwAttrs->clockSource != EUSCI_B_SPI_CLOCKSOURCE_SMCLK) {
        DebugP_log1("SDSPI:(%p) Error! Using unsupported clock source.",
            hwAttrs->baseAddr);
        object->driveNumber = DRIVE_NOT_MOUNTED;
        return (NULL);
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
            DebugP_log3("SDSPI:(%p) ACLK input (%d) too low for bit rate (%d)",
                hwAttrs->baseAddr, powerFreqs.ACLK, params->bitRate);
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
            object->driveNumber = DRIVE_NOT_MOUNTED;
            return (NULL);
        }
        clockFreq = powerFreqs.ACLK;
    }
    else {    /* hwAttrs->clockSource == EUSCI_B_SPI_CLOCKSOURCE_SMCLK */
        if (params->bitRate >= powerFreqs.SMCLK) {
            DebugP_log3("SDSPI:(%p) SMCLK input (%d) too low for bit rate (%d)",
                hwAttrs->baseAddr, powerFreqs.SMCLK, params->bitRate);
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
            object->driveNumber = DRIVE_NOT_MOUNTED;
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
                Power_setConstraint((PowerMSP432_DISALLOW_PERFLEVEL_0 + i));
            }
        }
    }

    /* Register function to reconfigure peripheral on perf level changes */
    Power_registerNotify(&object->perfChangeNotify,
        PowerMSP432_START_CHANGE_PERF_LEVEL | PowerMSP432_DONE_CHANGE_PERF_LEVEL,
        perfChangeNotifyFxn, (uintptr_t) handle);

    /* Determine time scaling for timeouts */
    uSClockPeriod = ClockP_getSystemTickPeriod();
    DebugP_assert(uSClockPeriod != 0);

    /* Configure pins used for SPI ( CLK | RX | TX ) */

    /* MISO pin */
    mode = PinConfigModuleFunction(hwAttrs->somiPin);
    value = PinConfigValue(hwAttrs->somiPin);

    if (value != 0) {
        mode = GPIO_PRIMARY_MODULE_FUNCTION;
        mapPin(PinConfigPort(hwAttrs->somiPin),
                (hwAttrs->somiPin) & 0x7 /* pin */, value);
    }

    MAP_GPIO_setAsInputPinWithPullUpResistor(PinConfigPort(hwAttrs->somiPin),
            PinConfigPin(hwAttrs->somiPin));
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
        PinConfigPort(hwAttrs->somiPin), PinConfigPin(hwAttrs->somiPin),
        mode);

    /* SCK pin */
    mode = PinConfigModuleFunction(hwAttrs->sckPin);
    value = PinConfigValue(hwAttrs->sckPin);

    if (value != 0) {
        mode = GPIO_PRIMARY_MODULE_FUNCTION;
        mapPin(PinConfigPort(hwAttrs->sckPin),
                (hwAttrs->sckPin) & 0x7 /* pin */, value);
    }
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(
        PinConfigPort(hwAttrs->sckPin), PinConfigPin(hwAttrs->sckPin),
        mode);

    /* MOSI pin */
    mode = PinConfigModuleFunction(hwAttrs->simoPin);
    value = PinConfigValue(hwAttrs->simoPin);

    if (value != 0) {
        mode = GPIO_PRIMARY_MODULE_FUNCTION;
        mapPin(PinConfigPort(hwAttrs->simoPin),
                (hwAttrs->simoPin) & 0x7 /* pin */, value);
    }
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(
        PinConfigPort(hwAttrs->simoPin), PinConfigPin(hwAttrs->simoPin),
        mode);

    /* Pin used for Chip Select */
    MAP_GPIO_setAsOutputPin(PinConfigPort(hwAttrs->csPin),
            PinConfigPin(hwAttrs->csPin));

    /* Raise the chip select pin */
    MAP_GPIO_setOutputHighOnPin(PinConfigPort(hwAttrs->csPin),
            PinConfigPin(hwAttrs->csPin));

    /*
     * Store SPI parameters & initialize peripheral.  These are used to
     * re/initialize the peripheral when opened or changing performance level.
     */
    object->bitRate = params->bitRate;
    object->diskState = STA_NOINIT;
    initHw(object, hwAttrs, clockFreq);

    /* Register the new disk_*() functions */
    dresult = disk_register(object->driveNumber, SDSPIMSP432_diskInitialize,
        SDSPIMSP432_diskStatus, SDSPIMSP432_diskRead, SDSPIMSP432_diskWrite,
        SDSPIMSP432_diskIOctrl);
    if (dresult != RES_OK) {
        DebugP_log1("SDSPI:(%p) Error! Disk functions not registered",
            hwAttrs->baseAddr);
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
        SDSPIMSP432_close(handle);
        return (NULL);
    }

    /*
     * Register the filesystem with FatFs. This operation does not access the
     * SDCard yet.
     */
    path[0] = '0' + object->driveNumber;
    path[1] = ':';
    path[2] = '\0';

    fresult = f_mount(&(object->filesystem), path, 0);
    if (fresult != FR_OK) {
        DebugP_log2("SDSPI:(%p) Error! Drive %d not mounted",
            hwAttrs->baseAddr, object->driveNumber);
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
        SDSPIMSP432_close(handle);
        return (NULL);
    }

    /* Store the new SDSPI handle for this FatFs drive number */
    sdspiHandles[drv] = handle;

    /* Allow performance level changes  */
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    DebugP_log1("SDSPI:(%p) opened", hwAttrs->baseAddr);

    return (handle);
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

    //portMapReconfigure = PMAP_DISABLE_RECONFIGURATION; // ?

    /*  Code from pmap.c: */
    //Get write-access to port mapping registers:
    PMAP->KEYID = PMAP_KEYID_VAL;

    //Enable/Disable reconfiguration during runtime
//    PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | PMAP_DISABLE_RECONFIGURATION;
    PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | PMAP_ENABLE_RECONFIGURATION;
    HWREG8((uint32_t)PMAP_BASE + pin + pmap) = value;
    //Disable write-access to port mapping registers:

    PMAP->KEYID = 0;
}
