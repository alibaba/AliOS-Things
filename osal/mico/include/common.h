/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __Common_h__
#define __Common_h__

// ==== STD LIB ====
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined __GNUC__
#include "platform_toolchain.h"
#elif defined ( __ICCARM__ )
#include "EWARM/platform_toolchain.h"
#elif defined ( __CC_ARM ) //KEIL
#include "RVMDK/platform_toolchain.h"
#endif 

//#ifdef CONFIG_PLATFORM_8195A
//#include "rtl_lib.h"
//#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TARGET_RT_LITTLE_ENDIAN

/* Use this macro to define an RTOS-aware interrupt handler where RTOS
 * primitives can be safely accessed
 *
 * @usage:
 * WWD_RTOS_DEFINE_ISR( my_irq_handler )
 * {
 *     // Do something here
 * }
 */
#if defined( __GNUC__ )

/* Section where IRQ handlers are placed */
#define IRQ_SECTION ".text.irq"

#define MICO_RTOS_DEFINE_ISR( function ) \
        void function( void ); \
        __attribute__(( interrupt, used, section(IRQ_SECTION) )) void function( void )


#elif defined ( __IAR_SYSTEMS_ICC__ )

#define MICO_RTOS_DEFINE_ISR( function ) \
        __irq __root void function( void ); \
        __irq __root void function( void )

#elif defined ( __CC_ARM ) //KEIL    

#define MICO_RTOS_DEFINE_ISR( function ) \
        void function( void ); \
        __attribute__(( used )) void function( void )   
        
#else

#define MICO_RTOS_DEFINE_ISR( function ) \
        void function( void );

#endif

// ==== COMPATIBILITY TYPES
//typedef uint8_t         Boolean;
typedef uint8_t         mico_bool_t;

#define MICO_FALSE      (0)
#define MICO_TRUE       (1)

#if( !defined( INT_MAX ) )
    #define INT_MAX     2147483647
#endif


// ==== MiCO Timer Typedef ====
#define NANOSECONDS  1000000UL
#define MICROSECONDS 1000
#define MILLISECONDS      (1)
#define SECONDS           (1000)
#define MINUTES           (60 * SECONDS)
#define HOURS             (60 * MINUTES)
#define DAYS              (24 * HOURS)

typedef uint32_t  mico_time_t;        /**< Time value in milliseconds */
typedef uint32_t  mico_utc_time_t;    /**< UTC Time in seconds        */
typedef uint64_t  mico_utc_time_ms_t; /**< UTC Time in milliseconds   */

// ==== OSStatus ====
typedef int         OSStatus;

#define kNoErr                      0       //! No error occurred.
#define kGeneralErr                -1       //! General error.
#define kInProgressErr              1       //! Operation in progress.

// Generic error codes are in the range -6700 to -6779.

#define kGenericErrorBase           -6700   //! Starting error code for all generic errors.

