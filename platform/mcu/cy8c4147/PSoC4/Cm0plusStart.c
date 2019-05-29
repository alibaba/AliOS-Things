/***************************************************************************//**
* \file Cm0Start.c
* \version 5.70
*
* \brief Startup code for the ARM CM0.
*
********************************************************************************
* \copyright
* Copyright 2010-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <limits.h>
#include "cydevice_trm.h"
#include "cytypes.h"
#include "cyfitter_cfg.h"
#include "CyLib.h"
#include "cyfitter.h"
#include "cyapicallbacks.h"

#define CY_NUM_VECTORS              (CY_INT_IRQ_BASE + CY_NUM_INTERRUPTS)

#if (CY_IP_CPUSS_CM0)
    #define CY_CPUSS_CONFIG_VECT_IN_RAM (( uint32 ) 0x01)
#endif /* (CY_IP_CPUSS_CM0) */


#if (CY_IP_CPUSS_CM0)
    /* CPUSS Configuration register */
    #define CY_CPUSS_CONFIG_REG         (*(reg32 *) CYREG_CPUSS_CONFIG)
    #define CY_CPUSS_CONFIG_PTR         ( (reg32 *) CYREG_CPUSS_CONFIG)
#endif /* (CY_IP_CPUSS_CM0) */


#if defined (__ICCARM__)
    #define CY_NUM_ROM_VECTORS      (CY_NUM_VECTORS)
#else
    #define CY_NUM_ROM_VECTORS      (4u)
#endif  /* defined (__ICCARM__) */

/* Vector table address in SRAM */
#define CY_CPUSS_CONFIG_VECT_ADDR_IN_RAM    (0x20000000u)

#ifndef CY_SYS_INITIAL_STACK_POINTER

    #if defined(__ARMCC_VERSION)
        #define CY_SYS_INITIAL_STACK_POINTER ((cyisraddress)(uint32)&Image$$ARM_LIB_STACK$$ZI$$Limit)
    #elif defined (__GNUC__)
        #define CY_SYS_INITIAL_STACK_POINTER (&__cy_stack)
    #elif defined (__ICCARM__)
        #pragma language=extended
        #pragma segment="CSTACK"
        #define CY_SYS_INITIAL_STACK_POINTER  { .__ptr = __sfe( "CSTACK" ) }

        extern void __iar_program_start( void );
        extern void __iar_data_init3 (void);
    #endif  /* (__ARMCC_VERSION) */

#endif /* CY_SYS_INITIAL_STACK_POINTER */


#if defined(__GNUC__)
    #include <errno.h>
    extern int  end;
#endif  /* defined(__GNUC__) */

/* Extern functions */
extern void CyBtldr_CheckLaunch(void);

/* Function prototypes */
void initialize_psoc(void);

/* Global variables */
#if !defined (__ICCARM__)
    CY_NOINIT static uint32 cySysNoInitDataValid;
#endif  /* !defined (__ICCARM__) */


#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_STANDARD)

    /*******************************************************************************
    This variable is used by the Bootloader/Bootloadable components to schedule
    what application will be started after a software reset.
    *******************************************************************************/
    #if (__ARMCC_VERSION)
        __attribute__ ((section(".bootloaderruntype"), zero_init))
    #elif defined (__GNUC__)
        __attribute__ ((section(".bootloaderruntype")))
    #elif defined (__ICCARM__)
        #pragma location=".bootloaderruntype"
    #endif /* (__ARMCC_VERSION) */
    volatile uint32 cyBtldrRunType;

#endif  /* (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_STANDARD) */


/*******************************************************************************
* Function Name: IntDefaultHandler
****************************************************************************//**
*
*  This function is called for all interrupts, other than a reset that is called
*  before the system is setup.
*
*******************************************************************************/
CY_NORETURN
CY_ISR(IntDefaultHandler)
{
    /***************************************************************************
    * We must not get here. If we do, a serious problem occurs, so go into
    * an infinite loop.
    ***************************************************************************/

    #if defined(__GNUC__)
        if (errno == ENOMEM)
        {
            #ifdef CY_BOOT_INT_DEFAULT_HANDLER_ENOMEM_EXCEPTION_CALLBACK
                CyBoot_IntDefaultHandler_Enomem_Exception_Callback();
            #endif /* CY_BOOT_INT_DEFAULT_HANDLER_ENOMEM_EXCEPTION_CALLBACK */
            
            while(1)
            {
                /* Out Of Heap Space
                 * This can be increased in the System tab of the Design Wide Resources.
                 */
            }
        }
        else
    #endif
        {
            #ifdef CY_BOOT_INT_DEFAULT_HANDLER_EXCEPTION_ENTRY_CALLBACK
                CyBoot_IntDefaultHandler_Exception_EntryCallback();
            #endif /* CY_BOOT_INT_DEFAULT_HANDLER_EXCEPTION_ENTRY_CALLBACK */

            while(1)
            {

            }
        }
}

