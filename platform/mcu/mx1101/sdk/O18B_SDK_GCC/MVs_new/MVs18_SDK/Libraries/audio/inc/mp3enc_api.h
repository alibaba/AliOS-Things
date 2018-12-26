/**
 *************************************************************************************
 * @file	mp3enc_api.h
 * @brief	MPEG 1,2 Layer III Encoder API interface.
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v1.5.2
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __MP3ENC_API_H__
#define __MP3ENC_API_H__

//#include "typedefine.h"

#define HAN_SIZE    512
#define SBLIMIT     32
#define SBCODING_LIMIT 27 // adjustable between coding quality and efficiency, maximum=32
#define SAMPLES_PER_FRAME 1152
#define SAMPLES_PER_GRANULE 576
#define MP3_ENCODER_OUTBUF_CAPACITY		2048	// capacity (in bytes) of output buffer for encoded bitstream

//#define PI          3.14159265358979
//#define PI4         0.78539816339745
//#define PI12        0.26179938779915
//#define PI36        0.087266462599717

#define ABS(x) ((x) < 0 ? (-(x)) : (x))

/**
 * Error code of MP3 Encoder
 */
typedef enum _MP3_ENCODE_ERROR_CODE
{
	// The followings are unrecoverable errors, i.e. encoder can't continue
    MP3_ERROR_UNRECOVERABLE = -256,
	MP3_ERROR_NOT_SUPPORTED_SAMPLE_RATE,	/**< the specified sample rate is not supported */
	MP3_ERROR_NOT_SUPPORTED_BITRATE,		/**< the specified bitrate is not supported */

	// The followings are recoverable errors, i.e. encoder can continue
	MP3_ERROR_RECOVERABLE = -128,
	MP3_ERROR_NOT_ENOUGH_SAMPLES,			/**< not enough PCM samples for encoding */	
	MP3_ERROR_FRAME_BITS_BROKEN,			/**< encoded bits of a frame is not correct */
	MP3_ERROR_OUT_OF_BUFFER,				/**< not enough output buffer for encoding */

	// No Error
	MP3_ERROR_OK = 0,						/**< no error              */

}MP3_ENCODE_ERROR_CODE;


/** MPEG modes */
typedef enum {
    MODE_STEREO = 0,	/**< Stereo */
    MODE_JOINT_STEREO,	/**< Joint Stereo */
    MODE_DUAL_CHANNEL,	/**< Dual Channel */
    MODE_MONO,			/**< Mono */
} MPEG_mode;

/** MPEG Version.
 *	MPEG2 is for Lower Sampling Frequencies - LSF < 32000.
 */
typedef enum {
	MPEG_VERSION_INVALID = -1,	/**< Invalid MPEG version */
	MPEG_VERSION_2 = 0,			/**< MPEG-2	 - for sample rates less than 32k */
    MPEG_VERSION_1 = 1			/**< MPEG-1 */
} MPEG_version;

/* Side information */
typedef struct {
        uint16_t part2_3_length;
		uint16_t part2_length;
        uint16_t big_values;
        uint16_t count1;
        uint16_t global_gain;
        uint16_t scalefac_compress;

        uint8_t table_select[3];
		uint8_t count1table_select;

        uint16_t region0_count;
        uint16_t region1_count;
        uint16_t preflag;
        uint16_t scalefac_scale;
        
        uint16_t sfb_lmax;
        uint16_t address1;
        uint16_t address2;
        uint16_t address3;
        
		int32_t quantizerStepSize;        
		uint8_t slen[4];
} gr_info;

typedef struct {
    int main_data_begin; /* unsigned -> int */
    unsigned private_bits;
    int resvDrain;
    uint8_t scfsi[2][4];
    struct {
        struct {
            gr_info tt;
        } ch[2];
    } gr[2];
} L3_side_info_t;

//typedef struct {
//    double  l[2][2][21];
//} L3_psy_ratio_t;
//
//typedef struct {
//        double  l[2][2][21];
//} L3_psy_xmin_t;

typedef struct {
    int8_t l[2][2][22];            /* [cb] */
    int8_t s[2][2][13][3];         /* [window][cb] */
} L3_scalefac_t;

struct scalefac_struct
{
   int l[23];
   int s[14];
};