#define kUnknownErr                 -6700   //! Unknown error occurred.
#define kOptionErr                  -6701   //! Option was not acceptable.
#define kSelectorErr                -6702   //! Selector passed in is invalid or unknown.
#define kExecutionStateErr          -6703   //! Call made in the wrong execution state (e.g. called at interrupt time).
#define kPathErr                    -6704   //! Path is invalid, too long, or otherwise not usable.
#define kParamErr                   -6705   //! Parameter is incorrect, missing, or not appropriate.
#define kUserRequiredErr            -6706   //! User interaction is required.
#define kCommandErr                 -6707   //! Command invalid or not supported.
#define kIDErr                      -6708   //! Unknown, invalid, or inappropriate identifier.
#define kStateErr                   -6709   //! Not in appropriate state to perform operation.
#define kRangeErr                   -6710   //! Index is out of range or not valid.
#define kRequestErr                 -6711   //! Request was improperly formed or not appropriate.
#define kResponseErr                -6712   //! Response was incorrect or out of sequence.
#define kChecksumErr                -6713   //! Checksum does not match the actual data.
#define kNotHandledErr              -6714   //! Operation was not handled (or not handled completely).
#define kVersionErr                 -6715   //! Version is not correct or not compatible.
#define kSignatureErr               -6716   //! Signature did not match what was expected.
#define kFormatErr                  -6717   //! Unknown, invalid, or inappropriate file/data format.
#define kNotInitializedErr          -6718   //! Action request before needed services were initialized.
#define kAlreadyInitializedErr      -6719   //! Attempt made to initialize when already initialized.
#define kNotInUseErr                -6720   //! Object not in use (e.g. cannot abort if not already in use).
#define kAlreadyInUseErr            -6721   //! Object is in use (e.g. cannot reuse active param blocks).
#define kTimeoutErr                 -6722   //! Timeout occurred.
#define kCanceledErr                -6723   //! Operation canceled (successful cancel).
#define kAlreadyCanceledErr         -6724   //! Operation has already been canceled.
#define kCannotCancelErr            -6725   //! Operation could not be canceled (maybe already done or invalid).
#define kDeletedErr                 -6726   //! Object has already been deleted.
#define kNotFoundErr                -6727   //! Something was not found.
#define kNoMemoryErr                -6728   //! Not enough memory was available to perform the operation.
#define kNoResourcesErr             -6729   //! Resources unavailable to perform the operation.
#define kDuplicateErr               -6730   //! Duplicate found or something is a duplicate.
#define kImmutableErr               -6731   //! Entity is not changeable.
#define kUnsupportedDataErr         -6732   //! Data is unknown or not supported.
#define kIntegrityErr               -6733   //! Data is corrupt.
#define kIncompatibleErr            -6734   //! Data is not compatible or it is in an incompatible format.
#define kUnsupportedErr             -6735   //! Feature or option is not supported.
#define kUnexpectedErr              -6736   //! Error occurred that was not expected.
#define kValueErr                   -6737   //! Value is not appropriate.
#define kNotReadableErr             -6738   //! Could not read or reading is not allowed.
#define kNotWritableErr             -6739   //! Could not write or writing is not allowed.
#define kBadReferenceErr            -6740   //! An invalid or inappropriate reference was specified.
#define kFlagErr                    -6741   //! An invalid, inappropriate, or unsupported flag was specified.
#define kMalformedErr               -6742   //! Something was not formed correctly.
#define kSizeErr                    -6743   //! Size was too big, too small, or not appropriate.
#define kNameErr                    -6744   //! Name was not correct, allowed, or appropriate.
#define kNotPreparedErr             -6745   //! Device or service is not ready.
#define kReadErr                    -6746   //! Could not read.
#define kWriteErr                   -6747   //! Could not write.
#define kMismatchErr                -6748   //! Something does not match.
#define kDateErr                    -6749   //! Date is invalid or out-of-range.
#define kUnderrunErr                -6750   //! Less data than expected.
#define kOverrunErr                 -6751   //! More data than expected.
#define kEndingErr                  -6752   //! Connection, session, or something is ending.
#define kConnectionErr              -6753   //! Connection failed or could not be established.
#define kAuthenticationErr          -6754   //! Authentication failed or is not supported.
#define kOpenErr                    -6755   //! Could not open file, pipe, device, etc.
#define kTypeErr                    -6756   //! Incorrect or incompatible type (e.g. file, data, etc.).
#define kSkipErr                    -6757   //! Items should be or was skipped.
#define kNoAckErr                   -6758   //! No acknowledge.
#define kCollisionErr               -6759   //! Collision occurred (e.g. two on bus at same time).
#define kBackoffErr                 -6760   //! Backoff in progress and operation intentionally failed.
#define kNoAddressAckErr            -6761   //! No acknowledge of address.
#define kInternalErr                -6762   //! An error internal to the implementation occurred.
#define kNoSpaceErr                 -6763   //! Not enough space to perform operation.
#define kCountErr                   -6764   //! Count is incorrect.
#define kEndOfDataErr               -6765   //! Reached the end of the data (e.g. recv returned 0).
#define kWouldBlockErr              -6766   //! Would need to block to continue (e.g. non-blocking read/write).
#define kLookErr                    -6767   //! Special case that needs to be looked at (e.g. interleaved data).
#define kSecurityRequiredErr        -6768   //! Security is required for the operation (e.g. must use encryption).
#define kOrderErr                   -6769   //! Order is incorrect.
#define kUpgradeErr                 -6770   //! Must upgrade.
#define kAsyncNoErr                 -6771   //! Async operation successfully started and is now in progress.
#define kDeprecatedErr              -6772   //! Operation or data is deprecated.
#define kPermissionErr              -6773   //! Permission denied.