#if defined(__ARMCC_VERSION)

/* Local function for device reset. */
extern void Reset(void);

/* Application entry point. */
extern void $Super$$main(void);

/* Linker-generated Stack Base addresses, Two Region and One Region */
extern unsigned long Image$$ARM_LIB_STACK$$ZI$$Limit;

/* RealView C Library initialization. */
extern int __main(void);


/*******************************************************************************
* Function Name: Reset
****************************************************************************//**
*
* This function handles the reset interrupt for the MDK toolchains.
* This is the first bit of code that is executed at startup.
*
*******************************************************************************/
void Reset(void)
{
    #if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLE || CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
        /* The bootloadable application image is started at Reset() handler
        * as a result of a branch instruction execution from the bootloader.
        * So, the stack pointer needs to be reset to be sure that
        * there is no garbage in the stack.
        */
        register uint32_t msp __asm("msp");
        msp = (uint32_t)&Image$$ARM_LIB_STACK$$ZI$$Limit;
    #endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLE || CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

    #if(CY_IP_SRSSLT)
        CySysWdtDisable();
    #endif  /* (CY_IP_SRSSLT) */

    #if ((CYDEV_BOOTLOADER_ENABLE) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
        CyBtldr_CheckLaunch();
    #endif /* ((CYDEV_BOOTLOADER_ENABLE) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)) */

    __main();
}

/*******************************************************************************
* Function Name: $Sub$$main
****************************************************************************//**
*
* This function is called immediately before the users main
*
*******************************************************************************/
__attribute__ ((noreturn, __noinline__))
void $Sub$$main(void)
{
    initialize_psoc();

    /* Call original main */
    $Super$$main();

    while (1)
    {
        /* If main returns it is undefined what we should do. */
    }
}

#elif defined(__GNUC__)

/* Stack Base address */
extern void __cy_stack(void);

/* Application entry point. */
extern int main(void);

/* The static objects constructors initializer */
extern void __libc_init_array(void);

typedef unsigned char __cy_byte_align8 __attribute ((aligned (8)));

struct __cy_region
{
    __cy_byte_align8 *init; /* Initial contents of this region.  */
    __cy_byte_align8 *data; /* Start address of region.  */
    size_t init_size;       /* Size of initial data.  */
    size_t zero_size;       /* Additional size to be zeroed.  */
};

extern const struct __cy_region __cy_regions[];
extern const char __cy_region_num __attribute__((weak));
#define __cy_region_num ((size_t)&__cy_region_num)


/*******************************************************************************
* System Calls of the Red Hat newlib C Library
*******************************************************************************/


/*******************************************************************************
* Function Name: _exit
****************************************************************************//**
*
* Exit a program without cleaning up files. If your system doesn't provide
* this, it is best to avoid linking with subroutines that require it (exit,
* system).
*
* \param status: Status caused program exit.
*
*******************************************************************************/
__attribute__((weak))
void _exit(int status)
{
    CyHalt((uint8) status);

    while(1)
    {

    }
}


/*******************************************************************************
* Function Name: _sbrk
****************************************************************************//**
*
* Increase program data space. As malloc and related functions depend on this,
* it is useful to have a working implementation. The following suffices for a
* standalone system; it exploits the symbol end automatically defined by the
* GNU linker.
*
* \param nbytes: The number of bytes requested (if the parameter value is positive)
* from the heap or returned back to the heap (if the parameter value is
* negative).
*
*******************************************************************************/
__attribute__((weak))
void * _sbrk (int nbytes)
{
    extern int  end;            /* Symbol defined by linker map. Start of free memory (as symbol). */
    void *      returnValue;

    /* The statically held previous end of the heap, with its initialization. */
    static uint8 *heapPointer = (uint8 *) &end;                 /* Previous end */

    if (((heapPointer + nbytes) - (uint8 *) &end) <= CYDEV_HEAP_SIZE)
    {
        returnValue  = (void *) heapPointer;
        heapPointer += nbytes;
    }
    else
    {
        errno = ENOMEM;
        returnValue = (void *) -1;
    }

    return (returnValue);
}


