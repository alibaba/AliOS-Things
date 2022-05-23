// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _ESP_AUDIO_H_
#define _ESP_AUDIO_H_
#include "audio_def.h"
#include "audio_common.h"
#include "audio_element.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *esp_audio_handle_t;

#define DEFAULT_ESP_AUDIO_CONFIG() {\
    .in_stream_buf_size = 10*1024,\
    .out_stream_buf_size = 4*1024,\
    .resample_rate = 0,\
    .evt_que = NULL,\
    .cb_func = NULL,\
    .cb_ctx = NULL,\
    .prefer_type = ESP_AUDIO_PREFER_MEM,\
    .vol_handle = NULL,\
    .vol_set = NULL,\
    .vol_get = NULL,\
    .task_prio = 6,\
    .task_stack = 4 * 1024,\
}

/**
 * @brief esp_audio configuration parameters
 */
typedef struct {
    int                         in_stream_buf_size;         /*!< Input buffer size */
    int                         out_stream_buf_size;        /*!< Output buffer size */
    /**
     * Destination sample rate, 0: disable resample; others: 44.1K, 48K, 32K, 16K, 8K has supported
     * It should be make sure same with I2S stream `sample_rate`
     */
    int                         resample_rate;                             
    QueueHandle_t               evt_que;                    /*!< For received esp_audio events (optional)*/
    esp_audio_event_callback    cb_func;                    /*!< esp_audio events callback (optional)*/
    void                       *cb_ctx;                     /*!< esp_audio callback context (optional)*/

    /**
     * esp_audio works on sepcific type, default memory is preferred.
     * - `ESP_AUDIO_PREFER_MEM` mode stopped the previous linked elements before the new pipeline starting, except out stream element.
     * - `ESP_AUDIO_PREFER_SPEED` mode  kept the previous linked elements before the new pipeline starting, except out stream element.
    */
    esp_audio_prefer_t          prefer_type;

    void                       *vol_handle;                 /*!< Volume change instance */
    audio_volume_set            vol_set;                    /*!< Set volume callback */
    audio_volume_get            vol_get;                    /*!< Get volume callback*/
    int                         task_prio;                  /*!< esp_audio task priority*/
    int                         task_stack;                 /*!< Size of esp_audio task stack */
} esp_audio_cfg_t;

/**
 * @brief esp_audio setup parameters by manual
 */
typedef struct {
    audio_codec_type_t  set_type;               /*!< Set codec type */
    int                 set_sample_rate;        /*!< Set music sample rate */
    int                 set_channel;            /*!< Set music channels */
    int                 set_pos;                /*!< Set starting position */
    int                 set_time;               /*!< Set starting position of the microseconds time (optional)*/
    char               *set_uri;                /*!< Set URI */
    char               *set_in_stream;          /*!< Tag of in_stream */
    char               *set_codec;              /*!< Tag of the codec */
    char               *set_out_stream;         /*!< Tag of out_stream */
} esp_audio_setup_t;

/**
 * @brief esp_audio information
 */
typedef struct {
    audio_element_info_t            codec_info;             /*!< Codec information */
    audio_element_handle_t          in_el;                  /*!< Handle of the in stream */
    audio_element_handle_t          out_el;                 /*!< Handle of the out stream */
    audio_element_handle_t          codec_el;               /*!< Handle of the codec */
    audio_element_handle_t          filter_el;              /*!< Handle of the filter */
    esp_audio_state_t               st;                     /*!< The state of esp_audio */
    int                             time_pos;               /*!< Position of the microseconds time */
    float                           audio_speed;            /*!< Play speed of audio */
    int64_t                         in_stream_total_size;   /*!< Total size of in stream */
} esp_audio_info_t;

/**
 * @brief esp_audio play speed 
 */