#define kGenericErrorEnd            -6779   //! Last generic error code (inclusive)


// ==== C TYPE SAFE MACROS ====
//---------------------------------------------------------------------------------------------------------------------------
/*! @group      ctype safe macros
    @abstract   Wrappers for the ctype.h macros make them safe when used with signed characters.
    @discussion

    Some implementations of the ctype.h macros use the character value to directly index into a table.
    This can lead to crashes and other problems when used with signed characters if the character value
    is greater than 127 because the values 128-255 will appear to be negative if viewed as a signed char.
    A negative subscript to an array causes it to index before the beginning and access invalid memory.

    To work around this, these *_safe wrappers mask the value and cast it to an unsigned char.
*/

#define isalnum_safe( X )       isalnum(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define isalpha_safe( X )       isalpha(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define iscntrl_safe( X )       iscntrl(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define isdigit_safe( X )       isdigit(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define isgraph_safe( X )       isgraph(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define islower_safe( X )       islower(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define isoctal_safe( X )       isoctal(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define isprint_safe( X )       isprint(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define ispunct_safe( X )       ispunct(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define isspace_safe( X )       isspace(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define isupper_safe( X )       isupper(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define isxdigit_safe( X )      isxdigit( ( (unsigned char)( ( X ) & 0xFF ) ) )
#define tolower_safe( X )       tolower(  ( (unsigned char)( ( X ) & 0xFF ) ) )
#define toupper_safe( X )       toupper(  ( (unsigned char)( ( X ) & 0xFF ) ) )


// ==== SHA DEFINES ====
#define SHA_DIGEST_LENGTH                   20
#define SHA_CTX                             SHA1Context
#define SHA1_Init( CTX )                    SHA1Reset( (CTX) )
#define SHA1_Update( CTX, PTR, LEN )        SHA1Input( (CTX), (PTR), (LEN) )
#define SHA1_Final( DIGEST, CTX )           SHA1Result( (CTX), (DIGEST) )
#define SHA1( PTR, LEN, DIGEST )            SHA1Direct( (PTR), (LEN), (DIGEST) )

#define SHA512_DIGEST_LENGTH                64
#define SHA512_CTX                          SHA512Context
#define SHA512_Init( CTX )                  SHA512Reset( (CTX) )
#define SHA512_Update( CTX, PTR, LEN )      SHA512Input( (CTX), (PTR), (LEN) )
#define SHA512_Final( DIGEST, CTX )         SHA512Result( (CTX), (DIGEST) )
#define SHA512( PTR, LEN, DIGEST )          SHA512Direct( (PTR), (LEN), (DIGEST) )

#define SHA3_DIGEST_LENGTH                  64
#define SHA3_CTX                            SHA3_CTX_compat
#define SHA3_Init( CTX )                    SHA3_Init_compat( (CTX) )
#define SHA3_Update( CTX, PTR, LEN )        SHA3_Update_compat( (CTX), (PTR), (LEN) )
#define SHA3_Final( DIGEST, CTX )           SHA3_Final_compat( (DIGEST), (CTX) )
#define SHA3( PTR, LEN, DIGEST )            SHA3_compat( (PTR), (LEN), DIGEST )


// ==== MIN / MAX ====
//---------------------------------------------------------------------------------------------------------------------------
/*! @function   Min
    @abstract   Returns the lesser of X and Y.
*/
#if( !defined( Min ) )
    #define Min( X, Y )     ( ( ( X ) < ( Y ) ) ? ( X ) : ( Y ) )
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @function   Max
    @abstract   Returns the greater of X and Y.
