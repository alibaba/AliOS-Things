
#pragma once

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifdef __GNUC__
#define MICO_ASSERTION_FAIL_ACTION() __asm__("bkpt")
#elif defined ( __IAR_SYSTEMS_ICC__ )
#define MICO_ASSERTION_FAIL_ACTION() __asm("bkpt 0")
#elif defined ( __CC_ARM )
#define MICO_ASSERTION_FAIL_ACTION() __asm("bkpt 0")
#endif


#if defined ( __GNUC__ )

#if defined ( __clang__ )

static inline /*@noreturn@*/void MICO_TRIGGER_BREAKPOINT( void ) __attribute__((analyzer_noreturn))
{
    __asm__("bkpt");
}

#else

#define MICO_TRIGGER_BREAKPOINT( ) do { __asm__("bkpt"); } while (0)

#endif /* #if defined ( __clang__ ) */

#ifdef NO_VECTORS
#define MICO_DISABLE_INTERRUPTS() do { __asm__("" : : : "memory"); } while (0)
#define MICO_ENABLE_INTERRUPTS() do { __asm__("" : : : "memory"); } while (0)
#else
#define MICO_DISABLE_INTERRUPTS() do { __asm__("CPSID i" : : : "memory"); } while (0)
#define MICO_ENABLE_INTERRUPTS() do { __asm__("CPSIE i" : : : "memory"); } while (0)
#endif

#define CSPR_INTERRUPTS_DISABLED (0x80)

#elif defined ( __IAR_SYSTEMS_ICC__ )

#define MICO_TRIGGER_BREAKPOINT() do { __asm("bkpt 0"); } while (0)

#define MICO_DISABLE_INTERRUPTS() do { __asm("CPSID i"); } while (0)
#define MICO_ENABLE_INTERRUPTS() do { __asm("CPSIE i"); } while (0)

#endif
 
/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
