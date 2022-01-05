/** \file vproc_data_types.h
* vproc_data_types.h
*
*  This file is the header for all standard types used in the API code.
*
****************************************************************************
* Copyright Microsemi Inc, 2018. All rights reserved.
* Licensed under the MIT License. See LICENSE.txt in the project
* root for license information.
*
***************************************************************************/

#ifndef VP_API_TYPES_H
#define VP_API_TYPES_H
/* For maximum that can be stored in an int - if file exists in library */
#include "limits.h"
#include "esp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL (0)
#endif

#ifdef EXTERN
#undef EXTERN
#error EXTERN was redefined!
#endif /* undef EXTERN */

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif /* __cplusplus */

/********************* DECLARATIONS ***************************/
/* Constants */
#define FALSE   (0)     /* Boolean constant */
#define TRUE    (1)     /* Boolean constant */
#ifndef __cplusplus
/* C++ language provides a boolean data type; So no need to define
 * one more data type; Make use of it
 * NOTE: The 'C' potions of the VP-API assume C++ "bool" to be of the
 * same size as that of "char". Please make sure this assumption is correct.
 */

//typedef unsigned char bool;
#endif /* __cplusplus */
/****************** typedefs ***********************************/
/* These are the basic number types used */
/* for uint8, uint16, uint32, int8, int16, int32, bool */
//  PLATFORM SPECIFIC DEFINITIONS
typedef unsigned char   uchar;
typedef signed char    int8;
typedef unsigned char   UCharT;       // 8 bits unsigned -  PLATFORM SPECIFIC
typedef unsigned char   UInt8T;       // 8 bits unsigned -  PLATFORM SPECIFIC
typedef unsigned short  UInt16T;      // 16 bits unsigned -  PLATFORM SPECIFIC
typedef unsigned long   UInt32T;      // 32 bits unsigned -  PLATFORM SPECIFIC
typedef signed long     Int32T;       // 32 bits signed -  PLATFORM SPECIFIC
typedef unsigned char   uint8;        // 8 bits unsigned -  PLATFORM SPECIFIC
typedef unsigned short  uint16;       // 16 bits unsigned -  PLATFORM SPECIFIC
typedef   uint8*  uint8p;             // pointer to 8 bits unsigned -  PLATFORM SPECIFIC
typedef   uint16* uint16p;            // pointer to 16 bits unsigned -  PLATFORM SPECIFIC
typedef uint32_t   uint32;            // 32 bits unsigned -  PLATFORM SPECIFIC
typedef signed short    int16;        // 32 bits unsigned -  PLATFORM SPECIFIC
typedef   uint32* uint32p;
typedef   int8*   int8p;
typedef   int16*  int16p;
typedef   Int32T*  int32p;

/* external types */
/* Some compilers optimize the size of enumeration data types based on
 * the maximum data value assigned to the members of that data type.
 * 'Standard C' requires enumeration data types to be of the same size
 * as that of native 'int' implementation.
 * The VP-API from a portability persepective adds a 'dummy' member to
 * all enumeration data types that force the compilers to allocate the size
 * of enumeration data types to be equal to that of native 'int'
 * implementation */
#define FORCE_STANDARD_C_ENUM_SIZE  (INT_MAX)

/* Eliminate error messages that occur when comparing an enumeration constant
   < 0 */
#define FORCE_SIGNED_ENUM  (INT_MIN)

/* Define any API specific basic data type ranges (that are necessary) */
#define VP_INT16_MAX    (SHRT_MAX)
#define VP_INT16_MIN    (SHRT_MIN)
#define VP_INT32_MAX    (LONG_MAX)
#define VP_INT32_MIN    (LONG_MIN)

/*firmware data structures*/
typedef struct {
    uint16 buf[16];      /*the firmware data block to send to the device*/
    uint16  numWords;    /*the number of words within the block of data stored in buf[]*/
    uint32 targetAddr;   /*the target base address to write to register 0x00c of the device*/
    uint8 useTargetAddr; /*this value is either 0 or 1. When 1 the tarGetAddr must be written to the device*/
} twFwr;

typedef struct {
    twFwr* st_Fwr;
    uint32 byteCount;   /*The total number of bytes within the firmware - NOT USED*/
    uint8 havePrgmBase;
    uint32 prgmBase;
    uint32 execAddr;   /*The execution start address of the firmware in RAM*/
    uint16 twFirmwareStreamLen;  /*The number of blocks within the firmware*/
} twFirmware;

/*config record structures*/
typedef struct {
    uint16 reg;   /*the register */
    uint16 value; /*the value to write into reg */
} dataArr;

#ifdef __cplusplus
}
#endif

#endif /* VP_API_TYPES_H */

