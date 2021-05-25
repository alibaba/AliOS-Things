/******************************************************************************
 * @file     csky_common_tables.h
 * @brief    This file has extern declaration for common tables like
 *           Bitreverse, reciprocal etc which are used across different functions.
 * @version  V1.0
 * @date     20. Dec 2016
 ******************************************************************************/
/* ---------------------------------------------------------------------------
 * Copyright (C) 2016 CSKY Limited. All rights reserved.
 *
 * Redistribution and use of this software in source and binary forms,
 * with or without modification, are permitted provided that the following
 * conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of CSKY Ltd. nor the names of CSKY's contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission of CSKY Ltd.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------------- */

#ifndef _CSKY_COMMON_TABLES_H
#define _CSKY_COMMON_TABLES_H

#include "csky_math.h"

extern const uint16_t cskyBitRevTable[1024];
extern const q15_t cskyRecipTableQ15[64];
extern const q31_t cskyRecipTableQ31[64];
extern const uint32_t twiddleCoef_16[32];
extern const uint32_t twiddleCoef_32[64];
extern const uint32_t twiddleCoef_64[128];
extern const uint32_t twiddleCoef_128[256];
extern const uint32_t twiddleCoef_256[512];
extern const uint32_t twiddleCoef_512[1024];
extern const uint32_t twiddleCoef_1024[2048];
extern const uint32_t twiddleCoef_2048[4096];
extern const uint32_t twiddleCoef_4096[8192];
extern const q31_t twiddleCoef_16_q31[24];
extern const q31_t twiddleCoef_32_q31[48];
extern const q31_t twiddleCoef_64_q31[96];
extern const q31_t twiddleCoef_128_q31[192];
extern const q31_t twiddleCoef_256_q31[384];
extern const q31_t twiddleCoef_512_q31[768];
extern const q31_t twiddleCoef_1024_q31[1536];
extern const q31_t twiddleCoef_2048_q31[3072];
extern const q31_t twiddleCoef_4096_q31[6144];
extern const q15_t twiddleCoef_16_q15[24];
extern const q15_t twiddleCoef_32_q15[48];
extern const q15_t twiddleCoef_64_q15[96];
extern const q15_t twiddleCoef_128_q15[192];
extern const q15_t twiddleCoef_256_q15[384];
extern const q15_t twiddleCoef_512_q15[768];
extern const q15_t twiddleCoef_1024_q15[1536];
extern const q15_t twiddleCoef_2048_q15[3072];
extern const q15_t twiddleCoef_4096_q15[6144];
extern const float32_t twiddleCoef_rfft_32[32];
extern const float32_t twiddleCoef_rfft_64[64];
extern const float32_t twiddleCoef_rfft_128[128];
extern const float32_t twiddleCoef_rfft_256[256];
extern const float32_t twiddleCoef_rfft_512[512];
extern const float32_t twiddleCoef_rfft_1024[1024];
extern const float32_t twiddleCoef_rfft_2048[2048];
extern const float32_t twiddleCoef_rfft_4096[4096];
extern const float32_t twiddleCoef_rfft_8192[8192];

extern const q15_t twiddleCoef_fast_16_q15[24];
extern const q15_t twiddleCoef_fast_32_q15[56];
extern const q15_t twiddleCoef_fast_64_q15[120];
extern const q15_t twiddleCoef_fast_128_q15[248];
extern const q15_t twiddleCoef_fast_256_q15[504];
extern const q15_t twiddleCoef_fast_512_q15[1016];
extern const q15_t twiddleCoef_fast_1024_q15[2040];
extern const q15_t twiddleCoef_fast_2048_q15[4088];
extern const q15_t twiddleCoef_fast_4096_q15[8184];

extern const q31_t twiddleCoef_fast_16_q31[24];
extern const q31_t twiddleCoef_fast_32_q31[56];
extern const q31_t twiddleCoef_fast_64_q31[120];
extern const q31_t twiddleCoef_fast_128_q31[248];
extern const q31_t twiddleCoef_fast_256_q31[504];
extern const q31_t twiddleCoef_fast_512_q31[1016];
extern const q31_t twiddleCoef_fast_1024_q31[2040];
extern const q31_t twiddleCoef_fast_2048_q31[4088];
extern const q31_t twiddleCoef_fast_4096_q31[8184];

