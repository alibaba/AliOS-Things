/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __CRC_h__
#define __CRC_h__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


/******************CRC-16/XMODEM       x16+x12+x5+1******************************

*******************************************************************************/

/**
 * @brief             caculate the CRC16 result for each byte
 *
 * @param crcIn       The context to save CRC16 result.
 * @param byte        each byte of input data.
 *
 * @retval            return the CRC16 result
 */


typedef struct {
    unsigned short crc;
} CRC16_Ctx;

/**
 * @brief             initialize the CRC16 Ctx
 *
 * @param inCtx   holds CRC16 result
 *
 * @retval            none
 */
void ota_CRC16_Init( CRC16_Ctx *inCtx );


/**
 * @brief             Caculate the CRC16 result
 *
 * @param inCtx   holds CRC16 result during caculation process
 * @param inSrc       input data
 * @param inLen       length of input data
 *
 * @retval            none
 */
void ota_CRC16_Update( CRC16_Ctx *inCtx, const void *inSrc, size_t inLen );


/**
 * @brief             output CRC16 result
 *
 * @param inCtx   holds CRC16 result
 * @param outResutl   holds CRC16 final result
 *
 * @retval            none
 */
void ota_CRC16_Final( CRC16_Ctx *inCtx, unsigned short *outResult );

/**
 * @brief             caculate the CRC32 result for each byte
 *
 * @param crcIn       The context to save CRC32 result.
 * @param byte        each byte of input data.
 *
 * @retval            return the CRC32 result
 */


typedef struct {
    unsigned int crc;
} CRC32_Ctx;

/**
 * @brief             initialize the CRC32 Ctx
 *
 * @param inCtx   holds CRC32 result
 *
 * @retval            none
 */
void ota_CRC32_Init( CRC32_Ctx *inCtx );


/**
 * @brief             Caculate the CRC32 result
 *
 * @param inCtx   holds CRC32 result during caculation process
 * @param inSrc       input data
 * @param inLen       length of input data
 *
 * @retval            none
 */
void ota_CRC32_Update( CRC32_Ctx *inCtx, const void *inSrc, size_t inLen );


/**
 * @brief             output CRC32 result
 *
 * @param inCtx   holds CRC32 result
 * @param outResutl   holds CRC32 final result
 *
 * @retval            none
 */
void ota_CRC32_Final( CRC32_Ctx *inCtx, unsigned int *outResult );
#endif //__CRC_h__
