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
 * File: lightduer_dcs_local.h
 * Auth: Gang Chen (chengang12@baidu.com)
 * Desc: Provide some functions for dcs module locally.
 */
#ifndef BAIDU_DUER_LIGHTDUER_DCS_LOCAL_H
#define BAIDU_DUER_LIGHTDUER_DCS_LOCAL_H

#include <stdlib.h>
#include <stdint.h>
#include "baidu_json.h"
#include "lightduer_types.h"
#include "lightduer_log.h"
#include "lightduer_dcs_router.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DCS_DIALOG_FINISHED,
    DCS_RECORD_STARTED,
    DCS_SPEECH_NEED_PLAY,
    DCS_SPEECH_FINISHED,
    DCS_AUDIO_NEED_PLAY,
    DCS_AUDIO_FINISHED,
    DCS_AUDIO_STOPPED,
    DCS_NEED_OPEN_MIC,
    DCS_ALERT_STARTED,
    DCS_ALERT_FINISHED,
} duer_dcs_channel_switch_event_t;

#ifdef DCS_CRITICAL_LOCK_DEBUG
#define DUER_DCS_CRITICAL_ENTER() \
    do {\
        DUER_LOGI("%s entry", __FUNCTION__);\
        duer_dcs_critical_enter_internal();\
        DUER_LOGI("in");\
    } while (0)

#define DUER_DCS_CRITICAL_EXIT() \
    do {\
        DUER_LOGI("%s exit", __FUNCTION__);\
        duer_dcs_critical_exit_internal();\
        DUER_LOGI("out");\
    } while (0)
#else
#define DUER_DCS_CRITICAL_ENTER() duer_dcs_critical_enter_internal()
#define DUER_DCS_CRITICAL_EXIT() duer_dcs_critical_exit_internal()
#endif

// resouce path
extern const char *DCS_DUER_DIRECTIVE_PATH;
extern const char *DCS_DUER_PRIVATE_PATH;
extern const char *DCS_IOTCLOUD_DIRECTIVE_PATH;
extern const char *DCS_IOTCLOUD_EVENT_PATH;

// namespace
extern const char *DCS_AUDIO_PLAYER_NAMESPACE;
extern const char *DCS_VOICE_OUTPUT_NAMESPACE;
extern const char *DCS_VOICE_INPUT_NAMESPACE;
extern const char *DCS_SCREEN_NAMESPACE;
extern const char *DCS_SCREEN_EXTENDED_CARD_NAMESPACE;
extern const char *DCS_IOT_CLOUD_SYSTEM_NAMESPACE;
extern const char *DCS_PLAYBACK_CONTROL_NAMESPACE;
extern const char *DCS_RECOMMEND_NAMESPACE;

// message keys
extern const char *DCS_DIRECTIVE_KEY;
extern const char *DCS_CLIENT_CONTEXT_KEY;
extern const char *DCS_EVENT_KEY;
extern const char *DCS_HEADER_KEY;
extern const char *DCS_PAYLOAD_KEY;

// header keys
extern const char *DCS_NAMESPACE_KEY;
extern const char *DCS_NAME_KEY;
extern const char *DCS_MESSAGE_ID_KEY;
extern const char *DCS_DIALOG_REQUEST_ID_KEY;

// payload keys
extern const char *DCS_TOKEN_KEY;
extern const char *DCS_TYPE_KEY;
extern const char *DCS_CODE_KEY;
extern const char *DCS_VOLUME_KEY;
extern const char *DCS_URL_KEY;
extern const char *DCS_ACTIVE_ALERTS_KEY;
extern const char *DCS_TARGET_KEY;
extern const char *DCS_SCHEDULED_TIME_KEY;
extern const char *DCS_PLAYER_ACTIVITY_KEY;
extern const char *DCS_ALL_ALERTS_KEY;
extern const char *DCS_ERROR_KEY;
extern const char *DCS_MUTE_KEY;
extern const char *DCS_INITIATOR_KEY;
extern const char *DCS_AUDIO_ITEM_KEY;
extern const char *DCS_CLEAR_BEHAVIOR_KEY;
extern const char *DCS_PLAY_BEHAVIOR_KEY;
extern const char *DCS_INACTIVE_TIME_IN_SECONDS_KEY;
extern const char *DCS_OFFSET_IN_MILLISECONDS_KEY;
extern const char *DCS_TIMEOUT_IN_MILLISECONDS_KEY;
extern const char *DCS_METADATA_KEY;
extern const char *DCS_TEXT_KEY;
extern const char *DCS_CONNECTION_SWITCH_KEY;
extern const char *DCS_DESCRIPTION_KEY;
extern const char *DCS_BLUETOOTH_KEY;
extern const char *DCS_MUTED_KEY;
extern const char *DCS_UNPARSED_DIRECTIVE_KEY;
extern const char *DCS_FORMAT_KEY;
extern const char *DCS_MESSAGE_KEY;
extern const char *DCS_QUERY_KEY;
extern const char *DCS_TIME_OF_RECOMMEND;
extern const char *DCS_STUTTER_DURATION;
extern const char *DCS_PREVIOUS_DIALOG_ID_KEY;
extern const char *DCS_ACTIVE_DIALOG_ID_KEY;

