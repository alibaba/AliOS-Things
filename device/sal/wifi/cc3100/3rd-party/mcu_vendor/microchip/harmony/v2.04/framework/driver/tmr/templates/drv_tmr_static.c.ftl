/*******************************************************************************
  Timer Static Driver File

  File Name:
    drv_tmr_static.c

  Company:
    Microchip Technology Inc.   

  Summary:
    Timer driver implementation for the static single instance driver.

  Description:
    The Timer device driver provides a simple interface to manage the Timer
    modules on Microchip microcontrollers.
    
  Remarks:
    None
 *******************************************************************************/

/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublTMRense terms in the accompanying lTMRense agreement).

You should refer to the lTMRense agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTTMRULAR PURPOSE.
IN NO EVENT SHALL MTMRROCHIP OR ITS LTMRENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRTMRT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVTMRES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Header Includes
// *****************************************************************************
// *****************************************************************************
#include "driver/tmr/drv_tmr_static.h"
#include "driver/tmr/src/drv_tmr_variant_mapping.h"

<#if CONFIG_USE_DRV_TMR == true>
<#if CONFIG_DRV_TMR_DRIVER_MODE == "STATIC">
typedef struct
{
    DRV_TMR_CALLBACK alarmFunc;  // For alarm registering
    uint32_t alarmCount;    // For AlarmHasElapsed function
    bool    alarmEnabled;   // For Enable/Disable function
    bool    alarmPeriodic;      // Keep Alarm enabled or disable it
    uintptr_t   alarmContext;   // For Alarm Callback
    uint32_t    alarmPeriod;    // For Period Set/Get
} DRV_TMR_ALARM_OBJ;

static bool _DRV_TMR_ClockSourceSet(TMR_MODULE_ID timerId, DRV_TMR_CLK_SOURCES clockSource)
{
    bool clockSet = true;
    /* Clock Source Selection */
    if(clockSource == DRV_TMR_CLKSOURCE_INTERNAL)
    {
        if ( PLIB_TMR_ExistsClockSource ( timerId ) )
        {               
            PLIB_TMR_ClockSourceSelect ( timerId, TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK );           
        }
        else
        {
            /* If clock source feature doesn't exist for any specific timer module instance,
            then by default internal peripheral clock is considered as timer source, so do nothing */ 
        }
    }
    /* External Synchronous Clock Source Selection */
    else if(!(clockSource & 0x10))
    {
        if ( PLIB_TMR_ExistsClockSource ( timerId ) )
        {               
            if ( PLIB_TMR_ExistsClockSourceSync ( timerId )  )
            {
                PLIB_TMR_ClockSourceSelect ( timerId, (TMR_CLOCK_SOURCE)(clockSource & 0x0F) );                
                PLIB_TMR_ClockSourceExternalSyncEnable ( timerId );                    
            }
            /* If Synchronization feature doesn't exist for any specific timer module 
            instance with external clock source then it is synchronous by default */
            else if (clockSource == DRV_TMR_CLKSOURCE_EXTERNAL_SYNCHRONOUS)
            {
                PLIB_TMR_ClockSourceSelect ( timerId, TMR_CLOCK_SOURCE_EXTERNAL_INPUT_PIN );
            }
            else
            {
                clockSet = false;
            }  
        }
        else
        {
            clockSet = false;
        }        
    }
    /* External Asynchronous Clock Source Selection */
    else if(clockSource & 0x10)
    {
        if ( PLIB_TMR_ExistsClockSourceSync ( timerId ) )
        {
            PLIB_TMR_ClockSourceSelect ( timerId, (TMR_CLOCK_SOURCE)(clockSource & 0x0F) );
            PLIB_TMR_ClockSourceExternalSyncDisable ( timerId );
        }
        else
        {
            clockSet = false;
        }        
    }
    
    return clockSet;
}

// Prescaler selection
static bool _DRV_TMR_ClockPrescaleSet(TMR_MODULE_ID timerId, TMR_PRESCALE  prescale)
{
    if( PLIB_TMR_ExistsPrescale( timerId ) )
    {
        PLIB_TMR_PrescaleSelect( timerId , prescale );
        return true;
    }
    return false;
}

