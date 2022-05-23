/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __RECORDER_ENGINE_H__
#define __RECORDER_ENGINE_H__

#define REC_ONE_BLOCK_SIZE 2880     // 90ms[16k,16bit,1channel]

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_REC_ENGINE_CONFIG() {\
    .one_frame_duration_ms      = 10,\
    .sensitivity                = 0,\
    .vad_off_delay_ms           = 600,\
    .wakeup_time_ms             = 10000,\
    .task_core                  = 1,\
    .enable_wwe                 = true,\
    .open                       = NULL,\
    .fetch                      = NULL,\
    .close                      = NULL,\
    .evt_cb                     = NULL,\
}
/*
// Ther are some decription of recorder events with 4 use case.

///////////////////////////// Use case 1: Single dialog by rec_engine_trigger_start ///////////////////////
                      +                                  + wakeup time  +
                      |                                  +--------------+
                      |                                  |              |
                      |                                  |              |
Wakeup Time  +--------+                                  |              +-----------------+
                      |                                  |              |
                      |         +---------------+        |              |
                      |         |   voice       |        |              |
                      |         |               |        |              |
Voice level  +------------------+               +-----------------------+-----------------+
                      |                         |        |
                      |         +------------------------+              |
                      |         |               |        |              |
                      |         |               |        |              |
EVENT        +------------------+               |        +--------------------------------+
                      |\        |\              |  vad   |\             |\
                      | \       | \             |  off   | \            | \
                      |  WAKEUP |  VAD START    |  time  |  VAD STOP    |   WAKEUP END
                      +  START  +               +        +              +

///////////////////////////// Use case 2: Single dialog by voice wakeup /////////////////////////////////
                          +                                  + wakeup time  +
                          |                                  +--------------+
                          |                                  |              |
                          |                                  |              |
Wakeup Time  +------------+                                  |              +------------+
                          |                                  |              |
                 +--------+         +---------------+        |              |
                 | wakeup |         |   voice       |        |              |
                 |  word  |         |               |        |              |
Voice level  +------------+---------+               +-----------------------+------------+
                          |                         |        |
                          |         +------------------------+              |
                          |         |               |        |              |
                          |         |               |        |              |
EVENT        +----------------------+               |        +---------------------------+
                          |\        |\              |  vad   |\             |\
                          | \       | \             |  off   | \            | \
                          |  WAKEUP |  VAD START    |  time  |  VAD STOP    |   WAKEUP END
                          +  START  +               +        +              +

/////////////////////////////// Use case 3: Multi dialog after rec_engine_trigger_start ////////////////////
                         +                                                        + wakeup time +
                         |                                                        +-------------+
                         |                                                        |             |
                         |                                                        |             |
Wakeup Time  +-----------+                                                        |             +----------+
                         +         +----------+      +-------+   +---------+      |             |
                         |         |  voice   |      | music |   |  voice  |      |             |
                         |         |          |      |       |   |         |      |             |
Voice level  +-----------+---------+          +------+       +---+         +-------------------------------+
                         |                    |                  |         |      |             |
                         |                    +----------------+ |         |      |             |
                         |       SUSPEND ON \ |   SUSPEND OFF\ | |         |      |             |
                         |                   \|               \| |         |      |             |
SUSPEND ON   +--------------------------------+                +-------------------------------------------+
                         |                    |                  |         |      |             |
                         |         +----------+                  |         +------+             |
                         |         |          |                  |         |      |             |
                         |         |          |                  |         |      |             |
EVENT        +---------------------+          +------------------------------------------------------------+
                         |\        |\         |\                /|         |      |\            |\
                         | \       | \        | \              / |         | vad  | \           | \
                         |  WAKEUP |  VAD     |  VAD       VAD   |         | off  | VAD         |  WAKEUP
                         +  START  +  START   +  STOP      START +         + time + STOP        +  END

////////////////////////////////////// Use case 4: Multi dialog after voice wakeup ////////////////////////
                         +                                                        + wakeup time +
                         |                                                        +-------------+
                         |                                                        |             |
                         |                                                        |             |
Wakeup Time  +-----------+                                                        |             +---------+
                +--------+         +----------+      +-------+   +---------+      |             |
                | wakeup |         |  voice   |      | music |   |  voice  |      |             |
                |   word |         |          |      |       |   |         |      |             |
Voice level  +--+--------+---------+          +------+       +---+         +------------------------------+
                         |                    |                  |         |      |             |
                         |                    +----------------+ |         |      |             |
                         |       SUSPEND ON \ |   SUSPEND OFF\ | |         |      |             |
                         |                   \|               \| |         |      |             |
SUSPEND ON   +--------------------------------+                +------------------------------------------+
                         |                    |                  |         |      |             |
                         |         +----------+                  |         +------+             |
                         |         |          |                  |         |      |             |
                         |         |          |                  |         |      |             |
EVENT        +---------------------+          +-----------------------------------------------------------+
                         |\        |\         |\                /|         |      |\            |\
                         | \       | \        | \              / |         | vad  | \           | \
                         |  WAKEUP |  VAD     |  VAD       VAD   |         | off  | VAD         |  WAKEUP
                         +  START  +  START   +  STOP      START +         + time + STOP        +  END

///////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
typedef enum {
    REC_EVENT_WAKEUP_START,
    REC_EVENT_WAKEUP_END,
    REC_EVENT_VAD_START,
    REC_EVENT_VAD_STOP,
} rec_event_type_t;

typedef void (*rec_callback)(rec_event_type_t type, void *user_data);
typedef esp_err_t (*rec_open)(void **handle);
typedef esp_err_t (*rec_fetch)(void *handle, char *data, int data_size);
typedef esp_err_t (*rec_close)(void *handle);

typedef enum {
    REC_VOICE_SUSPEND_OFF,
    REC_VOICE_SUSPEND_ON,
} rec_voice_suspend_t;

/**
 * @brief recorder configuration parameters
 */
