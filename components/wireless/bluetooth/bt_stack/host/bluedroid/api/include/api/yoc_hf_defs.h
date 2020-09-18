// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __YOC_HF_DEFS_H__
#define __YOC_HF_DEFS_H__

#include "yoc_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


/// Bluetooth HFP audio volume control target
typedef enum {
    YOC_HF_VOLUME_CONTROL_TARGET_SPK = 0,             /*!< speaker */
    YOC_HF_VOLUME_CONTROL_TARGET_MIC,                 /*!< microphone */
} yoc_hf_volume_control_target_t;

/// +CIND roaming status indicator values
typedef enum {
    YOC_HF_ROAMING_STATUS_INACTIVE = 0,               /*!< roaming is not active */
    YOC_HF_ROAMING_STATUS_ACTIVE,                     /*!< a roaming is active */
} yoc_hf_roaming_status_t;

/// +CIND call status indicator values
typedef enum {
    YOC_HF_CALL_STATUS_NO_CALLS = 0,                  /*!< no call in progress  */
    YOC_HF_CALL_STATUS_CALL_IN_PROGRESS = 1,          /*!< call is present(active or held) */
} yoc_hf_call_status_t;

/// +CIND call setup status indicator values
typedef enum {
    YOC_HF_CALL_SETUP_STATUS_NONE = 0,                /*!< no call setup in progress */
    YOC_HF_CALL_SETUP_STATUS_INCOMING = 1,            /*!< incoming call setup in progress */
    YOC_HF_CALL_SETUP_STATUS_OUTGOING_DIALING = 2,    /*!< outgoing call setup in dialing state */
    YOC_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING = 3,   /*!< outgoing call setup in alerting state */
} yoc_hf_call_setup_status_t;

/// +CIND call held indicator values
typedef enum {
    YOC_HF_CALL_HELD_STATUS_NONE = 0,                 /*!< no calls held */
    YOC_HF_CALL_HELD_STATUS_HELD_AND_ACTIVE = 1,      /*!< both active and held call */
    YOC_HF_CALL_HELD_STATUS_HELD = 2,                 /*!< call on hold, no active call*/
} yoc_hf_call_held_status_t;

/// +CIND network service availability status
typedef enum {
    YOC_HF_SERVICE_AVAILABILITY_STATUS_UNAVAILABLE = 0, /*!< service not available */
    YOC_HF_SERVICE_AVAILABILITY_STATUS_AVAILABLE,       /*!< service available */
} yoc_hf_service_availability_status_t;

/// +CLCC status of the call
typedef enum {
    YOC_HF_CURRENT_CALL_STATUS_ACTIVE = 0,            /*!< active */
    YOC_HF_CURRENT_CALL_STATUS_HELD = 1,              /*!< held */
    YOC_HF_CURRENT_CALL_STATUS_DIALING = 2,           /*!< dialing (outgoing calls only) */
    YOC_HF_CURRENT_CALL_STATUS_ALERTING = 3,          /*!< alerting (outgoing calls only) */
    YOC_HF_CURRENT_CALL_STATUS_INCOMING = 4,          /*!< incoming (incoming calls only) */
    YOC_HF_CURRENT_CALL_STATUS_WAITING = 5,           /*!< waiting (incoming calls only) */
    YOC_HF_CURRENT_CALL_STATUS_HELD_BY_RYOC_HOLD = 6, /*!< call held by response and hold */
} yoc_hf_current_call_status_t;

/// +CLCC direction of the call
typedef enum {
    YOC_HF_CURRENT_CALL_DIRECTION_OUTGOING = 0,       /*!< outgoing */
    YOC_HF_CURRENT_CALL_DIRECTION_INCOMING = 1,       /*!< incoming */
} yoc_hf_current_call_direction_t;

/// +CLCC multi-party call flag
typedef enum {
    YOC_HF_CURRENT_CALL_MPTY_TYPE_SINGLE = 0,         /*!< not a member of a multi-party call */
    YOC_HF_CURRENT_CALL_MPTY_TYPE_MULTI = 1,          /*!< member of a multi-party call */
} yoc_hf_current_call_mpty_type_t;

/// +CLCC call mode
typedef enum {
    YOC_HF_CURRENT_CALL_MODE_VOICE = 0,
    YOC_HF_CURRENT_CALL_MODE_DATA = 1,
    YOC_HF_CURRENT_CALL_MODE_FAX = 2,
} yoc_hf_current_call_mode_t;

/// +CLCC address type
typedef enum {
    YOC_HF_CALL_ADDR_TYPE_UNKNOWN = 0x81,            /*!< unkown address type */
    YOC_HF_CALL_ADDR_TYPE_INTERNATIONAL = 0x91,      /*!< international address */
} yoc_hf_call_addr_type_t;

/// +CNUM service type of the phone number
typedef enum {
    YOC_HF_SUBSCRIBER_SERVICE_TYPE_UNKNOWN = 0,      /*!< unknown */
    YOC_HF_SUBSCRIBER_SERVICE_TYPE_VOICE,            /*!< voice service */
    YOC_HF_SUBSCRIBER_SERVICE_TYPE_FAX,              /*!< fax service */
} yoc_hf_subscriber_service_type_t;

/// +BTRH response and hold result code
typedef enum {
    YOC_HF_BTRH_STATUS_HELD = 0,       /*!< incoming call is put on held in AG */
    YOC_HF_BTRH_STATUS_ACCEPTED,       /*!< held incoming call is accepted in AG */
    YOC_HF_BTRH_STATUS_REJECTED,       /*!< held incoming call is rejected in AG */
} yoc_hf_btrh_status_t;

