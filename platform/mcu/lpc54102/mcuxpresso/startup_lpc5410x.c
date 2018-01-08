//*****************************************************************************
// LPC54102 startup code for use with MCUXpresso IDE
//
// Version : 230317
//*****************************************************************************
//
// Copyright(C) NXP Semiconductors, 2017
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// o Redistributions of source code must retain the above copyright notice, this list
//   of conditions and the following disclaimer.
//
// o Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
//
// o Neither the name of copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************

#if defined (DEBUG)
#pragma GCC push_options
#pragma GCC optimize ("Og")
#endif // (DEBUG)

#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" {
    extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__ ((weak))
#define WEAK_AV __attribute__ ((weak, section(".after_vectors")))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
//*****************************************************************************
#if !defined (CORE_M0PLUS)
#include "NXP/crp.h"
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif

#include "k_api.h" // krhino_intrpt_enter, krhino_intrpt_exit

//*****************************************************************************
// Declaration of external SystemInit function
//*****************************************************************************
#if defined (__USE_CMSIS)
extern void SystemInit(void);
#endif // (__USE_CMSIS)

//*****************************************************************************
#if !defined (DONT_ENABLE_SWVTRACECLK) && !defined (CORE_M0PLUS)
// Allow confirmation that SWV trace has been enabled
unsigned int __SWVtrace_Enabled;
#endif

//*****************************************************************************
// Forward declaration of the core exception handlers.
// When the application defines a handler (with the same name), this will
// automatically take precedence over these weak definitions
//*****************************************************************************
     void ResetISR(void);
#if defined (__MULTICORE_MASTER)
void ResetISR2(void);
#endif
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

//*****************************************************************************
// Forward declaration of the application IRQ handlers. When the application
// defines a handler (with the same name), this will automatically take
// precedence over weak definitions below
//*****************************************************************************
// External Interrupts - Available on M0+/M4
WEAK void WDT_IRQHandler(void);
WEAK void BOD_IRQHandler(void);
WEAK void Reserved18_IRQHandler(void);
WEAK void DMA0_IRQHandler(void);
WEAK void GINT0_IRQHandler(void);
WEAK void PIN_INT0_IRQHandler(void);
WEAK void PIN_INT1_IRQHandler(void);
WEAK void PIN_INT2_IRQHandler(void);
WEAK void PIN_INT3_IRQHandler(void);
WEAK void UTICK0_IRQHandler(void);
WEAK void MRT0_IRQHandler(void);
WEAK void CTIMER0_IRQHandler(void);
WEAK void CTIMER1_IRQHandler(void);
WEAK void CTIMER2_IRQHandler(void);
WEAK void CTIMER3_IRQHandler(void);
WEAK void CTIMER4_IRQHandler(void);
WEAK void SCT0_IRQHandler(void);
WEAK void USART0_IRQHandler(void);
WEAK void USART1_IRQHandler(void);
WEAK void USART2_IRQHandler(void);
WEAK void USART3_IRQHandler(void);
WEAK void I2C0_IRQHandler(void);
WEAK void I2C1_IRQHandler(void);
WEAK void I2C2_IRQHandler(void);
WEAK void SPI0_IRQHandler(void);
WEAK void SPI1_IRQHandler(void);
WEAK void ADC0_SEQA_IRQHandler(void);
WEAK void ADC0_SEQB_IRQHandler(void);
WEAK void ADC0_THCMP_IRQHandler(void);
WEAK void RTC_IRQHandler(void);
WEAK void Reserved46_IRQHandler(void);
WEAK void MAILBOX_IRQHandler(void);
// External Interrupts - For M4 only
#if defined (CORE_M4)
WEAK void GINT1_IRQHandler(void);
WEAK void PIN_INT4_IRQHandler(void);
WEAK void PIN_INT5_IRQHandler(void);
WEAK void PIN_INT6_IRQHandler(void);
WEAK void PIN_INT7_IRQHandler(void);
WEAK void Reserved53_IRQHandler(void);
WEAK void Reserved54_IRQHandler(void);
WEAK void Reserved55_IRQHandler(void);
WEAK void RIT_IRQHandler(void);
#endif

