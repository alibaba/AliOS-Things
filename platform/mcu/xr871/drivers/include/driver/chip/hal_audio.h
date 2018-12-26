/**
  * @file  hal_audio.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HAL_AUDIO_H_H
#define HAL_AUDIO_H_H

typedef enum {
	DAIFMT_CBM_CFM,			/*(codec clk & FRM master)        use((i2s slave))*/
	DAIFMT_CBS_CFM,			/*(codec clk slave & FRM master)  not use*/
	DAIFMT_CBM_CFS,			/*(codec clk master & frame slave) not use*/
	DAIFMT_CBS_CFS,			/*(codec clk & FRM slave)         use(i2s master)*/
} PCM_ClkMode;

typedef enum {
	DAIFMT_I2S,			/*(standard i2s format).    use*/
	DAIFMT_RIGHT_J,			/*(right justfied format).*/
	DAIFMT_LEFT_J,			/*(left justfied format) */
	DAIFMT_DSP_A,			/*(pcm. MSB is available on 2nd BCLK rising edge after LRC rising edge). use*/
	DAIFMT_DSP_B,			/*(pcm. MSB is available on 1nd BCLK rising edge after LRC rising*/
} PCM_TranFmt;

typedef enum {
	DAIFMT_NB_NF,			/*(normal bit clock + frame)  use*/
	DAIFMT_NB_IF,			/*(normal BCLK + inv FRM) */
	DAIFMT_IB_NF,			/*(invert BCLK + nor FRM)  use*/
	DAIFMT_IB_IF,			/*(invert BCLK + FRM)*/
} PCM_SignalInv;


#endif
