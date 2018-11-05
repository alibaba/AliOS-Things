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
<#if CONFIG_DRV_SPI_USE_MASTER_MODE>
    if (pInit->spiMode == DRV_SPI_MODE_MASTER)
    {
        mode = _SPI_DRV_VTABLE_MASTER;
    }
    else
</#if>
<#if CONFIG_DRV_SPI_USE_SLAVE_MODE>
    if (pInit->spiMode == DRV_SPI_MODE_SLAVE)
    {
        mode = _SPI_DRV_VTABLE_SLAVE;
    }
    else
</#if>
    {
        SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
        return -1;
    }

<#if CONFIG_DRV_SPI_USE_POLLED_MODE>
    if (pInit->taskMode == DRV_SPI_TASK_MODE_POLLED)
    {
        mode |= DRV_SPI_TASK_MODE_POLLED;
    }
    else
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE>
    if (pInit->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        mode |= DRV_SPI_TASK_MODE_ISR;
    }
    else
</#if>
    {
        SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
        return -1;
    }

<#if CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE>
    if (pInit->bufferType == DRV_SPI_BUFFER_TYPE_STANDARD)
    {
        mode |= _SPI_DRV_VTABLE_RM;
    }
    else
</#if>
<#if CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE>
    if (pInit->bufferType == DRV_SPI_BUFFER_TYPE_ENHANCED)
    {
        mode |= _SPI_DRV_VTABLE_EBM;
    }
    else
</#if>
    {
        SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
        return -1;
    }

<#if CONFIG_DRV_SPI_USE_8BIT_MODE>
    if (pInit->commWidth == SPI_COMMUNICATION_WIDTH_8BITS)
    {
        mode |= _SPI_DRV_VTABLE_8BIT;
    }
    else
</#if>
<#if CONFIG_DRV_SPI_USE_16BIT_MODE>
    if (pInit->commWidth == SPI_COMMUNICATION_WIDTH_16BITS)
    {
        mode |= _SPI_DRV_VTABLE_16BIT;
    }
    else
</#if>
<#if CONFIG_DRV_SPI_USE_32BIT_MODE>
    if (pInit->commWidth == SPI_COMMUNICATION_WIDTH_32BITS)
    {
        mode |= _SPI_DRV_VTABLE_32BIT;
    }
    else
</#if>
    {
        SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
        return -1;
    }
<#macro spiVTableCaseStatement taskMode spiMode bufferMode  bitWidth>
    <#if taskMode=="Polled">
        <#assign tma="P"/>
    <#else>
        <#assign tma="I"/>
    </#if>
    <#if spiMode=="Master">
        <#assign sma="M"/>
    <#else>
        <#assign sma="S"/>
    </#if>
    <#if bufferMode="RM">
        <#assign bma="R"/>
    <#else>
        <#assign bma="E"/>
    </#if>
    case _SPI_DRV_VTABLE_${tma}_${sma}_${bma}_${bitWidth}:
        driverObject->vfMainTask = DRV_SPI_${taskMode}${spiMode}${bufferMode}${bitWidth}BitTasks;
    <#if CONFIG_DRV_SPI_USE_DMA>
        driverObject->sendDMAHander = DRV_SPI_${taskMode}DMA${spiMode}SendEventHandler${bitWidth}bit;
        driverObject->receiveDMAHander = DRV_SPI_${taskMode}DMA${spiMode}ReceiveEventHandler${bitWidth}bit;
    </#if>
        break;
</#macro>
    switch (mode)
    {
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_8BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Slave" "RM" "8"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_16BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Slave" "RM" "16"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_32BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Slave" "RM" "32"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_8BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Slave" "EBM" "8"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_16BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Slave" "EBM" "16"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_32BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Slave" "EBM" "32"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_8BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Slave" "RM" "8"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_16BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Slave" "RM" "16"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_32BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Slave" "RM" "32"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_8BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Slave" "EBM" "8"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_16BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Slave" "EBM" "16"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_SLAVE_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_32BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Slave" "EBM" "32"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_8BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Master" "RM" "8"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_16BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Master" "RM" "16"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_32BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Master" "RM" "32"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_8BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Master" "EBM" "8"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_16BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Master" "EBM" "16"/>
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_32BIT_MODE>
    <@spiVTableCaseStatement "Polled" "Master" "EBM" "32"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_8BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Master" "RM" "8"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_16BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Master" "RM" "16"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_32BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Master" "RM" "32"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_8BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Master" "EBM" "8"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_16BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Master" "EBM" "16"/>
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE && CONFIG_DRV_SPI_USE_MASTER_MODE && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE && CONFIG_DRV_SPI_USE_32BIT_MODE>
    <@spiVTableCaseStatement "ISR" "Master" "EBM" "32"/>
</#if>
    default:
        SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
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
<#if !CONFIG_DRV_SPI_USE_SLAVE_MODE>
    PLIB_SPI_MasterEnable ( spiId  );
<#elseif !CONFIG_DRV_SPI_USE_MASTER_MODE>
    PLIB_SPI_SlaveEnable ( spiId  );
<#else>
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
        SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
        return -1;
    }