typedef struct {
    int           one_frame_duration_ms;      /*!< Duration of one frame (optional) */
    int           sensitivity;                /*!< For response accuracy rate sensitivity. Default 0: 90%, 1: 95% */
    int           vad_off_delay_ms;           /*!< Vad off delay to stop if no voice is detected */
    int           wakeup_time_ms;             /*!< Time of wakeup */
    bool          support_encoding;           /*!< Support encoding data */
    const char   *extension;                  /*!< Encoding format."amr" or "amrwb" support */
    int           task_core;                  /*!< Recorder task running in core (0 or 1) */
    bool          enable_wwe;                 /*!< Enable Wake Word Engine or not */
    rec_open      open;                       /*!< Recorder open callback function */
    rec_fetch     fetch;                      /*!< Recorder fetch data callback function */
    rec_close     close;                      /*!< Recorder close callback function */
    rec_callback  evt_cb;                     /*!< Recorder event callback function */
    void          *user_data;                 /*!< Pointer to user data (optional) */
} rec_config_t;

/**
 * @brief Create recorder engine according to parameters.
 *
 * @note Sample rate is 16k, 1 channel, 16bits, by default.
 *       Upon completion of this function rec_open callback will be triggered.
 *
 * @param cfg   See rec_config_t structure for additional details
 *
 * @return
 *     - 0: Success
 *     - -1: Error
 */
esp_err_t rec_engine_create(rec_config_t *cfg);

/**
 * @brief Read voice data after REC_EVENT_VAD_START.
 *
 * @param buffer        data pointer
 * @param buffer_size   Size of buffer, must be equal to REC_ONE_BLOCK_SIZE.
 * @param waiting_time  Timeout for reading data. Default time of REC_ONE_BLOCK_SIZE is 100ms, larger than 100ms is recommended.
 *
 * @return
 *      - -2    : timeout of read
 *      - -1    : parameters invalid or task not running.
 *      - 0     : last voice block.
 *      - others: voice block index.
 */
int rec_engine_data_read(uint8_t *buffer, int buffer_size, int waiting_time);

/**
 * @brief Suspend or enable voice detection by vad.
 *
 * @param flag  REC_VOICE_SUSPEND_ON: Voice detection is suspended
 *              REC_VOICE_SUSPEND_OFF: Voice detection is not suspended
 *
 * @return
 *      - 0: Success
 *      - -1: Error
 */
esp_err_t rec_engine_detect_suspend(rec_voice_suspend_t flag);

/**
 * @brief Start recording by force.
 *
 * @return
 *      - 0: Success
 *      - -1: Error
 */
esp_err_t rec_engine_trigger_start(void);

/**
 * @brief Stop recording by force.
 *
 * @return
 *      - 0: Success
 *      - -1: Error
 */
esp_err_t rec_engine_trigger_stop(void);

/**
 * @brief Destroy the recorder engine.
 *
 * @note Upon completion of this function rec_close callback will be triggered.
 *
 * @return
 *      - 0: Success
 *      - -1: Error
 */
esp_err_t rec_engine_destroy(void);

/**
 * @brief Disable or enable the VAD(voice activity detection).
 *
 * @note Enable vad by default.
 *       Usage: Call this function before `rec_engine_trigger_start` to disable voice activity detection,
 *              Call this funciton after `rec_engine_trigger_stop` to enable voice activity detection.
 *              Even if disable voice activity detection, the `REC_EVENT_VAD_START` and `REC_EVENT_VAD_STOP` events
 *              still notified when `rec_engine_trigger_start` and `rec_engine_trigger_stop` called.
 *
 * @param vad_enable true is enable vad, false disable vad
 *
 * @return
 *      - 0: Success
 *      - -1: Error
 */
esp_err_t rec_engine_vad_enable(bool vad_enable);

/**
 * @brief Enable the recoder encoding, or not.
 *
 * @note `support_encoding` must be set, `rec_engine_enc_enable` can be used.
 *       Disable encoding by default.
 *
 * @param enc_enable true is enable encoding, false is disable.
 *
 * @return
 *     - 0: Success
 *     - -1: Error
 */
esp_err_t rec_engine_enc_enable(bool enc_enable);

/**
 * @brief Read voice data after REC_EVENT_VAD_START.
 *
 * @note `support_encoding` and `rec_engine_enc_enable` must be set.
 *
 * @param buffer        data pointer
 * @param buffer_size   Size of buffer, must be equal to REC_ONE_BLOCK_SIZE.
 * @param waiting_time  Timeout for reading data.
 * @param out_size      Valid size of buffer.
 *
 * @return
 *      - -2    : timeout of read
 *      - -1    : parameters invalid or not encoding mode.
 *      - 0     : success.
 *      - others: voice block index.
 */
esp_err_t rec_engine_enc_data_read(uint8_t *buffer, int buffer_size, int waiting_time, int *out_size);

/**
 * @brief Enable the recoder mute, or not.
 *
 * @note if enable mute, no data fill the buffer, so the `rec_engine_enc_data_read` and `rec_engine_data_read` will be blocked.
 *
 * @param mute_enable true is mute, false is not.
 *
 * @return
 *     - 0: Success
 *     - -1: Error
 */
esp_err_t rec_engine_mute_enable(bool mute_enable);

/**
 * @brief Get recorder engine wakeup state.
 *
 * @param wakeup_start_t true is WAKEUP_START, false is not.
 *
 * @return
 *     - 0: Success
 *     - -1: Error
 */
esp_err_t rec_engine_get_wakeup_stat(bool *wakeup_start_t);

#ifdef __cplusplus
}
#endif

#endif
