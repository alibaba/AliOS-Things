/******************************************************************************
 * @file     csky_const_structs.h
 * @brief    This file has constant structs that are initialized for
 *           user convenience.  For example, some can be given as
 *           arguments to the csky_cfft_f32() function.
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

#ifndef _CSKY_CONST_STRUCTS_H
#define _CSKY_CONST_STRUCTS_H

#include "csky_math.h"
#include "csky_common_tables.h"

   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len16;
   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len32;
   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len64;
   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len128;
   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len256;
   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len512;
   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len1024;
   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len2048;
   extern const csky_cfft_instance_f32 csky_cfft_sR_f32_len4096;

   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len16;
   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len32;
   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len64;
   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len128;
   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len256;
   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len512;
   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len1024;
   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len2048;
   extern const csky_cfft_instance_q31 csky_cfft_sR_q31_len4096;

   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len16;
   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len32;
   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len64;
   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len128;
   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len256;
   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len512;
   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len1024;
   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len2048;
   extern const csky_cfft_instance_q15 csky_cfft_sR_q15_len4096;

   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len32;
   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len64;
   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len128;
   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len256;
   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len512;
   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len1024;
   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len2048;
   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len4096;
   extern csky_rfft_instance_q15 csky_rfft_sR_q15_len8192;


   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len32;
   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len64;
   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len128;
   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len256;
   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len512;
   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len1024;
   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len2048;
   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len4096;
   extern csky_rfft_instance_q15 csky_inv_rfft_sR_q15_len8192;


   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len32;
   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len64;
   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len128;
   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len256;
   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len512;
   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len1024;
   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len2048;
   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len4096;
   extern csky_rfft_instance_q31 csky_rfft_sR_q31_len8192;

   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len32;
   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len64;
   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len128;
   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len256;
   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len512;
   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len1024;
   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len2048;
   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len4096;
   extern csky_rfft_instance_q31 csky_inv_rfft_sR_q31_len8192;


   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len32;
   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len64;
   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len128;
   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len256;
   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len512;
   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len1024;
   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len2048;
   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len4096;
   extern csky_rfft_fast_instance_f32 csky_rfft_sR_f32_len8192;

   extern csky_dct4_instance_q15 csky_dct4_sR_q15_len128;
   extern csky_dct4_instance_q15 csky_dct4_sR_q15_len512;
   extern csky_dct4_instance_q15 csky_dct4_sR_q15_len2048;
   extern csky_dct4_instance_q15 csky_dct4_sR_q15_len8192;

   extern csky_dct4_instance_q31 csky_dct4_sR_q31_len128;
   extern csky_dct4_instance_q31 csky_dct4_sR_q31_len512;
   extern csky_dct4_instance_q31 csky_dct4_sR_q31_len2048;
   extern csky_dct4_instance_q31 csky_dct4_sR_q31_len8192;

   extern csky_dct4_instance_f32 csky_dct4_sR_f32_len128;
   extern csky_dct4_instance_f32 csky_dct4_sR_f32_len512;
   extern csky_dct4_instance_f32 csky_dct4_sR_f32_len2048;
   extern csky_dct4_instance_f32 csky_dct4_sR_f32_len8192;
#endif
