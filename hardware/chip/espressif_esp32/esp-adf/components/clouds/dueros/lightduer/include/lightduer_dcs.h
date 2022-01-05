/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * File: lightduer_dcs.h
 * Auth: Gang Chen (chengang12@baidu.com)
 * Desc: Light duer DCS APIS.
 */
#ifndef BAIDU_DUER_LIGHTDUER_DCS_H
#define BAIDU_DUER_LIGHTDUER_DCS_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DCS_PLAY_CMD,
    DCS_PAUSE_CMD,
    DCS_PREVIOUS_CMD,
    DCS_NEXT_CMD,
    DCS_PLAY_CONTROL_EVENT_NUMBER,
} duer_dcs_play_control_cmd_t;

typedef enum {
    DCS_MEDIA_ERROR_UNKNOWN,               // unknown error
    // server invalid response, such as bad request, forbidden, not found .etc
    DCS_MEDIA_ERROR_INVALID_REQUEST,
    DCS_MEDIA_ERROR_SERVICE_UNAVAILABLE,   // device cannot connect to server
    DCS_MEDIA_ERROR_INTERNAL_SERVER_ERROR, // server failed to handle device's request
    DCS_MEDIA_ERROR_INTERNAL_DEVICE_ERROR, // device internal error
} duer_dcs_audio_error_t;

typedef enum {
    DCS_RECOMMEND_POWER_ON,
    DCS_RECOMMEND_OUT_OF_BOX,
    DCS_RECOMMEND_TIME_NUMBER,
} duer_dcs_recommend_time_t;

typedef struct {
    const char *url;
    int offset;
    const char *audio_item_id;
} duer_dcs_audio_info_t;

enum duer_dcs_device_capability {
    DCS_TTS_HTTPS_PROTOCAL_SUPPORTED = 0x01, // the device support https protocal to playing tts
    DCS_WECHAT_SUPPORTED             = 0x02, // the device support wechat
};

/**
 * Internal used private namespace.
 */
extern const char *DCS_PRIVATE_NAMESPACE;

/**
 * Initialize the dcs framework.
 *
 * @return none.
 */
void duer_dcs_framework_init(void);

/**
 * Uninitialize the DCS module.
 *
 * @return none.
 */
void duer_dcs_uninitialize(void);

/**
 * DESC:
 * Initialize dcs voice input interface.
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_voice_input_init(void);

/**
 * DESC:
 * Notify DCS when recorder start to record.
 *
 * PARAM: none
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_on_listen_started(void);

/**
 * DESC:
 * There is no VAD in translate scene,
 * hence developer needs to call this function to notify DCS framework after recording finished.
 *
 * PARAM: none
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_on_listen_stopped(void);

/**
 * DESC:
 * Developer needs to implement this interface to start recording.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_dcs_listen_handler(void);

/**
 * DESC:
 * Developer needs to implement this interface to stop recording.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_dcs_stop_listen_handler(void);

/**
 * DESC:
 * Initialize dcs voice output interface
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_voice_output_init(void);

/**
 * DESC:
 *
 * It should be called when speech finished, used to notify DCS level.
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_speech_on_finished(void);

/**
 * DESC:
 * Developer needs to implement this interface to play speech.
 *
 * PARAM:
 * @param[in] url: the url of the speech need to play
 *
 * @RETURN: none.
 */
void duer_dcs_speak_handler(const char *url);

/**
 * DESC:
 * Developer needs to implement this interface to stop speech.
 *
 * PARAM:
 * @param: none
 *
 * @RETURN: none.
 */
void duer_dcs_stop_speak_handler(void);

/**
 * DESC:
 * Initialize dcs speaker controler interface to enable volume control function.
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_speaker_control_init(void);

/**
 * DESC:
 * Notify DCS when volume changed
 *
 * PARAM: none
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_on_volume_changed(void);

/**
 * DESC:
 * Notify DCS when mute state changed.
 *
 * PARAM: none
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_on_mute(void);

/**
 * DESC:
 * Developer needs to implement this interface, it is used to get volume state.
 *
 * @param[out] volume: current volume value.
 * @param[out] is_mute: current mute state.
 *
 * @RETURN: none.
 */
void duer_dcs_get_speaker_state(int *volume, duer_bool *is_mute);