extern const uint32_t twiddleCoef_fast_16[24];
extern const uint32_t twiddleCoef_fast_32[56];
extern const uint32_t twiddleCoef_fast_64[120];
extern const uint32_t twiddleCoef_fast_128[248];
extern const uint32_t twiddleCoef_fast_256[504];
extern const uint32_t twiddleCoef_fast_512[1016];
extern const uint32_t twiddleCoef_fast_1024[2040];
extern const uint32_t twiddleCoef_fast_2048[4088];
extern const uint32_t twiddleCoef_fast_4096[8184];

extern const q15_t realCoefAQ15_8192[8192];
extern const q31_t realCoefAQ31_8192[8192];
extern const q15_t realCoefBQ15_8192[8192];
extern const q31_t realCoefBQ31_8192[8192];

/*Tables for RFFT.*/
extern const q15_t ALIGN4 realCoefAQ15_32[32];
extern const q15_t ALIGN4 realCoefAQ15_64[64];
extern const q15_t ALIGN4 realCoefAQ15_128[128];
extern const q15_t ALIGN4 realCoefAQ15_256[256];
extern const q15_t ALIGN4 realCoefAQ15_512[512];
extern const q15_t ALIGN4 realCoefAQ15_1024[1024];
extern const q15_t ALIGN4 realCoefAQ15_2048[2048];
extern const q15_t ALIGN4 realCoefAQ15_4096[4096];

extern const q15_t ALIGN4 realCoefBQ15_32[32];
extern const q15_t ALIGN4 realCoefBQ15_64[64];
extern const q15_t ALIGN4 realCoefBQ15_128[128];
extern const q15_t ALIGN4 realCoefBQ15_256[256];
extern const q15_t ALIGN4 realCoefBQ15_512[512];
extern const q15_t ALIGN4 realCoefBQ15_1024[1024];
extern const q15_t ALIGN4 realCoefBQ15_2048[2048];
extern const q15_t ALIGN4 realCoefBQ15_4096[4096];

extern const q31_t realCoefAQ31_32[32];
extern const q31_t realCoefAQ31_64[64];
extern const q31_t realCoefAQ31_128[128];
extern const q31_t realCoefAQ31_256[256];
extern const q31_t realCoefAQ31_512[512];
extern const q31_t realCoefAQ31_1024[1024];
extern const q31_t realCoefAQ31_2048[2048];
extern const q31_t realCoefAQ31_4096[4096];

extern const q31_t realCoefBQ31_32[32];
extern const q31_t realCoefBQ31_64[64];
extern const q31_t realCoefBQ31_128[128];
extern const q31_t realCoefBQ31_256[256];
extern const q31_t realCoefBQ31_512[512];
extern const q31_t realCoefBQ31_1024[1024];
extern const q31_t realCoefBQ31_2048[2048];
extern const q31_t realCoefBQ31_4096[4096];


extern const float32_t realCoefA[8192];
extern const float32_t realCoefB[8192];


/*Tables for DCT4*/
extern const q15_t ALIGN4 WeightsQ15_128[128+2];
extern const q15_t ALIGN4 WeightsQ15_512[512+2];
extern const q15_t ALIGN4 WeightsQ15_2048[2048+2];
extern const q15_t ALIGN4 WeightsQ15_8192[8192+2];

extern const q15_t ALIGN4 cos_factorsQ15_128[128];
extern const q15_t ALIGN4 cos_factorsQ15_512[512];
extern const q15_t ALIGN4 cos_factorsQ15_2048[2048];
extern const q15_t ALIGN4 cos_factorsQ15_8192[8192];


extern const q31_t WeightsQ31_128[128+2];
extern const q31_t WeightsQ31_512[512+2];
extern const q31_t WeightsQ31_2048[2048+2];
extern const q31_t WeightsQ31_8192[8192+2];

extern const q31_t cos_factorsQ31_128[128];
extern const q31_t cos_factorsQ31_512[512];
extern const q31_t cos_factorsQ31_2048[2048];
extern const q31_t cos_factorsQ31_8192[8192];


extern const float32_t Weights_128[128+2];
extern const float32_t Weights_512[512+2];
extern const float32_t Weights_2048[2048+2];
extern const float32_t Weights_8192[8192+2];