/// AT+BTRH response and hold action code
typedef enum {
    YOC_HF_BTRH_CMD_HOLD = 0,          /*!< put the incoming call on hold */
    YOC_HF_BTRH_CMD_ACCEPT = 1,        /*!< accept a held incoming call */
    YOC_HF_BTRH_CMD_REJECT = 2,        /*!< reject a held incoming call */
} yoc_hf_btrh_cmd_t;

/// response indication codes for AT commands
typedef enum {
    YOC_HF_AT_RESPONSE_CODE_OK = 0,         /*!< acknowledges execution of a command line */
    YOC_HF_AT_RESPONSE_CODE_ERR,            /*!< command not accepted */
    YOC_HF_AT_RESPONSE_CODE_NO_CARRIER,     /*!< connection terminated */
    YOC_HF_AT_RESPONSE_CODE_BUSY,           /*!< busy signal detected */
    YOC_HF_AT_RESPONSE_CODE_NO_ANSWER,      /*!< connection completion timeout */
    YOC_HF_AT_RESPONSE_CODE_DELAYED,        /*!< delayed */
    YOC_HF_AT_RESPONSE_CODE_BLACKLISTED,    /*!< blacklisted */
    YOC_HF_AT_RESPONSE_CODE_CME,            /*!< CME error */
} yoc_hf_at_response_code_t;

/// voice recognition state
typedef enum {
    YOC_HF_VR_STATE_DISABLED = 0,           /*!< voice recognition disabled */
    YOC_HF_VR_STATE_ENABLED,                /*!< voice recognition enabled */
} yoc_hf_vr_state_t;

/// AT+CHLD command values
typedef enum {
    YOC_HF_CHLD_TYPE_REL = 0,               /*!< <0>, Terminate all held or set UDUB("busy") to a waiting call */
    YOC_HF_CHLD_TYPE_REL_ACC,               /*!< <1>, Terminate all active calls and accepts a waiting/held call */
    YOC_HF_CHLD_TYPE_HOLD_ACC,              /*!< <2>, Hold all active calls and accepts a waiting/held call */
    YOC_HF_CHLD_TYPE_MERGE,                 /*!< <3>, Add all held calls to a conference */
    YOC_HF_CHLD_TYPE_MERGE_DETACH,          /*!< <4>, connect the two calls and disconnects the subscriber from both calls */
    YOC_HF_CHLD_TYPE_REL_X,                 /*!< <1x>, releases specified calls only */
    YOC_HF_CHLD_TYPE_PRIV_X,                /*!< <2x>, request private consultation mode with specified call */
} yoc_hf_chld_type_t;

/// Extended Audio Gateway Error Result Code Response
typedef enum {
    YOC_HF_CME_AG_FAILURE = 0,                    /*!< ag failure */
    YOC_HF_CME_NO_CONNECTION_TO_PHONE = 1,        /*!< no connection to phone */
    YOC_HF_CME_OPERATION_NOT_ALLOWED = 3,         /*!< operation not allowed */
    YOC_HF_CME_OPERATION_NOT_SUPPORTED = 4,       /*!< operation not supported */
    YOC_HF_CME_PH_SIM_PIN_REQUIRED = 5,           /*!< PH-SIM PIN Required */
    YOC_HF_CME_SIM_NOT_INSERTED = 10,             /*!< SIM not inserted */
    YOC_HF_CME_SIM_PIN_REQUIRED = 11,             /*!< SIM PIN required */
    YOC_HF_CME_SIM_PUK_REQUIRED = 12,             /*!< SIM PUK required */
    YOC_HF_CME_SIM_FAILURE = 13,                  /*!< SIM failure */
    YOC_HF_CME_SIM_BUSY = 14,                     /*!< SIM busy */
    YOC_HF_CME_INCORRECT_PASSWORD = 16,           /*!< incorrect password */
    YOC_HF_CME_SIM_PIN2_REQUIRED = 17,            /*!< SIM PIN2 required */
    YOC_HF_CME_SIM_PUK2_REQUIRED = 18,            /*!< SIM PUK2 required */
    YOC_HF_CME_MEMEORY_FULL = 20,                 /*!< memory full */
    YOC_HF_CME_INVALID_INDEX = 21,                /*!< invalid index */
    YOC_HF_CME_MEMEORY_FAILURE = 23,              /*!< memory failure */
    YOC_HF_CME_TEXT_STRING_TOO_LONG = 24,         /*!< test string too long */
    YOC_HF_CME_INVALID_CHARACTERS_IN_TEXT_STRING = 25,  /*!< invalid characters in text string */
    YOC_HF_CME_DIAL_STRING_TOO_LONG = 26,         /*!< dial string too long*/
    YOC_HF_CME_INVALID_CHARACTERS_IN_DIAL_STRING = 27,  /*!< invalid characters in dial string */
    YOC_HF_CME_NO_NETWORK_SERVICE = 30,           /*!< no network service */
    YOC_HF_CME_NETWORK_TIMEOUT = 31,              /*!< network timeout */
    YOC_HF_CME_NETWORK_NOT_ALLOWED = 32,          /*!< network not allowed --emergency calls only */
} yoc_hf_cme_err_t;

#ifdef __cplusplus
}
#endif

#endif /* __YOC_HF_DEFS_H__ */
