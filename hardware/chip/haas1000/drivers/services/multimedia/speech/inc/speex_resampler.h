/* Copyright (C) 2007 Jean-Marc Valin

      
   File: speex_resampler.h
   Resampling code
      
   The design goals of this code are:
      - Very fast algorithm
      - Low memory requirement
      - Good *perceptual* quality (and not best SNR)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SPEEX_RESAMPLER_H
#define SPEEX_RESAMPLER_H



/* We don't support visibility on Win32 */
#define EXPORT

//#define RESAMPLE_CAL


typedef short spx_int16_t;
typedef int spx_int32_t;
typedef unsigned short spx_uint16_t;
typedef unsigned int spx_uint32_t;



typedef short spx_word16_t;
typedef int spx_word32_t;



#define SPEEX_RESAMPLER_QUALITY_MAX 10
#define SPEEX_RESAMPLER_QUALITY_MIN 0
#define SPEEX_RESAMPLER_QUALITY_DEFAULT 4
#define SPEEX_RESAMPLER_QUALITY_VOIP 3
#define SPEEX_RESAMPLER_QUALITY_DESKTOP 5

enum {
   RESAMPLER_ERR_SUCCESS         = 0,
   RESAMPLER_ERR_ALLOC_FAILED    = 1,
   RESAMPLER_ERR_BAD_STATE       = 2,
   RESAMPLER_ERR_INVALID_ARG     = 3,
   RESAMPLER_ERR_PTR_OVERLAP     = 4,
   
   RESAMPLER_ERR_MAX_ERROR
};


enum {
	RESAMPLER_MODE_CAL = 0,
	RESAMPLER_MODE_8KTO64K = 1,
	RESAMPLER_MODE_64KTO8K = 2,
	
	RESAMPLER_MODE_ERROR
};






#define RESAMPLE_MAXCHANNEL 1//xuml add
#define RESAMPLE_MAXSINCBUF 512//xuml add
#define RESAMPLE_MAXMEMBUF 680//xuml add


struct SpeexResamplerState_;
typedef struct SpeexResamplerState_ SpeexResamplerState;


typedef int(*resampler_basic_func)(SpeexResamplerState *, spx_uint32_t, const spx_word16_t *, spx_uint32_t *, spx_word16_t *, spx_uint32_t *);

struct SpeexResamplerState_ {
	spx_uint32_t in_rate;
	spx_uint32_t out_rate;
	spx_uint32_t num_rate;
	spx_uint32_t den_rate;

	int    quality;
	spx_uint32_t nb_channels;
	spx_uint32_t filt_len;
	spx_uint32_t mem_alloc_size;
	spx_uint32_t buffer_size;
	int          int_advance;
	int          frac_advance;
	float  cutoff;
	spx_uint32_t oversample;
	int          initialised;
	int          started;

	/* These are per-channel */
	spx_int32_t  last_sample[RESAMPLE_MAXCHANNEL];
	spx_uint32_t samp_frac_num[RESAMPLE_MAXCHANNEL];
	spx_uint32_t magic_samples[RESAMPLE_MAXCHANNEL];

	spx_word16_t mem[RESAMPLE_MAXMEMBUF];
	//spx_word16_t *mem;

#ifdef RESAMPLE_CAL
	spx_word16_t sinc_table[RESAMPLE_MAXSINCBUF];
#else
	const spx_word16_t *sinc_table;
#endif
	spx_uint32_t sinc_table_length;
	resampler_basic_func resampler_ptr;

	int    in_stride;
	int    out_stride;

	int    mode;


};





/** Create a new resampler with integer input and output rates.
 * @param nb_channels Number of channels to be processed
 * @param in_rate Input sampling rate (integer number of Hz).
 * @param out_rate Output sampling rate (integer number of Hz).
 * @param quality Resampling quality between 0 and 10, where 0 has poor quality
 * and 10 has very high quality.
 * @return Newly created resampler state
 * @retval NULL Error: not enough memory
 */
void speex_resampler_init(SpeexResamplerState *st,
						  spx_uint32_t nb_channels, 
                          spx_uint32_t in_rate, 
                          spx_uint32_t out_rate, 
                          int quality,
						  int mode,
                          int *err);

/** Create a new resampler with fractional input/output rates. The sampling 
 * rate ratio is an arbitrary rational number with both the numerator and 
 * denominator being 32-bit integers.
 * @param nb_channels Number of channels to be processed
 * @param ratio_num Numerator of the sampling rate ratio
 * @param ratio_den Denominator of the sampling rate ratio
 * @param in_rate Input sampling rate rounded to the nearest integer (in Hz).
 * @param out_rate Output sampling rate rounded to the nearest integer (in Hz).
 * @param quality Resampling quality between 0 and 10, where 0 has poor quality
 * and 10 has very high quality.
 * @return Newly created resampler state
 * @retval NULL Error: not enough memory
 */
void speex_resampler_init_frac(SpeexResamplerState *st, 
							   spx_uint32_t nb_channels,
                               spx_uint32_t ratio_num, 
                               spx_uint32_t ratio_den, 
                               spx_uint32_t in_rate, 
                               spx_uint32_t out_rate, 
                               int quality,
							   int mode,
                               int *err);



/** Resample an int array. The input and output buffers must *not* overlap.
 * @param st Resampler state
 * @param channel_index Index of the channel to process for the multi-channel 
 * base (0 otherwise)
 * @param in Input buffer
 * @param in_len Number of input samples in the input buffer. Returns the number
 * of samples processed
 * @param out Output buffer
 * @param out_len Size of the output buffer. Returns the number of samples written
 */
int speex_resampler_process_int(SpeexResamplerState *st, 
                                 spx_uint32_t channel_index, 
                                 const spx_int16_t *in, 
                                 spx_uint32_t *in_len, 
                                 spx_int16_t *out, 
                                 spx_uint32_t *out_len);




/** Set (change) the input/output sampling rates and resampling ratio 
 * (fractional values in Hz supported).
 * @param st Resampler state
 * @param ratio_num Numerator of the sampling rate ratio
 * @param ratio_den Denominator of the sampling rate ratio
 * @param in_rate Input sampling rate rounded to the nearest integer (in Hz).
 * @param out_rate Output sampling rate rounded to the nearest integer (in Hz).
 */
int speex_resampler_set_rate_frac(SpeexResamplerState *st, 
                                   spx_uint32_t ratio_num, 
                                   spx_uint32_t ratio_den, 
                                   spx_uint32_t in_rate, 
                                   spx_uint32_t out_rate);


/** Set (change) the conversion quality.
 * @param st Resampler state
 * @param quality Resampling quality between 0 and 10, where 0 has poor 
 * quality and 10 has very high quality.
 */
int speex_resampler_set_quality(SpeexResamplerState *st, 
                                 int quality);



#endif

