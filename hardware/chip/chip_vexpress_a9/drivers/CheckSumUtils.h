/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __CheckSumUtils_h__
#define __CheckSumUtils_h__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/******************CRC-8 XMODEM       x8+x5+x4+1******************************

*******************************************************************************/

typedef struct {
    uint8_t crc;
} CRC8_Context;

/**
 * @brief             initialize the CRC8 Context
 *
 * @param inContext   holds CRC8 result
 *
 * @retval            none
 */
void CRC8_Init( CRC8_Context *inContext );


/**
 * @brief             Caculate the CRC8 result
 *
 * @param inContext   holds CRC8 result during caculation process
 * @param inSrc       input data
 * @param inLen       length of input data
 *
 * @retval            none
 */
void CRC8_Update( CRC8_Context *inContext, const void *inSrc, size_t inLen );


/**
 * @brief             output CRC8 result
 *
 * @param inContext   holds CRC8 result
 * @param outResutl   holds CRC8 final result
 *
 * @retval            none
 */
void CRC8_Final( CRC8_Context *inContext, uint8_t *outResult );
/**
  * @}
  */



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
    uint16_t crc;
} CRC16_Context;

/**
 * @brief             initialize the CRC16 Context
 *
 * @param inContext   holds CRC16 result
 *
 * @retval            none
 */
void CRC16_Init( CRC16_Context *inContext );


/**
 * @brief             Caculate the CRC16 result
 *
 * @param inContext   holds CRC16 result during caculation process
 * @param inSrc       input data
 * @param inLen       length of input data
 *
 * @retval            none
 */
void CRC16_Update( CRC16_Context *inContext, const void *inSrc, size_t inLen );


/**
 * @brief             output CRC16 result
 *
 * @param inContext   holds CRC16 result
 * @param outResutl   holds CRC16 final result
 *
 * @retval            none
 */
void CRC16_Final( CRC16_Context *inContext, uint16_t *outResult );

/**
  * @}
  */

/**
  * @}
  */
#endif //__CheckSumUtils_h__


