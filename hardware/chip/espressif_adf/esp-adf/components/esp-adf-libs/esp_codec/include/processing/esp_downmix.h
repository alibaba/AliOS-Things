// Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD.>
// All rights reserved.

#ifndef _ESP_DOWNMIX_H_
#define _ESP_DOWNMIX_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define GAIN_MAX 100          /*!< the maximum gain of input stream  */
#define GAIN_MIN -100         /*!< the minimum gain of input stream */
#define SAMPLERATE_MAX 100000 /*!< the maximum samplerate of input stream  */
#define SAMPLERATE_MIN 4000   /*!< the minimum samplerate of input stream  */
#define SOURCE_NUM_MAX 8      /*!< the maximum number of input streams */

/**
Function Description
Mixing less or equal than`SOURCE_NUM_MAX` of input streams into output stream with the correspondingly gains. And the number of channels of output stream can be set. refer to `esp_downmix_output_type_t`.
The downmix process has 3 work status:
* Bypass Downmixing -- Only the first streams will be played;
* Switch on Downmixing -- All input streams will enter the transition period, during which the gains of these streams will be changed from the original level to the target level; then enter the stable period, sharing a same target gain;
* Switch off Downmixing -- All input streams and the others stream will enter the transition period, during which the gains of these streams will be changed back to their original levels; then enter the stable period, with their original gains, respectively. After that, the audio device enters the bypass status.
Note that, the sample rates of all input streams must be the same. Otherwise, an error occurs.
*/

/**
* @brief      The type of output stream
*/
typedef enum {
    ESP_DOWNMIX_OUTPUT_TYPE_ONE_CHANNEL = 1, /*!< Output stream is mono */
    ESP_DOWNMIX_OUTPUT_TYPE_TWO_CHANNEL = 2, /*!< Output stream is stereo */
    ESP_DOWNMIX_OUTPUT_TYPE_MAX,             /*!< The maximum value */
} esp_downmix_output_type_t;

/**
* @brief      The work status of mixer
*/
typedef enum {
    ESP_DOWNMIX_WORK_MODE_INVALID = -1,    /*!< Invalid status */
    ESP_DOWNMIX_WORK_MODE_BYPASS = 0,      /*!< Only the first stream will be played */
    ESP_DOWNMIX_WORK_MODE_SWITCH_ON = 1,   /*!< The all input stream will enter the transition period, during which the gains of these will be changed from the original level to the target level; then enter the stable period, sharing a same target gain */
    ESP_DOWNMIX_WORK_MODE_SWITCH_OFF = 2,  /*!< The all input stream will enter the transition period, during which the gains of these will be changed back to their original levels; then enter the stable period, with their original gains, respectively. After that, the audio device enters the bypass status */
    ESP_DOWNMIX_WORK_MODE_MAX,             /*!< The maximum value */
} esp_downmix_work_mode_t;

/**
* @brief      Content of per channel of output stream 
*
* @note       If output stream is stereo, the function of `ESP_DOWNMIX_OUT_CTX_ONLY_LEFT` and `ESP_DOWNMIX_OUT_CTX_ONLY_RIGHT` is the same as `ESP_DOWNMIX_OUT_CTX_NORMAL`.
*/
typedef enum {
    ESP_DOWNMIX_OUT_CTX_LEFT_RIGHT = 0,       /*!< Include left and right channel content of all input streams in per channel of output stream */
    ESP_DOWNMIX_OUT_CTX_ONLY_LEFT = 1,        /*!< Include only left channel content of all input streams in per channel of output stream. */
    ESP_DOWNMIX_OUT_CTX_ONLY_RIGHT = 2,       /*!< Include only right channel content of all input streams in per channel of output stream. */
    ESP_DOWNMIX_OUT_CTX_NORMAL = 3,           /*!< Include left or right channel content of all input streams in left or right channel of output stream respectively. If all input streams are mono, per channel of output stream contain all content of all input streams.*/
    ESP_DOWNMIX_OUT_CTX_MAX,                  /*!< The maximum value. */
} esp_downmix_out_ctx_type_t;

/**
* @brief      Input stream infomation
*/
typedef struct {
    int samplerate;    /*!< Sample rate */
    int channel;       /*!< the number of channel(s) of the input stream;*/
    int bits_num;      /*!< Only 16-bit PCM audio is supported */
    float gain[2];     /*!< The gain is expressed using the logarithmic decibel (dB) units (dB gain).
                            When the downmixing is switched on, the gains of the input streams will be gradually changed from gain[0] to gain[1] in the transition period, and stay at gain[1] in the stable period;
                            When the downmixing is switched off, the gains of the input streams will be gradually changed back from gain[1] to gain[0] in the transition period, and stay at gain[0] in the stable period;
                            For the input streams:
                            - gain[0]: the original gain of the input streams before the downmixing process. Usually, if the downmixing is not used, gain[0] is usually set to 0 dB.
                            - gain[1]: the target gain of the input streams after the downmixing process.*/
    int transit_time; /*!< The length of the transition period in milliseconds, which is the same for "switch on down-mixing" and "switch off down-mixing". */
} esp_downmix_input_info_t;

/**
* @brief      Downmix information
*/
typedef struct {
    esp_downmix_input_info_t *source_info;   /*!< Input streams infomation*/
    int source_num;                                         /*!< The number of input streams*/
    esp_downmix_out_ctx_type_t out_ctx;                     /*!< Select content of per channel of output stream. refer to `esp_downmix_out_ctx_type_t`*/
    esp_downmix_work_mode_t mode;                           /*!< The work status with downmixing. refer to `esp_downmix_work_mode_t`*/
    esp_downmix_output_type_t output_type;                  /*!< The type of output stream by processed downmix*/
} esp_downmix_info_t;

/**
* @brief      Creates the Downmix handle
*
* @param      downmix         the downmix information
*
* @return     The Downmix handle for esp_downmix_process and esp_downmix_close. NULL: creating Downmix handle failed
*/
void *esp_downmix_open(esp_downmix_info_t *downmix);

/**
* @brief      Processes the stream through downmixing.
*
* @param      downmix_handle       the Downmix handle created and returned by esp_downmix_open()
* @param      inbuf                the buffer that stores the input stream, which is in PCM format
* @param      outbuf               the buffer that stores the output stream, which is in PCM format
* @param      sample               The number of sample per downmix processing
* @param      work_mode            the work mode. For details, please check the description in esp_downmix_work_mode_t.
*
* @return     The length of the output stream (in bytes), which is also in PCM format. A negative return value indicates an error has occurred.
*/
int esp_downmix_process(void *downmix_handle, unsigned char *inbuf[], unsigned char *outbuf, int sample, esp_downmix_work_mode_t work_mode);

/**
* @brief      Releases the Downmix handle.
*
* @param      downmix_handle       the Downmix handle.
*/
void esp_downmix_close(void *downmix_handle);

#ifdef __cplusplus
}
#endif

#endif
