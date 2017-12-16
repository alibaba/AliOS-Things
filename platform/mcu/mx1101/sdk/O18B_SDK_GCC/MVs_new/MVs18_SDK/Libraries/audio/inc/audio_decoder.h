/**
 *************************************************************************************
 * @file    audio_decoder.h
 * @brief   Audio Decoder API Interface.
 *
 * @author  Aissen Li, Alfred Zhao, Lean Xiong
 * @version V6.5.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __AUDIO_DECODER_H__
#define __AUDIO_DECODER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "audio_utility.h"

#define MAX_TAG_LEN             64          /**< 64 bytes,the last two bytes are 0x00 always */

/**
 * SBC decode error code
 */
typedef enum _SBC_DECODE_ERROR_CODE
{
	// The followings are unrecoverable errors, i.e. decoder can't continue
	SBC_ERROR_UNRECOVERABLE = -256,
	SBC_ERROR_END_OF_STREAM,    		/**< file or bit stream end */

	// The followings are recoverable errors, i.e. decoder can continue
	SBC_ERROR_RECOVERABLE = -128,		/**< decoder can continue  */
	SBC_ERROR_BUFFER_OVERFLOW,			/**< buffer overflow       */
	SBC_ERROR_SYNC_INCORRECT,			/**< synchronize incorrect */
	SBC_ERROR_BITPOOL_OUT_BOUNDS,		/**< bitpool out of bounds */
	SBC_ERROR_CRC8_INCORRECT,			/**< CRC8 check incorrect  */
	SBC_ERROR_STREAM_EMPTY,			    /**< stream empty          */

	// No Error
	SBC_ERROR_OK = 0,					/**< no error              */

} SBC_DECODE_ERROR_CODE;



/**
 * Audio Decoder Type Set
 */
typedef enum _DecoderType
{
    UNKOWN_DECODER               = 0,       /**< Unknown decoder                     */
    PURE_SOFTWARE_DECODERS       = 1,       /**< Pure software decoders as follows   */
    WAV_DECODER,                            /**< WAV  decoder */
    FLAC_DECODER,                           /**< FLAC decoder */
    AAC_DECODER,                            /**< AAC  decoder */
    AIF_DECODER,                            /**< AIFF and AIFC decoder */
    WITH_HARDWARE_DECODERS       = 128,     /**< Decoders with hardware as follows   */
    MP3_DECODER,                            /**< MP3  decoder */
    WMA_DECODER,                            /**< WAM  decoder */
    SBC_DECODER,                            /**< SBC  decoder */
} DecoderType;


/**
 * MPEG Version Set
 */
typedef enum _MpegVersionSet
{
    MPEG_NULL   = 0,    /**< Unkown MPPEG version */
    MPEG_VER1   = 1,    /**< MPEG version 1       */
    MPEG_VER2   = 2,    /**< MPEG version 2       */
    MPEG_VER2d5 = 3     /**< MPEG version 2.5     */
} MpegVersionSet;


/**
 * ID3 Version Set
 */
typedef enum _ID3VersionSet
{
    ID3_NULL = 0,   /**< No ID3 version existed     */
    ID3_VER1 = 1,   /**< ID3 version 1 existed only */
    ID3_VER2 = 2,   /**< ID3 version 2 existed only */
    ID3_V1V2 = 3    /**< Both ID3 V1 and V2 existed */
} ID3VersionSet;


/**
 * Tag Char Set
 *
 * @note if the most significant 4 bits of char set is "0x0", it means all the strings encoded with the same chat set.
 *       if the most significant 4 bits of char set is "0x1", it means all the strings (title, artist, album, comment,
 *       genre_str) encoded with differet chat set, the other bits fileds indicates as following:
 *           bits [3:0]   means the char set of "title",
 *           bits [7:4]   means the char set of "artist",
 *           bits [11:8]  means the char set of "album",
 *           bits [15:12] means the char set of "comment",
 *           bits [19:16] means the char set of "genre_str",
 *       if the most significant 4 bits of char set is other value, it means reserved for furture use.
 */