<#macro DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE TMR_INSTANCE TMR_CLKSRC TMR_PRESCALE
TMR_MODE TMR_PERIOD TMR_INT_SRC TMR_INT_VEC TMR_INT_PRI TMR_INT_SPRI TMR_GATED_MODE
DRV_TMR_ALARM_FUNCS>
// *****************************************************************************
// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} static driver data
// *****************************************************************************
// *****************************************************************************

<#if DRV_TMR_ALARM_FUNCS == true>
    <#lt>static DRV_TMR_ALARM_OBJ      DRV_TMR${DRV_INSTANCE}_DATA;
</#if>
    <#lt>static bool                   DRV_TMR${DRV_INSTANCE}_Running;

// *****************************************************************************
// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} static driver functions
// *****************************************************************************
// *****************************************************************************
void DRV_TMR${DRV_INSTANCE}_Initialize(void)
{   
    /* Initialize Timer Instance${DRV_INSTANCE} */
    /* Disable Timer */
    PLIB_TMR_Stop(${TMR_INSTANCE});
    /* Select clock source */
<#if TMR_CLKSRC == "DRV_TMR_CLKSOURCE_INTERNAL">	
    PLIB_TMR_ClockSourceSelect ( ${TMR_INSTANCE}, TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK );
<#elseif TMR_CLKSRC == "DRV_TMR_CLKSOURCE_EXTERNAL_SYNCHRONOUS">
    PLIB_TMR_ClockSourceSelect ( ${TMR_INSTANCE}, TMR_CLOCK_SOURCE_EXTERNAL_INPUT_PIN);
<#if TMR_INSTANCE == "TMR_ID_1">	
    PLIB_TMR_ClockSourceExternalSyncEnable ( ${TMR_INSTANCE} );                    
</#if>	
<#elseif TMR_CLKSRC == "DRV_TMR_CLKSOURCE_EXTERNAL_ASYNCHRONOUS">
    PLIB_TMR_ClockSourceSelect ( ${TMR_INSTANCE}, TMR_CLOCK_SOURCE_EXTERNAL_INPUT_PIN);                
<#if TMR_INSTANCE == "TMR_ID_1">	
    PLIB_TMR_ClockSourceExternalSyncDisable ( ${TMR_INSTANCE} );                    
</#if>	
<#elseif TMR_CLKSRC == "DRV_TMR_CLKSOURCE_SECONDARY_OSCILLATOR_SYNCHRONOUS">
    PLIB_TMR_ClockSourceSelect ( ${TMR_INSTANCE}, TMR_CLOCK_SOURCE_SECONDARY_OSCILLATOR);                
<#if TMR_INSTANCE == "TMR_ID_1">	
    PLIB_TMR_ClockSourceExternalSyncEnable ( ${TMR_INSTANCE} );                    
</#if>                    
<#elseif TMR_CLKSRC == "DRV_TMR_CLKSOURCE_SECONDARY_OSCILLATOR_ASYNCHRONOUS">
    PLIB_TMR_ClockSourceSelect ( ${TMR_INSTANCE}, TMR_CLOCK_SOURCE_SECONDARY_OSCILLATOR);                
<#if TMR_INSTANCE == "TMR_ID_1">	
    PLIB_TMR_ClockSourceExternalSyncDisable ( ${TMR_INSTANCE} );                    
</#if>                   
<#elseif TMR_CLKSRC == "DRV_TMR_CLKSOURCE_LPRC_OSCILLATOR_SYNCHRONOUS">
    PLIB_TMR_ClockSourceSelect ( ${TMR_INSTANCE}, TMR_CLOCK_SOURCE_LOW_POWER_RC_OSCILLATOR);                
<#if TMR_INSTANCE == "TMR_ID_1">	
    PLIB_TMR_ClockSourceExternalSyncEnable ( ${TMR_INSTANCE} );                    
</#if>                    
<#elseif TMR_CLKSRC == "DRV_TMR_CLKSOURCE_LPRC_OSCILLATOR_ASYNCHRONOUS">
    PLIB_TMR_ClockSourceSelect ( ${TMR_INSTANCE}, TMR_CLOCK_SOURCE_LOW_POWER_RC_OSCILLATOR);                
<#if TMR_INSTANCE == "TMR_ID_1">	
    PLIB_TMR_ClockSourceExternalSyncDisable ( ${TMR_INSTANCE} );                    
</#if>                  
</#if>
    /* Select prescalar value */
    PLIB_TMR_PrescaleSelect(${TMR_INSTANCE}, ${TMR_PRESCALE});
<#if TMR_INSTANCE != "TMR_ID_1" &&
     TMR_INSTANCE != "TMR_ID_3" &&
     TMR_INSTANCE != "TMR_ID_5" &&
     TMR_INSTANCE != "TMR_ID_7" &&
     TMR_INSTANCE != "TMR_ID_9" &&
     TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    /* Enable 32 bit mode */
    PLIB_TMR_Mode32BitEnable(${TMR_INSTANCE});
    /* Clear counter */
    PLIB_TMR_Counter32BitClear(${TMR_INSTANCE});
    /*Set period */ 
    PLIB_TMR_Period32BitSet(${TMR_INSTANCE}, ${TMR_PERIOD}UL);
<#else>
    /* Enable 16 bit mode */
    PLIB_TMR_Mode16BitEnable(${TMR_INSTANCE});
    /* Clear counter */ 
    PLIB_TMR_Counter16BitClear(${TMR_INSTANCE});
    /*Set period */ 
    PLIB_TMR_Period16BitSet(${TMR_INSTANCE}, ${TMR_PERIOD});
</#if>
<#if DRV_TMR_ALARM_FUNCS == true>
    /* Initialize the data structure */
    DRV_TMR${DRV_INSTANCE}_DATA.alarmFunc = NULL;
    DRV_TMR${DRV_INSTANCE}_DATA.alarmCount = 0;
    DRV_TMR${DRV_INSTANCE}_DATA.alarmEnabled = false;
    DRV_TMR${DRV_INSTANCE}_DATA.alarmPeriodic = false;
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
    /* Setup Interrupt */   
    PLIB_INT_VectorPrioritySet(INT_ID_0, ${TMR_INT_VEC}, ${TMR_INT_PRI});
    PLIB_INT_VectorSubPrioritySet(INT_ID_0, ${TMR_INT_VEC}, ${TMR_INT_SPRI});          
</#if>
}

