/**
 *************************************************************************************
 * @file    audio_encoder.h
 * @brief   Audio Encoder API Interface.
 *
 * @author  Aissen Li, ZHAO Ying (Alfred)
 * @version V4.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __AUDIO_ENCODER_H__
#define __AUDIO_ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "audio_utility.h"

/**
 * @brief Audio encoder type
 */
typedef enum _EncoderType
{
    PCM_ENCODER,    /**<   PCM  encoder */
    ADPCM_ENCODER,  /**< ADPCM  encoder */
    MP2_ENCODER     /**<   MP2  encoder */
} EncoderType;

/**
 * Audio Encoder Context
 */
typedef struct _AudioEncoderContext
{
    int32_t        encoder_type;        /**< Audio encoder type, value must be in @code EncoderType     */
    uint32_t       encoder_size;        /**< Audio encoder size                                         */
    int32_t        error_code;          /**< Audio encoder return error code.                           */

    uint32_t       num_channels;        /**< Encode params: number of channels                          */
    uint32_t       sampling_rate;       /**< Encode params: sampling rate                               */
    uint32_t       bitrate;             /**< Encode params: bitrate in kbps (kilo-bit per second)       */
    uint32_t       other_options;       /**< Encode params: other encode option interface for extending */

    void*          frame;               /**< State info pointer,can be PCMEncoderContext                ,/
                                             ADPCMEncoderContext, MP2EncoderContext                     */
    BufferContext* bc;                  /**< Buffer context pointer                                     */
    uint8_t*       outbuf;              /**< Output buffer start address                                */

    uint32_t       pcm_data_length;     /**< Number of encoded PCM samples (for each channel)           */

    int32_t        (*initialize)(void); /**< Function pointer for audio encoder initialize              */
    int32_t        (*encode)(uint8_t** encoded_data, uint32_t *length);       
                                        /**< Function pointer for audio encoder decode                  */
    int32_t        (*close)(void);      /**< Function pointer for audio encoder close                   */
    int32_t        (*update_stream_header)(uint8_t** header, uint32_t* length);
                                        /**< Function pointer for audio encoder update stream header    */
    uint32_t       (*pcm_samples_available)(void);
                                        /**< Function pointer for checking available pcm samples        +/
                                             function prototype @see pcm_samples_available_t            */
    uint32_t       (*pcm_receive)(int16_t *pcm_data, uint32_t num_samples_required);
                                        /**< Function pointer for receiving pcm samples to encode.      +/
                                             function prototype @see pcm_receive_t                      */
}AudioEncoderContext;


/*******************************************************************************
 * Global Variable Definition
 *******************************************************************************/
/**
 * Global AudioEncoderContext pointer
 */
extern AudioEncoderContext* audio_encoder;

/**
 * @brief Checking available pcm samples, function prototype as follow:
 *        uint32_t pcm_samples_available(void);
 * @return number of available pcm samples, unit in samples
 */
typedef uint32_t (*pcm_samples_available_t)(void);

/**
 * @brief Receive pcm samples to encoder, function prototype as follow:
 *        uint32_t pcm_receive(int16_t* pcm_data, uint32_t num_samples_required);
 * @param[in] pcm_data              destination address for storing the reveived pcm samples.
 * @param[in] num_samples_required  request for receiving num_samples_required samples
 * @return number of samples actually received, unit in samples
 *
 * @note the received pcm samples are stored as Microsoft Wave format,
 *       L0/R0/L1/R1 .... for stereo (each PCM sample Lx or Rx is 16-bit)
 *       M0/M1/M2/M3 .... for mono (each PCM sample Mx is 16-bit)
 */
typedef uint32_t (*pcm_receive_t)(int16_t*, uint32_t);

/**
 * @brief  Initialize audio encoder
 * @param[in]  ram_addr                 ram address for encoder context
 * @param[in]  pcm_samples_available    function pointer for checking available pcm samples, @see pcm_samples_available_t
 * @param[in]  pcm_receive              function pointer for receive pcm samples to encoder, @see pcm_receive_t
 * @param[in]  encoder_type             Encoder type enum, value must be in @code EncoderType
 * @param[in]  num_channels             Encode params: number of channels
 * @param[in]  sampling_rate            Encode params: sampling rate in Hz
 * @param[in]  bitrate                  Encode params: bitrate in kbps
 * @param[in]  other_options            Encode params: other encode option interface for extending
 * @return initialize result, RT_FAILURE, RT_SUCCESS
 */
int32_t audio_encoder_initialize(uint8_t*                   ram_addr,
                                 pcm_samples_available_t    pcm_samples_available,
                                 pcm_receive_t              pcm_receive,
                                 int32_t                    encoder_type,
                                 uint32_t                   num_channels,
                                 uint32_t                   sampling_rate,
                                 uint32_t                   bitrate,
                                 uint32_t                   other_options);

/**
 * @brief  Update encoded stream header
 * @param[out]  stream header start address, equal to NULL if no stream header existed
 * @param[out]  length  stream header length, unit in byte, equal to ZERO if no stream header existed
 * @return update result. RT_FAILURE, RT_SUCCESS
 */
int32_t audio_encoder_update_stream_header(uint8_t** header, uint32_t* length);

/**
 * @brief  Encode an audio frame
 * @param[out] encoded_data  address of encoded data.
 * @param[out] length        length of encoded data, unit in byte
 * @return encoding result. RT_FAILURE, RT_SUCCESS
 */
int32_t audio_encoder_encode(uint8_t** encoded_data, uint32_t* length);

/**
 * @brief  Close audio encoder
 * @return close result, RT_FAILURE, RT_SUCCESS
 */
int32_t audio_encoder_close(void);

/**
 * @brief  Get error code from encoder context
 * @return error code
 */
int32_t audio_encoder_get_error_code(void);

/**
 * @brief  Get audio encoder library version
 * @return audio encoder library version string
 */
const uint8_t* audio_encoder_get_lib_version(void);

#define GetLibVersionAudioEncoder   audio_encoder_get_lib_version

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__AUDIO_ENCODER_H__
