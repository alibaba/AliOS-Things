//*****************************************************************************
//
// startup.c - Boot code for Stellaris.
//
// Copyright (c) 2005,2006 Luminary Micro, Inc.  All rights reserved.
//
// Software License Agreement
//
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's Stellaris Family of microcontroller products.
//
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  Any use in violation
// of the foregoing restrictions may subject the user to criminal sanctions
// under applicable laws, as well as to civil liability for the breach of the
// terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 852 of the Stellaris Driver Library.
//
//*****************************************************************************
#include "chip_info.h"
#include "debug.h"
// modified by Martin Thomas - don't blame Luminary if it does
// not work for you.

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
// mthomas: attribute for stack-aligment (see README_mthomas.txt)
#ifdef __GNUC__
void reset_handler(void) __attribute__((__interrupt__));
static void nmi_handler(void) __attribute__((__interrupt__));
static void hardfault_handler(void) __attribute__((__interrupt__));
static void IntDefaultHandler(void) __attribute__((__interrupt__));
static void mpufault_handker(void) __attribute__((__interrupt__));
static void busfault_handker(void) __attribute__((__interrupt__));
static void usagefault_handker(void) __attribute__((__interrupt__));
void svcall_interrupt(void) __attribute__((weak,__interrupt__));
void pendsv_interrupt(void) __attribute__((weak,__interrupt__));
void systick_interrupt(void) __attribute__((weak));

void GpioInterrupt(void); //__attribute__((weak));
void RtcInterrupt(void) __attribute__((weak,__interrupt__));
void IrInterrupt(void) __attribute__((weak,__interrupt__));
void FuartInterrupt(void) __attribute__((weak,__interrupt__));
void BuartInterrupt(void) __attribute__((weak,__interrupt__));
void PwcInterrupt(void) __attribute__((weak,__interrupt__));
void Timer0Interrupt(void) __attribute__((weak,__interrupt__));
void UsbInterrupt(void) __attribute__((weak,__interrupt__));
void DmaCh0Interrupt(void) __attribute__((weak,__interrupt__));
void DmaCh1Interrupt(void) __attribute__((weak,__interrupt__));
void audio_decoder_interrupt_handler(void) __attribute__((weak,__interrupt__));
void SpisInterrupt(void) __attribute__((weak,__interrupt__));
void SdInterrupt(void) __attribute__((weak,__interrupt__));
void SpimInterrupt(void) __attribute__((weak,__interrupt__));
void SarAdcInterrupt(void) __attribute__((weak,__interrupt__));
void I2cInterrupt(void) __attribute__((weak,__interrupt__));
void Timer1Interrupt(void) __attribute__((weak,__interrupt__));
void WatchDogInterrupt(void) __attribute__((weak,__interrupt__));
#else
void ResetISR(void);
static void NmiSR(void);
static void FaultISR(void);
static void IntDefaultHandler(void);
#endif

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
#ifndef STACK_SIZE
#define STACK_SIZE                              256
#endif

// mthomas: added section -> alignment thru linker-script
//__attribute__ ((section(".stackarea"))) static unsigned long pulStack[STACK_SIZE];

//*****************************************************************************
//
// The minimal vector table for a Cortex M3.  Note that the proper constructs
// must be placed on this to ensure that it ends up at physical address
// 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
	(void(*)(void))(0x20000000 + CFG_SYS_STACK_SIZE),	// The initial stack pointer
	reset_handler,                               // The reset handler
	nmi_handler,                                  // The NMI handler
	hardfault_handler,			//#3 :Hard Fault Handler
	mpufault_handker,			//#4 :MPU Fault Handler
	busfault_handker,			//#5 :Bus Fault Handler
	usagefault_handker,			//#6 :Usage Fault Handler
	0,							//#7 :Reserved
	0,							//#8 :Reserved
	0,							//#9 :Reserved
	0,							//#10:Reserved
	svcall_interrupt,			//#11:SVCall Handler
	0,							//#12:Debug Monitor Handler
	0,							//#13:Reserved
	pendsv_interrupt,			//#14:PendSV Handler
	systick_interrupt,			//#15:SysTick Handler


	//-----------External Interrupts---------------------
	GpioInterrupt, 				//#16: GPIO
	RtcInterrupt,				//#17: RTC
	IrInterrupt,		    	//#18: IR
	FuartInterrupt,				//#19: FUART
	BuartInterrupt,				//#20: BUART
	PwcInterrupt,				//#21: PWC
	Timer0Interrupt,			//#22: TIMER0
	UsbInterrupt,				//#23: USB
	DmaCh0Interrupt,   			//#24: DMA CH0
	DmaCh1Interrupt,		    //#25: DMA CH1
	audio_decoder_interrupt_handler,      		//26: DECODER
	SpisInterrupt,              //#27: SPIS
	SdInterrupt,				//#28: SDIO
	SpimInterrupt,              //#29: SPIM
	Timer1Interrupt,			//#30: TIMER1
	WatchDogInterrupt,			//#31: WatchDog
};

__attribute__ ((section(".stub_section")))
void stub(void)
{
__asm__ __volatile__(

    		".long 0xFFFFFFFF \n\n"	//0x80
    		".long 0xFFFFFFFF \n\n"
    		".long 0xFFFFFFFF \n\n"
    		".long 0x100000 \n\n"		//constant data @ 0x8C
    		".long 0x1D0000 \n\n"		//user data
    		".byte 0x12 \n\n"
    		".byte 7 \n\n"
    		".byte 9 \n\n"
    		".byte 6 \n\n"
    		".long 0xFFFFFFFF \n\n"	//code crc
    		".long 0xB0BEBDC9 \n\n"	//magic number
    		".long 0x00000706 \n\n"	//32KHz external oscillator input/output capacitance calibration value
    		".long 0xFFFFFFFF \n\n"	//fast code crc	@ 0xA4
		    ".rept (0x100-0xA8)/4 \n\n"
    		".long 0xFFFFFFFF \n\n"
		     ".endr \n\n"

    );
}
//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long __etext;
extern unsigned long __data_start__;
extern unsigned long _edata;
extern unsigned long __bss_start__;
extern unsigned long _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied main() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void reset_handler(void)
{
    unsigned long *pulSrc, *pulDest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = &__etext;
    for(pulDest = &__data_start__; pulDest < &_edata; )
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    for(pulDest = &__bss_start__; pulDest < &_ebss; )
    {
        *pulDest++ = 0;
    }

    //
    // Call the application's entry point.
    //
    main();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
nmi_handler(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    	DBG("hard_fault1\n");
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
hardfault_handler(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    	DBG("hard_fault\n");
    }
}

static void
mpufault_handker(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    	DBG("hard_fault2\n");
    }
}

static void
busfault_handker(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    	DBG("hard_fault3\n");
    }
}

static void
usagefault_handker(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    	DBG("hard_fault4\n");
    }
}


//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
IntDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    	DBG("hard_fault5\n");
    }
}
