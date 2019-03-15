/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __Debug_h__
#define __Debug_h__

#include "mico_rtos.h"
#include "platform_assert.h"

// ==== LOGGING ====
#ifdef __GNUC__
#define SHORT_FILE __FILENAME__
#else
#define SHORT_FILE strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__
#endif

#define YesOrNo(x) (x ? "YES" : "NO")

#ifdef DEBUG
#ifndef MICO_DISABLE_STDIO
#ifndef NO_MICO_RTOS
extern int mico_debug_enabled;
extern mico_mutex_t stdio_tx_mutex;

#define custom_log(N, M, ...) do {if (mico_debug_enabled==0)break;\
                                      mico_rtos_lock_mutex( &stdio_tx_mutex );\
                                      printf("[%ld][%s: %s:%4d] " M "\r\n", mico_rtos_get_time(), N, SHORT_FILE, __LINE__, ##__VA_ARGS__);\
                                      mico_rtos_unlock_mutex( &stdio_tx_mutex );}while(0==1)

#ifndef MICO_ASSERT_INFO_DISABLE
#define debug_print_assert(A,B,C,D,E,F) do {if (mico_debug_enabled==0)break;\
                                                     mico_rtos_lock_mutex( &stdio_tx_mutex );\
                                                     printf("[%ld][MICO:%s:%s:%4d] **ASSERT** %s""\r\n", mico_rtos_get_time(), D, F, E, (C!=NULL) ? C : "" );\
                                                     mico_rtos_unlock_mutex( &stdio_tx_mutex );}while(0==1)
#else  // !MICO_ASSERT_INFO_ENABLE
#define debug_print_assert(A,B,C,D,E,F)
#endif  // MICO_ASSERT_INFO_ENABLE

#ifdef TRACE
#define custom_log_trace(N) do {if (mico_debug_enabled==0)break;\
                                        mico_rtos_lock_mutex( &stdio_tx_mutex );\
                                        printf("[%s: [TRACE] %s] %s()\r\n", N, SHORT_FILE, __PRETTY_FUNCTION__);\
                                        mico_rtos_unlock_mutex( &stdio_tx_mutex );}while(0==1)
#else  // !TRACE
#define custom_log_trace(N)
#endif // TRACE  
#else // NO_MICO_RTOS  
#define custom_log(N, M, ...) do {printf("[%s: %s:%4d] " M "\r\n",  N, SHORT_FILE, __LINE__, ##__VA_ARGS__);}while(0==1)


#ifndef MICO_ASSERT_INFO_DISABLE
#define debug_print_assert(A,B,C,D,E,F) do {printf("[MICO:%s:%s:%4d] **ASSERT** %s""\r\n", D, F, E, (C!=NULL) ? C : "" );}while(0==1)
#else
#define debug_print_assert(A,B,C,D,E,F)
#endif

#ifdef TRACE
#define custom_log_trace(N) do {printf("[%s: [TRACE] %s] %s()\r\n", N, SHORT_FILE, __PRETTY_FUNCTION__);}while(0==1)
#else  // !TRACE
#define custom_log_trace(N)
#endif // TRACE  
#endif
#else
#define custom_log(N, M, ...)

#define custom_log_trace(N)

#define debug_print_assert(A,B,C,D,E,F)
#endif   //MICO_DISABLE_STDIO                                      
#else // DEBUG = 0
// IF !DEBUG, make the logs NO-OP
#define custom_log(N, M, ...)

#define custom_log_trace(N)

#define debug_print_assert(A,B,C,D,E,F)
#endif // DEBUG

// ==== PLATFORM TIMEING FUNCTIONS ====
#ifdef TIME_PLATFORM
#define function_timer_log(M, N, ...) fprintf(stderr, "[FUNCTION TIMER: " N "()] " M "\n", ##__VA_ARGS__)

#define TIMEPLATFORM( FUNC, FUNC_NAME )                                             \
                do                                                                      \
                {                                                                       \
                    struct timespec startTime;                                          \
                    clock_gettime(CLOCK_MONOTONIC, &startTime);                         \
                    { FUNC; }                                                           \
                    struct timespec endTime;                                            \
                    clock_gettime(CLOCK_MONOTONIC, &endTime);                           \
                    struct timespec timeDiff = TimeDifference( startTime, endTime );    \
                    function_timer_log("%lld us",                                    \
                                        FUNC_NAME,                                   \
                                        ElapsedTimeInMicroseconds( timeDiff ));      \
                }                                                                       \
                while( 1==0 )
