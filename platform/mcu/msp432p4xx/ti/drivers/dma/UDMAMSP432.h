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
/** ============================================================================
 *  @file       UDMAMSP432.h
 *
 *  @brief      uDMA driver implementation for MSP432.
 *
 *  This driver is intended for use only by TI-RTOS drivers that use the uDMA
 *  peripheral (e.g., SPI).  This driver is mainly used for management of
 *  the control table base address of the UDMA peripheral, and to create
 *  the DMA error Hwi.
 *
 *  The application should only define the memory for the control table and
 *  set up the UDMAMSP432_HWAttrs and UDMAMSP432_Config structures.
 *
 *  The UDMAMSP432 header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/dma/UDMAMSP432.h>
 *  @endcode
 *
 *  ============================================================================
 */

#ifndef ti_drivers_dma_UDMAMSP432__include
#define ti_drivers_dma_UDMAMSP432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/dpl/HwiP.h>

/*!
 *  @brief      UDMA error function pointer
 */
typedef void (*UDMAMSP432_ErrorFxn)(uintptr_t arg);

/*!
 *  @brief      UDMAMSP432 Hardware attributes
 *
 *  This structure contains the base address of the uDMA control
 *  table, and uDMA error interrupt attributes.
 *
 *  The control table is used by the uDMA controller to store channel
 *  control structures.  The control table can be located anywhere in
 *  system memory, but must be contiguous and aligned on a 1024-byte boundary.
 *
 *  dmaErrorFxn is the uDMA peripheral's error interrupt handler.
 *
 *  intPriority is priority of the uDMA peripheral's error interrupt, as
 *  defined by the underlying OS.  It is passed unmodified to the
 *  underlying OS's interrupt handler creation code, so you need to
 *  refer to the OS documentation for usage.  If the
 *  driver uses the ti.dpl interface instead of making OS
 *  calls directly, then the HwiP port handles the interrupt priority
 *  in an OS specific way.  In the case of the SYS/BIOS port,
 *  intPriority is passed unmodified to Hwi_create().
 *
 *  A sample structure is shown below:
 *  @code
 *
 *  #include <ti/devices/msp432p4xx/driverlib/udma.h>
 *
 *  #if defined(__TI_COMPILER_VERSION__)
 *  #pragma DATA_ALIGN(dmaControlTable, 1024)
 *  #elif defined(__IAR_SYSTEMS_ICC__)
 *  #pragma data_alignment=1024
 *  #elif defined(__GNUC__)
 *  __attribute__ ((aligned (1024)))
 *  #endif
 *
 *  static tDMAControlTable dmaControlTable[64];
 *
 *  #include <ti/drivers/dma/UDMAMSP432.h>
 *
 *  UDMAMSP432_Object udmaMSP432Object;
 *
 *  const UDMAMSP432_HWAttrs udmaMSP432HWAttrs = {
 *          .controlBaseAddr = (void *)dmaControlTable,
 *          .dmaErrorFxn = UDMAMSP432_errorFxn,
 *          .intNum = INT_UDMAERR,
 *          .intPriority = (~0)
 *  };
 *  @endcode
 *
 */
typedef struct UDMAMSP432_HWAttrs {
    void           *controlBaseAddr; /*!< uDMA control registers base address */
    UDMAMSP432_ErrorFxn dmaErrorFxn; /*!< uDMA error interrupt handler */
    uint8_t         intNum;          /*!< uDMA error interrupt number */
    uint8_t         intPriority;     /*!< uDMA error interrupt priority. */
} UDMAMSP432_HWAttrs;

/*!
 *  @brief      UDMAMSP432 Global configuration
 *
 *  The UDMAMSP432_Config structure contains pointers used by the UDMAMSP432
 *  driver.
 *
 *  This structure needs to be defined before calling UDMAMSP432_init() and
 *  it must not be changed thereafter.
 */
typedef struct UDMAMSP432_Config {
    void              *object;            /*!< Pointer to UDMAMSP432 object */
    void const        *hwAttrs;           /*!< Pointer to hardware attributes */
} UDMAMSP432_Config;

/*!
 *  @brief      A handle that is returned from a UDMAMSP432_open() call.
 */
typedef struct UDMAMSP432_Config      *UDMAMSP432_Handle;

/*!
 *  @brief  UDMAMSP432 object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct UDMAMSP432_Object {
    bool unused;
} UDMAMSP432_Object;

/*!
 *  @brief  UDMAMSP432 Transfer configuration
 *
 *  The UDMAMSP432_Transfer structure contains parameters for initializing a
 *  DMA transfer using a given DMA channel number.
 *
 *  This struct is used in UDMAMSP432_setupTransfer().
 */
typedef struct UDMAMSP432_Transfer {
    uint32_t         dmaChannel;                 /*!< DMA channel */
    uint32_t         structSelect;               /*!< */
    uint32_t         ctlOptions;                 /*!< DMA control options */
    uint32_t         transferMode;               /*!< DMA transfer mode. Basic or PingPong */
    void             *dmaTransferSource;         /*!< Source of DMA transfer */
    void             *dmaTransferDestination;    /*!< Destination of DMA transfer */
    uint32_t         transferSize;               /*!< Size of DMA transfer */
} UDMAMSP432_Transfer;