*/
#if( !defined( Max ) )
    #define Max( X, Y )     ( ( ( X ) > ( Y ) ) ? ( X ) : ( Y ) )
#endif


// ==== Alignment / Endian safe read/write/swap macros ====
#define ReadBig16( PTR ) \
    ( (uint16_t)( \
        ( ( (uint16_t)( (uint8_t *)(PTR) )[ 0 ] ) << 8 ) | \
          ( (uint16_t)( (uint8_t *)(PTR) )[ 1 ] ) ) )

#define ReadBig32( PTR ) \
    ( (uint32_t)( \
        ( ( (uint32_t)( (uint8_t *)(PTR) )[ 0 ] ) << 24 ) | \
        ( ( (uint32_t)( (uint8_t *)(PTR) )[ 1 ] ) << 16 ) | \
        ( ( (uint32_t)( (uint8_t *)(PTR) )[ 2 ] ) <<  8 ) | \
          ( (uint32_t)( (uint8_t *)(PTR) )[ 3 ] ) ) )

#define ReadBig48( PTR ) \
    ( (uint64_t)( \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 0 ] ) << 40 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 1 ] ) << 32 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 2 ] ) << 24 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 3 ] ) << 16 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 4 ] ) <<  8 ) | \
          ( (uint64_t)( (uint8_t *)(PTR) )[ 5 ] ) ) )

#define ReadBig64( PTR ) \
    ( (uint64_t)( \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 0 ] ) << 56 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 1 ] ) << 48 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 2 ] ) << 40 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 3 ] ) << 32 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 4 ] ) << 24 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 5 ] ) << 16 ) | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 6 ] ) <<  8 ) | \
          ( (uint64_t)( (uint8_t *)(PTR) )[ 7 ] ) ) )

// Big endian Writing
#define WriteBig16( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)(   (X)         & 0xFF ); \
    \
    }   while( 0 )

#define WriteBig32( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)( ( (X) >> 24 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)( ( (X) >> 16 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 2 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 3 ] = (uint8_t)(   (X)         & 0xFF ); \
    \
    }   while( 0 )

#define WriteBig48( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)( ( (X) >> 40 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)( ( (X) >> 32 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 2 ] = (uint8_t)( ( (X) >> 24 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 3 ] = (uint8_t)( ( (X) >> 16 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 4 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 5 ] = (uint8_t)(   (X)         & 0xFF ); \
    \
    }   while( 0 )

#define WriteBig64( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)( ( (X) >> 56 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)( ( (X) >> 48 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 2 ] = (uint8_t)( ( (X) >> 40 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 3 ] = (uint8_t)( ( (X) >> 32 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 4 ] = (uint8_t)( ( (X) >> 24 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 5 ] = (uint8_t)( ( (X) >> 16 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 6 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 7 ] = (uint8_t)(   (X)         & 0xFF ); \
    \
    }   while( 0 )

// Little endian reading
#define ReadLittle16( PTR ) \
    ( (uint16_t)( \
          ( (uint16_t)( (uint8_t *)(PTR) )[ 0 ] ) | \
        ( ( (uint16_t)( (uint8_t *)(PTR) )[ 1 ] ) <<  8 ) ) )

#define ReadLittle32( PTR ) \
    ( (uint32_t)( \
          ( (uint32_t)( (uint8_t *)(PTR) )[ 0 ] ) | \
        ( ( (uint32_t)( (uint8_t *)(PTR) )[ 1 ] ) <<  8 ) | \
        ( ( (uint32_t)( (uint8_t *)(PTR) )[ 2 ] ) << 16 ) | \
        ( ( (uint32_t)( (uint8_t *)(PTR) )[ 3 ] ) << 24 ) ) )