static void _DRV_TMR${DRV_INSTANCE}_Resume(bool resume)
{
    if (resume)
    {
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
        PLIB_INT_SourceFlagClear(INT_ID_0, ${TMR_INT_SRC});
        PLIB_INT_SourceEnable(INT_ID_0, ${TMR_INT_SRC});
</#if>
        PLIB_TMR_Start(${TMR_INSTANCE});
    }
}

bool DRV_TMR${DRV_INSTANCE}_Start(void)
{
    /* Start Timer*/
    _DRV_TMR${DRV_INSTANCE}_Resume(true);
    DRV_TMR${DRV_INSTANCE}_Running = true;
    
    return true;
}

static bool _DRV_TMR${DRV_INSTANCE}_Suspend(void)
{
    if (DRV_TMR${DRV_INSTANCE}_Running)
    {
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
        PLIB_INT_SourceDisable(INT_ID_0, ${TMR_INT_SRC});
</#if>
        PLIB_TMR_Stop(${TMR_INSTANCE});
        return (true);
    }
    
    return (false);
}

void DRV_TMR${DRV_INSTANCE}_Stop(void)
{
    _DRV_TMR${DRV_INSTANCE}_Suspend();
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
    PLIB_INT_SourceFlagClear(INT_ID_0, ${TMR_INT_SRC});
</#if>
    DRV_TMR${DRV_INSTANCE}_Running = false;
}

