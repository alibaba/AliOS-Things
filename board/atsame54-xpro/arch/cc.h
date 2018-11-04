/**
 * \file
 *
 * \brief lwIP abstraction layer.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef CC_H_INCLUDED
#define CC_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

/* Define platform endianness */
#if BYTE_ORDER != LITTLE_ENDIAN
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#endif

/* Types based on stdint.h */
typedef uint8_t   u8_t;
typedef int8_t    s8_t;
typedef uint16_t  u16_t;
typedef int16_t   s16_t;
typedef uint32_t  u32_t;
typedef int32_t   s32_t;
typedef uintptr_t mem_ptr_t;

/* Define (sn)printf formatters for these lwIP types */
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

/* Compiler hints for packing lwip's structures */
#if defined(__CC_ARM)
/* Setup PACKing macros for MDK Tools */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined(__ICCARM__)
/* Setup PACKing macros for EWARM Tools */
#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined(__GNUC__)
/* Setup PACKing macros for GCC Tools */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#else
#error "This compiler does not support."
#endif

#define LWIP_MAILBOX_QUEUE  1

/* define LWIP_COMPAT_MUTEX
    to let sys.h use binary semaphores instead of mutexes - as before in 1.3.2
    Refer CHANGELOG
*/
#define LWIP_COMPAT_MUTEX 0

/* Make lwip/arch.h define the codes which are used throughout */
#define LWIP_PROVIDE_ERRNO

/* Debug facilities. LWIP_DEBUG must be defined to read output */
#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)                                                                                          \
	{                                                                                                                  \
		printf x;                                                                                                      \
	}
#define LWIP_PLATFORM_ASSERT(x)                                                                                        \
	{                                                                                                                  \
		printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__);                                   \
		while (1)                                                                                                      \
			;                                                                                                          \
	}
#else
#define LWIP_PLATFORM_DIAG(x)                                                                                          \
	{                                                                                                                  \
		;                                                                                                              \
	}
#define LWIP_PLATFORM_ASSERT(x)                                                                                        \
	{                                                                                                                  \
		while (1)                                                                                                      \
			;                                                                                                          \
	}
#endif

#endif /* CC_H_INCLUDED */