typedef enum _TagCharSet
{
    CHAR_SET_UNKOWN,
    CHAR_SET_ISO_8859_1,
    CHAR_SET_UTF_16,
    CHAR_SET_UTF_8,
    CHAR_SET_DIVERSE = 0x10000000
} TagCharSet;


/**
 * Stream Type
 */
typedef enum _StreamType
{
    STREAM_TYPE_UNKNOWN,/**< UNKNOWN stream   */
    STREAM_TYPE_MP2,	/**< MP2 stream       */
    STREAM_TYPE_MP3,	/**< MP3 stream       */
    STREAM_TYPE_WMA,	/**< WMA stream       */
    STREAM_TYPE_SBC,	/**< SBC stream       */
    STREAM_TYPE_PCM,	/**< raw PAM stream   */
    STREAM_TYPE_ADPCM,	/**< IMA_ADPCM stream */
    STREAM_TYPE_FLAC,	/**< FLAC stream      */
    STREAM_TYPE_AAC,	/**< AAC  stream      */
    STREAM_TYPE_ALAW,   /**< CCITT G711 A-law */
    STREAM_TYPE_MULAW,  /**< CCITT G711 u-law */
} StreamType;


/**
 * Song Information
 */
typedef struct _SongInfo
{
    /************************************ Stream information ********************************/
    int32_t     stream_type;            /**< Stream type, must be in @code StreamTypeSet    */
    uint32_t    num_channels;           /**< Number of channels                             */
    uint32_t    sampling_rate;          /**< Sampling rate, unit: Hz                        */
    uint32_t    bitrate;                /**< Bit rate, unit:bps                             */
    uint32_t    duration;               /**< Total play time, unit:ms                       */
    uint32_t    file_size;              /**< Song file size in bytes.                       */
    uint32_t    vbr_flag;               /**< VBR flag, 0:CBR, 1:VBR                         */
    
    /*********************************** PCM information ************************************/
    uint16_t    pcm_data_mode;          /**< PCM data mode, @see PCM_DATA_MODE              */
    uint16_t    pcm_data_length;        /**< PCM data length in samples after decoding (    +/
                                             irrelevant to #channels and possibly changeable+/
                                             for each frame, e.g. WMA)                      */
    int32_t*    pcm0_addr;              /**< PCM start address of channel 0/L, this address +/
                                             is always valid.                               */
    int32_t*    pcm1_addr;              /**< PCM start address of channel 1/R, this address +/
                                             is valid only when pcm_data_mode equal to      +/
                                             DATA_MODE_STEREO_HIGH, DATA_MODE_STEREO_LOW or +/
                                             DATA_MODE_STEREO_S.                            */
    
    /*********************************** Tag information ************************************/	
    int32_t    char_set;                /**< Char set for tag info, @see TagCharSet, can    +/ 
                                             be ISO_8859_1, UTF_16, UTF_8                   */
    uint8_t    title[MAX_TAG_LEN]  ;    /**< Title   , end with "\0\0"                      */
    uint8_t    artist[MAX_TAG_LEN] ;    /**< Author  , end with "\0\0"                      */
    uint8_t    album[MAX_TAG_LEN]  ;    /**< Album   , end with "\0\0"                      */
    uint8_t    comment[MAX_TAG_LEN];    /**< Comments,                                      */
    uint8_t    genre_str[MAX_TAG_LEN];  /**< Genre string for ID3v2                         */    
    uint8_t    year[6];                 /**< Years   , year in string, e.g. '2''0''1''2'    */
    uint8_t    track;                   /**< Track   , track number of music                */
    uint8_t    genre;                   /**< Genre   , style index of music                 */

} SongInfo;


/**
 * Audio Decoder Context
 */