/**
 * DESC:
 * Developer needs to implement this interface to set volume.
 *
 * PARAM:
 * @param[in] volume: the value of the volume need to set
 *
 * @RETURN: none.
 */
void duer_dcs_volume_set_handler(int volume);

/**
 * DESC:
 * Developer needs to implement this interface to adjust volume.
 *
 * PARAM:
 * @param[in] volume: the value need to adjusted.
 *
 * @RETURN: none.
 */
void duer_dcs_volume_adjust_handler(int volume);

/**
 * DESC:
 * Developer needs to implement this interface to change mute state.
 *
 * PARAM:
 * @param[in] is_mute: set/discard mute.
 *
 * @RETURN: none.
 */
void duer_dcs_mute_handler(duer_bool is_mute);

/**
 * DESC:
 * Initialize dcs audio player interface.
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_audio_player_init(void);

/**
 * DESC:
 * User can also use this API to subscribe FM.
 *
 * PARAM[in] url: the url identify the FM.
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_on_link_clicked(const char *url);

/**
 * DESC:
 * Notify DCS when an audio is finished.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_dcs_audio_on_finished(void);

/**
 * DESC:
 * Notify DCS when failed to play audio.
 *
 * PARAM[in] type: the error type
 * PARAM[in] msg: the error message
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_audio_play_failed(duer_dcs_audio_error_t type, const char *msg);

/**
 * DESC:
 * Report StreamMetadataExtracted event if metadata is found in the audio.
 *
 * PARAM[in] metadata: the metadata need to report, its layout is:
 *                     "metadata": {
 *                         "{{STRING}}": "{{STRING}}",
 *                         "{{STRING}}": {{BOOLEAN}}
 *                         "{{STRING}}": "{{STRING NUMBER}}"
 *                         ...
 *                     }
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_audio_report_metadata(baidu_json *metadata);

/**
 * DESC:
 * Notify DCS when audio is stuttered.
 *
 * PARAM[in] is_stuttuered: DUER_TRUE when stutter started(download speed lower than play speed),
 *                          DUER_FALSE when stutter finished(the audio resume play).
 */
int duer_dcs_audio_on_stuttered(duer_bool is_stuttered);

/**
 * DESC:
 * Developer needs to implement this interface to play audio.
 *
 * PARAM:
 * @param[in] audio_info: the info of the audio need to play
 *
 * @RETURN: none.
 */
void duer_dcs_audio_play_handler(const duer_dcs_audio_info_t *audio_info);

/**
 * DESC:
 * Developer needs to implement this interface to stop audio player.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_dcs_audio_stop_handler(void);

/**
 * DESC:
 * Notify DCS when an audio is stopped not by DCS API.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_dcs_audio_on_stopped(void);

/**
 * DESC:
 * Developer needs to implement this interface to resume audio play.
 *
 * PARAM:
 * @param[in] audio_info: the info of the audio need to resumed
 *
 * @RETURN: none.
 */
void duer_dcs_audio_resume_handler(const duer_dcs_audio_info_t *audio_info);

/**
 * DESC:
 * Developer needs to implement this interface to pause audio play.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_dcs_audio_pause_handler(void);

/**
 * DESC:
 * Developer needs to implement this interface, it's used to get the audio play progress.
 *
 * PARAM: none
 *
 * @RETURN: the play position of the audio.
 */
int duer_dcs_audio_get_play_progress(void);

/**
 * DESC:
 * Realize play control(play, pause, next/previous audio) by send command to DCS.
 *
 * PARAM[in] cmd: command type.
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_send_play_control_cmd(duer_dcs_play_control_cmd_t cmd);

/**
 * DESC:
 * Report current state after device boot.
 *
 * PARAM: none
 *
 * @RETURN: 0 if succuss, negative if failed.
 */
int duer_dcs_sync_state(void);

/**
 * DESC:
 * Sending an "Exited" event to close the multi dialogue.
 *
 * @RETURN: 0 if succuss, negative if failed.
 */
int duer_dcs_close_multi_dialog(void);