typedef enum {
    ESP_AUDIO_PLAY_SPEED_UNKNOW = -1,
    ESP_AUDIO_PLAY_SPEED_0_50   = 0,
    ESP_AUDIO_PLAY_SPEED_0_75   = 1,
    ESP_AUDIO_PLAY_SPEED_1_00   = 2,
    ESP_AUDIO_PLAY_SPEED_1_25   = 3,
    ESP_AUDIO_PLAY_SPEED_1_50   = 4,
    ESP_AUDIO_PLAY_SPEED_1_75   = 5,
    ESP_AUDIO_PLAY_SPEED_2_00   = 6,
    ESP_AUDIO_PLAY_SPEED_MAX    = 7,
} esp_audio_play_speed_t;

/**
 * @brief Create esp_audio instance according to 'cfg' parameter
 *
 * This function create an esp_audio instance, at the specified configuration.
 *
 * @param[in] cfg  Provide esp_audio initialization configuration
 *
 * @return
 *      - NULL: Error
 *      - Others: esp_audio instance fully certifying
 */
esp_audio_handle_t esp_audio_create(const esp_audio_cfg_t *cfg);

/**
 * @brief Specific esp_audio instance will be destroyed
 *
 * @param[in] handle  The esp_audio instance
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no instance to free, call esp_audio_init first
 */
audio_err_t esp_audio_destroy(esp_audio_handle_t handle);

/**
 * @brief Add audio input stream to specific esp_audio instance
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] in_stream Audio stream instance
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 *      - ESP_ERR_AUDIO_MEMORY_LACK: allocate memory fail
 */
audio_err_t esp_audio_input_stream_add(esp_audio_handle_t handle, audio_element_handle_t in_stream);

/**
 * @brief Add audio output stream to specific esp_audio instance
 *
 * @param[in] handle       The esp_audio instance
 * @param[in] out_stream   The audio stream element instance
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 *      - ESP_ERR_AUDIO_MEMORY_LACK: allocate memory fail
 */
audio_err_t esp_audio_output_stream_add(esp_audio_handle_t handle, audio_element_handle_t out_stream);

/**
 * @brief Add a new codec lib that can decode or encode a music file
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] type      The audio codec type(encoder or decoder)
 * @param[in] lib       To provide audio stream element
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 *      - ESP_ERR_AUDIO_MEMORY_LACK: allocate memory fail
 */
audio_err_t esp_audio_codec_lib_add(esp_audio_handle_t handle, audio_codec_type_t type, audio_element_handle_t lib);

/**
 * @brief Check if this kind of music extension is supported or not
 *
 * @note This function just query the codec which has already add by esp_audio_codec_lib_add.
 *       The max length of extension is 6.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] type      The CODEC_ENCODER or CODEC_DECODER
 * @param[in] extension Such as "mp3", "wav", "aac"
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: supported
 *      - ESP_ERR_AUDIO_NOT_SUPPORT: not support
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_codec_lib_query(esp_audio_handle_t handle, audio_codec_type_t type, const char *extension);

/**
 * @brief Play the given uri
 *
 * The esp_audio_play have follow activity, setup inputstream, outputstream and codec by uri, start all of them.
 * There is a rule that esp_audio will select input stream, codec and output stream by URI field.

 * Rule of URI field are as follow.
 *     - `UF_SCHEMA` field of URI for choose input stream from existing streams. e.g:"http","file"
 *     - `UF_PATH` field of URI for choose codec from existing codecs. e.g:"/audio/mp3_music.mp3"
 *     - `UF_FRAGMENT` field of URI for choose output stream from existing streams, output stream is I2S by default.
 *     - `UF_USERINFO` field of URI for specific sample rate and channels at encode mode.
 *
 *     The format "user:password" in the userinfo field, "user" is sample rate, "password" is channels.
 *
 * Now esp_audio_play support follow URIs.
 *     - "https://dl.espressif.com/dl/audio/mp3_music.mp3"
 *     - "http://media-ice.musicradio.com/ClassicFMMP3"
 *     - "file://sdcard/test.mp3"
 *     - "iis://16000:2@from.pcm/rec.wav#file"
 *     - "iis://16000:1@record.pcm/record.wav#raw"
 *     - "aadp://44100:2@bt/sink/stream.pcm"
 *     - "hfp://8000:1@bt/hfp/stream.pcm"
 *
 * @note
 *     - The URI parse by `http_parser_parse_url`,any illegal string will be return `ESP_ERR_AUDIO_INVALID_URI`.
 *     - If the esp_decoder codec is added to `handle`, then the `handle` of esp_decoder will be set as the default decoder,
 *       even if other decoders are added.
 *     - Enabled `CONFIG_FATFS_API_ENCODING_UTF_8`, the URI can be support Chinese characters.
 *     - Asynchronous interface
 *     - The maximum of block time can be modify by `esp_audio_play_timeout_set`, default value is 25 seconds.
 *
 * @param handle The esp_audio_handle_t instance
 * @param uri    Such as "file://sdcard/test.wav" or "http://iot.espressif.com/file/example.mp3".
 *               If NULL to be set, the uri setup by`esp_audio_setup` will used.
 * @param type   Specific handle type decoder or encoder
 * @param pos    Specific starting position by bytes
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_TIMEOUT: timeout the play activity
 *      - ESP_ERR_AUDIO_NOT_SUPPORT: Currently status is AUDIO_STATUS_RUNNING
 *      - ESP_ERR_AUDIO_INVALID_URI: URI is illegal
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 *      - ESP_ERR_AUDIO_STOP_BY_USER: Exit without play due to esp_audio_stop has been called.
 */