#define ReadLittle48( PTR ) \
    ( (uint64_t)( \
          ( (uint64_t)( (uint8_t *)(PTR) )[ 0 ] )           | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 1 ] ) <<  8 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 2 ] ) << 16 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 3 ] ) << 24 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 4 ] ) << 32 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 5 ] ) << 40 ) ) )

#define ReadLittle64( PTR ) \
    ( (uint64_t)( \
          ( (uint64_t)( (uint8_t *)(PTR) )[ 0 ] )           | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 1 ] ) <<  8 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 2 ] ) << 16 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 3 ] ) << 24 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 4 ] ) << 32 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 5 ] ) << 40 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 6 ] ) << 48 )   | \
        ( ( (uint64_t)( (uint8_t *)(PTR) )[ 7 ] ) << 56 ) ) )

// Little endian writing
#define WriteLittle16( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)(   (X)         & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
    \
    }   while( 0 )

#define WriteLittle32( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)(   (X)         & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 2 ] = (uint8_t)( ( (X) >> 16 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 3 ] = (uint8_t)( ( (X) >> 24 ) & 0xFF ); \
    \
    }   while( 0 )

#define WriteLittle48( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)(   (X)         & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 2 ] = (uint8_t)( ( (X) >> 16 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 3 ] = (uint8_t)( ( (X) >> 24 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 4 ] = (uint8_t)( ( (X) >> 32 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 5 ] = (uint8_t)( ( (X) >> 40 ) & 0xFF ); \
    \
    }   while( 0 )

#define WriteLittle64( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)(   (X)         & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 2 ] = (uint8_t)( ( (X) >> 16 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 3 ] = (uint8_t)( ( (X) >> 24 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 4 ] = (uint8_t)( ( (X) >> 32 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 5 ] = (uint8_t)( ( (X) >> 40 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 6 ] = (uint8_t)( ( (X) >> 48 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 7 ] = (uint8_t)( ( (X) >> 56 ) & 0xFF ); \
    \
    }   while( 0 )


#if( !defined( TARGET_RT_LITTLE_ENDIAN ) && !defined( TARGET_RT_BIG_ENDIAN ) )
    #error unknown byte order - update your Makefile to define the target platform endianness as TARGET_RT_BIG_ENDIAN / TARGET_RT_LITTLE_ENDIAN
#endif

#if( !defined( TARGET_RT_LITTLE_ENDIAN ) )
    #define ReadHost16( PTR )           ReadBig16( (PTR) )
    #define ReadHost32( PTR )           ReadBig32( (PTR) )
    #define ReadHost48( PTR )           ReadBig48( (PTR) )
    #define ReadHost64( PTR )           ReadBig64( (PTR) )

    #define WriteHost16( PTR, X )       WriteBig16( (PTR), (X) )
    #define WriteHost32( PTR, X )       WriteBig32( (PTR), (X) )
    #define WriteHost48( PTR, X )       WriteBig48( (PTR), (X) )
    #define WriteHost64( PTR, X )       WriteBig64( (PTR), (X) )
#else
    #define ReadHost16( PTR )           ReadLittle16( (PTR) )
    #define ReadHost32( PTR )           ReadLittle32( (PTR) )
    #define ReadHost48( PTR )           ReadLittle48( (PTR) )
    #define ReadHost64( PTR )           ReadLittle64( (PTR) )

    #define WriteHost16( PTR, X )       WriteLittle16( (PTR), (X) )
    #define WriteHost32( PTR, X )       WriteLittle32( (PTR), (X) )
    #define WriteHost48( PTR, X )       WriteLittle48( (PTR), (X) )
    #define WriteHost64( PTR, X )       WriteLittle64( (PTR), (X) )
#endif

// Unconditional swap read/write.
#if( !defined( TARGET_RT_LITTLE_ENDIAN ) )
    #define ReadSwap16( PTR )           ReadLittle16( (PTR) )
    #define ReadSwap32( PTR )           ReadLittle32( (PTR) )
    #define ReadSwap48( PTR )           ReadLittle48( (PTR) )
    #define ReadSwap64( PTR )           ReadLittle64( (PTR) )

    #define WriteSwap16( PTR, X )       WriteLittle16( (PTR), (X) )
    #define WriteSwap32( PTR, X )       WriteLittle32( (PTR), (X) )
    #define WriteSwap48( PTR, X )       WriteLittle48( (PTR), (X) )
    #define WriteSwap64( PTR, X )       WriteLittle64( (PTR), (X) )