/*!
 *  @brief  UDMAMSP432 Ping Pong Transfer configuration
 *
 *  The UDMAMSP432_Transfer structure contains parameters for initializing a
 *  DMA ping pong transfer using a given DMA channel number.
 *
 *  This struct is used in UDMAMSP432_setupPingPongTransfer() and
 *  UDMAMSP432_PingPongToggleBuffer().
 */
typedef struct UDMAMSP432_PingPongTransfer {
    uint32_t         dmaChannel;                  /*!< DMA channel */
    uint32_t         ctlOptions;                  /*!< DMA control options */
    uint32_t         transferMode;                /*!< DMA transfer mode. Basic or PingPong */
    void             *dmaTransferSource;          /*!< Source of DMA transfer */
    void             *dmaPrimaryDestination;      /*!< Primary Destination of DMA transfer */
    void             *dmaAlternateDestination;    /*!< Alternate Destination of DMA transfer */
    uint32_t         transferSize;                /*!< Size of DMA transfer */
} UDMAMSP432_PingPongTransfer;

/*!
 *  @brief  Function to close the DMA driver
 *
 *  Close a DMA handle returned from UDMAMSP432_open().
 *
 *  @pre    UDMAMSP432_open() has to be called first.
 *          Calling context: Task
 *
 *  @param  handle  A UDMAMSP432_Handle returned from UDMAMSP432_open()
 *  @param  channelNum  A DMA channel defined in dma.h (e.g. DMA_CH0_EUSCIB0TX0)
 *  @param  intNum   DMA_INT0 - DMA_INT3 associated with the channelNum
 *
 *  @return none
 *
 *  @sa     UDMAMSP432_open
 */
extern void UDMAMSP432_close(UDMAMSP432_Handle handle, uint32_t channelNum, uint8_t intNum);
/*!
 *  @brief  Function to initialize the MSP432 DMA driver
 *
 *  The function will set the isOpen flag to false, and should be called prior
 *  to opening the DMA driver.
 *
 *  @return none
 *
 *  @sa     UDMAMSP432_open()
 */
extern void UDMAMSP432_init();

/*!
 *  @brief  Function to initialize the MSP432 DMA peripheral and corresponding interrupt
 *
 *  UDMAMSP432_open() opens the DMA peripheral. It calls UDMAMSP432_setupInterrupt()
 *  This function can be called multiple times.
 *  @pre    UDMAMSP432_init() has to be called first.
 *          Calling context: Task
 *
 *  @param  channelNum  A DMA channel defined in dma.h (e.g. DMA_CH0_EUSCIB0TX0)
 *  @param  intNum      The DMA interrupt to be used for the module (DMA_INT0 - DMA_INT3)
 *  @param  priority    The interrupt priority used in the module
 *  @param  hwiFxn      A pointer to the desired ISR to call
 *  @param  arg         A pointer to a struct of args for the ISR
 *
 *  @return UDMAMSP432_Handle on success or NULL if an error has occurred.
 *
 *  @sa     UDMAMSP432_close()
 */
extern UDMAMSP432_Handle UDMAMSP432_open(uint32_t channelNum, uint8_t intNum, uint32_t priority,
            void (*hwiFxn)(uintptr_t), uintptr_t arg);

/*!
 *  @brief  Function to set up a DMA channel for data transfer
 *
 *  The function will set the channel control, set the channel transfer,
 *  assign the channel, and enable the channel.
 *
 *  @pre    UDMAMSP432_open() has to be called first.
 *
 *  @param  transfer  Pointer to a UDMAMSP432_Transfer param struct
 *
 *  @return true on success or false if an error has occurred
 */
extern bool UDMAMSP432_setupTransfer(UDMAMSP432_Transfer *transfer);

/*!
 *  @brief  Function to set up a DMA channel for ping pong data transfer
 *
 *  The function will set the channel control, set the channel transfer,
 *  assign the channel, and enable the channel. It will also set up the
 *  alternate data structure for use with ping pong mode. For continuous
 *  transfering, set transfer.transferMode = UDMA_MODE_PING_PONG. For
 *  one-shot across two buffers, set transfer.transferMode = UDMA_MODE_BASIC.
 *
 *  @pre    UDMAMSP432_open() has to be called first.
 *
 *  @param  transfer  Pointer to a UDMAMSP432_PingPongTransfer param struct
 *
 *  @return true on success or false if an error has occurred
 */
extern bool UDMAMSP432_setupPingPongTransfer(UDMAMSP432_PingPongTransfer *transfer);

/*!
 *  @brief  Function to toggle between two buffers during ping pong mode
 *
 *  The function will alternate setting the destination buffer between transfer's
 *  dmaTransferDestinationOne and dmaTransferDestinationTwo parameters. This should be called in the ISR that the peripheral
 *  using DMA is using.
 *
 *  @pre    UDMAMSP432_setupPingPongTransfer() has to be called first.
 *
 *  @param  transfer  Pointer to a UDMAMSP432_PingPongTransfer param struct
 */
extern void UDMAMSP432_PingPongToggleBuffer(UDMAMSP432_PingPongTransfer *transfer);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_dma_UDMAMSP432__include */