DRV_TMR_CLIENT_STATUS DRV_TMR${DRV_INSTANCE}_ClientStatus ( void )
{
    if (DRV_TMR${DRV_INSTANCE}_Running)
        return DRV_TMR_CLIENT_STATUS_RUNNING;
    else
        return DRV_TMR_CLIENT_STATUS_READY;
}

void DRV_TMR${DRV_INSTANCE}_CounterValueSet(uint32_t value)
{
<#if TMR_INSTANCE != "TMR_ID_1" &&
     TMR_INSTANCE != "TMR_ID_3" &&
     TMR_INSTANCE != "TMR_ID_5" &&
     TMR_INSTANCE != "TMR_ID_7" &&
     TMR_INSTANCE != "TMR_ID_9" &&
     TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    /* Set 32-bit counter value*/
    PLIB_TMR_Counter32BitSet(${TMR_INSTANCE}, value);
<#else>
    /* Set 16-bit counter value*/
    PLIB_TMR_Counter16BitSet(${TMR_INSTANCE}, (uint16_t)value);
</#if>
}

uint32_t DRV_TMR${DRV_INSTANCE}_CounterValueGet(void)
{
<#if TMR_INSTANCE != "TMR_ID_1" &&
     TMR_INSTANCE != "TMR_ID_3" &&
     TMR_INSTANCE != "TMR_ID_5" &&
     TMR_INSTANCE != "TMR_ID_7" &&
     TMR_INSTANCE != "TMR_ID_9" &&
     TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    /* Get 32-bit counter value*/
    return PLIB_TMR_Counter32BitGet(${TMR_INSTANCE});
<#else>
    /* Get 16-bit counter value*/
    return (uint32_t) PLIB_TMR_Counter16BitGet(${TMR_INSTANCE});
</#if>
}

void DRV_TMR${DRV_INSTANCE}_CounterClear(void)
{
<#if TMR_INSTANCE != "TMR_ID_1" &&
     TMR_INSTANCE != "TMR_ID_3" &&
     TMR_INSTANCE != "TMR_ID_5" &&
     TMR_INSTANCE != "TMR_ID_7" &&
     TMR_INSTANCE != "TMR_ID_9" &&
     TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    /* Clear 32-bit counter value*/
    PLIB_TMR_Counter32BitClear(${TMR_INSTANCE});
<#else>
    /* Clear 16-bit counter value*/
    PLIB_TMR_Counter16BitClear(${TMR_INSTANCE});
</#if>
}

DRV_TMR_OPERATION_MODE DRV_TMR${DRV_INSTANCE}_DividerRangeGet
(
	DRV_TMR_DIVIDER_RANGE * pDivRange
)
{
	if(pDivRange)
	{
<#if TMR_INSTANCE != "TMR_ID_1" &&
     TMR_INSTANCE != "TMR_ID_3" &&
     TMR_INSTANCE != "TMR_ID_5" &&
     TMR_INSTANCE != "TMR_ID_7" &&
     TMR_INSTANCE != "TMR_ID_9" &&
     TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
        pDivRange->dividerMax = DRV_TIMER_DIVIDER_MAX_32BIT;
        pDivRange->dividerMin = DRV_TIMER_DIVIDER_MIN_32BIT;
		pDivRange->dividerStep = 1;
		return DRV_TMR_OPERATION_MODE_32_BIT;
<#else>
        pDivRange->dividerMax = DRV_TIMER_DIVIDER_MAX_16BIT;
        pDivRange->dividerMin = DRV_TIMER_DIVIDER_MIN_16BIT;
		pDivRange->dividerStep = 1;
		return DRV_TMR_OPERATION_MODE_16_BIT;
</#if>	
	}
	return DRV_TMR_OPERATION_MODE_NONE;
}

uint32_t DRV_TMR${DRV_INSTANCE}_CounterFrequencyGet(void)
{
    <#if TMR_CLKSRC = "DRV_TMR_CLKSOURCE_INTERNAL">
        <#lt>    uint32_t prescale, tmrBaseFreq;
    
        <#lt>    tmrBaseFreq = SYS_CLK_PeripheralFrequencyGet ( CLK_BUS_FOR_TIMER_PERIPHERAL );
        <#lt>    prescale = PLIB_TMR_PrescaleGet(${TMR_INSTANCE});
        <#lt>    return ( tmrBaseFreq / prescale );
    <#else>
        <#lt>    return 0;
    </#if>
}