/**
 * DESC:
 * Initialize dcs screen interface.
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_screen_init(void);

/**
 * DESC:
 * Developer needs to implement this interface to get the ASR result.
 *
 * PARAM[in] text: the ASR text result.
 * PARAM[in] type: "INTERMEDIATE" or "FINAL".
 *
 * @RETURN: DUER_OK if success,
 *          DUER_MSG_RSP_BAD_REQUEST if the payload is invalid,
 *          DUER_ERR_FAILED if other error happened.
 */
duer_status_t duer_dcs_input_text_handler(const char *text, const char *type);

/**
 * DESC:
 * Developer needs to implement this interface to get render card.
 *
 * PARAM[in] payload: the information of the render card, please reference the DCS document.
 *
 * @RETURN: DUER_OK if success,
 *          DUER_MSG_RSP_BAD_REQUEST if the payload is invalid,
 *          DUER_ERR_FAILED if other error happened.
 */
duer_status_t duer_dcs_render_card_handler(baidu_json *payload);

/**
 * DESC:
 * Developer needs to implement this interface to open/close bluetooth.
 *
 * PARAM[in] is_switch: open bluetooth if is_switch is DUER_TRUE, otherwise close bluetooth.
 * PARAM[in] target: Reserved parameter, currently its value is "default".
 *
 * @RETURN: None
 */
void duer_dcs_bluetooth_set_handler(duer_bool is_switch, const char *target);

/**
 * DESC:
 * Developer needs to implement this interface to connect/disconnect bluetooth to device.
 *
 * PARAM[in] is_connect: connect bluetooth if is_connect is DUER_TRUE, otherwise disconnect bluetooth.
 * PARAM[in] target: Reserved parameter, currently its value is "default".
 *
 * @RETURN: None
 */
void duer_dcs_bluetooth_connect_handler(duer_bool is_connect, const char *target);

/**
 * DESC:
 * Initialize device control interface, such as control the bluetooth.
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_device_control_init(void);

/**
 * DESC:
 * Used to declare the device capability, such as: whether https or wechat are supported.
 * It is unnecessary if the device don't have the capability defined in duer_dcs_device_capability.
 * @PARAM[in] capability: the device capability, it's the or value of the member in
 *                        duer_dcs_device_capability.
 * @RETURN:   DUER_OK if success, negative if failed.
 */
duer_status_t duer_dcs_capability_declare(duer_u32_t capability);

/**
 * DESC:
 * Sending RecommendationRequested event, and DCS server will recommend a resource to device.
 * Currently, only the scene of "POWER_ON" is supported, that means this function should be called
 * only one time(when device power_on).
 *
 * @PARAM[in] type: type of the scene.
 * @RETURN: 0 if succuss, negative if failed.
 */
int duer_dcs_recommend_request(duer_dcs_recommend_time_t time);

/**
 * DESC:
 * Similar to duer_dcs_on_listen_started, except not create new dialog id, but get current one.
 *
 * PARAM: none
 *
 * @RETURN: 0 when success, negative when fail.
 */
int duer_dcs_on_listen_started_with_current_dialogid(void);

/**
 * DESC:
 * Developer needs to implement this interface to pause local player.
 *
 * PARAM: none
 *
 * @RETURN: None
 */
void duer_dcs_local_player_pause_handler(void);

/**
 * DESC:
 * Developer needs to implement this interface to resume local player.
 *
 * PARAM: none
 *
 * @RETURN: None
 */
void duer_dcs_local_player_resume_handler(void);

/**
 * DESC:
 * Developer needs to implement this interface to stop local player.
 *
 * PARAM: none
 *
 * @RETURN: None
 */
void duer_dcs_local_player_stop_handler(void);

/**
 * DESC:
 * Notify DCS module when local player paused.
 *
 * PARAM: none
 *
 * @RETURN: None
 */
void duer_dcs_local_player_on_paused(void);

/**
 * DESC:
 * Notify DCS module when local player playing.
 *
 * PARAM: none
 *
 * @RETURN: None
 */
void duer_dcs_local_player_on_played(void);

/**
 * DESC:
 * Notify DCS module when local player stopped.
 *
 * PARAM: none
 *
 * @RETURN: None
 */
void duer_dcs_local_player_on_stopped(void);

/**
 * DESC:
 * Get the client context.
 *
 * PARAM: none
 *
 * @RETURN: NULL if failed, pointer point to the client context if success.
 */
baidu_json *duer_dcs_get_client_context(void);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_DCS_H*/