//*****************************************************************************
// Forward declaration of the driver IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the driver
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//*****************************************************************************
void WDT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void BOD_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved18_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GINT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void UTICK0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void MRT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTIMER4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SCT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USART0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USART1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USART2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USART3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void I2C0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void I2C1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void I2C2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SPI0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SPI1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC0_SEQA_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC0_SEQB_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC0_THCMP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void RTC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved46_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void MAILBOX_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
// External Interrupts - For M4 only
#if defined (CORE_M4)
void GINT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT5_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT6_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIN_INT7_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved53_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved54_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved55_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void RIT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
#endif

//*****************************************************************************
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//*****************************************************************************
#if defined (__REDLIB__)
extern void __main(void);
#endif
extern int main(void);

//*****************************************************************************
// External declaration for the pointer to the stack top from the Linker Script
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
//
// External declaration for LPC MCU vector table checksum from  Linker Script
//
//*****************************************************************************
WEAK extern void __valid_user_code_checksum();

//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif

//*****************************************************************************
// The vector table.
// This relies on the linker script to place at correct location in memory.
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
extern void * __Vectors __attribute__ ((alias ("g_pfnVectors")));

__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
#if defined (CORE_M4)
    // Core Level - CM4
    &_vStackTop,                       // The initial stack pointer
    ResetISR,                          // The reset handler
    NMI_Handler,                       // The NMI handler
    HardFault_Handler,                 // The hard fault handler
    MemManage_Handler,                 // The MPU fault handler
    BusFault_Handler,                  // The bus fault handler
    UsageFault_Handler,                // The usage fault handler
    __valid_user_code_checksum,        // LPC MCU checksum
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    SVC_Handler,                       // SVCall handler
    DebugMon_Handler,                  // Debug monitor handler
    0,                                 // Reserved
    PendSV_Handler,                    // The PendSV handler
    SysTick_Handler,                   // The SysTick handler
#else
    // Core Level -  CM0plus
    &_vStackTop,                       // The initial stack pointer
    ResetISR,                          // The reset handler
    NMI_Handler,                       // The NMI handler
    HardFault_Handler,                 // The hard fault handler
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    SVC_Handler,                       // SVCall handler
    0,                                 // Reserved
    0,                                 // Reserved
    PendSV_Handler,                    // The PendSV handler
    SysTick_Handler,                   // The SysTick handler
#endif

    // External Interrupts - Available on M0+/M4
    WDT_IRQHandler,          // 16: Windowed watchdog timer, Brownout detect
    BOD_IRQHandler,          // 17: BOD interrupt
    Reserved18_IRQHandler,   // 18: Reserved interrupt
    DMA0_IRQHandler,         // 19: DMA controller
    GINT0_IRQHandler,        // 20: GPIO group 0
    PIN_INT0_IRQHandler,     // 21: Pin interrupt 0 or pattern match engine slice 0
    PIN_INT1_IRQHandler,     // 22: Pin interrupt 1or pattern match engine slice 1
    PIN_INT2_IRQHandler,     // 23: Pin interrupt 2 or pattern match engine slice 2
    PIN_INT3_IRQHandler,     // 24: Pin interrupt 3 or pattern match engine slice 3
    UTICK0_IRQHandler,       // 25: Micro-tick Timer
    MRT0_IRQHandler,         // 26: Multi-rate timer
    CTIMER0_IRQHandler,      // 27: Standard counter/timer CTIMER0
    CTIMER1_IRQHandler,      // 28: Standard counter/timer CTIMER1
    CTIMER2_IRQHandler,      // 29: Standard counter/timer CTIMER2
    CTIMER3_IRQHandler,      // 30: Standard counter/timer CTIMER3
    CTIMER4_IRQHandler,      // 31: Standard counter/timer CTIMER4
    SCT0_IRQHandler,         // 32: SCTimer/PWM
    USART0_IRQHandler,       // 33: USART0
    USART1_IRQHandler,       // 34: USART1
    USART2_IRQHandler,       // 35: USART2
    USART3_IRQHandler,       // 36: USART3
    I2C0_IRQHandler,         // 37: I2C0
    I2C1_IRQHandler,         // 38: I2C1
    I2C2_IRQHandler,         // 39: I2C2
    SPI0_IRQHandler,         // 40: SPI0
    SPI1_IRQHandler,         // 41: SPI1
    ADC0_SEQA_IRQHandler,    // 42: ADC0 sequence A completion.
    ADC0_SEQB_IRQHandler,    // 43: ADC0 sequence B completion.
    ADC0_THCMP_IRQHandler,   // 44: ADC0 threshold compare and error.
    RTC_IRQHandler,          // 45: RTC alarm and wake-up interrupts
    Reserved46_IRQHandler,   // 46: Reserved interrupt
    MAILBOX_IRQHandler,      // 47: Mailbox interrupt (present on selected devices)
    // External Interrupts - For M4 only