TMR_PRESCALE DRV_TMR${DRV_INSTANCE}_PrescalerGet(void)
{
    uint16_t prescale_value;
    /* Call the PLIB directly */
    prescale_value = PLIB_TMR_PrescaleGet(${TMR_INSTANCE});
    
    switch(prescale_value)
    {
        case 1: return TMR_PRESCALE_VALUE_1;
        case 2: return TMR_PRESCALE_VALUE_2;
        case 4: return TMR_PRESCALE_VALUE_4;
        case 8: return TMR_PRESCALE_VALUE_8;
        case 16: return TMR_PRESCALE_VALUE_16;
        case 32: return TMR_PRESCALE_VALUE_32;
        case 64: return TMR_PRESCALE_VALUE_64;
        case 256: return TMR_PRESCALE_VALUE_256;
        default: return TMR_PRESCALE_VALUE_1;
    }
}

void DRV_TMR${DRV_INSTANCE}_PeriodValueSet(uint32_t value)
{
<#if TMR_INSTANCE != "TMR_ID_1" &&
     TMR_INSTANCE != "TMR_ID_3" &&
     TMR_INSTANCE != "TMR_ID_5" &&
     TMR_INSTANCE != "TMR_ID_7" &&
     TMR_INSTANCE != "TMR_ID_9" &&
     TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    /* Set 32-bit counter value*/
    PLIB_TMR_Period32BitSet(${TMR_INSTANCE}, value);
<#else>
    /* Set 16-bit counter value*/
    PLIB_TMR_Period16BitSet(${TMR_INSTANCE}, (uint16_t)value);
</#if>
}

uint32_t DRV_TMR${DRV_INSTANCE}_PeriodValueGet(void)
{
<#if TMR_INSTANCE != "TMR_ID_1" &&
     TMR_INSTANCE != "TMR_ID_3" &&
     TMR_INSTANCE != "TMR_ID_5" &&
     TMR_INSTANCE != "TMR_ID_7" &&
     TMR_INSTANCE != "TMR_ID_9" &&
     TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    /* Get 32-bit counter value*/
    return PLIB_TMR_Period32BitGet(${TMR_INSTANCE});
<#else>
    /* Get 16-bit counter value*/
    return (uint32_t) PLIB_TMR_Period16BitGet(${TMR_INSTANCE});
</#if>
}

void DRV_TMR${DRV_INSTANCE}_StopInIdleDisable(void)
{
    PLIB_TMR_StopInIdleDisable(${TMR_INSTANCE});
}

void DRV_TMR${DRV_INSTANCE}_StopInIdleEnable(void)
{
    PLIB_TMR_StopInIdleDisable(${TMR_INSTANCE});
}

bool DRV_TMR${DRV_INSTANCE}_ClockSet
(
    DRV_TMR_CLK_SOURCES clockSource,
    TMR_PRESCALE        preScale
)
{
    bool success = false;
    bool resume = _DRV_TMR${DRV_INSTANCE}_Suspend();
    
    if (_DRV_TMR_ClockSourceSet(${TMR_INSTANCE}, clockSource) &&
        _DRV_TMR_ClockPrescaleSet(${TMR_INSTANCE}, preScale))
    {
        success = true;
    }
    
    _DRV_TMR${DRV_INSTANCE}_Resume(resume);
    return success;
}

<#if TMR_GATED_MODE == true>
bool DRV_TMR${DRV_INSTANCE}_GateModeClear(void)
{
#if defined(PLIB_TMR_ExistsGatedTimeAccumulation)
    if (PLIB_TMR_ExistsGatedTimeAccumulation(${TMR_INSTANCE}))
    {
        bool resume = _DRV_TMR${DRV_INSTANCE}_Suspend();
        PLIB_TMR_GateDisable(${TMR_INSTANCE});
        _DRV_TMR${DRV_INSTANCE}_Resume(resume);
        return true;
    }
#endif    
    return false;
}

