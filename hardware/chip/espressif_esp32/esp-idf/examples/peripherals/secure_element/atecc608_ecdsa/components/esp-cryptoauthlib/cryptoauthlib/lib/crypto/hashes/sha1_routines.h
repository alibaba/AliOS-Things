/**
 * \file
 * \brief Software implementation of the SHA1 algorithm.
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

#ifndef __SHA1_ROUTINES_DOT_H__
#define __SHA1_ROUTINES_DOT_H__

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifdef WIN32
#include <windows.h>
#include <assert.h>
#endif

#include <stdint.h>


#ifndef U8
#define U8 uint8_t
#endif

#ifndef U16
#define U16 uint16_t
#endif

#ifndef U32
#define U32 uint32_t
#endif


#ifndef memcpy_P
#define memcpy_P memmove
#endif

#ifndef strcpy_P
#define strcpy_P strcpy
#endif

#ifndef _WDRESET
#define _WDRESET()
#define _NOP()
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    U32 h[20 / 4];      // Ensure it's word aligned
    U32 buf[64 / 4];    // Ensure it's word aligned
    U32 byteCount;
    U32 byteCountHi;
} CL_HashContext;

#define leftRotate(x, n) (x) = (((x) << (n)) | ((x) >> (32 - (n))))

void shaEngine(U32 *buf, U32 *h);
void CL_hashInit(CL_HashContext *ctx);
void CL_hashUpdate(CL_HashContext *ctx, const U8 *src, int nbytes);
void CL_hashFinal(CL_HashContext *ctx, U8 *dest);
void CL_hash(U8 *msg, int msgBytes, U8 *dest);

#ifdef __cplusplus
}
#endif

#endif // __SHA1_ROUTINES_DOT_H__