/*******************************************************************************
* Function Name: Start_c
****************************************************************************//**
*
* This function handles initializing the .data and .bss sections in
* preparation for running the standard c code.  Once initialization is complete
* it will call main().  This function will never return.
*
*******************************************************************************/
void Start_c(void)  __attribute__ ((noreturn, noinline));
void Start_c(void)
{
    #ifdef CY_BOOT_START_C_CALLBACK
        CyBoot_Start_c_Callback();
    #else
        unsigned regions = __cy_region_num;
        const struct __cy_region *rptr = __cy_regions;

        /* Initialize memory */
        for (regions = __cy_region_num; regions != 0u; regions--)
        {
            uint32 *src = (uint32 *)rptr->init;
            uint32 *dst = (uint32 *)rptr->data;
            unsigned limit = rptr->init_size;
            unsigned count;

            for (count = 0u; count != limit; count += sizeof (uint32))
            {
                *dst = *src;
                dst++;
                src++;
            }
            limit = rptr->zero_size;
            for (count = 0u; count != limit; count += sizeof (uint32))
            {
                *dst = 0u;
                dst++;
            }

            rptr++;
        }

        /* Invoke static objects constructors */
        __libc_init_array();
        (void) main();

        while (1)
        {
            /* If main returns, make sure we don't return. */
        }

    #endif /* CY_BOOT_START_C_CALLBACK */
}


/*******************************************************************************
* Function Name: Reset
****************************************************************************//**
*
* This function handles the reset interrupt for the GCC toolchain.  This is
* the first bit of code that is executed at startup.
*
*******************************************************************************/
void Reset(void)
{
    #if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLE || CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)

        /* The bootloadable application image is started at Reset() handler
        * as a result of a branch instruction execution from the bootloader.
        * So, the stack pointer needs to be reset to be sure that
        * there is no garbage in the stack.
        */
        __asm volatile ("MSR msp, %0\n" : : "r" ((uint32)&__cy_stack) : "sp");
    #endif /* CYDEV_PROJ_TYPE_LOADABLE */

    #if(CY_IP_SRSSLT)
        CySysWdtDisable();
    #endif  /* (CY_IP_SRSSLT) */

    #if ((CYDEV_BOOTLOADER_ENABLE) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
        CyBtldr_CheckLaunch();
    #endif /* ((CYDEV_BOOTLOADER_ENABLE) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)) */
    Start_c();
}

#elif defined (__ICCARM__)


/*******************************************************************************
* Function Name: __low_level_init
****************************************************************************//**
*
* This function performs early initializations for the IAR Embedded
* Workbench IDE. It is executed in the context of reset interrupt handler
* before the data sections are initialized.
*
* \return The value that determines whether or not data sections should be
* initialized by the system startup code:
*    0 - skip data sections initialization;
*    1 - initialize data sections;
*
*******************************************************************************/
#pragma inline = never
int __low_level_init(void)
{
    #if(CY_IP_SRSSLT)
        CySysWdtDisable();
    #endif  /* (CY_IP_SRSSLT) */

#if ((CYDEV_BOOTLOADER_ENABLE) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
    CyBtldr_CheckLaunch();
#endif /* ((CYDEV_BOOTLOADER_ENABLE) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)) */

    /* Initialize data sections */
    __iar_data_init3();

    initialize_psoc();

    return 0;
}

#endif /* __GNUC__ */


/*******************************************************************************
* Ram Interrupt Vector table storage area. Must be placed at 0x20000000.
*******************************************************************************/

#if defined (__ICCARM__)
    #pragma location=".ramvectors"
#elif defined (__ARMCC_VERSION)
    #ifndef CY_SYS_RAM_VECTOR_SECTION
        #define CY_SYS_RAM_VECTOR_SECTION __attribute__((section(".ramvectors"), zero_init))
    #endif /* CY_SYS_RAM_VECTOR_SECTION */
    CY_SYS_RAM_VECTOR_SECTION    
#else
    #ifndef CY_SYS_RAM_VECTOR_SECTION
        #define CY_SYS_RAM_VECTOR_SECTION CY_SECTION(".ramvectors")
    #endif /* CY_SYS_RAM_VECTOR_SECTION */
    CY_SYS_RAM_VECTOR_SECTION
