#ifndef __psptypes_h__
#define __psptypes_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: psptypes.h$
* $Version : 3.0.3.0$
* $Date    : Sep-25-2008$
*
* Comments:
*
*   This file contains the definitions of the basic MQX types.
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                            STANDARD TYPES
*/

/*
**  The following typedefs allow us to minimize portability problems
**  due to the various C compilers (even for the same processor) not
**  agreeing on the sizes of "int"s and "short int"s and "longs".
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef  NULL
   #undef  NULL
#endif
#ifdef __cplusplus 
#define NULL 0
#else
#define NULL ((pointer)0)
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

#define LSB(a)      ((a)._byte.byte0)
#define MSB(a)      ((a)._byte.byte1)

#define LOWER_LSB(a)    ((a)._byte.byte0)
#define LOWER_MSB(a)    ((a)._byte.byte1)
#define UPPER_LSB(a)    ((a)._byte.byte2)
#define UPPER_MSB(a)    ((a)._byte.byte3)

#define _PTR_      *
#define _CODE_PTR_      *


#define BYTESWAP16(x)	(uint_16)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#define BYTESWAP32(val)	(uint_32)((BYTESWAP16((uint_32)(val) & (uint_32)0xFFFF) << 0x10) |  \
					(BYTESWAP16((uint_32)((val) >> 0x10))))
#define BIG_ENDIAN

#ifdef BIG_ENDIAN
#define ieee_htons(x)		(intu16)(x)
#define ieee_htonl(x)		(intu32)(x)
#define ieee_ntohs(x)		(intu16)(x)
#define ieee_ntohl(x)		(intu32)(x)
#else
#define ieee_htons(x)		BYTESWAP16(x)
#define ieee_htonl(x)		BYTESWAP32(x)
#define ieee_ntohs(x)		BYTESWAP16(x)
#define ieee_ntohl(x)		BYTESWAP32(x)
#endif

#define UNUSED(x)   (void)x;


typedef char _PTR_                    char_ptr;                   /* signed character       */
typedef unsigned char  uchar, _PTR_   uchar_ptr;                  /* unsigned character     */
typedef volatile char _PTR_                    vchar_ptr;         /* signed character       */
typedef volatile unsigned char  vuchar, _PTR_   vuchar_ptr;       /* unsigned character     */

typedef signed   char   int_8, _PTR_   int_8_ptr;                 /* 8-bit signed integer   */
typedef unsigned char  uint_8, _PTR_  uint_8_ptr;                 /* 8-bit signed integer   */
typedef volatile signed   char   vint_8, _PTR_   vint_8_ptr;      /* 8-bit volatile signed integer   */
typedef volatile unsigned char  vuint_8, _PTR_  vuint_8_ptr;      /* 8-bit volatile signed integer   */

typedef short int_16, _PTR_  int_16_ptr;                          /* 16-bit signed integer  */
typedef unsigned short uint_16, _PTR_ uint_16_ptr;                /* 16-bit unsigned integer*/
typedef volatile short vint_16, _PTR_  vint_16_ptr;               /* 16-bit volatile signed integer  */
typedef volatile unsigned short vuint_16, _PTR_ vuint_16_ptr;     /* 16-bit volatile unsigned integer*/

typedef long  int_32, _PTR_  int_32_ptr;                          /* 32-bit signed integer  */
typedef unsigned long  uint_32, _PTR_ uint_32_ptr;                /* 32-bit unsigned integer*/
typedef volatile          long  vint_32, _PTR_  vint_32_ptr;      /* 32-bit signed integer  */
typedef volatile unsigned long  vuint_32, _PTR_ vuint_32_ptr;     /* 32-bit unsigned integer*/

typedef long  long  int_64, _PTR_  int_64_ptr;                    /* 64-bit signed   */
typedef unsigned long long  uint_64, _PTR_ uint_64_ptr;           /* 64-bit unsigned */
typedef volatile   long  long  vint_64, _PTR_  vint_64_ptr;       /* 64-bit signed   */
typedef volatile unsigned long long  vuint_64, _PTR_ vuint_64_ptr;/* 64-bit unsigned */

typedef unsigned char  boolean;                                   /* Machine representation of a boolean */

typedef void _PTR_     pointer;                                   /* Machine representation of a pointer */

typedef union _BYTE                                               /* definition of 8 bit word */
{
    uint_8 _byte;
    struct
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    }Bit;
} BYTE;

typedef union _WORD                                               /* definition of 16 bit word */
{
    uint_16 _word;
    struct
    {
        uint_8 byte1;
        uint_8 byte0;
    }_byte;
    struct
    {
        BYTE HighB;
        BYTE LowB;
    }_Byte;
} WORD;


typedef union _DWORD                                              /* definition of 32 bit word */
{
    uint_32 _dword;
    struct
    {
        uint_8 byte3;
        uint_8 byte2;
        uint_8 byte1;
        uint_8 byte0;
    }_byte;
    struct
    {
        uint_16 word1;
        uint_16 word0;
    }_word;
    struct
    {
        BYTE Byte3;
        BYTE Byte2;
        BYTE Byte1;
        BYTE Byte0;
    }_Byte;
    struct
    {
        WORD Word1;
        WORD Word0;
    }_Word;
} DWORD;

#endif
/* EOF */