// directive name
extern const char *DCS_SPEAK_NAME;
extern const char *DCS_PLAY_NAME;
extern const char *DCS_STOP_NAME;
extern const char *DCS_CLEAR_QUEUE_NAME;
extern const char *DCS_LISTEN_NAME;
extern const char *DCS_GET_STATUS_NAME;
extern const char *DCS_STOP_SPEAK_NAME;
extern const char *DCS_SET_BLUETOOTH_NAME;
extern const char *DCS_RENDER_CARD_NAME;
extern const char *DCS_SET_ALERT_NAME;
extern const char *DCS_STOP_LISTEN_NAME;
extern const char *DCS_SET_BLUETOOTH_CONNECTION_NAME;
extern const char *DCS_SET_MUTE_NAME;
extern const char *DCS_DELETE_ALERT_NAME;
extern const char *DCS_RENDER_VOICE_INPUT_TEXT_NAME;
extern const char *DCS_SET_VOLUME_NAME;
extern const char *DCS_ADJUST_VOLUME_NAME;
extern const char *DCS_TEXT_INPUT_NAME;
extern const char *DCS_RENDER_WEATHER;
extern const char *DCS_RENDER_PLAYER_INFO;
extern const char *DCS_RENDER_AUDIO_LIST;
extern const char *DCS_RENDER_ALBUM_LIST;
extern const char *DCS_SET_ACTIVE_DIALOG;

// internal directive.
extern const char *DCS_DIALOGUE_FINISHED_NAME;
extern const char *DCS_THROW_EXCEPTION_NAME;
extern const char *DCS_RESET_USER_INACTIVITY_NAME;
extern const char *DCS_NOP_NAME;
extern const char *DCS_IOT_CLOUD_CONTEXT;

// event name
extern const char *DCS_SYNCHRONIZE_STATE_NAME;
extern const char *DCS_EXCEPTION_ENCOUNTERED_NAME;
extern const char *DCS_VOLUME_STATE_NAME;
extern const char *DCS_LISTEN_STARTED_NAME;
extern const char *DCS_SPEECH_STARTED_NAME;
extern const char *DCS_ALERTS_STATE_NAME;
extern const char *DCS_LISTEN_TIMED_OUT_NAME;
extern const char *DCS_EXITED_NAME;
extern const char *DCS_PLAYBACK_STATE_NAME;
extern const char *DCS_SPEECH_FINISHED_NAME;
extern const char *DCS_VOLUME_CHANGED_NAME;
extern const char *DCS_MUTE_CHANGED_NAME;
extern const char *DCS_SPEECH_STATE_NAME;
extern const char *DCS_LINK_CLICKED_NAME;
extern const char *DCS_RECOMMEND_NAME;

// internal exception type
extern const char *DCS_UNEXPECTED_INFORMATION_RECEIVED_TYPE;
extern const char *DCS_INTERNAL_ERROR_TYPE;
extern const char *DCS_UNSUPPORTED_OPERATION_TYPE;

/**
 * DESC:
 * Get current dialog request id.
 *
 * PARAM: none
 *
 * @RETURN: current dialog request id
 */
const char *duer_get_request_id_internal(void);

/**
 * DESC:
 * Create a new dialog request id for each conversation.
 *
 * PARAM: none
 *
 * @RETURN: the new dialog request id
 */
const char *duer_create_request_id_internal(void);

/**
 * DESC:
 * Get the client context.
 *
 * PARAM: none
 *
 * @RETURN: NULL if failed, pointer point to the client context if success.
 */
baidu_json *duer_get_client_context_internal(void);

/**
 * DESC:
 * Check whether there is a speech waiting to play.
 *
 * PARAM: none
 *
 * @RETURN: DUER_TRUE if a speech is waiting to play, DUER_FALSE if not.
 */
duer_bool duer_speech_need_play_internal(void);