typedef struct _AudioDecoderContext
{
    int32_t        decoder_type;         /**< Audio decoder type, value must be in @code DecoderType     */
    uint32_t       decoder_size;         /**< Audio decoder size, equal to sizeof(AudioDecoderContext)   +/
                                                                          sizeof(input_buffer_capacity)  +/
                                                                          sizeof(frame_context_size)     +/
                                                                          sizeof(BufferContext)          +/
                                                                          sizeof(SongInfo)               */

    int32_t        error_code;           /**< Audio decoder return error code.                           */

    uint8_t*       inbuf;                /**< Input buffer                                               */
    void*          frame;                /**< Audio decoder state info context pointer,can be MP3Context,+/
                                              WMAContext, SBCContext, WAVContext, FLACContext            */
    BufferContext* bc;                   /**< Buffer context pointer                                     */
    SongInfo*      song_info;            /**< Song information context pointer                           */
                                           
    int32_t        (*initialize)(void);  /**< Function pointer for initializing audio decoder            */
    int32_t        (*decode)(void);      /**< Function pointer for decoding one audio frame              */
    int32_t        (*close)(void);       /**< Function pointer for closing audio decoder                 */
    int32_t        (*can_continue)(void);/**< Function pointer for checking whether the audio decoder    +/
                                              can go on, if can, then decode, else abort audio decoder   */
    int32_t        (*seek)(uint32_t seek_time);/**< Function pointer for seeking to special time to play */
    
}AudioDecoderContext;

/*******************************************************************************
 * Global Variable Definition
 *******************************************************************************/
/**
 * Global AudioDecoderContext pointer
 */
extern AudioDecoderContext* audio_decoder;

/**
 * @brief  Initialize audio decoder
 * @param  ram_addr     ram address
 * @param  io_handle    IO handle
 * @param  io_type      IOType pointer, value must be in @code IOType
 * @param  decoder_type audio decoder type, value must be in @code DecoderType
 * @return initialize result, RT_FAILURE, RT_SUCCESS
 */
int32_t audio_decoder_initialize(uint8_t* ram_addr, void* io_handle, int32_t io_type, int32_t decoder_type);

/**
 * @brief  Decode an audio frame
 * @return RT_FAILURE, RT_SUCCESS
 */
int32_t audio_decoder_decode(void);

/**
 * @brief  Close audio decoder
 * @return close result, RT_FAILURE, RT_SUCCESS
 */
int32_t audio_decoder_close(void);

/**
 * @brief  Check whether the audio decoder can go on
 * @return check result, RT_NO, RT_YES
 */
int32_t audio_decoder_can_continue(void);

/**
 * @brief  Get error code from AudioDecoderContext
 * @return error code
 */
int32_t audio_decoder_get_error_code(void);

/**
 * @brief  Seek to file position according as the specified time.
 * @param  seek_time   the specified time in millisecond(ms)
 * @return RT_SUCCESS, RT_FAILURE
*/
int32_t audio_decoder_seek(uint32_t seek_time);

/**
 * @brief  Get song information.
 * @return SongInfo pointer
 */
SongInfo* audio_decoder_get_song_info(void);

/**
 * @brief  Get audio decoder's entire context size in bytes.
 * @return audio decoder's entire context size in bytes.
 */
uint32_t audio_decoder_get_context_size(void);

/**
 * @brief  Whether current audio decoder with hardware decoding.
 * @return RT_YES, RT_NO
 */
int32_t is_audio_decoder_with_hardware(void);


//#if defined(USE_HW_DECODER)

/**
 * @brief  Audio Decoder Interrupt Callback function, called by @code audio_decoder_interrupt_handler
 *         this function can be overload to process message about XR_DONE && TX_DONE signals
 * @param  intterupt_type   Interrupt type, 0:XR_DONE Interrupt, 1:TX_DONE Interrupt
 * @return NONE
 */