#if defined (CORE_M4)
    GINT1_IRQHandler,        // 48: GPIO group 1
    PIN_INT4_IRQHandler,     // 49: Pin interrupt 4 or pattern match engine slice 4 int
    PIN_INT5_IRQHandler,     // 50: Pin interrupt 5 or pattern match engine slice 5 int
    PIN_INT6_IRQHandler,     // 51: Pin interrupt 6 or pattern match engine slice 6 int
    PIN_INT7_IRQHandler,     // 52: Pin interrupt 7 or pattern match engine slice 7 int
    Reserved53_IRQHandler,   // 53: Reserved interrupt
    Reserved54_IRQHandler,   // 54: Reserved interrupt
    Reserved55_IRQHandler,   // 55: Reserved interrupt
    RIT_IRQHandler,          // 56: Repetitive Interrupt Timer
#endif


}; /* End of g_pfnVectors */

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
__attribute__ ((section(".after_vectors.init_data")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int *pulSrc = (unsigned int*) romstart;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors.init_bss")))
void bss_init(unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = 0;
}

//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//*****************************************************************************

#if defined (__MULTICORE_MASTER)
//#define  cpu_ctrl 0x40000300
//#define  coproc_boot   0x40000304
//#define  set coproc_stack  0x40000308
__attribute__ ((naked, section(".after_vectors.reset")))
void ResetISR(void) {
    asm volatile(
        ".set   cpu_ctrl,       0x40000300\t\n"
        ".set   coproc_boot,    0x40000304\t\n"
        ".set   coproc_stack,   0x40000308\t\n"
        "MOVS   R5, #1\t\n"
        "LDR    R0, =0xE000ED00\t\n"
        "LDR    R1, [R0]\t\n"           // READ CPUID register
        "LDR    R2,=0x410CC601\t\n"     // CM0 R0p1 identifier
        "EORS   R1,R1,R2\t\n"           // XOR to see if we are C0
        "LDR    R3, =cpu_ctrl\t\n"      // get address of CPU_CTRL
        "LDR    R1,[R3]\t\n"            // read cpu_ctrl reg into R1
        "BEQ.N  cm0_boot\t\n"
    "cm4_boot:\t\n"
        "LDR    R0,=coproc_boot\t\n"    // coproc boot address
        "LDR    R0,[R0]\t\n"            // get address to branch to
        "MOVS   R0,R0\t\n"              // Check if 0
        "BEQ.N  check_master_m4\t\n"    // if zero in boot reg, we just branch to  real reset
        "BX     R0\t\n"                 // otherwise, we branch to boot address
    "commonboot:\t\n"
        "LDR    R0, =ResetISR2\t\n"     // Jump to 'real' reset handler
        "BX     R0\t\n"
    "cm0_boot:\t\n"
        "LDR    R0,=coproc_boot\t\n"    // coproc boot address
        "LDR    R0,[R0]\t\n"            // get address to branch to
        "MOVS   R0,R0\t\n"              // Check if 0
        "BEQ.N  check_master_m0\t\n"    // if zero in boot reg, we just branch to  real reset
        "LDR    R1,=coproc_stack\t\n"   // pickup coprocesor stackpointer (from syscon CPSTACK)
        "LDR    R1,[R1]\t\n"
        "MOV    SP,R1\t\n"
        "BX     R0\t\n"                 // goto boot address
    "check_master_m0:\t\n"
        "ANDS   R1,R1,R5\t\n"           // bit test bit0
        "BEQ.N  commonboot\t\n"         // if we get 0, that means we are masters
        "B.N    goto_sleep_pending_reset\t\n"   // Otherwise, there is no startup vector for slave, so we go to sleep
    "check_master_m4:\t\n"
        "ANDS   R1,R1,R5\t\n"           // bit test bit0
        "BNE.N  commonboot\t\n"         // if we get 1, that means we are masters
    "goto_sleep_pending_reset:\t\n"
        "MOV     SP,R5\t\n"             // load 0x1 into SP so that any stacking (eg on NMI) will not cause us to wakeup
            // and write to uninitialised Stack area (instead it will LOCK us up before we cause damage)
            // this code should only be reached if debugger bypassed ROM or we changed master without giving
            // correct start address, the only way out of this is through a debugger change of SP and PC
    "sleepo:\t\n"
        "WFI\t\n"                       // go to sleep
        "B.N    sleepo\t\n"
    );
}

void ResetISR2(void) {

#else
__attribute__ ((section(".after_vectors.reset")))
void ResetISR(void) {
#endif

    // Disable interrupts
    __asm volatile ("cpsid i");

    // Enable SRAM clock used by Stack
    __asm volatile ("LDR R0, =0x400000C8\n\t"
                    "MOV R1, #24\n\t"
                    "STR R1, [R0]");

#if defined (__USE_CMSIS)
// If __USE_CMSIS defined, then call CMSIS SystemInit code
    SystemInit();
#endif // (__USE_CMSIS)

    //
    // Copy the data sections from flash to SRAM.
    //
	unsigned int LoadAddr, ExeAddr, SectionLen;
	unsigned int *SectionTableAddr;

	// Load base address of Global Section Table
	SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
	while (SectionTableAddr < &__data_section_table_end) {
		LoadAddr = *SectionTableAddr++;
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		data_init(LoadAddr, ExeAddr, SectionLen);
	}

	// At this point, SectionTableAddr = &__bss_section_table;
	// Zero fill the bss segment
	while (SectionTableAddr < &__bss_section_table_end) {
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		bss_init(ExeAddr, SectionLen);
	}

    // Optionally enable Cortex-M4 SWV trace (off by default at reset)
    // Note - your board support must also set up pinmuxing such that
    // SWO is output on GPIO PIO0-15 (FUNC2) or PIO1_1 (FUNC2).
#if !defined (DONT_ENABLE_SWVTRACECLK) && !defined (CORE_M0PLUS)
    volatile unsigned int *TRACECLKDIV = (unsigned int *) 0x400000E4;
    volatile unsigned int *SYSAHBCLKCTRLSET = (unsigned int *) 0x400000C8;
    volatile unsigned int *SYSAHBCLKCTRL = (unsigned int *) 0x400000C0;
    // Write 0x00000001 to TRACECLKDIV – Trace divider
    *TRACECLKDIV = 1;
    // Enable IOCON peripheral clock (for SWO on PIO0-15 or PIO1_1)
    // by setting bit13 via SYSAHBCLKCTRLSET[0]
    *SYSAHBCLKCTRLSET = 1 << 13; // 0x2000
    // Read  SYSAHBCLKCTRL[0] and check bit 13
    __SWVtrace_Enabled = ((*SYSAHBCLKCTRL & 1 << 13) == 1 << 13);
#endif

#if !defined (__USE_CMSIS)
// Assume that if __USE_CMSIS defined, then CMSIS SystemInit code
// will enable the FPU
#if defined (__VFP_FP__) && !defined (__SOFTFP__)
    //
    // Code to enable the Cortex-M4 FPU only included
    // if appropriate build options have been selected.
    // Code taken from Section 7.1, Cortex-M4 TRM (DDI0439C)
    //
    // Read CPACR (located at address 0xE000ED88)
    // Set bits 20-23 to enable CP10 and CP11 coprocessors
    // Write back the modified value to the CPACR
	asm volatile ("LDR.W R0, =0xE000ED88\n\t"
                  "LDR R1, [R0]\n\t"
                  "ORR R1, R1, #(0xF << 20)\n\t"
                  "STR R1, [R0]");
#endif // (__VFP_FP__) && !(__SOFTFP__)
#endif // (__USE_CMSIS)

#if !defined (__USE_CMSIS)
// Assume that if __USE_CMSIS defined, then CMSIS SystemInit code
// will setup the VTOR register

    // Check to see if we are running the code from a non-zero
    // address (eg RAM, external flash), in which case we need
    // to modify the VTOR register to tell the CPU that the
    // vector table is located at a non-0x0 address.
    unsigned int * pSCB_VTOR = (unsigned int *) 0xE000ED08;
    if ((unsigned int *)g_pfnVectors!=(unsigned int *) 0x00000000) {
        *pSCB_VTOR = (unsigned int)g_pfnVectors;
    }
#endif // (__USE_CMSIS)

#if defined (__cplusplus)
    //
    // Call C++ library initialisation
    //
    __libc_init_array();
#endif

    // Reenable interrupts
    __asm volatile ("cpsie i");

#if defined (__REDLIB__)
	// Call the Redlib library, which in turn calls main()
	__main();
#else
	main();
#endif

	//
	// main() shouldn't return, but if it does, we'll just enter an infinite loop
	//
	while (1) {
		;
	}
}

//*****************************************************************************
// Default core exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
WEAK_AV void NMI_Handler(void)
{ while(1) {}
}

WEAK_AV void HardFault_Handler(void)
{ while(1) {}
}

WEAK_AV void SVC_Handler(void)
{ while(1) {}
}

WEAK_AV void PendSV_Handler(void)
{ while(1) {}
}

WEAK_AV void SysTick_Handler(void)
{ while(1) {}
}

// Core Level Interrupts - For M4 only
#if defined (CORE_M4)
WEAK_AV void MemManage_Handler(void)
{ while(1) {}
}

WEAK_AV void BusFault_Handler(void)
{ while(1) {}
}

WEAK_AV void UsageFault_Handler(void)
{ while(1) {}
}

WEAK_AV void DebugMon_Handler(void)
{ while(1) {}
}
#endif  // External Interrupts - For M4 only

//*****************************************************************************
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*****************************************************************************
WEAK_AV void IntDefaultHandler(void)
{ while(1) {}
}

//*****************************************************************************
// Default application exception handlers. Override the ones here by defining
// your own handler routines in your application code. These routines call
// driver exception handlers or IntDefaultHandler() if no driver exception
// handler is included.
//*****************************************************************************
WEAK void WDT_IRQHandler(void)
{
   krhino_intrpt_enter();
   WDT_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void BOD_IRQHandler(void)
{
   krhino_intrpt_enter();
   BOD_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void Reserved18_IRQHandler(void)
{  
   krhino_intrpt_enter();
   Reserved18_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void DMA0_IRQHandler(void)
{
   krhino_intrpt_enter();
   DMA0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void GINT0_IRQHandler(void)
{
   krhino_intrpt_enter();
   GINT0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void PIN_INT0_IRQHandler(void)
{
   krhino_intrpt_enter();
   PIN_INT0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void PIN_INT1_IRQHandler(void)
{
   krhino_intrpt_enter();
   PIN_INT1_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void PIN_INT2_IRQHandler(void)
{
   krhino_intrpt_enter();
   PIN_INT2_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void PIN_INT3_IRQHandler(void)
{
   krhino_intrpt_enter();
   PIN_INT3_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void UTICK0_IRQHandler(void)
{  
   krhino_intrpt_enter();
   UTICK0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void MRT0_IRQHandler(void)
{
   krhino_intrpt_enter();
   MRT0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void CTIMER0_IRQHandler(void)
{
   krhino_intrpt_enter();
   CTIMER0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void CTIMER1_IRQHandler(void)
{
   krhino_intrpt_enter();
   CTIMER1_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void CTIMER2_IRQHandler(void)
{
   krhino_intrpt_enter();
   CTIMER2_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void CTIMER3_IRQHandler(void)
{
   krhino_intrpt_enter();
   CTIMER3_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void CTIMER4_IRQHandler(void)
{
   krhino_intrpt_enter();
   CTIMER4_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void SCT0_IRQHandler(void)
{  
   krhino_intrpt_enter();
   SCT0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void USART0_IRQHandler(void)
{   
   krhino_intrpt_enter();
   USART0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void USART1_IRQHandler(void)
{   
   krhino_intrpt_enter();
   USART1_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void USART2_IRQHandler(void)
{
   krhino_intrpt_enter();
   USART2_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void USART3_IRQHandler(void)
{
   krhino_intrpt_enter();
   USART3_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void I2C0_IRQHandler(void)
{
   krhino_intrpt_enter();
   I2C0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void I2C1_IRQHandler(void)
{
   krhino_intrpt_enter();
   I2C1_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void I2C2_IRQHandler(void)
{
   krhino_intrpt_enter();
   I2C2_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void SPI0_IRQHandler(void)
{
   krhino_intrpt_enter();
   SPI0_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void SPI1_IRQHandler(void)
{
   krhino_intrpt_enter();
   SPI1_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void ADC0_SEQA_IRQHandler(void)
{
   krhino_intrpt_enter();
   ADC0_SEQA_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void ADC0_SEQB_IRQHandler(void)
{
   krhino_intrpt_enter();
   ADC0_SEQB_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void ADC0_THCMP_IRQHandler(void)
{
   krhino_intrpt_enter();
   ADC0_THCMP_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void RTC_IRQHandler(void)
{
   krhino_intrpt_enter();
   RTC_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void Reserved46_IRQHandler(void)
{
   krhino_intrpt_enter();
   Reserved46_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void MAILBOX_IRQHandler(void)
{
   krhino_intrpt_enter();
   MAILBOX_DriverIRQHandler();
   krhino_intrpt_exit();
}
// External Interrupts - For M4 only
#if defined (CORE_M4)
WEAK void GINT1_IRQHandler(void)
{
   krhino_intrpt_enter();
   GINT1_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void PIN_INT4_IRQHandler(void)
{
   krhino_intrpt_enter();
   PIN_INT4_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void PIN_INT5_IRQHandler(void)
{
   krhino_intrpt_enter();
   PIN_INT5_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void PIN_INT6_IRQHandler(void)
{
   krhino_intrpt_enter();
   PIN_INT6_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void PIN_INT7_IRQHandler(void)
{
   krhino_intrpt_enter();
   PIN_INT7_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void Reserved53_IRQHandler(void)
{
   krhino_intrpt_enter();
   Reserved53_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void Reserved54_IRQHandler(void)
{
   krhino_intrpt_enter();
   Reserved54_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void Reserved55_IRQHandler(void)
{
   krhino_intrpt_enter();
   Reserved55_DriverIRQHandler();
   krhino_intrpt_exit();
}

WEAK void RIT_IRQHandler(void)
{
   krhino_intrpt_enter();
   RIT_DriverIRQHandler();
   krhino_intrpt_exit();
}
#endif  // External Interrupts - For M4 only

//*****************************************************************************

#if defined (DEBUG)
#pragma GCC pop_options
#endif // (DEBUG)