#else
    #define ReadSwap16( PTR )           ReadBig16( (PTR) )
    #define ReadSwap32( PTR )           ReadBig32( (PTR) )
    #define ReadSwap48( PTR )           ReadBig48( (PTR) )
    #define ReadSwap64( PTR )           ReadBig64( (PTR) )

    #define WriteSwap16( PTR, X )       WriteBig16( (PTR), (X) )
    #define WriteSwap32( PTR, X )       WriteBig32( (PTR), (X) )
    #define WriteSwap48( PTR, X )       WriteBig48( (PTR), (X) )
    #define WriteSwap64( PTR, X )       WriteBig64( (PTR), (X) )
#endif

// Memory swaps
#if( !defined( TARGET_RT_LITTLE_ENDIAN ) )
    #define HostToBig16Mem( SRC, LEN, DST )         do {} while( 0 )
    #define BigToHost16Mem( SRC, LEN, DST )         do {} while( 0 )

    #define LittleToHost16Mem( SRC, LEN, DST )      Swap16Mem( (SRC), (LEN), (DST) )
    #define LittleToHost16Mem( SRC, LEN, DST )      Swap16Mem( (SRC), (LEN), (DST) )
#else
    #define HostToBig16Mem( SRC, LEN, DST )         Swap16Mem( (SRC), (LEN), (DST) )
    #define BigToHost16Mem( SRC, LEN, DST )         Swap16Mem( (SRC), (LEN), (DST) )

    #define HostToLittle16Mem( SRC, LEN, DST )      do {} while( 0 )
    #define LittleToHost16Mem( SRC, LEN, DST )      do {} while( 0 )
#endif

// Unconditional endian swaps
#if !defined (Swap16)
#define Swap16( X ) \
    ( (uint16_t)( \
        ( ( ( (uint16_t)(X) ) << 8 ) & UINT16_C( 0xFF00 ) ) | \
        ( ( ( (uint16_t)(X) ) >> 8 ) & UINT16_C( 0x00FF ) ) ) )
#endif

#if !defined (Swap32)
#define Swap32( X ) \
    ( (uint32_t)( \
        ( ( ( (uint32_t)(X) ) << 24 ) & UINT32_C( 0xFF000000 ) ) | \
        ( ( ( (uint32_t)(X) ) <<  8 ) & UINT32_C( 0x00FF0000 ) ) | \
        ( ( ( (uint32_t)(X) ) >>  8 ) & UINT32_C( 0x0000FF00 ) ) | \
        ( ( ( (uint32_t)(X) ) >> 24 ) & UINT32_C( 0x000000FF ) ) ) )
#endif

#if !defined (Swap64)
#define Swap64( X ) \
    ( (uint64_t)( \
        ( ( ( (uint64_t)(X) ) << 56 ) & UINT64_C( 0xFF00000000000000 ) ) | \
        ( ( ( (uint64_t)(X) ) << 40 ) & UINT64_C( 0x00FF000000000000 ) ) | \
        ( ( ( (uint64_t)(X) ) << 24 ) & UINT64_C( 0x0000FF0000000000 ) ) | \
        ( ( ( (uint64_t)(X) ) <<  8 ) & UINT64_C( 0x000000FF00000000 ) ) | \
        ( ( ( (uint64_t)(X) ) >>  8 ) & UINT64_C( 0x00000000FF000000 ) ) | \
        ( ( ( (uint64_t)(X) ) >> 24 ) & UINT64_C( 0x0000000000FF0000 ) ) | \
        ( ( ( (uint64_t)(X) ) >> 40 ) & UINT64_C( 0x000000000000FF00 ) ) | \
        ( ( ( (uint64_t)(X) ) >> 56 ) & UINT64_C( 0x00000000000000FF ) ) ) )