#else
#define function_timer_log(M, N, ...)

#define TIMEPLATFORM( FUNC, FUNC_NAME )                                             \
                do                                                                      \
                {                                                                       \
                    { FUNC; }                                                           \
                }                                                                       \
                while( 1==0 )
#endif


// ==== BRANCH PREDICTION & EXPRESSION EVALUATION ====
#if( !defined( unlikely ) )
//#define unlikely( EXPRESSSION )     __builtin_expect( !!(EXPRESSSION), 0 )
#define unlikely( EXPRESSSION )     !!(EXPRESSSION)
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    check
    @abstract   Check that an expression is true (non-zero).
    @discussion

    If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
    function name, etc.) using the default debugging output method.

    Code inside check() statements is not compiled into production builds.
*/

#if( !defined( check ) )
#define check( X )                                                                                  \
        do                                                                                              \
        {                                                                                               \
            if( unlikely( !(X) ) )                                                                      \
            {                                                                                           \
                debug_print_assert( 0, #X, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );             \
            }                                                                                           \
                                                                                                        \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    check_string
    @abstract   Check that an expression is true (non-zero) with an explanation.
    @discussion

    If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
    function name, etc.) using the default debugging output method.

    Code inside check() statements is not compiled into production builds.
*/

#if( !defined( check_string ) )
#define check_string( X, STR )                                                                                  \
        do                                                                                              \
        {                                                                                               \
            if( unlikely( !(X) ) )                                                                      \
            {                                                                                           \
                debug_print_assert( 0, #X, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );              \
                MICO_ASSERTION_FAIL_ACTION();                                                           \
            }                                                                                           \
                                                                                                        \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require
    @abstract   Requires that an expression evaluate to true.
    @discussion

    If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
    function name, etc.) using the default debugging output method then jumps to a label.
*/

#if( !defined( require ) )
#define require( X, LABEL )                                                                             \
        do                                                                                                  \
        {                                                                                                   \
            if( unlikely( !(X) ) )                                                                          \
            {                                                                                               \
                debug_print_assert( 0, #X, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );                 \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_string
    @abstract   Requires that an expression evaluate to true with an explanation.
    @discussion

    If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
    function name, etc.) and a custom explanation string using the default debugging output method then jumps to a label.
*/

#if( !defined( require_string ) )
#define require_string( X, LABEL, STR )                                                                 \
        do                                                                                                  \
        {                                                                                                   \
            if( unlikely( !(X) ) )                                                                          \
            {                                                                                               \
                debug_print_assert( 0, #X, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );                  \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_quiet
    @abstract   Requires that an expression evaluate to true.
    @discussion

    If expression evalulates to false, this jumps to a label. No debugging information is printed.
*/

#if( !defined( require_quiet ) )
#define require_quiet( X, LABEL )                                                                       \
        do                                                                                                  \
        {                                                                                                   \
            if( unlikely( !(X) ) )                                                                          \
            {                                                                                               \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_noerr
    @abstract   Require that an error code is noErr (0).
    @discussion

    If the error code is non-0, this prints debugging information (actual expression string, file, line number,
    function name, etc.) using the default debugging output method then jumps to a label.
*/

#if( !defined( require_noerr ) )
#define require_noerr( ERR, LABEL )                                                                     \
        do                                                                                                  \
        {                                                                                                   \
            OSStatus        localErr;                                                                       \
                                                                                                            \
            localErr = (OSStatus)(ERR);                                                                     \
            if( unlikely( localErr != 0 ) )                                                                 \
            {                                                                                               \
                debug_print_assert( localErr, NULL, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );        \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_noerr_string
    @abstract   Require that an error code is noErr (0).
    @discussion

    If the error code is non-0, this prints debugging information (actual expression string, file, line number,
    function name, etc.), and a custom explanation string using the default debugging output method using the
    default debugging output method then jumps to a label.
*/

#if( !defined( require_noerr_string ) )
#define require_noerr_string( ERR, LABEL, STR )                                                         \
        do                                                                                                  \
        {                                                                                                   \
            OSStatus        localErr;                                                                       \
                                                                                                            \
            localErr = (OSStatus)(ERR);                                                                     \
            if( unlikely( localErr != 0 ) )                                                                 \
            {                                                                                               \
                debug_print_assert( localErr, NULL, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );         \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_noerr_action_string
    @abstract   Require that an error code is noErr (0).
    @discussion

    If the error code is non-0, this prints debugging information (actual expression string, file, line number,
    function name, etc.), and a custom explanation string using the default debugging output method using the
    default debugging output method then executes an action and jumps to a label.
*/

#if( !defined( require_noerr_action_string ) )
#define require_noerr_action_string( ERR, LABEL, ACTION, STR )                                          \
        do                                                                                                  \
        {                                                                                                   \
            OSStatus        localErr;                                                                       \
                                                                                                            \
            localErr = (OSStatus)(ERR);                                                                     \
            if( unlikely( localErr != 0 ) )                                                                 \
            {                                                                                               \
                debug_print_assert( localErr, NULL, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );         \
                { ACTION; }                                                                                 \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_noerr_quiet
    @abstract   Require that an error code is noErr (0).
    @discussion

    If the error code is non-0, this jumps to a label. No debugging information is printed.
*/

#if( !defined( require_noerr_quiet ) )
#define require_noerr_quiet( ERR, LABEL )                                                               \
        do                                                                                                  \
        {                                                                                                   \
            if( unlikely( (ERR) != 0 ) )                                                                    \
            {                                                                                               \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_noerr_action
    @abstract   Require that an error code is noErr (0) with an action to execute otherwise.
    @discussion

    If the error code is non-0, this prints debugging information (actual expression string, file, line number,
    function name, etc.) using the default debugging output method then executes an action and jumps to a label.
*/

#if( !defined( require_noerr_action ) )
#define require_noerr_action( ERR, LABEL, ACTION )                                                      \
        do                                                                                                  \
        {                                                                                                   \
            OSStatus        localErr;                                                                       \
                                                                                                            \
            localErr = (OSStatus)(ERR);                                                                     \
            if( unlikely( localErr != 0 ) )                                                                 \
            {                                                                                               \
                debug_print_assert( localErr, NULL, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );        \
                { ACTION; }                                                                                 \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_noerr_action_quiet
    @abstract   Require that an error code is noErr (0) with an action to execute otherwise.
    @discussion

    If the error code is non-0, this executes an action and jumps to a label. No debugging information is printed.
*/

#if( !defined( require_noerr_action_quiet ) )
#define require_noerr_action_quiet( ERR, LABEL, ACTION )                                                \
        do                                                                                                  \
        {                                                                                                   \
            if( unlikely( (ERR) != 0 ) )                                                                    \
            {                                                                                               \
                { ACTION; }                                                                                 \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_action
    @abstract   Requires that an expression evaluate to true with an action to execute otherwise.
    @discussion

    If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
    function name, etc.) using the default debugging output method then executes an action and jumps to a label.
*/

#if( !defined( require_action ) )
#define require_action( X, LABEL, ACTION )                                                              \
        do                                                                                                  \
        {                                                                                                   \
            if( unlikely( !(X) ) )                                                                          \
            {                                                                                               \
                debug_print_assert( 0, #X, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );                 \
                { ACTION; }                                                                                 \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_action_string
    @abstract   Requires that an expression evaluate to true with an explanation and action to execute otherwise.
    @discussion

    If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
    function name, etc.) and a custom explanation string using the default debugging output method then executes an
    action and jumps to a label.
*/

#if( !defined( require_action_string ) )
#define require_action_string( X, LABEL, ACTION, STR )                                                  \
        do                                                                                                  \
        {                                                                                                   \
            if( unlikely( !(X) ) )                                                                          \
            {                                                                                               \
                debug_print_assert( 0, #X, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );                  \
                { ACTION; }                                                                                 \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    require_action_quiet
    @abstract   Requires that an expression evaluate to true with an action to execute otherwise.
    @discussion

    If expression evalulates to false, this executes an action and jumps to a label. No debugging information is printed.
*/

#if( !defined( require_action_quiet ) )
#define require_action_quiet( X, LABEL, ACTION )                                                        \
        do                                                                                                  \
        {                                                                                                   \
            if( unlikely( !(X) ) )                                                                          \
            {                                                                                               \
                { ACTION; }                                                                                 \
                goto LABEL;                                                                                 \
            }                                                                                               \
                                                                                                            \
        }   while( 1==0 )
#endif

// ==== ERROR MAPPING ====
#define global_value_errno( VALUE )                 ( errno ? errno : kUnknownErr )

#define map_global_value_errno( TEST, VALUE )       ( (TEST) ? 0 : global_value_errno(VALUE) )
#define map_global_noerr_errno( ERR )               ( !(ERR) ? 0 : global_value_errno(ERR) )
#define map_fd_creation_errno( FD )                 ( IsValidFD( FD ) ? 0 : global_value_errno( FD ) )
#define map_noerr_errno( ERR )                      map_global_noerr_errno( (ERR) )

#define socket_errno( SOCK )                        ( errno ? errno : kUnknownErr )
#define socket_value_errno( SOCK, VALUE )           socket_errno( SOCK )
#define map_socket_value_errno( SOCK, TEST, VALUE ) ( (TEST) ? 0 : socket_value_errno( (SOCK), (VALUE) ) )
#define map_socket_noerr_errno( SOCK, ERR )         ( !(ERR) ? 0 : socket_errno( (SOCK) ) )


//---------------------------------------------------------------------------------------------------------------------------
/*! @defined    check_ptr_overlap
    @abstract   Checks that two ptrs do not overlap.
*/

#define check_ptr_overlap( P1, P1_SIZE, P2, P2_SIZE )                                   \
    do                                                                                  \
    {                                                                                   \
        check( !( ( (uintptr_t)(P1) >=     (uintptr_t)(P2) ) &&                         \
                  ( (uintptr_t)(P1) <  ( ( (uintptr_t)(P2) ) + (P2_SIZE) ) ) ) );       \
        check( !( ( (uintptr_t)(P2) >=     (uintptr_t)(P1) ) &&                         \
                  ( (uintptr_t)(P2) <  ( ( (uintptr_t)(P1) ) + (P1_SIZE) ) ) ) );       \
                                                                                        \
    }   while( 1==0 )

#define IsValidFD( X )              ( ( X ) >= 0 )


//------------------------------------------Memory debug------------------------------------------------------------------

typedef struct {
    int num_of_chunks; /**< number of free chunks*/
    int total_memory; /**< maximum total allocated space*/
    int allocted_memory; /**< total allocated space*/
    int free_memory; /**< total free space*/
} micoMemInfo_t;

#define MicoGetMemoryInfo           mico_memory_info

/**
 * @brief  Get memory usage information
 *
 * @param  None
 *
 * @return Point to structure of memory usage information in heap
 */
micoMemInfo_t *MicoGetMemoryInfo( void );

//---------------------------------------------------------------------------------------------------------------------------

#ifdef DEBUG
#include "platform_assert.h"
#define MICO_BREAK_IF_DEBUG( ) MICO_ASSERTION_FAIL_ACTION()
#else
#define MICO_BREAK_IF_DEBUG( )
#endif

#ifdef PRINT_PLATFORM_PERMISSION
int platform_wprint_permission(void);
#define PRINT_PLATFORM_PERMISSION_FUNC() platform_print_permission()
#else
#ifdef DEBUG
#define PRINT_PLATFORM_PERMISSION_FUNC() 1
#else
#define PRINT_PLATFORM_PERMISSION_FUNC() 0
#endif
#endif

/******************************************************
 *             Print declarations
 ******************************************************/
#define PRINT_ENABLE_LIB_INFO
#define PRINT_ENABLE_LIB_DEBUG
#define PRINT_ENABLE_LIB_ERROR

#define PRINT_MACRO(args) do {if (PRINT_PLATFORM_PERMISSION_FUNC()) printf args;} while(0==1)

/* printing macros for general SDK/Library functions*/
#ifdef PRINT_ENABLE_LIB_INFO
#define WPRINT_LIB_INFO(args) PRINT_MACRO(args)
#else
#define WPRINT_LIB_INFO(args)
#endif
#ifdef PRINT_ENABLE_LIB_DEBUG
#define WPRINT_LIB_DEBUG(args) PRINT_MACRO(args)
#else
#define WPRINT_LIB_DEBUG(args)
#endif
#ifdef PRINT_ENABLE_LIB_ERROR
#define WPRINT_LIB_ERROR(args) { PRINT_MACRO(args); MICO_BREAK_IF_DEBUG(); }
#else
#define WPRINT_LIB_ERROR(args) { MICO_BREAK_IF_DEBUG(); }
#endif


#endif // __Debug_h__