/**
 * DESC:
 * Pause the audio player.
 *
 * PARAM is_breaking: breaking a audio or a new audio is pending
 *
 * @RETURN: none.
 */
void duer_pause_audio_internal(duer_bool is_breaking);

/**
 * DESC:
 * Resume the audio player.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_resume_audio_internal(void);

/**
 * DESC:
 * Notify that speech is stopped.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_speech_on_stop_internal(void);

/**
 * DESC:
 * Notify that audio is stopped.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_dcs_audio_on_stopped_internal(void);

/**
 * DESC:
 * Report exception.
 *
 * @PARAM[in] directive: which directive cause to the exception
 * @PARAM[in] directive_len: the length of directive, if directive_len <=0, the value got from strlen
 * @PARAM[in] type: exception type
 * @PARAM[in] msg: excetpion content
 *
 * @RETURN: 0 if success, negative if failed.
 */
int duer_report_exception_internal(const char* directive, size_t directive_len,
                                   const char* type, const char* msg);

/**
 * DESC:
 * get Report exception in baidu_json format.
 *
 * @PARAM[in] directive: which directive cause to the exception
 * @PARAM[in] directive_len: the length of directive, if directive_len <=0, the value got from strlen
 * @PARAM[in] type: exception type
 * @PARAM[in] msg: excetpion content
 *
 * @RETURN: NULL if failed.
 */
baidu_json *duer_get_exception_internal(const char *directive, size_t directive_len,
                                        const char *type, const char *msg);

/**
 * DESC:
 * Declare the system interface.
 *
 * @PARAM: none
 *
 * @RETURN: none.
 */
void duer_declare_sys_interface_internal(void);

/**
 * DESC:
 * Used to reset user activety time.
 *
 * @PARAM: none
 *
 * @RETURN: none.
 */
void duer_user_activity_internal(void);

/**
 * DESC:
 * Returns a pointer to a new string which is a duplicate of the string 'str'.
 *
 * @PARAM[in] str: the string need to duplicated.
 *
 * @RETURN: a pointer to the duplicated string, or NULL if insufficient memory was available.
 */
char *duer_strdup_internal(const char *str);

/**
 * DESC:
 * Used to check whether there is a multiple rounds dialogue.
 *
 * @PARAM: none
 *
 * @RETURN: DUER_TRUE if it is multiple round dialogue.
 */
duer_bool duer_is_multiple_round_dialogue(void);

/**
 * DESC:
 * Checking whether a audio is playing.
 *
 * @RETURN: Reture DUER_TRUE if audio is playing, else returen DUER_FALSE.
 */
duer_bool duer_audio_is_playing_internal(void);

/**
 * DESC:
 * Chosing the highest priority play channel.
 *
 * @RETURN: None.
 */
void duer_play_channel_control_internal(duer_dcs_channel_switch_event_t event);

/**
 * DESC:
 * Checking whether a audio is paused by higher play channel.
 *
 * @RETURN: Reture DUER_TRUE if audio is paused, else returen DUER_FALSE.
 */
duer_bool duer_audio_is_paused_internal(void);

/**
 * DESC:
 * Initialize the local resource, such as lock.
 *
 * @RETURN: None.
 */
void duer_dcs_local_init_internal(void);

/**
 * DESC:
 * Checking whether the micphone is recording or not.
 *
 * @RETURN: Reture DUER_TRUE if audio is paused, else returen DUER_FALSE.
 */
duer_bool duer_is_recording_internal(void);

/**
 * DESC:
 * Starting the audio player.
 *
 * @RETURN: None.
 */
void duer_start_audio_play_internal(void);

/**
 * DESC:
 * DCS mode use it to report data.
 *
 * @PARAM[in] data: the report data.
 * @PARAM[in] is_transparent: some items might be added to data if it is DUER_FALSE,
 *                            such as the English to Chinese or Chinese to English translate flag.
 *
 * @RETURN: success return DUER_OK, failed return DUER_ERR_FAILED.
 */
int duer_dcs_data_report_internal(baidu_json *data, duer_bool is_transparent);

/**
 * DESC:
 * Open micphone.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_open_mic_internal(void);

/**
 * DESC:
 * Cancel the cached directive.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_cancel_caching_directive_internal(void);

/**
 * DESC:
 * Used to check should we resume audio after dialog finish or not.
 *
 * @PARAM: none.
 *
 * @RETURN: DUER_TRUE: don't resume audio until dialog finish directive executed.
 */
duer_bool duer_wait_dialog_finished_internal(void);

/**
 * DESC:
 * Used to exit multi dialog by reporting Exited event.
 *
 * @PARAM: none.
 *
 * @RETURN: DUER_OK if success, negative if failed.
 */