#endif

// Host<->Network/Big endian swaps
#if( !defined( TARGET_RT_LITTLE_ENDIAN ) )
    #define hton16( X )     (X)
    #define ntoh16( X )     (X)

    #define hton32( X )     (X)
    #define ntoh32( X )     (X)

    #define hton64( X )     (X)
    #define ntoh64( X )     (X)
#else
    #define hton16( X )     Swap16( X )
    #define ntoh16( X )     Swap16( X )

    #define hton32( X )     Swap32( X )
    #define ntoh32( X )     Swap32( X )

    #define hton64( X )     Swap64( X )
    #define ntoh64( X )     Swap64( X )
#endif

    #define htons( X )      hton16( X )
    #define ntohs( X )      ntoh16( X )

    #define htonl( X )      hton32( X )
    #define ntohl( X )      ntoh32( X )

//---------------------------------------------------------------------------------------------------------------------------
/*! @function   BitArray
    @abstract   Macros for working with bit arrays.
    @discussion

    This treats bit numbers starting from the left so bit 0 is 0x80 in byte 0, bit 1 is 0x40 in bit 0,
    bit 8 is 0x80 in byte 1, etc. For example, the following ASCII art shows how the bits are arranged:

                                 1 1 1 1 1 1 1 1 1 1 2 2 2 2
    Bit      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |    x          |x              |  x           x| = 0x20 0x80 0x41 (bits 2, 8, 17, and 23).
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    Byte    0               1               2
*/
#define BitArray_MinBytes( ARRAY, N_BYTES )         memrlen( (ARRAY), (N_BYTES) )
#define BitArray_MaxBytes( BITS )                   ( ( (BITS) + 7 ) / 8 )
#define BitArray_MaxBits( ARRAY_BYTES )             ( (ARRAY_BYTES) * 8 )
#define BitArray_Clear( ARRAY_PTR, ARRAY_BYTES )    memset( (ARRAY_PTR), 0, (ARRAY_BYTES) );
#define BitArray_GetBit( PTR, LEN, BIT ) \
    ( ( (BIT) < BitArray_MaxBits( (LEN) ) ) && ( (PTR)[ (BIT) / 8 ] & ( 1 << ( 7 - ( (BIT) & 7 ) ) ) ) )
#define BitArray_SetBit( ARRAY, BIT )               ( (ARRAY)[ (BIT) / 8 ] |=  ( 1 << ( 7 - ( (BIT) & 7 ) ) ) )
#define BitArray_ClearBit( ARRAY, BIT )             ( (ARRAY)[ (BIT) / 8 ] &= ~( 1 << ( 7 - ( (BIT) & 7 ) ) ) )

//---------------------------------------------------------------------------------------------------------------------------
/*! @group      BitRotates
    @abstract   Rotates X COUNT bits to the left or right.
*/
#define ROTL( X, N, SIZE )          ( ( (X) << (N) ) | ( (X) >> ( (SIZE) - N ) ) )
#define ROTR( X, N, SIZE )          ( ( (X) >> (N) ) | ( (X) << ( (SIZE) - N ) ) )

#define ROTL32( X, N )              ROTL( (X), (N), 32 )
#define ROTR32( X, N )              ROTR( (X), (N), 32 )

#define ROTL64( X, N )              ROTL( (X), (N), 64 )
#define ROTR64( X, N )              ROTR( (X), (N), 64 )

#define RotateBitsLeft( X, N )      ROTL( (X), (N), sizeof( (X) ) * 8 )
#define RotateBitsRight( X, N )     ROTR( (X), (N), sizeof( (X) ) * 8 )

// ==== Macros for minimum-width integer constants ====
#if( !defined( INT8_C ) )
    #define INT8_C( value )         value
#endif

#if( !defined( INT16_C ) )
    #define INT16_C( value )        value
#endif

#if( !defined( INT32_C ) )
    #define INT32_C( value )        value
#endif