bool DRV_TMR${DRV_INSTANCE}_GateModeSet(void)
{
#if defined(PLIB_TMR_ExistsGatedTimeAccumulation)
    if (PLIB_TMR_ExistsGatedTimeAccumulation(${TMR_INSTANCE}))
    {
        bool resume = _DRV_TMR${DRV_INSTANCE}_Suspend();
        PLIB_TMR_GateEnable(${TMR_INSTANCE});
        _DRV_TMR${DRV_INSTANCE}_Resume(resume);
        return true;
    }
#endif    
    return false;
}
</#if>
<#if DRV_TMR_ALARM_FUNCS == true>
bool DRV_TMR${DRV_INSTANCE}_AlarmRegister
(
    uint32_t divider, 
    bool isPeriodic, 
    uintptr_t context, 
    DRV_TMR_CALLBACK callBack 
)
{
<#if TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    bool success = (divider >= DRV_TIMER_DIVIDER_MIN_32BIT && 
                    divider <= DRV_TIMER_DIVIDER_MAX_32BIT);
<#else>
    bool success = (divider >= DRV_TIMER_DIVIDER_MIN_16BIT && 
                    divider <= DRV_TIMER_DIVIDER_MAX_16BIT);
</#if>
    if (success)
    {
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>    
        PLIB_INT_SourceDisable(INT_ID_0, ${TMR_INT_SRC});
</#if>
        PLIB_TMR_Stop(${TMR_INSTANCE});
<#if TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
        PLIB_TMR_Period32BitSet(${TMR_INSTANCE}, divider - 1);
<#else>
        PLIB_TMR_Period16BitSet(${TMR_INSTANCE}, (uint16_t)divider - 1);
</#if>
        DRV_TMR${DRV_INSTANCE}_DATA.alarmPeriodic = isPeriodic;
        DRV_TMR${DRV_INSTANCE}_DATA.alarmContext = context;
        DRV_TMR${DRV_INSTANCE}_DATA.alarmFunc = callBack;
        return (success);
    }
    else
        return false;
}

void DRV_TMR${DRV_INSTANCE}_AlarmEnable(bool enable)
{
    if (DRV_TMR${DRV_INSTANCE}_DATA.alarmFunc != NULL)
    {
        DRV_TMR${DRV_INSTANCE}_DATA.alarmEnabled = enable;
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>    
        PLIB_INT_SourceEnable(INT_ID_0, ${TMR_INT_SRC});
</#if>
    }
}

bool DRV_TMR${DRV_INSTANCE}_AlarmDisable(void)
{
    bool retVal = DRV_TMR${DRV_INSTANCE}_DATA.alarmEnabled;
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>    
        PLIB_INT_SourceDisable(INT_ID_0, ${TMR_INT_SRC});
</#if>
    DRV_TMR${DRV_INSTANCE}_DATA.alarmEnabled = false;
    
    return retVal;
}

void DRV_TMR${DRV_INSTANCE}_AlarmPeriodSet(uint32_t value)
{
    bool resume = _DRV_TMR${DRV_INSTANCE}_Suspend();
    DRV_TMR${DRV_INSTANCE}_DATA.alarmPeriod = value;
<#if TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    PLIB_TMR_Period32BitSet(${TMR_INSTANCE}, value);
<#else>
    PLIB_TMR_Period16BitSet(${TMR_INSTANCE}, (uint16_t)value);
</#if>
    _DRV_TMR${DRV_INSTANCE}_Resume(resume);
}

uint32_t DRV_TMR${DRV_INSTANCE}_AlarmPeriodGet(void)
{
<#if TMR_MODE == "DRV_TMR_OPERATION_MODE_32_BIT">
    return PLIB_TMR_Period32BitGet(${TMR_INSTANCE});
<#else>
    return PLIB_TMR_Period16BitGet(${TMR_INSTANCE});
</#if>
}

