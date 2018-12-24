/**
 ******************************************************************************
 * @file    mx_debug.h
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Apr-2018
 * @brief   This header contains defines, macros, and functions to aid in
 *          debugging the project.
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#ifndef __MX_DEBUG_H__
#define __MX_DEBUG_H__

#include "mx_opt.h"
#include "mx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// ==== LOGGING ====
#ifdef __GNUC__
#define SHORT_FILE __FILENAME__
#else
#define SHORT_FILE strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__
#endif

#define YesOrNo(x) (x ? "YES" : "NO")

#ifdef MX_DEBUG
#ifndef MX_DISABLE_STDIO
    #define custom_log(N, M, ...) do {printf("[%s: %s:%4d] " M "\r\n",  N, SHORT_FILE, __LINE__, ##__VA_ARGS__);}while(0==1)
    #define custom_print(M, ...) do {printf( M, ##__VA_ARGS__);}while(0==1)

    #ifndef MX_ASSERT_INFO_DISABLE                                       
        #define debug_print_assert(A,B,C,D,E,F) do {printf("[MX:%s:%s:%4d] **ASSERT** %s""\r\n", D, F, E, (C!=NULL) ? C : "" );}while(0==1)
    #else 
        #define debug_print_assert(A,B,C,D,E,F)
    #endif

    #ifdef TRACE
        #define custom_log_trace(N) do {printf("[%s: [TRACE] %s] %s()\r\n", N, SHORT_FILE, __PRETTY_FUNCTION__);}while(0==1)
    #else  // !TRACE
        #define custom_log_trace(N)
    #endif // TRACE                                  
#else
    #define custom_log(N, M, ...)
    #define custom_print(M, ...)
    #define custom_log_trace(N)

    #define debug_print_assert(A,B,C,D,E,F)                                           
#endif   //MX_DISABLE_STDIO                                      
#else // DEBUG = 0
    // IF !DEBUG, make the logs NO-OP
    #define custom_log(N, M, ...)
    #define custom_print(M, ...)
    #define custom_log_trace(N)

    #define debug_print_assert(A,B,C,D,E,F)
#endif // DEBUG


/** Debug level: ALL messages*/
#define MX_DEBUG_LEVEL_ALL     0x00
/** Debug level: Warnings. bad checksums, dropped packets, ... */
#define MX_DEBUG_LEVEL_WARNING 0x01
/** Debug level: Serious. memory allocation failures, ... */
#define MX_DEBUG_LEVEL_SERIOUS 0x02
/** Debug level: Severe */
#define MX_DEBUG_LEVEL_SEVERE  0x03

#define MX_DEBUG_MASK_LEVEL    0x03
/* compatibility define only */
#define MX_DEBUG_LEVEL_OFF     MX_DEBUG_LEVEL_ALL

/** flag for LWIP_DEBUGF to enable that debug message */
#define MX_DEBUG_ON            0x80U
/** flag for LWIP_DEBUGF to disable that debug message */
#define MX_DEBUG_OFF           0x00U

#ifdef MX_DEBUG
#define MX_LOG(D, T, M, ...) do { \
                                   if ( ((D) & MX_DEBUG_ON) && \
                                        ((D) & MX_DEBUG_TYPES_ON) && \
                                        ((int16_t)((D) & MX_DEBUG_MASK_LEVEL) >= MX_DEBUG_MIN_LEVEL)) { \
                                        custom_log(T, M, ##__VA_ARGS__); \
                                   } \
                               } while(0)
#define MX_PRINT(D, M, ...) do { \
                                   if ( ((D) & MX_DEBUG_ON) && \
                                        ((D) & MX_DEBUG_TYPES_ON) && \
                                        ((int16_t)((D) & MX_DEBUG_MASK_LEVEL) >= MX_DEBUG_MIN_LEVEL)) { \
                                        custom_print(M, ##__VA_ARGS__); \
                                   } \
                               } while(0)
#define MX_LOG_TRACE(T) custom_log_trace(T)
#else
#define MX_LOG(D, T, M, ...)
#define MX_LOG_TRACE(T)
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
                MX_ASSERTION_FAIL_ACTION();                                                           \
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

#ifdef __cplusplus
} /*"C" */
#endif

#endif // __MX_DEBUG_H__

