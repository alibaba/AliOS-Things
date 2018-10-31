/*******************************************************************************
  SPI Driver Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
   drv_spi_api.i

  Summary:
 SPI Driver implementation for functions that the client API uses that change
 depending on what compile time options there are.

  Description:
    The SPI Driver provides a interface to access the SPI hardware on the PIC32
    microcontroller.  This file implements the SPI Driver. This file
    should be included in the project if SPI driver functionality is needed.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#include "driver/spi/src/dynamic/drv_spi_internal.h"

#define _SPI_DRV_VTABLE_POLLED      0x0000
#define _SPI_DRV_VTABLE_ISR         0x0001

#define _SPI_DRV_VTABLE_SLAVE       0x0000
#define _SPI_DRV_VTABLE_MASTER      0x0002

#define _SPI_DRV_VTABLE_RM          0x0000
#define _SPI_DRV_VTABLE_EBM         0x0004

#define _SPI_DRV_VTABLE_8BIT        0x0008
#define _SPI_DRV_VTABLE_16BIT       0x0010
#define _SPI_DRV_VTABLE_32BIT       0x0020

#define _SPI_DRV_VTABLE_P_S_R_8 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_8BIT
#define _SPI_DRV_VTABLE_P_S_R_16 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_16BIT
#define _SPI_DRV_VTABLE_P_S_R_32 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_32BIT

#define _SPI_DRV_VTABLE_P_S_E_8 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_8BIT
#define _SPI_DRV_VTABLE_P_S_E_16 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_16BIT
#define _SPI_DRV_VTABLE_P_S_E_32 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_32BIT

#define _SPI_DRV_VTABLE_I_S_R_8 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_8BIT
#define _SPI_DRV_VTABLE_I_S_R_16 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_16BIT
#define _SPI_DRV_VTABLE_I_S_R_32 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_32BIT

#define _SPI_DRV_VTABLE_I_S_E_8 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_8BIT
#define _SPI_DRV_VTABLE_I_S_E_16 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_16BIT
#define _SPI_DRV_VTABLE_I_S_E_32 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_SLAVE | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_32BIT

#define _SPI_DRV_VTABLE_P_M_R_8 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_8BIT
#define _SPI_DRV_VTABLE_P_M_R_16 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_16BIT
#define _SPI_DRV_VTABLE_P_M_R_32 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_32BIT

#define _SPI_DRV_VTABLE_P_M_E_8 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_8BIT
#define _SPI_DRV_VTABLE_P_M_E_16 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_16BIT
#define _SPI_DRV_VTABLE_P_M_E_32 _SPI_DRV_VTABLE_POLLED | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_32BIT

#define _SPI_DRV_VTABLE_I_M_R_8 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_8BIT
#define _SPI_DRV_VTABLE_I_M_R_16 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_16BIT
#define _SPI_DRV_VTABLE_I_M_R_32 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_RM | _SPI_DRV_VTABLE_32BIT

#define _SPI_DRV_VTABLE_I_M_E_8 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_8BIT
#define _SPI_DRV_VTABLE_I_M_E_16 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_16BIT
#define _SPI_DRV_VTABLE_I_M_E_32 _SPI_DRV_VTABLE_ISR | _SPI_DRV_VTABLE_MASTER | _SPI_DRV_VTABLE_EBM | _SPI_DRV_VTABLE_32BIT

int32_t DRV_SPI_SetVTable(struct DRV_SPI_DRIVER_OBJECT * driverObject, const DRV_SPI_INIT * const pInit)
{
    uint8_t mode = 0;
#if DRV_SPI_MASTER
    if (pInit->spiMode == DRV_SPI_MODE_MASTER)
    {
        mode = _SPI_DRV_VTABLE_MASTER;
    }
    else
#endif
#if DRV_SPI_SLAVE
    if (pInit->spiMode == DRV_SPI_MODE_SLAVE)
    {
        mode = _SPI_DRV_VTABLE_SLAVE;
    }
    else
#endif
    {
        SYS_ASSERT(false, "Invalid SPI Configuration");
        return -1;
    }

#if DRV_SPI_POLLED
    if (pInit->taskMode == DRV_SPI_TASK_MODE_POLLED)
    {
        mode |= DRV_SPI_TASK_MODE_POLLED;
    }
    else
#endif
#if DRV_SPI_ISR
    if (pInit->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        mode |= DRV_SPI_TASK_MODE_ISR;
    }
    else
#endif
    {
        SYS_ASSERT(false, "Invalid SPI Configuration");
        return -1;
    }

#if DRV_SPI_RM
    if (pInit->bufferType == DRV_SPI_BUFFER_TYPE_STANDARD)
    {
        mode |= _SPI_DRV_VTABLE_RM;
    }
    else
#endif
#if DRV_SPI_EBM
    if (pInit->bufferType == DRV_SPI_BUFFER_TYPE_ENHANCED)
    {
        mode |= _SPI_DRV_VTABLE_EBM;
    }
    else
#endif
    {
        SYS_ASSERT(false, "Invalid SPI Configuration");
        return -1;
    }

#if DRV_SPI_8BIT
    if (pInit->commWidth == SPI_COMMUNICATION_WIDTH_8BITS)
    {
        mode |= _SPI_DRV_VTABLE_8BIT;
    }
    else
#endif
#if DRV_SPI_16BIT
    if (pInit->commWidth == SPI_COMMUNICATION_WIDTH_16BITS)
    {
        mode |= _SPI_DRV_VTABLE_16BIT;
    }
    else
#endif
#if DRV_SPI_32BIT
    if (pInit->commWidth == SPI_COMMUNICATION_WIDTH_32BITS)
    {
        mode |= _SPI_DRV_VTABLE_32BIT;
    }
    else
#endif
    {
        SYS_ASSERT(false, "Invalid SPI Configuration");
        return -1;
    }

    switch (mode)
    {
#if DRV_SPI_POLLED && DRV_SPI_SLAVE && DRV_SPI_RM && DRV_SPI_8BIT
    case _SPI_DRV_VTABLE_P_S_R_8:
        driverObject->vfMainTask = DRV_SPI_PolledSlaveRM8BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMASlaveSendEventHandler8bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMASlaveReceiveEventHandler8bit;
#endif
        break;
#endif
#if DRV_SPI_POLLED && DRV_SPI_SLAVE && DRV_SPI_RM && DRV_SPI_16BIT
    case _SPI_DRV_VTABLE_P_S_R_16:
        driverObject->vfMainTask = DRV_SPI_PolledSlaveRM16BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMASlaveSendEventHandler16bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMASlaveReceiveEventHandler16bit;
#endif
        break;
#endif
#if DRV_SPI_POLLED && DRV_SPI_SLAVE && DRV_SPI_RM && DRV_SPI_32BIT
    case _SPI_DRV_VTABLE_P_S_R_32:
        driverObject->vfMainTask = DRV_SPI_PolledSlaveRM32BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMASlaveSendEventHandler32bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMASlaveReceiveEventHandler32bit;
#endif
        break;
#endif

#if DRV_SPI_POLLED && DRV_SPI_SLAVE && DRV_SPI_EBM && DRV_SPI_8BIT
    case _SPI_DRV_VTABLE_P_S_E_8:
        driverObject->vfMainTask = DRV_SPI_PolledSlaveEBM8BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMASlaveSendEventHandler8bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMASlaveReceiveEventHandler8bit;
#endif
        break;
#endif
#if DRV_SPI_POLLED && DRV_SPI_SLAVE && DRV_SPI_EBM && DRV_SPI_16BIT
    case _SPI_DRV_VTABLE_P_S_E_16:
        driverObject->vfMainTask = DRV_SPI_PolledSlaveEBM16BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMASlaveSendEventHandler16bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMASlaveReceiveEventHandler16bit;
#endif
        break;
#endif
#if DRV_SPI_POLLED && DRV_SPI_SLAVE && DRV_SPI_EBM && DRV_SPI_32BIT
    case _SPI_DRV_VTABLE_P_S_E_32:
        driverObject->vfMainTask = DRV_SPI_PolledSlaveEBM32BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMASlaveSendEventHandler32bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMASlaveReceiveEventHandler32bit;
#endif
        break;
#endif

#if DRV_SPI_ISR && DRV_SPI_SLAVE && DRV_SPI_RM && DRV_SPI_8BIT
    case _SPI_DRV_VTABLE_I_S_R_8:
        driverObject->vfMainTask = DRV_SPI_ISRSlaveRM8BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMASlaveSendEventHandler8bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMASlaveReceiveEventHandler8bit;
#endif
        break;
#endif
#if DRV_SPI_ISR && DRV_SPI_SLAVE && DRV_SPI_RM && DRV_SPI_16BIT
    case _SPI_DRV_VTABLE_I_S_R_16:
        driverObject->vfMainTask = DRV_SPI_ISRSlaveRM16BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMASlaveSendEventHandler16bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMASlaveReceiveEventHandler16bit;
#endif
        break;
#endif
#if DRV_SPI_ISR && DRV_SPI_SLAVE && DRV_SPI_RM && DRV_SPI_32BIT
    case _SPI_DRV_VTABLE_I_S_R_32:
        driverObject->vfMainTask = DRV_SPI_ISRSlaveRM32BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMASlaveSendEventHandler32bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMASlaveReceiveEventHandler32bit;
#endif
        break;
#endif

#if DRV_SPI_ISR && DRV_SPI_SLAVE && DRV_SPI_EBM && DRV_SPI_8BIT
    case _SPI_DRV_VTABLE_I_S_E_8:
        driverObject->vfMainTask = DRV_SPI_ISRSlaveEBM8BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMASlaveSendEventHandler8bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMASlaveReceiveEventHandler8bit;
#endif
        break;
#endif
#if DRV_SPI_ISR && DRV_SPI_SLAVE && DRV_SPI_EBM && DRV_SPI_16BIT
    case _SPI_DRV_VTABLE_I_S_E_16:
        driverObject->vfMainTask = DRV_SPI_ISRSlaveEBM16BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMASlaveSendEventHandler16bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMASlaveReceiveEventHandler16bit;
#endif
        break;
#endif
#if DRV_SPI_ISR && DRV_SPI_SLAVE && DRV_SPI_EBM && DRV_SPI_32BIT
    case _SPI_DRV_VTABLE_I_S_E_32:
        driverObject->vfMainTask = DRV_SPI_ISRSlaveEBM32BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMASlaveSendEventHandler32bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMASlaveReceiveEventHandler32bit;
#endif
        break;
#endif

#if DRV_SPI_POLLED && DRV_SPI_MASTER && DRV_SPI_RM && DRV_SPI_8BIT
    case _SPI_DRV_VTABLE_P_M_R_8:
        driverObject->vfMainTask = DRV_SPI_PolledMasterRM8BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMAMasterSendEventHandler8bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMAMasterReceiveEventHandler8bit;
#endif
        break;
#endif
#if DRV_SPI_POLLED && DRV_SPI_MASTER && DRV_SPI_RM && DRV_SPI_16BIT
    case _SPI_DRV_VTABLE_P_M_R_16:
        driverObject->vfMainTask = DRV_SPI_PolledMasterRM16BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMAMasterSendEventHandler16bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMAMasterReceiveEventHandler16bit;
#endif
        break;
#endif
#if DRV_SPI_POLLED && DRV_SPI_MASTER && DRV_SPI_RM && DRV_SPI_32BIT
    case _SPI_DRV_VTABLE_P_M_R_32:
        driverObject->vfMainTask = DRV_SPI_PolledMasterRM32BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMAMasterSendEventHandler32bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMAMasterReceiveEventHandler32bit;
#endif
        break;
#endif


#if DRV_SPI_POLLED && DRV_SPI_MASTER && DRV_SPI_EBM && DRV_SPI_8BIT
    case _SPI_DRV_VTABLE_P_M_E_8:
        driverObject->vfMainTask = DRV_SPI_PolledMasterEBM8BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMAMasterSendEventHandler8bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMAMasterReceiveEventHandler8bit;
#endif
        break;
#endif
#if DRV_SPI_POLLED && DRV_SPI_MASTER && DRV_SPI_EBM && DRV_SPI_16BIT
    case _SPI_DRV_VTABLE_P_M_E_16:
        driverObject->vfMainTask = DRV_SPI_PolledMasterEBM16BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMAMasterSendEventHandler16bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMAMasterReceiveEventHandler16bit;
#endif
        break;
#endif
#if DRV_SPI_POLLED && DRV_SPI_MASTER && DRV_SPI_EBM && DRV_SPI_32BIT
    case _SPI_DRV_VTABLE_P_M_E_32:
        driverObject->vfMainTask = DRV_SPI_PolledMasterEBM32BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_PolledDMAMasterSendEventHandler32bit;
        driverObject->receiveDMAHander = DRV_SPI_PolledDMAMasterReceiveEventHandler32bit;
#endif
        break;
#endif

#if DRV_SPI_ISR && DRV_SPI_MASTER && DRV_SPI_RM && DRV_SPI_8BIT
    case _SPI_DRV_VTABLE_I_M_R_8:
        driverObject->vfMainTask = DRV_SPI_ISRMasterRM8BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMAMasterSendEventHandler8bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMAMasterReceiveEventHandler8bit;
#endif
        break;
#endif
#if DRV_SPI_ISR && DRV_SPI_MASTER && DRV_SPI_RM && DRV_SPI_16BIT
    case _SPI_DRV_VTABLE_I_M_R_16:
        driverObject->vfMainTask = DRV_SPI_ISRMasterRM16BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMAMasterSendEventHandler16bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMAMasterReceiveEventHandler16bit;
#endif
        break;
#endif
#if DRV_SPI_ISR && DRV_SPI_MASTER && DRV_SPI_RM && DRV_SPI_32BIT
    case _SPI_DRV_VTABLE_I_M_R_32:
        driverObject->vfMainTask = DRV_SPI_ISRMasterRM32BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMAMasterSendEventHandler32bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMAMasterReceiveEventHandler32bit;
#endif
        break;
#endif

#if DRV_SPI_ISR && DRV_SPI_MASTER && DRV_SPI_EBM && DRV_SPI_8BIT
    case _SPI_DRV_VTABLE_I_M_E_8:
        driverObject->vfMainTask = DRV_SPI_ISRMasterEBM8BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMAMasterSendEventHandler8bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMAMasterReceiveEventHandler8bit;
#endif
        break;
#endif
#if DRV_SPI_ISR && DRV_SPI_MASTER && DRV_SPI_EBM && DRV_SPI_16BIT
    case _SPI_DRV_VTABLE_I_M_E_16:
        driverObject->vfMainTask = DRV_SPI_ISRMasterEBM16BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMAMasterSendEventHandler16bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMAMasterReceiveEventHandler16bit;
#endif
        break;
#endif
#if DRV_SPI_ISR && DRV_SPI_MASTER && DRV_SPI_EBM && DRV_SPI_32BIT
    case _SPI_DRV_VTABLE_I_M_E_32:
        driverObject->vfMainTask = DRV_SPI_ISRMasterEBM32BitTasks;
#if DRV_SPI_DMA
        driverObject->sendDMAHander = DRV_SPI_ISRDMAMasterSendEventHandler32bit;
        driverObject->receiveDMAHander = DRV_SPI_ISRDMAMasterReceiveEventHandler32bit;
#endif
        break;
#endif
    default:
        SYS_ASSERT(false, "Invalid SPI Configuration");
        return -1;

    }
    return 0;
}

int32_t DRV_SPI_SetupHardware(struct DRV_SPI_DRIVER_OBJECT * driverObject, const DRV_SPI_INIT * const init)
{
    const register SPI_MODULE_ID spiId = init->spiId;

/* disable the SPI*/
    PLIB_SPI_Disable(spiId);

    /* Set up Master or Slave Mode*/