extern const float32_t cos_factors_128[128];
extern const float32_t cos_factors_512[512];
extern const float32_t cos_factors_2048[2048];
extern const float32_t cos_factors_8192[8192];

/* floating-point bit reversal tables */
#define CSKYBITREVINDEXTABLE__16_TABLE_LENGTH ((uint16_t)20  )
#define CSKYBITREVINDEXTABLE__32_TABLE_LENGTH ((uint16_t)48  )
#define CSKYBITREVINDEXTABLE__64_TABLE_LENGTH ((uint16_t)56  )
#define CSKYBITREVINDEXTABLE_128_TABLE_LENGTH ((uint16_t)208 )
#define CSKYBITREVINDEXTABLE_256_TABLE_LENGTH ((uint16_t)440 )
#define CSKYBITREVINDEXTABLE_512_TABLE_LENGTH ((uint16_t)448 )
#define CSKYBITREVINDEXTABLE1024_TABLE_LENGTH ((uint16_t)1800)
#define CSKYBITREVINDEXTABLE2048_TABLE_LENGTH ((uint16_t)3808)
#define CSKYBITREVINDEXTABLE4096_TABLE_LENGTH ((uint16_t)4032)

extern const uint16_t cskyBitRevIndexTable16[CSKYBITREVINDEXTABLE__16_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable32[CSKYBITREVINDEXTABLE__32_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable64[CSKYBITREVINDEXTABLE__64_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable128[CSKYBITREVINDEXTABLE_128_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable256[CSKYBITREVINDEXTABLE_256_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable512[CSKYBITREVINDEXTABLE_512_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable1024[CSKYBITREVINDEXTABLE1024_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable2048[CSKYBITREVINDEXTABLE2048_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable4096[CSKYBITREVINDEXTABLE4096_TABLE_LENGTH];

/* fixed-point bit reversal tables */
#define CSKYBITREVINDEXTABLE_FIXED___16_TABLE_LENGTH ((uint16_t)12  )
#define CSKYBITREVINDEXTABLE_FIXED___32_TABLE_LENGTH ((uint16_t)24  )
#define CSKYBITREVINDEXTABLE_FIXED___64_TABLE_LENGTH ((uint16_t)56  )
#define CSKYBITREVINDEXTABLE_FIXED__128_TABLE_LENGTH ((uint16_t)112 )
#define CSKYBITREVINDEXTABLE_FIXED__256_TABLE_LENGTH ((uint16_t)240 )
#define CSKYBITREVINDEXTABLE_FIXED__512_TABLE_LENGTH ((uint16_t)480 )
#define CSKYBITREVINDEXTABLE_FIXED_1024_TABLE_LENGTH ((uint16_t)992 )
#define CSKYBITREVINDEXTABLE_FIXED_2048_TABLE_LENGTH ((uint16_t)1984)
#define CSKYBITREVINDEXTABLE_FIXED_4096_TABLE_LENGTH ((uint16_t)4032)

extern const uint16_t cskyBitRevIndexTable_fixed_16[CSKYBITREVINDEXTABLE_FIXED___16_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable_fixed_32[CSKYBITREVINDEXTABLE_FIXED___32_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable_fixed_64[CSKYBITREVINDEXTABLE_FIXED___64_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable_fixed_128[CSKYBITREVINDEXTABLE_FIXED__128_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable_fixed_256[CSKYBITREVINDEXTABLE_FIXED__256_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable_fixed_512[CSKYBITREVINDEXTABLE_FIXED__512_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable_fixed_1024[CSKYBITREVINDEXTABLE_FIXED_1024_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable_fixed_2048[CSKYBITREVINDEXTABLE_FIXED_2048_TABLE_LENGTH];
extern const uint16_t cskyBitRevIndexTable_fixed_4096[CSKYBITREVINDEXTABLE_FIXED_4096_TABLE_LENGTH];

/* Tables for Fast Math Sine and Cosine */
extern const float32_t sinTable_f32[FAST_MATH_TABLE_SIZE + 1];
extern const q31_t sinTable_q31[FAST_MATH_TABLE_SIZE + 1];
extern const q15_t sinTable_q15[FAST_MATH_TABLE_SIZE + 1];

#endif /*  CSKY_COMMON_TABLES_H */