void audio_decoder_interrupt_callback(int32_t intterupt_type);

/**
 * @brief  Audio Decoder Interrupt Service Routine, Process XR_DONE && TX_DONE signals
 * @param  NONE
 * @return NONE
 */
void audio_decoder_interrupt_handler(void);

/**
 * @brief  Wait for XR_DONE signal, which indicates hardware decoder has completed its decoding task. This function is used in decoder interrupt handler as well.
 * @param  NONE
 * @return NONE
 */
void audio_decoder_wait_for_hardware_decoder_done(void);

/**
 * @brief  Start PCM data transfer
 * @param  NONE
 * @return NONE
 */
void audio_decoder_start_pcm_transfer(void);

/**
 * @brief  Check XR_DONE signal to see whether hard-ware decoder is done
 * @return check result, RT_NO, RT_YES
 */
int32_t audio_decoder_check_xr_done(void);

/**
 * @brief  Clear xr done flag as 0.
 * @param  NONE
 * @return NONE
 */
void audio_decoder_clear_xr_done(void);

/**
 * @brief  Check TX_DONE signal to see whether PCM data transfer is done
 * @return check result, RT_NO, RT_YES
 */
int32_t audio_decoder_check_pcm_transfer_done(void);

/**
 * @brief  Clear PCM data transfer done flag as 0.
 * @param  NONE
 * @return NONE
 */
void audio_decoder_clear_pcm_transfer_done(void);


//#endif//USE_HW_DECODER

/**
 * @brief  Convert pcm data layout from pcm_data_mode to DATA_MODE_STEREO_R_L or DATA_MODE_MONO_S for mono and stereo respectively
 * @param  pcm_data_l        left  channel of pcm data
 * @param  pcm_data_r        right channel of pcm data
 * @param  num_pcm_samples   pcm samples to be converted
 * @param  src_pcm_data_mode the source pcm data layout, value must be in @code PCM_DATA_MODE
 * @return convert result, RT_SUCCESS or RT_FAILURE
 *
 * @note now src_pcm_data_mode can be only DATA_MODE_MONO_HIGH, DATA_MODE_STEREO_L_R, DATA_MODE_STEREO_S
 */
int32_t audio_pcm_data_layout_convert(uint16_t* pcm_data_l, uint16_t* pcm_data_r, uint32_t num_pcm_samples, int32_t src_pcm_data_mode);

/**
 * @brief  Convert audio decoder's decoded pcm data layout to DATA_MODE_STEREO_R_L or DATA_MODE_MONO_S for mono and stereo respectively
 * @return convert result, RT_SUCCESS or RT_FAILURE
 */
int32_t audio_decoder_convert_pcm_data_layout(void);

/**
 * @brief  Get MPEG version for MP2/3 songs
 * @return MPEG version value, @see MpegVersionSet enumeration
 */
int32_t audio_decoder_get_mpeg_version(void);

/**
 * @brief  Get ID3 version for MP2/3 songs
 * @return ID3 version value, @see ID3VersionSet enumeration
 */
int32_t audio_decoder_get_id3_version(void);

/**
 * @brief  Get audio decoder library version
 * @return audio decoder library version string
 */
const uint8_t* audio_decoder_get_lib_version(void);

/**
 * @brief  Set mp3 decoder ID3 parser.
 * @param  property mp3 decoder ID3 parser property. Its values can be as followings:
 *          0: DO NOT PARSE ANY ID3
 *          1: PARSE ID3v1 only
 *          2: PARSE ID3v2 only
 *          3: PARSE ID3v1 and ID3v2 both, but parse ID3v2 only if ID3v2 existed, parse ID3v1 when ID3v2 not existed.
 * @return NONE
 */
void mp3_decoder_id3_parse_set(uint8_t property);

#define GetLibVersionAudioDecoder   audio_decoder_get_lib_version

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__AUDIO_DECODER_H__