#if DRV_SPI_MASTER_ONLY
    PLIB_SPI_MasterEnable ( spiId  );
#elif DRV_SPI_SLAVE_ONLY
    PLIB_SPI_SlaveEnable ( spiId  );
#else
    if (init->spiMode == DRV_SPI_MODE_MASTER)
    {
        PLIB_SPI_MasterEnable ( spiId  );
    }
    else
    if (init->spiMode == DRV_SPI_MODE_SLAVE)
    {
        PLIB_SPI_SlaveEnable ( spiId  );

    }
    else
    {
        SYS_ASSERT(false, "Invalid SPI Configuration");
        return -1;
    }
#endif

    /* Set up if the SPI is allowed to run while the rest of the CPU is in idle mode*/
    if (init->allowIdleRun)
    {
        PLIB_SPI_StopInIdleDisable( spiId  );
    }
    else
    {
        PLIB_SPI_StopInIdleEnable( spiId  );
    }

    /* Set up close Polarity and output data phase*/
    switch(init->clockMode)
    {
        case DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_RISE:
            PLIB_SPI_ClockPolaritySelect( spiId, SPI_CLOCK_POLARITY_IDLE_LOW );
            PLIB_SPI_OutputDataPhaseSelect( spiId, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK );
            break;
        case DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_FALL:
            PLIB_SPI_ClockPolaritySelect( spiId, SPI_CLOCK_POLARITY_IDLE_LOW );
            PLIB_SPI_OutputDataPhaseSelect( spiId, SPI_OUTPUT_DATA_PHASE_ON_ACTIVE_TO_IDLE_CLOCK );
            break;
        case DRV_SPI_CLOCK_MODE_IDLE_HIGH_EDGE_FALL: //TODO: Make sure these are right
            PLIB_SPI_ClockPolaritySelect( spiId, SPI_CLOCK_POLARITY_IDLE_HIGH );
            PLIB_SPI_OutputDataPhaseSelect( spiId, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK );
            break;
        case DRV_SPI_CLOCK_MODE_IDLE_HIGH_EDGE_RISE:
            PLIB_SPI_ClockPolaritySelect( spiId, SPI_CLOCK_POLARITY_IDLE_HIGH );
            PLIB_SPI_OutputDataPhaseSelect( spiId, SPI_OUTPUT_DATA_PHASE_ON_ACTIVE_TO_IDLE_CLOCK );
            break;
        default:
             SYS_ASSERT(false, "Invalid SPI Configuration");
            return -1;
    }

    // Set up the  Input Sample Phase
    PLIB_SPI_InputSamplePhaseSelect ( spiId, init->inputSamplePhase);

    if(init->spiMode == DRV_SPI_MODE_SLAVE )
    {
        PLIB_SPI_SlaveSelectEnable ( spiId  );
    }
    else
    {
        PLIB_SPI_SlaveSelectDisable ( spiId  );
    }

    /* Communication Width Selection */
    PLIB_SPI_CommunicationWidthSelect ( spiId, init->commWidth );

    /* Baudrate selection */
    PLIB_SPI_BaudRateSet( spiId , SYS_CLK_PeripheralFrequencyGet(init->spiClk), init->baudRate );
    driverObject->currentBaudRate = init->baudRate;
    driverObject->baudRate = init->baudRate;
    
    switch (init->spiProtocolType)
    {
        case DRV_SPI_PROTOCOL_TYPE_STANDARD:
             PLIB_SPI_FramedCommunicationDisable( spiId  );

	     #if defined (PLIB_SPI_ExistsAudioProtocolControl)
             if (PLIB_SPI_ExistsAudioProtocolControl(spiId))
             {
                 PLIB_SPI_AudioProtocolDisable(spiId);
             }
	     #endif

             break;

        case DRV_SPI_PROTOCOL_TYPE_FRAMED:

            PLIB_SPI_FrameSyncPulseDirectionSelect(spiId, init->framePulseDirection);
            PLIB_SPI_FrameSyncPulsePolaritySelect(spiId, init->framePulsePolarity);
            PLIB_SPI_FrameSyncPulseEdgeSelect(spiId, init->framePulseEdge);
            PLIB_SPI_FrameSyncPulseWidthSelect(spiId, init->framePulseWidth);
            PLIB_SPI_FrameSyncPulseCounterSelect(spiId, init->frameSyncPulse);

	    #if defined (PLIB_SPI_ExistsAudioProtocolControl)
            if (PLIB_SPI_ExistsAudioProtocolControl(spiId))
            {
                PLIB_SPI_AudioProtocolDisable(spiId);
            }
	    #endif	
            PLIB_SPI_FramedCommunicationEnable( spiId  );
            break;

        case DRV_SPI_PROTOCOL_TYPE_AUDIO:
             PLIB_SPI_FramedCommunicationDisable( spiId  );
             
	    #if defined (PLIB_SPI_ExistsAudioProtocolControl)
	     if (PLIB_SPI_ExistsAudioProtocolControl(spiId))
             {
                 PLIB_SPI_AudioTransmitModeSelect(spiId, init->audioTransmitMode);
                 PLIB_SPI_AudioProtocolModeSelect(spiId, init->audioProtocolMode);
                 PLIB_SPI_AudioProtocolEnable(spiId);
             }
         #else
            {
                 SYS_ASSERT(false, "Invalid SPI Configuration");
                return -1;
             }
        #endif
             break;
        default:
             SYS_ASSERT(false, "Invalid SPI Configuration");
            return -1;
    }