#endif  /* defined (__ICCARM__) */
cyisraddress CyRamVectors[CY_NUM_VECTORS];


/*******************************************************************************
* Rom Interrupt Vector table storage area. Must be 256-byte aligned.
*******************************************************************************/

#if defined(__ARMCC_VERSION)
    /* Suppress diagnostic message 1296-D: extended constant initialiser used */
    #pragma diag_suppress 1296
#endif  /* defined(__ARMCC_VERSION) */

#if defined (__ICCARM__)
    #pragma location=".romvectors"
    const intvec_elem __vector_table[CY_NUM_ROM_VECTORS] =
#else
    #ifndef CY_SYS_ROM_VECTOR_SECTION
        #define CY_SYS_ROM_VECTOR_SECTION CY_SECTION(".romvectors")
    #endif /* CY_SYS_ROM_VECTOR_SECTION */
    CY_SYS_ROM_VECTOR_SECTION
    const cyisraddress RomVectors[CY_NUM_ROM_VECTORS] =
#endif  /* defined (__ICCARM__) */
{
    CY_SYS_INITIAL_STACK_POINTER,   /* The initial stack pointer  0 */
    #if defined (__ICCARM__) /* The reset handler          1 */
        __iar_program_start,
    #else
        (cyisraddress)&Reset,
    #endif  /* defined (__ICCARM__) */
    &IntDefaultHandler,      /* The NMI handler            2 */
    &IntDefaultHandler,      /* The hard fault handler     3 */
};

#if defined(__ARMCC_VERSION)
    #pragma diag_default 1296
#endif  /* defined(__ARMCC_VERSION) */


/*******************************************************************************
* Function Name: initialize_psoc
****************************************************************************//**
*
* This function is used to initialize the PSoC chip before calling main.
*
*******************************************************************************/
#if(defined(__GNUC__) && !defined(__ARMCC_VERSION))
__attribute__ ((constructor(101)))
#endif  /* (defined(__GNUC__) && !defined(__ARMCC_VERSION)) */
void initialize_psoc(void)
{
    uint32 indexInit;

    #if(CY_IP_CPUSSV2)
        #if (CY_IP_CPUSS_CM0)
            /***********************************************************************
            * Make sure that Vector Table is located at 0000_0000 in Flash, before
            * accessing RomVectors or calling functions that may be placed in
            * .psocinit (cyfitter_cfg and ClockSetup). Note The CY_CPUSS_CONFIG_REG
            * register is retention for the specified device family.
            ***********************************************************************/
            CY_CPUSS_CONFIG_REG &= (uint32) ~CY_CPUSS_CONFIG_VECT_IN_RAM;
        #endif /* (CY_IP_CPUSS_CM0) */
    #endif  /* (CY_IP_CPUSSV2) */

    /* Set Ram interrupt vectors to default functions. */
    for (indexInit = 0u; indexInit < CY_NUM_VECTORS; indexInit++)
    {
        CyRamVectors[indexInit] = (indexInit < CY_NUM_ROM_VECTORS) ?
            #if defined (__ICCARM__)
                __vector_table[indexInit].__fun : &IntDefaultHandler;
            #else
                RomVectors[indexInit] : &IntDefaultHandler;
            #endif  /* defined (__ICCARM__) */
    }

    /* Initialize configuration registers. */
    cyfitter_cfg();

    #if !defined (__ICCARM__)
        /* Actually, no need to clean this variable, just to make compiler happy. */
        cySysNoInitDataValid = 0u;
    #endif  /* !defined (__ICCARM__) */

    #if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_STANDARD)

        /* Need to make sure that this variable will not be optimized out */
        if (0u == cyBtldrRunType)
        {
            cyBtldrRunType = 0u;
        }

    #endif  /* (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_STANDARD) */

    #if (CY_IP_CPUSS_CM0)
        /* Vector Table is located at 0x2000:0000 in SRAM */
        CY_CPUSS_CONFIG_REG |= CY_CPUSS_CONFIG_VECT_IN_RAM;
	#else
		(*(uint32 *)CYREG_CM0P_VTOR) = CY_CPUSS_CONFIG_VECT_ADDR_IN_RAM;
    #endif /* (CY_IP_CPUSS_CM0) */
}


/* [] END OF FILE */
