/**
 **************************************************************************************
 * @file    adpcm_encoder.h
 * @brief   IMA ADPCM Encoder
 *
 * @author  Aissen Li
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __ADPCM_ENCODER_H__
#define __ADPCM_ENCODER_H__

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

typedef struct _ADPCMHeader
{
    uint8_t     chunk_id[4];		    /**<  0: 'R','I','F','F'	                                                    */
    uint32_t    chunk_size;			    /**<  4: equal to (filesize-8)                                                  */
    uint8_t     wave_id[4]; 		    /**<  8: 'W','A','V','E'                                                        */
    uint8_t     fmt_chunk_id[4];	    /**< 12: 'f','m','t',' '	                                                    */
    uint32_t    fmt_chunk_size;		    /**< 16: 16 for PCM, 20 for IMA-ADPCM                                           */
    uint16_t    format_tag;		        /**< 20: 0x0001 for PCM, 0x0011 for IMA-ADPCM & DVI-ADPCM   			        */
    uint16_t    num_channels;		    /**< 22: number of channels                                                     */
    uint32_t    sample_rate; 		    /**< 24: sample rate in Hz                                                      */
    uint32_t    byte_rate;			    /**< 28: average byterate, equal to (SampleRate*NumChannels*(BitsPerSample/8))  */
    uint16_t    block_align;		    /**< 32: equal to (NumChannels * BitsPerSample/8) 			                    */
    uint16_t    bits_per_sample;	    /**< 34: bits per samples, 16 for PCM, 4 for IMA-ADPCM                          */
    uint16_t    cb_size;                /**< 36: size of extension                                                      */
    uint16_t    samples_per_block;      /**< 38: samples per block                                                      */
    uint8_t     fact_chunk_id[4];       /**< 40: 'f','a','c','t'                                                        */
    uint32_t    fact_chunk_size;        /**< 44: fact chunk size                                                        */
    uint32_t    num_encoded_samples;    /**< 48: number of encoded samples                                              */
    uint8_t     data_chunk_id[4];		/**< 52: 'd','a','t','a'                                                        */
    uint32_t    data_chunk_size;		/**< 56: equal to (filesize-60)                                                 */
}ADPCMHeader;

/**
 * @brief ADPCM Encoder Context
 */
typedef struct _ADPCMEncoderContext
{
    ADPCMHeader  header;            /**< IMA-ADPCM header                                           */

    uint32_t sampling_rate;			/**< Sampling rate                                              */
    uint16_t num_channels;			/**< Number of channels                                         */
    uint16_t samples_per_block;		/**< Samples per block_size for each channel                    */
    uint16_t block_size;			/**< Block size, length of encoded data to be output in bytes   */
    uint16_t block_encoded_size;	/**< Encoded block size                                         */
    int32_t  total_encoded_blocks;	/**< Total encoded blocks				                        */
    int32_t  pre_sample[2];			/**< Predict sample value                                       */

	int8_t   step_table_index[2];	/**< Step size table index                                      */
	int8_t   reserved[2];           /**< Reserved for word align                                    */

    uint8_t* outbuf;                /**< Encoder output buffer for one block, minimum size is 2048  */

}ADPCMEncoderContext;

#define STAT_PREFIX		mv
#ifdef  STAT_PREFIX
#define STATCC1(x,y,z)	STATCC2(x,y,z)
#define STATCC2(x,y,z)	x##y##z  
#define STATNAME(func)	STATCC1(STAT_PREFIX, _, func)
#else
#define STATNAME(func)	func
#endif

#define adpcm_encoder_initialize                STATNAME(adpcm_encoder_initialize)
#define adpcm_encoder_update_stream_header      STATNAME(adpcm_encoder_update_stream_header)
#define adpcm_encoder_encode                    STATNAME(adpcm_encoder_encode)
#define adpcm_encoder_close                     STATNAME(adpcm_encoder_close)

/**
 * @brief  Initialize ADPCM encoder
 *
 * @param[in] adpcm ADPCM encoder struct pointer
 * @param[in] encoder parameter: channels mode
 * @param[in] encoder parameter: sampling rate
 * @param[in] encoder output buffer
 *
 * @return 1:SUCCESS, 0:FALIURE
 */
int32_t adpcm_encoder_initialize(ADPCMEncoderContext* adpcm, int32_t num_channels, int32_t sampling_rate, uint8_t* outbuf);

/**
 * @brief  Update ADPCM header
 *
 * @param[in]   adpcm   ADPCM encoder struct pointer
 * @param[out]  header  stream header start address, equal to NULL if no stream header existed
 * @param[out]  length  stream header length, unit in byte, equal to ZERO if no stream header existed
 *
 * @return 1:SUCCESS, 0:FALIURE
 */
int32_t adpcm_encoder_update_stream_header(ADPCMEncoderContext* adpcm, uint8_t** header, uint32_t* length);

/**
 * @brief  Encode PCM data using ADPCM algorithm
 *
 * @param[in]  adpcm         ADPCM encoder struct pointer
 * @param[out] encoded_data  address of encoded data.
 * @param[out] length        length of encoded data, unit in byte
 *
 * @return 1:SUCCESS, 0:FALIURE
 */
int32_t adpcm_encoder_encode(ADPCMEncoderContext* adpcm, int16_t* pcm/*[adpcm->samples_per_block * adpcm->num_channels]*/, uint8_t** encoded_data, uint32_t* length);

/**
 * @brief  Close ADPCM encoder
 *
 * @param[in] adpcm ADPCM encoder struct pointer
 *
 * @return 1:SUCCESS, 0:FALIURE
 */
int32_t adpcm_encoder_close(ADPCMEncoderContext* adpcm);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__ADPCM_ENCODER_H__