void DRV_TMR${DRV_INSTANCE}_AlarmDeregister(void)
{
    DRV_TMR${DRV_INSTANCE}_DATA.alarmEnabled = false;
    DRV_TMR${DRV_INSTANCE}_DATA.alarmFunc = NULL;
}

uint32_t DRV_TMR${DRV_INSTANCE}_AlarmHasElapsed(void)
{
    uint32_t alarmCountTemp = DRV_TMR${DRV_INSTANCE}_DATA.alarmCount;
    
    DRV_TMR${DRV_INSTANCE}_DATA.alarmCount = 0;
    return (alarmCountTemp);
}

void DRV_TMR${DRV_INSTANCE}_Tasks(void)
{
    if (DRV_TMR${DRV_INSTANCE}_DATA.alarmEnabled &&
        (DRV_TMR${DRV_INSTANCE}_DATA.alarmFunc != NULL))
    {	
		DRV_TMR${DRV_INSTANCE}_DATA.alarmCount++;
        DRV_TMR${DRV_INSTANCE}_DATA.alarmFunc(
            DRV_TMR${DRV_INSTANCE}_DATA.alarmContext, 
            DRV_TMR${DRV_INSTANCE}_DATA.alarmCount);
        if (!DRV_TMR${DRV_INSTANCE}_DATA.alarmPeriodic)
        {
            DRV_TMR${DRV_INSTANCE}_AlarmDisable();
        }
    }
}
</#if>
</#macro>

<#if CONFIG_DRV_TMR_INST_0 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="0" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX0
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX0 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX0
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX0 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX0
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX0 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX0 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX0 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX0
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX0 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX0/>
</#if>
<#if CONFIG_DRV_TMR_INST_1 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="1" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX1 
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX1 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX1
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX1 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX1
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX1 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX1 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX1 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX1
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX1 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX1/>
</#if>
<#if CONFIG_DRV_TMR_INST_2 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="2" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX2 
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX2 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX2
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX2 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX2
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX2 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX2 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX2 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX2
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX2 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX2/>
</#if>
<#if CONFIG_DRV_TMR_INST_3 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="3" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX3 
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX3 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX3
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX3 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX3
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX3 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX3 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX3 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX3
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX3 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX3/>
</#if>
<#if CONFIG_DRV_TMR_INST_4 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="4" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX4 
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX4 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX4
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX4 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX4
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX4 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX4 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX4 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX4
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX4 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX4/>
</#if>
<#if CONFIG_DRV_TMR_INST_5 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="5" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX5 
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX5 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX5
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX5 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX5
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX5 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX5 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX5 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX5
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX5 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX5/>
</#if>
<#if CONFIG_DRV_TMR_INST_6 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="6" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX6 
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX6 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX6
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX6 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX6
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX6 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX6 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX6 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX6
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX6 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX6/>
</#if>
<#if CONFIG_DRV_TMR_INST_7 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="7" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX7 
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX7 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX7
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX7 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX7
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX7 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX7 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX7 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX7
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX7 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX7/>
</#if>
<#if CONFIG_DRV_TMR_INST_8 == true>
<@DRV_TMR_STATIC_FUNCTIONS DRV_INSTANCE="8" TMR_INSTANCE=CONFIG_DRV_TMR_PERIPHERAL_ID_IDX8 
TMR_CLKSRC=CONFIG_DRV_TMR_CLOCK_SOURCE_IDX8 TMR_PRESCALE=CONFIG_DRV_TMR_PRESCALE_IDX8
TMR_MODE=CONFIG_DRV_TMR_OPERATION_MODE_IDX8 TMR_PERIOD=CONFIG_DRV_TMR_PERIOD_IDX8
TMR_INT_SRC=CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX8 TMR_INT_VEC=CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX8 
TMR_INT_PRI=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX8 TMR_INT_SPRI=CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX8
TMR_GATED_MODE=CONFIG_DRV_TMR_GATED_MODE_IDX8 DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX8/>
</#if>
</#if> <#--CONFIG_USE_DRV_TMR == true-->
</#if> <#--CONFIG_DRV_TMR_DRIVER_MODE == "STATIC"-->
/*******************************************************************************
 End of File
*/