audio_err_t esp_audio_play(esp_audio_handle_t handle, audio_codec_type_t type, const char *uri, int pos);

/**
 * @brief Play the given uri until music finished or error occured
 *
 * @note
 *     - All features are same with `esp_audio_play`
 *     - Synchronous interface
 *     - Support decoder mode only
 *     - No any events post during playing
 *
 * @param handle The esp_audio_handle_t instance
 * @param uri    Such as "file://sdcard/test.wav" or "http://iot.espressif.com/file/example.mp3",
 * @param pos    Specific starting position by bytes
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_TIMEOUT: timeout the play activity
 *      - ESP_ERR_AUDIO_NOT_SUPPORT: Currently status is AUDIO_STATUS_RUNNING
 *      - ESP_ERR_AUDIO_INVALID_URI: URI is illegal
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_sync_play(esp_audio_handle_t handle, const char *uri, int pos);

/**
 * @brief A synchronous interface for stop the esp_audio. The maximum of block time is 8000ms.
 *
 * @note 1. If user queue has been registered by evt_que, AUDIO_STATUS_STOPPED event for success
 *       or AUDIO_STATUS_ERROR event for error will be received.
 *       2. `TERMINATION_TYPE_DONE` only works with input stream which can't stopped by itself,
 *       e.g. `raw read/write stream`, others streams are no effect.
 *       3. The synchronous interface is used to ensure that working pipeline is stopped.
 *
 * @param[in] handle The esp_audio instance
 * @param[in] type   Stop immediately or done
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 *      - ESP_ERR_AUDIO_NOT_READY: The status is not AUDIO_STATUS_RUNNING or AUDIO_STATUS_PAUSED or element has not created
 *      - ESP_ERR_AUDIO_TIMEOUT: timeout(8000ms) the stop activity.
 */
audio_err_t esp_audio_stop(esp_audio_handle_t handle, audio_termination_type_t type);

/**
 * @brief Pause the esp_audio
 *
 * @note 1. Only support music and without live stream. If user queue has been registered by evt_que, AUDIO_STATUS_PAUSED event for success
 *       or AUDIO_STATUS_ERROR event for error will be received.
 *       2. The Paused music must be stoped by `esp_audio_stop` before new playing, otherwise got block on new play.
 *
 * @param[in] handle The esp_audio instance
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 *      - ESP_ERR_AUDIO_NOT_READY: the status is not running
 *      - ESP_ERR_AUDIO_TIMEOUT: timeout the pause activity.
 */
audio_err_t esp_audio_pause(esp_audio_handle_t handle);