</#if>

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
             SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
            return -1;
    }

    // Set up the  Input Sample Phase
    PLIB_SPI_InputSamplePhaseSelect ( spiId, init->inputSamplePhase);

    //Enable the SSx Pin on slave side if needed
    if(init->spiMode == DRV_SPI_MODE_SLAVE && (init->spiProtocolType == DRV_SPI_PROTOCOL_TYPE_FRAMED || init->spiSlaveSSPin))
    {
        PLIB_SPI_PinEnable(spiId, SPI_PIN_SLAVE_SELECT);
    }
    else
    {
        PLIB_SPI_PinDisable(spiId, SPI_PIN_SLAVE_SELECT);
    }

    /* Communication Width Selection */
    PLIB_SPI_CommunicationWidthSelect ( spiId, init->commWidth );

    /* Baudrate selection */
    #if defined (PLIB_SPI_ExistsBaudRateClock)
        if (init->baudClockSource == SPI_BAUD_RATE_PBCLK_CLOCK)
        {
            PLIB_SPI_BaudRateSet( spiId , SYS_CLK_PeripheralFrequencyGet(init->spiClk), init->baudRate );
        }
        else // if baud clock source is reference clock
        {
            PLIB_SPI_BaudRateSet( spiId , SYS_CLK_ReferenceFrequencyGet(CLK_BUS_REFERENCE_1), init->baudRate );
        }
    #else
        PLIB_SPI_BaudRateSet( spiId , SYS_CLK_PeripheralFrequencyGet(init->spiClk), init->baudRate );
    #endif
    
    driverObject->currentBaudRate = init->baudRate;
    driverObject->baudRate = init->baudRate;
    
    switch (init->spiProtocolType)
    {
        case DRV_SPI_PROTOCOL_TYPE_STANDARD:
             PLIB_SPI_FramedCommunicationDisable( spiId  );
<#if CONFIG_AUDIO_CODEC_SUPPORT>
             if (PLIB_SPI_ExistsAudioProtocolControl(spiId))
             {
                 PLIB_SPI_AudioProtocolDisable(spiId);
             }
</#if>
             break;

        case DRV_SPI_PROTOCOL_TYPE_FRAMED:
            
            #if defined (PLIB_SPI_ExistsFrameSyncPulseDirection)
                if (PLIB_SPI_ExistsFrameSyncPulseDirection(spiId))
                {
                    PLIB_SPI_FrameSyncPulseDirectionSelect(spiId, init->framePulseDirection);
                }
            #endif
            #if defined (PLIB_SPI_ExistsFrameSyncPulsePolarity)
                if (PLIB_SPI_ExistsFrameSyncPulsePolarity(spiId))
                {
                    PLIB_SPI_FrameSyncPulsePolaritySelect(spiId, init->framePulsePolarity);
                }
            #endif          
            #if defined (PLIB_SPI_ExistsFrameSyncPulseEdge)
                if (PLIB_SPI_ExistsFrameSyncPulseEdge(spiId))
                {
                    PLIB_SPI_FrameSyncPulseEdgeSelect(spiId, init->framePulseEdge);
                }
            #endif       
            #if defined (PLIB_SPI_ExistsFrameSyncPulseWidth)
                if (PLIB_SPI_ExistsFrameSyncPulseWidth(spiId))
                {
                    PLIB_SPI_FrameSyncPulseWidthSelect(spiId, init->framePulseWidth);
                }
            #endif   
            #if defined (PLIB_SPI_ExistsFrameSyncPulseCounter)
                if (PLIB_SPI_ExistsFrameSyncPulseCounter(spiId))
                {
                    PLIB_SPI_FrameSyncPulseCounterSelect(spiId, init->frameSyncPulse);
                }
            #endif
            

<#if CONFIG_AUDIO_CODEC_SUPPORT>
            if (PLIB_SPI_ExistsAudioProtocolControl(spiId))
            {
                PLIB_SPI_AudioProtocolDisable(spiId);
            }
</#if>
            PLIB_SPI_FramedCommunicationEnable( spiId  );
            break;

        case DRV_SPI_PROTOCOL_TYPE_AUDIO:
             PLIB_SPI_FramedCommunicationDisable( spiId  );
             
<#if CONFIG_AUDIO_CODEC_SUPPORT>
	     if (PLIB_SPI_ExistsAudioProtocolControl(spiId))
             {
                 PLIB_SPI_AudioTransmitModeSelect(spiId, init->audioTransmitMode);
                 PLIB_SPI_AudioProtocolModeSelect(spiId, init->audioProtocolMode);
                 PLIB_SPI_AudioProtocolEnable(spiId);
             }
<#else>
             {
                 SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
                return -1;
             }
</#if>
             break;
        default:
             SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
            return -1;
    }

<#if !CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE>
    <#if CONFIG_ENHANCED_BUFFER_MODE_SUPPORT>
    if (PLIB_SPI_ExistsFIFOControl( spiId  ))
    {
        PLIB_SPI_FIFODisable( spiId  );
    }
    </#if>
<#elseif !CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE>
    PLIB_SPI_FIFOEnable( spiId  );
    PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
    PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
<#else>
    switch (init->bufferType)
    {
        case DRV_SPI_BUFFER_TYPE_STANDARD:
<#if CONFIG_ENHANCED_BUFFER_MODE_SUPPORT>
            if (PLIB_SPI_ExistsFIFOControl( spiId  ))
            {
                PLIB_SPI_FIFODisable( spiId  );
            }
</#if>
            break;
        case DRV_SPI_BUFFER_TYPE_ENHANCED:
<#if CONFIG_ENHANCED_BUFFER_MODE_SUPPORT>
            if (PLIB_SPI_ExistsFIFOControl( spiId  ))
            {
                PLIB_SPI_FIFOEnable( spiId  );
                PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
                PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
            }
<#else>
            {
                SYS_ASSERT(false, "\r\nInvalid SPI Configuration");
            }
</#if>
            break;
        default:
             SYS_ASSERT(false, "\r\nInvalid SPI Configuration");
            return -1;            
    }
</#if>

    PLIB_SPI_BufferClear( spiId );
    PLIB_SPI_ReceiverOverflowClear ( spiId );

    // Note: We do not enable the SPI here, that will be done by the first client.
    return 0;
}