/** MPEG Encoder Context */
typedef struct _MP3EncoderContext
{
	int32_t sample_rate;			/**< sample rate in Hz */
	int32_t num_channels;			/**< number of channels */
	int32_t bitrate;				/**< bitrate in kbps */
	int32_t error_code;				/**< error code */
	uint32_t pcm_data_length;       /**< Number of encoded PCM samples (for each channel) */
	
	MPEG_version mpeg_version;		/**< MPEG-1, MPEG-2, ... */
	MPEG_mode mpeg_mode;			/**< stereo, joint stereo, mono, ... */
	
	int8_t mpeg_mode_ext;			/**< mode extension used when mode=JOINT STEREO */	
	int8_t jsbound;					/**< first band of joint stereo coding */
    int8_t sblimit;					/**< total number of sub bands */
	int8_t vbr;						/**< VBR flag (default=0) */
	
	int8_t bitrate_index;			/**< bitrate index */
	int8_t samplerate_index;		/**< sample rate index */	
	//int8_t tablenum;				/**< selected bit allocation table index */
	int8_t padding;					/**< padding flag (default=0) */	

	// Miscellaneous Options That Nobody Ever Uses
    int8_t emphasis;				/**<  default=0, i.e. No Emphasis */
    int8_t copyright;				/**< [FALSE] */
    int8_t original;				/**< [FALSE] */
    int8_t private_bit;				/**< [0] Your very own bit in the header. */
    int8_t error_protection;		/**< CRC error protection flag (default=0) */

	// subband analysis
	int32_t off[2];
	int16_t m[16][31];
	int16_t x[2][HAN_SIZE];	
	int32_t l3_sb_sample[2][SAMPLES_PER_GRANULE];
	
	// mdct
	int16_t l3_enc[2][2][SAMPLES_PER_GRANULE];
	int32_t mdct_freq[SAMPLES_PER_GRANULE];
	int32_t xrmax[2][2];
	int32_t xrexp[2][2];
	int16_t xr34[SAMPLES_PER_GRANULE];
	int32_t prev_step_size[2];

	// reservoir
	int32_t ResvSize; /* in bits */
	int32_t ResvMax;  /* in bits */
	
	//L3_psy_ratio_t  ratio;
	L3_side_info_t  side_info;
	L3_scalefac_t   scalefactor;

	float	avg_slots_per_frame;
	float  frac_slots_per_frame;
	int32_t whole_slots_per_frame;
	float  slot_lag;
	int32_t bits_per_frame;

	BufferContext bc;					/**< buffer context */
	//uint8_t outbuf[MP3_ENCODER_OUTBUF_CAPACITY];	/**< Output buffer start address */

} MP3EncoderContext;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/*******************************************************************************
 * MP3 encoder API functions prototype. 
 *******************************************************************************/ 
/**
 * @brief  Initialize the MP3 encoder.
 * @param[in] ct  pointer to the MP3 encoder context
 * @param[in] num_channels number of channels
 * @param[in] sample_rate sample rate
 * @param[in] bitrate bit rate in kbps. e.g. 192
 * @param[in] outbuf output buffer to store encoder output (1 frame)
 * @return initialization result (RT_FAILURE, RT_SUCCESS)
 */ 
int32_t	 mp3_encoder_initialize(MP3EncoderContext *ct, int32_t num_channels, int32_t sample_rate, int32_t bitrate, uint8_t *outbuf);


/**
 * @brief  Encode an MP3 frame.
 * @param[in] ct  pointer to the MP3 encoder context
 * @param[in] pcm one frame of PCM data. The data layout is the same as Microsoft WAVE format.
 * @param[out] encoded_data  start address of the encoded data.
 * @param[out] length        length of encoded data in bytes
 * @return encoding result. RT_FAILURE, RT_SUCCESS
 */
int32_t mp3_encoder_encode(MP3EncoderContext *ct, int16_t pcm[SAMPLES_PER_FRAME*2], uint8_t** encoded_data, uint32_t *length);


/**
 * @brief  Update MP3 stream header
 * @param[out]  header start address of the stream header, NULL if no stream header exists.
 * @param[out]  length  length of the stream header in bytes, ZERO if no stream header exists.
 * @return update result. RT_FAILURE, RT_SUCCESS
 */
int32_t mp3_encoder_update_stream_header(uint8_t** header, uint32_t* length);


/**
 * @brief  Close the MP3 encoder
 * @return close result, RT_FAILURE, RT_SUCCESS
 */
int32_t mp3_encoder_close(void);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__MP3ENC_API_H__
