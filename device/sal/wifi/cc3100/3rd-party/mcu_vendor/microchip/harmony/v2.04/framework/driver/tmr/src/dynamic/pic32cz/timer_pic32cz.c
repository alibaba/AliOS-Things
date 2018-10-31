/*******************************************************************************
  Timer Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    timer_pic32cz.c

  Summary:
    Timer Driver Dynamic Implementation

  Description:
    The timer driver provides a simple interface to manage the 
    timer modules on Microchip microcontrollers.  This file Implements 
    the core interface routines for the timer driver.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

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


// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include "timer_pic32cz.h"
#include "osal/osal.h"
//Base address for the Timer Counter modules
TC_Module_Table Module_Table[TC_NUMBER_OF_MODULES]={
	{.hw =  CAST(tc_registers_t,_TC0_BASE_ADDRESS)},
	{.hw =  CAST(tc_registers_t,_TC1_BASE_ADDRESS)},
	{.hw =  CAST(tc_registers_t,_TC2_BASE_ADDRESS)},
	{.hw =  CAST(tc_registers_t,_TC3_BASE_ADDRESS)}
};

 //This API is used to stop the timer operation
void tmr_stop
(
	TMR_MODULE_ID id
)
{	
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//Disable the Timer clock to stop the timer
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKDIS_Msk;	
}

void tmr_32bit_stop
(
	TMR_MODULE_ID id
)
{	
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//Disable the Timer clock to stop the timer
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL + 1].TC_CCR.w = TC_CCR_CLKDIS_Msk;
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKDIS_Msk;		
}

//This API is used to start the timer operation
void tmr_start
(
	TMR_MODULE_ID id
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//Enable the timer clock and trigger the timer using the software trigger
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKEN_Msk|TC_CCR_SWTRG_Msk;
	
}

void tmr_32bit_start
(
	TMR_MODULE_ID id
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//Enable the timer clock and trigger the timer using the software trigger
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKEN_Msk|TC_CCR_SWTRG_Msk;
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL + 1].TC_CCR.w = TC_CCR_CLKEN_Msk|TC_CCR_SWTRG_Msk;
}

//This API is used to select the clock source for the timer operation
void tmr_ClockSourceSelect
(
	TMR_MODULE_ID id,
	DRV_TMR_CLK_SOURCES clockSource
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	uint32_t tmr_cmr = 0;
	
	//Make a backup of the Timer Counter Mode register
	tmr_cmr = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CMR.w;	
	tmr_cmr = (tmr_cmr) & (~(TC_CMR_TCCLKS_Msk));
	
	//Disable the clock before modifying the clock source
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKDIS_Msk;
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CMR.w |= tmr_cmr | TC_CMR_TCCLKS(clockSource);
			
}

void tmr_32bit_ClockSourceSelect
(
	TMR_MODULE_ID id,
	DRV_TMR_CLK_SOURCES clockSource
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	uint32_t tmr_cmr = 0;
	
	//Make a backup of the Timer Counter Mode register
	tmr_cmr = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CMR.w;	
	tmr_cmr = (tmr_cmr) & (~(TC_CMR_TCCLKS_Msk));
	
	//Disable the clock before modifying the clock source
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL + 1].TC_CCR.w = TC_CCR_CLKDIS_Msk;
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL + 1].TC_CMR.w |= tmr_cmr | TC_CMR_TCCLKS(clockSource);
    Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKDIS_Msk;
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CMR.w |= tmr_cmr | TC_CMR_TCCLKS(TC_CMR_TCCLKS_XC0_Val);
}
//This API is used to check whether the timer has reached the specified counter value
bool tmr_interrupt_status
(
TMR_MODULE_ID id
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//return the value stored in the compare c bit of TC status register
	return(Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_SR.w & (TC_SR_CPCS_Msk));
}

//This API is used to get the current counter value
uint32_t tmr_counter_get
(
TMR_MODULE_ID id
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//return the value stored in the counter value register 
	return(Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CV.w);
}

uint32_t tmr_counter_32bit_get
(
TMR_MODULE_ID id
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
    uint32_t status = 0;
	uint32_t LSB1 = 0;
	uint32_t LSB2 = 0;
	uint32_t MSB  = 0;
	uint32_t counter = 0;
	status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
	LSB1 = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL + 1].TC_CV.w;
	MSB  = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CV.w;
	LSB2 = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL + 1].TC_CV.w;
	if(LSB2 < LSB1)
	{
		MSB  = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CV.w;
		counter = (MSB << 16)| LSB2;
	}
	else
	{
		counter = (MSB << 16)| LSB2;
	}
	OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, status);
	//return the value stored in the counter value register 
	return counter;
}


//This API is used to reset the counter value to zero
void tmr_counter_clear
(
TMR_MODULE_ID id
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//Issue a software trigger to clear the counter value and disable the timer clock
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKDIS_Msk | TC_CCR_SWTRG_Msk;
}

void tmr_counter_32bit_clear
(
TMR_MODULE_ID id
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//Issue a software trigger to clear the counter value and disable the timer clock
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL + 1].TC_CCR.w = TC_CCR_CLKDIS_Msk | TC_CCR_SWTRG_Msk;
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKDIS_Msk | TC_CCR_SWTRG_Msk;
}

//This API is used to set the period for the timer operation
void tmr_period_set
(
	TMR_MODULE_ID id,
	uint16_t period 
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	
	//Load the period value to the register c
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_RC.w = period ;
}

void tmr_period_32bit_set
(
	TMR_MODULE_ID id,
	uint32_t period 
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
    if( (period & (uint32_t)(0x0000ffff)) == 0)
    {
        Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_RC.w = (period >> 16);
    }
    else
    {
        Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_RC.w = ((period >> 16)) + 1 ;
    }
    Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL+1].TC_RC.w = (period & 0x0000ffff) + 0x2000;
    Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL+1].TC_RA.w = (period & 0x0000ffff);
}


//This API is used to initialise the Timer counter module
void tmr_setup
(
TMR_MODULE_ID id
)
{
	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	uint32_t tmr_cmr = 0;
	
	//Disable the Timer counter clock
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKDIS_Msk;
	tmr_cmr = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CMR.w;
	tmr_cmr = (tmr_cmr) & ((TC_CMR_TCCLKS_Msk));
	
	//Select the waveform mode and enable the register c compare interrupt
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CMR.w = tmr_cmr | TC_CMR_WAVE_Msk | TC_CMR_WAVSEL_UP_RC;
	
	//Enable the register c compare interrupt if interrupt mode is selected
	if (DRV_TMR_INTERRUPT_MODE)
	{
		Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_IER.w = TC_IER_CPCS_Msk;
	}
	
}

void tmr_32bit_setup
(
    TMR_MODULE_ID id
)
{
    	//Calculate the respective module and channel number corresponding to the instance ID
	uint8_t TIMER_MODULE = id / CHANNEL_PER_MODULE;
	uint8_t TIMER_CHANNEL = id % CHANNEL_PER_MODULE;
	uint32_t tmr_cmr0 = 0;
    uint32_t tmr_cmr1 = 0;
	
	//Disable the Timer counter clock
	Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CCR.w = TC_CCR_CLKDIS_Msk;
	tmr_cmr0 = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CMR.w;
	tmr_cmr0 = (tmr_cmr0) & ((TC_CMR_TCCLKS_Msk));
    Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_CMR.w = tmr_cmr0 | TC_CMR_WAVE_Msk | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPC(TC_CMR_ACPC_TOGGLE_Val);
    
    Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL+1].TC_CCR.w = TC_CCR_CLKDIS_Msk;
	tmr_cmr1 = Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL+1].TC_CMR.w;
	tmr_cmr1 = (tmr_cmr1) & ((TC_CMR_TCCLKS_Msk));
    Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL+1].TC_CMR.w = tmr_cmr1 | TC_CMR_WAVE_Msk | TC_CMR_ASWTRG(TC_CMR_ASWTRG_CLEAR_Val) | TC_CMR_ACPA(TC_CMR_ACPA_SET_Val) | TC_CMR_ACPC(TC_CMR_ACPC_CLEAR_Val) | TC_CMR_EEVT(TC_CMR_EEVT_XC1_Val) | TC_CMR_EEVTEDG(TC_CMR_EEVTEDG_EDGE_Val) | TC_CMR_ENETRG_Msk | TC_CMR_AEEVT(TC_CMR_AEEVT_CLEAR_Val);
    Module_Table[TIMER_MODULE].hw->TC_BMR.w |= TC_BMR_TC0XC0S(TC_BMR_TC0XC0S_TIOA1_Val) | TC_BMR_TC1XC1S(TC_BMR_TC1XC1S_TIOA0_Val);
 	if (DRV_TMR_INTERRUPT_MODE)
	{
		Module_Table[TIMER_MODULE].hw->TC_CHANNEL[TIMER_CHANNEL].TC_IER.w = TC_IER_CPCS_Msk;
	}
}
