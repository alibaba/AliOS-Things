/*******************************************************************************
  Timer Driver Interface Declarations for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr_static.h

  Summary:
    Timer driver interface declarations for the static single instance driver.

  Description:
    The Timer device driver provides a simple interface to manage the Timer
    modules on Microchip microcontrollers. This file defines the interface
    Declarations for the TMR driver.
    
  Remarks:
    Static interfaces incorporate the driver instance number within the names
    of the routines, eliminating the need for an object ID or object handle.
    
    Static single-open interfaces also eliminate the need for the open handle.
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
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTOCULAR PURPOSE.
IN NO EVENT SHALL MOCROCHIP OR ITS LOCENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STROCT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVOCES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_TMR_STATIC_H
#define _DRV_TMR_STATIC_H
// *****************************************************************************
// *****************************************************************************
// Section: Include Headers
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include "driver/tmr/drv_tmr.h"
#include "peripheral/tmr/plib_tmr.h"
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
#include "peripheral/int/plib_int.h"
</#if>

// maximum divider value for 32 bit operation mode
#define     DRV_TIMER_DIVIDER_MAX_32BIT     0xffffffff

// minimum divider value for 32 bit operation mode
#define     DRV_TIMER_DIVIDER_MIN_32BIT     0x2

// maximum divider value for 16 bit operation mode
#define     DRV_TIMER_DIVIDER_MAX_16BIT     0x10000

// minimum divider value for 16 bit operation mode
#define     DRV_TIMER_DIVIDER_MIN_16BIT     0x2

<#macro DRV_TMR_STATIC_API DRV_INSTANCE TMR_MODE DRV_TMR_GATED_MODE DRV_TMR_ALARM_FUNCS>
// *****************************************************************************
// *****************************************************************************
// Section: Interface Headers for Instance ${DRV_INSTANCE} for the static driver
// *****************************************************************************
// *****************************************************************************

void DRV_TMR${DRV_INSTANCE}_Initialize(void);
bool DRV_TMR${DRV_INSTANCE}_Start(void);
void DRV_TMR${DRV_INSTANCE}_Stop(void);
static inline void DRV_TMR${DRV_INSTANCE}_DeInitialize(void)
{
	DRV_TMR${DRV_INSTANCE}_Stop();
}
static inline SYS_STATUS DRV_TMR${DRV_INSTANCE}_Status(void)
{
	/* Return the status as ready always */
    return SYS_STATUS_READY; 
}
static inline void DRV_TMR${DRV_INSTANCE}_Open(void) {}
DRV_TMR_CLIENT_STATUS DRV_TMR${DRV_INSTANCE}_ClientStatus ( void );
static inline DRV_TMR_OPERATION_MODE DRV_TMR${DRV_INSTANCE}_OperationModeGet(void)
{
<#if TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    return DRV_TMR_OPERATION_MODE_32_BIT;
<#else>
    return DRV_TMR_OPERATION_MODE_16_BIT;
</#if>
}
static inline void DRV_TMR${DRV_INSTANCE}_Close(void) 
{
    DRV_TMR${DRV_INSTANCE}_Stop();
}
bool DRV_TMR${DRV_INSTANCE}_ClockSet
(
    DRV_TMR_CLK_SOURCES clockSource, 
    TMR_PRESCALE  prescale 
);
void DRV_TMR${DRV_INSTANCE}_CounterValueSet(uint32_t value);
uint32_t DRV_TMR${DRV_INSTANCE}_CounterValueGet(void);
void DRV_TMR${DRV_INSTANCE}_CounterClear(void);
TMR_PRESCALE DRV_TMR${DRV_INSTANCE}_PrescalerGet(void);
void DRV_TMR${DRV_INSTANCE}_PeriodValueSet(uint32_t value);
uint32_t DRV_TMR${DRV_INSTANCE}_PeriodValueGet(void);
void DRV_TMR${DRV_INSTANCE}_StopInIdleDisable(void);
void DRV_TMR${DRV_INSTANCE}_StopInIdleEnable(void);
<#if DRV_TMR_GATED_MODE>
bool DRV_TMR${DRV_INSTANCE}_GateModeClear(void);
bool DRV_TMR${DRV_INSTANCE}_GateModeSet(void);
</#if>
<#if DRV_TMR_ALARM_FUNCS>
bool DRV_TMR${DRV_INSTANCE}_AlarmRegister(
    uint32_t divider, 
    bool isPeriodic, 
    uintptr_t context, 
    DRV_TMR_CALLBACK callBack 
);
bool DRV_TMR${DRV_INSTANCE}_AlarmDisable(void);
void DRV_TMR${DRV_INSTANCE}_AlarmEnable(bool enable);
void DRV_TMR${DRV_INSTANCE}_AlarmPeriodSet(uint32_t value);
uint32_t DRV_TMR${DRV_INSTANCE}_AlarmPeriodGet(void);
void DRV_TMR${DRV_INSTANCE}_AlarmDeregister(void);
uint32_t DRV_TMR${DRV_INSTANCE}_AlarmHasElapsed(void);
void DRV_TMR${DRV_INSTANCE}_Tasks(void);
<#else>
static inline void DRV_TMR${DRV_INSTANCE}_Tasks(void) {}
</#if>
uint32_t DRV_TMR${DRV_INSTANCE}_CounterFrequencyGet(void);
DRV_TMR_OPERATION_MODE DRV_TMR${DRV_INSTANCE}_DividerRangeGet
(
    DRV_TMR_DIVIDER_RANGE * pDivRange
);
</#macro>

<#if CONFIG_DRV_TMR_INST_0 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="0" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX0 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX0 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX0/>
</#if>
<#if CONFIG_DRV_TMR_INST_1 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="1" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX1 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX1 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX1/>
</#if>
<#if CONFIG_DRV_TMR_INST_2 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="2" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX2 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX2 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX2/>
</#if>
<#if CONFIG_DRV_TMR_INST_3 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="3" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX3 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX3 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX3/>
</#if>
<#if CONFIG_DRV_TMR_INST_4 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="4" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX4 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX4 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX4/>
</#if>
<#if CONFIG_DRV_TMR_INST_5 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="5" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX5 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX5 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX5/>
</#if>
<#if CONFIG_DRV_TMR_INST_6 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="6" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX6 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX6 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX6/>
</#if>
<#if CONFIG_DRV_TMR_INST_7 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="7" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX7 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX7 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX7/>
</#if>
<#if CONFIG_DRV_TMR_INST_8 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="8" TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX8 DRV_TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX8 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX8/>
</#if>
#endif // #ifndef _DRV_TMR_STATIC_H

/*******************************************************************************
 End of File
*/