#if DRV_SPI_RM_ONLY
    #if defined (PLIB_SPI_ExistsFIFOControl)
    if (PLIB_SPI_ExistsFIFOControl( spiId  ))
    {
        PLIB_SPI_FIFODisable( spiId  );
    }
    #endif
#elif DRV_SPI_EBM_ONLY
    // This will cause a compilation error.  Which is what we want when the user request EBM mode on chips that don't support EBM.
    PLIB_SPI_FIFOEnable( spiId  );
    PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
    PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
#else
    switch (init->bufferType)
    {
        case DRV_SPI_BUFFER_TYPE_STANDARD:
            #if defined (PLIB_SPI_ExistsFIFOControl)
            if (PLIB_SPI_ExistsFIFOControl( spiId  ))
            {
                PLIB_SPI_FIFODisable( spiId  );
            }
            #endif
            break;
        case DRV_SPI_BUFFER_TYPE_ENHANCED:
    	    #if defined (PLIB_SPI_ExistsFIFOControl)
            if (PLIB_SPI_ExistsFIFOControl( spiId  ))
            {
                PLIB_SPI_FIFOEnable( spiId  );
                PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
                PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
            }
            #else
            {
                SYS_ASSERT(false, "Invalid SPI Configuration");
            }
            #endif
            break;
        default:
             SYS_ASSERT(false, "Invalid SPI Configuration");
            return -1;            
    }
#endif

    PLIB_SPI_BufferClear( spiId );
    PLIB_SPI_ReceiverOverflowClear ( spiId );

    // Note: We do not enable the SPI here, that will be done by the first client.
    return 0;
}