#define INT64_C_safe( value )       INT64_C( value )
#if( !defined( INT64_C ) )
    #if( defined( _MSC_VER ) )
        #define INT64_C( value )    value ## i64
    #else
        #define INT64_C( value )    value ## LL
    #endif
#endif

#define UINT8_C_safe( value )       UINT8_C( value )
#if( !defined( UINT8_C ) )
    #define UINT8_C( value )        value ## U
#endif

#define UINT16_C_safe( value )      UINT16_C( value )
#if( !defined( UINT16_C ) )
    #define UINT16_C( value )       value ## U
#endif

#define UINT32_C_safe( value )      UINT32_C( value )
#if( !defined( UINT32_C ) )
    #define UINT32_C( value )       value ## U
#endif

#define UINT64_C_safe( value )      UINT64_C( value )
#if( !defined( UINT64_C ) )
    #if( defined( _MSC_VER ) )
        #define UINT64_C( value )   value ## UI64
    #else
        #define UINT64_C( value )   value ## ULL
    #endif
#endif

// ==== SOCKET MACROS ====
#define IsValidSocket( X )                  ( ( X ) >= 0 )

/* Suppress unused parameter warning */
#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x) ( (void)(x) )
#endif

/* Suppress unused variable warning */
#ifndef UNUSED_VARIABLE
#define UNUSED_VARIABLE(x) ( (void)(x) )
#endif

/* Suppress unused variable warning occurring due to an assert which is disabled in release mode */
#ifndef REFERENCE_DEBUG_ONLY_VARIABLE
#define REFERENCE_DEBUG_ONLY_VARIABLE(x) ( (void)(x) )
#endif
#ifdef __cplusplus
}
#endif

#ifdef MICO_ENABLE_MALLOC_DEBUG
#include "malloc_debug.h"
extern void malloc_print_mallocs           ( void );
#else
#define calloc_named( name, nelems, elemsize) calloc ( nelems, elemsize )
#define calloc_named_hideleak( name, nelems, elemsize )  calloc ( nelems, elemsize )
#define realloc_named( name, ptr, size )      realloc( ptr, size )
#define malloc_named( name, size )            malloc ( size )
#define malloc_named_hideleak( name, size )   malloc ( size )
#define malloc_set_name( name )
#define malloc_leak_set_ignored( global_flag )
#define malloc_leak_set_base( global_flag )
#define malloc_leak_check( thread, global_flag )
#define malloc_transfer_to_curr_thread( block )
#define malloc_transfer_to_thread( block, thread )
#define malloc_print_mallocs( void )
#define malloc_debug_startup_finished( )
#endif /* ifdef MICO_ENABLE_MALLOC_DEBUG */

/**
 ******************************************************************************
 * Convert a nibble into a hex character
 *
 * @param[in] nibble  The value of the nibble in the lower 4 bits
 *
 * @return    The hex character corresponding to the nibble
 */
static inline ALWAYS_INLINE char nibble_to_hexchar( uint8_t nibble )
{
    if (nibble > 9)
    {
        return (char)('A' + (nibble - 10));
    }
    else
    {
        return (char) ('0' + nibble);
    }
}


/**
 ******************************************************************************
 * Convert a nibble into a hex character
 *
 * @param[in] nibble  The value of the nibble in the lower 4 bits
 *
 * @return    The hex character corresponding to the nibble
 */
static inline ALWAYS_INLINE char hexchar_to_nibble( char hexchar, uint8_t* nibble )
{
    if ( ( hexchar >= '0' ) && ( hexchar <= '9' ) )
    {
        *nibble = (uint8_t)( hexchar - '0' );
        return 0;
    }
    else if ( ( hexchar >= 'A' ) && ( hexchar <= 'F' ) )
    {
        *nibble = (uint8_t) ( hexchar - 'A' + 10 );
        return 0;
    }
    else if ( ( hexchar >= 'a' ) && ( hexchar <= 'f' ) )
    {
        *nibble = (uint8_t) ( hexchar - 'a' + 10 );
        return 0;
    }
    return -1;
}


#endif // __Common_h__

