
#pragma once
//#include "platform_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IRQ_SECTION ".text.irq"

#if defined ( __GNUC__ )
/* GCC */
#define PLATFORM_DEFINE_NAKED_ISR( function ) \
        void function( void ); \
        __attribute__(( naked, interrupt, used, section( IRQ_SECTION ) )) void function( void )

#define PLATFORM_DEFINE_ISR( name ) \
        void name( void ); \
        __attribute__(( interrupt, used, section( IRQ_SECTION ) )) void name( void )

#elif defined ( __IAR_SYSTEMS_ICC__ )
/* IAR Systems */
#define PLATFORM_DEFINE_ISR( name ) \
        void name( void ); \
        __irq __root void name( void )

#else

#define PLATFORM_DEFINE_ISR( name )

#endif


#if defined( __GNUC__ )

#define PLATFORM_MAP_ISR( function, irq_handler ) \
        extern void irq_handler( void ); \
        __attribute__(( alias( #function ))) void irq_handler ( void );

#elif defined ( __IAR_SYSTEMS_ICC__ )

#define PLATFORM_MAP_ISR( function, irq_handler ) \
        extern void irq_handler( void ); \
        _Pragma( TO_STRING( weak irq_handler=function ) )

#else

#define PLATFORM_MAP_ISR( function, irq_handler )

#endif


#if defined( __GNUC__ )

#define PLATFORM_SET_DEFAULT_ISR( irq_handler, default_handler ) \
        __attribute__(( weak, alias( #default_handler ))) void irq_handler ( void );

#elif defined ( __IAR_SYSTEMS_ICC__ )

#define PLATFORM_SET_DEFAULT_ISR( irq_handler, default_handler ) \
        _Pragma( TO_STRING( weak irq_handler=default_handler ) )

#else

#define PLATFORM_SET_DEFAULT_ISR( irq_handler, default_handler )

#endif

#ifdef __cplusplus
} /*extern "C" */
#endif
