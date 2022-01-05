/**
 * \file
 * \brief CryptoAuthLiub is meant to be portable across architectures, even
 *        non-Microchip architectures and compiler environments. This file is
 *        for isolating compiler specific macros.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */


#ifndef ATCA_COMPILER_H_
#define ATCA_COMPILER_H_

#if defined(__clang__)
/* Clang/LLVM. ---------------------------------------------- */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define ATCA_UINT16_HOST_TO_LE(x)  __builtin_bswap16(x)
#define ATCA_UINT16_LE_TO_HOST(x)  __builtin_bswap16(x)
#define ATCA_UINT32_HOST_TO_LE(x)  __builtin_bswap32(x)
#define ATCA_UINT32_HOST_TO_BE(x)  (x)
#define ATCA_UINT32_BE_TO_HOST(x)  (x)
#define ATCA_UINT64_HOST_TO_BE(x)  (x)
#define ATCA_UINT64_BE_TO_HOST(x)  (x)
#else
#define ATCA_UINT16_HOST_TO_LE(x)  (x)
#define ATCA_UINT16_LE_TO_HOST(x)  (x)
#define ATCA_UINT32_HOST_TO_LE(x)  (x)
#define ATCA_UINT32_HOST_TO_BE(x)  __builtin_bswap32(x)
#define ATCA_UINT32_BE_TO_HOST(x)  __builtin_bswap32(x)
#define ATCA_UINT64_HOST_TO_BE(x)  __builtin_bswap64(x)
#define ATCA_UINT64_BE_TO_HOST(x)  __builtin_bswap64(x)
#endif

#elif defined(__ICC) || defined(__INTEL_COMPILER)
/* Intel ICC/ICPC. ------------------------------------------ */

#elif defined(__GNUC__) || defined(__GNUG__)
/* GNU GCC/G++. --------------------------------------------- */
#if defined(__AVR32__)
#define ATCA_UINT16_HOST_TO_LE(x)  __builtin_bswap_16(x)
#define ATCA_UINT16_LE_TO_HOST(x)  __builtin_bswap_16(x)
#define ATCA_UINT32_HOST_TO_LE(x)  __builtin_bswap_32(x)
#define ATCA_UINT32_HOST_TO_BE(x)  (x)
#define ATCA_UINT32_BE_TO_HOST(x)  (x)
#define ATCA_UINT64_HOST_TO_BE(x)  (x)
#define ATCA_UINT64_BE_TO_HOST(x)  (x)
#define ATCA_NO_PRAGMA_PACK
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define ATCA_UINT16_HOST_TO_LE(x)  __builtin_bswap16(x)
#define ATCA_UINT16_LE_TO_HOST(x)  __builtin_bswap16(x)
#define ATCA_UINT32_HOST_TO_LE(x)  __builtin_bswap32(x)
#define ATCA_UINT32_HOST_TO_BE(x)  (x)
#define ATCA_UINT32_BE_TO_HOST(x)  (x)
#define ATCA_UINT64_HOST_TO_BE(x)  (x)
#define ATCA_UINT64_BE_TO_HOST(x)  (x)
#else
#define ATCA_UINT16_HOST_TO_LE(x)  (x)
#define ATCA_UINT16_LE_TO_HOST(x)  (x)
#define ATCA_UINT32_HOST_TO_LE(x)  (x)
#define ATCA_UINT32_HOST_TO_BE(x)  __builtin_bswap32(x)
#define ATCA_UINT32_BE_TO_HOST(x)  __builtin_bswap32(x)
#define ATCA_UINT64_HOST_TO_BE(x)  __builtin_bswap64(x)
#define ATCA_UINT64_BE_TO_HOST(x)  __builtin_bswap64(x)
#endif

#elif defined(__HP_cc) || defined(__HP_aCC)
/* Hewlett-Packard C/aC++. ---------------------------------- */

#elif defined(__IBMC__) || defined(__IBMCPP__)
/* IBM XL C/C++. -------------------------------------------- */

#elif defined(_MSC_VER)
/* Microsoft Visual Studio. --------------------------------- */
// MSVC is usually always little-endian architecture
#include <stdlib.h>
#define ATCA_UINT16_HOST_TO_LE(x)  (x)
#define ATCA_UINT16_LE_TO_HOST(x)  (x)
#define ATCA_UINT32_HOST_TO_LE(x)  (x)
#define ATCA_UINT32_HOST_TO_BE(x)  _byteswap_ulong(x)
#define ATCA_UINT32_BE_TO_HOST(x)  _byteswap_ulong(x)
#define ATCA_UINT64_HOST_TO_BE(x)  _byteswap_uint64(x)
#define ATCA_UINT64_BE_TO_HOST(x)  _byteswap_uint64(x)
#define strtok_r                   strtok_s

#elif defined(__PGI)
/* Portland Group PGCC/PGCPP. ------------------------------- */

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
/* Oracle Solaris Studio. ----------------------------------- */

#endif

#endif /* ATCA_COMPILER_H_ */