int duer_dcs_sys_exit_internal(void);

/**
 * DESC:
 * Used to notify DCS router that tts play finished.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_speak_directive_finished_internal(void);

/**
 * DESC:
 * Enter the critical areas.
 * We use the same lock to protect the most critical areas, as:
 * 1. it isn't hithly concurrent program
 * 2. the execute time of function in DCS module is short
 * 3. same recource
 *
 * @PARAM: none.
 *
 * @RETURN: DUER_OK if success, negative if failed.
 */
duer_status_t duer_dcs_critical_enter_internal(void);

/**
 * DESC:
 * Exit the critical areas.
 * We use the same lock to protect the most critical areas, as:
 * 1. it isn't hithly concurrent program
 * 2. the execute time of function in DCS module is short
 * 3. same recource
 *
 * @PARAM: none.
 *
 * @RETURN: DUER_OK if success, negative if failed.
 */
duer_status_t duer_dcs_critical_exit_internal(void);

/**
 * DESC:
 * Uninitialize the DCS voice output interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_voice_output_uninitialize_internal(void);

/**
 * DESC:
 * Uninitialize the DCS voice input interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_voice_input_uninitialize_internal(void);

/**
 * DESC:
 * Uninitialize the DCS system interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_sys_interface_uninitialize_internal(void);

/**
 * DESC:
 * Uninitialize the speaker control interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_speaker_control_uninitialize_internal(void);

/**
 * DESC:
 * Uninitialize the DCS screen interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_screen_uninitialize_internal(void);

/**
 * DESC:
 * Initialize the DCS play control interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_play_control_init_internal(void);

/**
 * DESC:
 * Uninitialize the DCS play control interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_play_control_uninitialize_internal(void);

/**
 * DESC:
 * Uninitialize the DCS device control interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_device_control_uninitialize_internal(void);

/**
 * DESC:
 * Uninitialize the DCS infrared control interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_infrared_control_uninitialize_internal(void);

/**
 * DESC:
 * Uninitialize the DCS audio player interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_audio_player_uninitialize_internal(void);

/**
 * DESC:
 * Uninitialize the DCS alert interface.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_alert_uninitialize_internal(void);

/**
 * DESC:
 * Used to stop the micphone when tts coming.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_listen_stop_internal(void);

/**
 * DESC:
 * Uninitialize the DCS local resource.
 *
 * @PARAM: none.
 *
 * @RETURN: none.
 */
void duer_dcs_local_uninitialize_internal(void);

/**
 * DESC:
 * Generate msg id
 * @PARAM: none.
 *
 * @RETURN: the new msg id.
 */
const char *duer_generate_msg_id_internal(void);

/**
 * Add dcs directive.
 *
 * @param directive: the directive info, include directive name, handler, etc.
 * @param count, how many directives will be added.
 * @param name_space: the namespace of the directives needed to add
 * @return 0 if success, negative if failed.
 */
duer_status_t duer_add_dcs_directive_internal(const duer_directive_list *directive,
                                              size_t count,
                                              const char *name_space);

/**
 * Register callback to get client context.
 * Sometimes dcs need to know the device's state.
 * Hence, some callbacks are needed to get client context.
 *
 * @param cb: the callback to get client context.
 * @return 0 if success, negative if failed.
 */
duer_status_t duer_reg_client_context_cb_internal(dcs_client_context_handler cb);

/**
 * DESC:
 * Checking whether local player is paused by higher play channel.
 *
 * @RETURN: Reture DUER_TRUE if local player is paused, else returen DUER_FALSE.
 */
duer_bool duer_local_player_is_paused_internal(void);

/**
 * DESC:
 * Checking whether local player is playing.
 *
 * @RETURN: Reture DUER_TRUE if local player is playing, else returen DUER_FALSE.
 */
duer_bool duer_local_player_is_playing_internal(void);

/**
 * DESC:
 * Pause the loacl player.
 *
 * @RETURN: none.
 */
void duer_pause_local_player_internal(void);

/**
 * DESC:
 * Resume the loacl player.
 *
 * @RETURN: none.
 */
void duer_resume_local_player_internal(void);

/**
 * DESC:
 * Stop the loacl player.
 *
 * @RETURN: none.
 */
void duer_stop_local_player_internal(void);

/**
 * Set dialog id.
 *
 * @param dialog_id: the new dialog id.
 * @return 0 if success, negative if failed.
 */
duer_status_t duer_set_dialog_id_internal(const char *dialog_id);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_DCS_LOCAL_H*/