/**
 * @brief Resume the music paused
 *
 * @note Only support music and without live stream. If user queue has been registered by evt_que, AUDIO_STATUS_RUNNING event for success
 *       or AUDIO_STATUS_ERROR event for error will be received.
 *
 * @param[in] handle The esp_audio instance
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 *      - ESP_ERR_AUDIO_TIMEOUT: timeout the resume activity.
 */
audio_err_t esp_audio_resume(esp_audio_handle_t handle);

/**
 * @brief Getting esp_audio play speed index, index value is from "esp_audio_speed_t" enum.
 *
 * @param[in]  handle          The esp_audio instance
 * @param[out] speed_index     Current audio play speed index. 
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_CTRL_HAL_FAIL: error with hardware.
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_speed_get(esp_audio_handle_t handle, esp_audio_play_speed_t *speed_index);

/**
 * @brief Use speed_index which is from "esp_audio_speed_t" enum to set esp_audio play speed.
 *
 * @param[in] handle          The esp_audio instance
 * @param[in] speed_index     Value from "esp_audio_speed_t" enum.  
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_CTRL_HAL_FAIL: error with hardware.
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_speed_set(esp_audio_handle_t handle, esp_audio_play_speed_t speed_index);

/**
 * @brief Use speed_index which is from "esp_audio_speed_t" enum to get esp_audio play speed which is float type.
 * 
 * @param[in]  handle          The esp_audio instance
 * @param[in]  speed_index     Current audio play speed index. 
 * @param[out] speed           Current audio play speed. 
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_CTRL_HAL_FAIL: error with hardware.
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_speed_idx_to_float(esp_audio_handle_t handle, esp_audio_play_speed_t speed_index, float *speed);

/**
 * @brief Setting esp_audio volume.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] vol       Specific volume will be set. 0-100 is legal. 0 will be mute.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_CTRL_HAL_FAIL: error with hardware.
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_vol_set(esp_audio_handle_t handle, int vol);

/**
 * @brief Get esp_audio volume
 *
 * @param[in] handle    The esp_audio instance
 * @param[out] vol      A pointer to int that indicates esp_audio volume.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_CTRL_HAL_FAIL: error with hardware.
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_vol_get(esp_audio_handle_t handle, int *vol);

/**
 * @brief Get esp_audio status
 *
 * @param[in] handle    The esp_audio instance
 * @param[out] state    A pointer to esp_audio_state_t that indicates esp_audio status.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no esp_audio instance or esp_audio does not playing
 */
audio_err_t esp_audio_state_get(esp_audio_handle_t handle, esp_audio_state_t *state);

/**
 * @brief Get the position in bytes of currently played music.
 *
 * @note This function works only with decoding music.
 *
 * @param[in] handle    The esp_audio instance
 * @param[out] pos      A pointer to int that indicates esp_audio decoding position.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no esp_audio instance
 *      - ESP_ERR_AUDIO_NOT_READY: no codec element
 */
audio_err_t esp_audio_pos_get(esp_audio_handle_t handle, int *pos);

/**
 * @brief Get the position in microseconds of currently played music.
 *
 * @note This function works only with decoding music.
 *
 * @param[in] handle    The esp_audio instance
 * @param[out] time     A pointer to int that indicates esp_audio decoding position.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no esp_audio instance
 *      - ESP_ERR_AUDIO_NOT_READY: no out stream
 */
audio_err_t esp_audio_time_get(esp_audio_handle_t handle, int *time);

/**
 * @brief Choose the `in_stream`, `codec` and `out_stream` definitely, and set `uri`.
 *
 * @note This function provide a manual way to select in/out stream and codec, should be called before the `esp_audio_play`,
 *       then ignore the `esp_audio_play` URI parameter only one time.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] sets      A pointer to esp_audio_setup_t.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no esp_audio instance
 *      - ESP_ERR_AUDIO_MEMORY_LACK: allocate memory fail
 */
audio_err_t esp_audio_setup(esp_audio_handle_t handle, esp_audio_setup_t *sets);

/* @brief Call this function before play to change the music source type.
 *
 * @note Music source type has been kept after music finished.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] type      Specific media source type will be set.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_media_type_set(esp_audio_handle_t handle, media_source_type_t type);

/* @brief Duplicate the current audio information to `info`.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] info      A pointer to esp_audio_info_t
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_MEMORY_LACK: allocate memory fail
 *      - ESP_ERR_AUDIO_NOT_READY: the status is not running.
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_info_get(esp_audio_handle_t handle, esp_audio_info_t *info);

/* @brief Restore the backup audio information to prevoise state.
 *
 * @note  If ESP_ERR_AUDIO_NO_ERROR returned, the info->codec_info.uri, info->in_el, info->codec_el,
 *        info->out_el and info->filter_el are set NULL.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] info      A pointer to esp_audio_info_t
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_FAIL: elements relink fail
 *      - ESP_ERR_AUDIO_MEMORY_LACK: allocate memory fail
 *      - ESP_ERR_AUDIO_TIMEOUT: timeout for relink
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 *      - ESP_ERR_AUDIO_NOT_READY: the status is AUDIO_STATUS_RUNNING or AUDIO_STATUS_PAUSED
 */
audio_err_t esp_audio_info_set(esp_audio_handle_t handle, esp_audio_info_t *info);

/* @brief Set esp_audio event callback.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] cb        A pointer to esp_audio_event_callback
 * @param[in] cb_ctx    The context of callback
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_callback_set(esp_audio_handle_t handle, esp_audio_event_callback cb, void *cb_ctx);

/**
 * @brief Seek the position in second of currently played music.
 *
 * @note This function works only with decoding music.
 *
 * @param[in] handle            The esp_audio instance
 * @param[out] seek_time_sec     A pointer to int that indicates esp_audio decoding position.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_FAIL: codec or allocation fail
 *      - ESP_ERR_AUDIO_TIMEOUT: timeout for sync the element status
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no esp_audio instance
 *      - ESP_ERR_AUDIO_NOT_SUPPORT: codec has finished
 *      - ESP_ERR_AUDIO_OUT_OF_RANGE: the seek_time_ms is out of the range
 *      - ESP_ERR_AUDIO_NOT_READY: the status is neither running nor paused
 */
audio_err_t esp_audio_seek(esp_audio_handle_t handle, int seek_time_sec);

/**
 * @brief Get the duration in microseconds of playing music.
 *
 * @note This function works only with decoding music.
 *
 * @param[in] handle        The esp_audio instance
 * @param[out] duration     A pointer to int that indicates decoding total time.
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no esp_audio instance
 *      - ESP_ERR_AUDIO_NOT_READY: no codec element or no in element
 */
audio_err_t esp_audio_duration_get(esp_audio_handle_t handle, int *duration);

/**
 * @brief Setting the maximum amount of time to waiting for `esp_audio_play` only.
 *
 * @param[in] handle        The esp_audio instance
 * @param[in] time_ms       The maximum amount of time
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: invalid arguments
 */
audio_err_t esp_audio_play_timeout_set(esp_audio_handle_t handle, int time_ms);

/**
 * @brief Get the type of `esp_audio_prefer_t`
 *
 * @param[in] handle        The esp_audio instance
 * @param[out] type         A pointer to esp_audio_prefer_t
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no esp_audio instance
 */
audio_err_t esp_audio_prefer_type_get(esp_audio_handle_t handle, esp_audio_prefer_t *type);

/**
 * @brief Set event queue to notify the esp_audio status
 *
 * @param[in] handle        The esp_audio instance
 * @param[out] que          A pointer to QueueHandle_t
 *
 * @return
 *      - ESP_ERR_AUDIO_NO_ERROR: on success
 *      - ESP_ERR_AUDIO_INVALID_PARAMETER: no esp_audio instance
 */
audio_err_t esp_audio_event_que_set(esp_audio_handle_t handle, QueueHandle_t que);

#ifdef __cplusplus
}
#endif

#endif
