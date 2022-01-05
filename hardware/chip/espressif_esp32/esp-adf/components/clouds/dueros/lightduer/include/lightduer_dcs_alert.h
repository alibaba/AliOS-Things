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
 * File: lightduer_dcs_alert.h
 * Auth: Gang Chen (chengang12@baidu.com)
 * Desc: Light duer alert APIs.
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DCS_ALERT_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DCS_ALERT_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SET_ALERT_SUCCESS,
    SET_ALERT_FAIL,
    DELETE_ALERT_SUCCESS,
    DELETE_ALERT_FAIL,
    ALERT_START,
    ALERT_STOP,
} duer_dcs_alert_event_type;

typedef struct {
    char *type;    // "ALARM" or "TIMER"
    char *time;    // the time of the alert, ISO 8601 format.
    char *token;   // the alert token, it is the unique identification of an alert
} duer_dcs_alert_info_type;

/**
 * DESC:
 * Init alert.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_dcs_alert_init(void);

/**
 * DESC:
 * Report alert event
 *
 * PARAM:
 * @param[in] token: the token of the alert.
 * @param[in] type: event type, refer to duer_alert_event_type.
 *
 * @return: 0 when success, negative value when failed.
 */
int duer_dcs_report_alert_event(const char *token, duer_dcs_alert_event_type type);

/**
 * DESC:
 * Developer needs to implement this interface to set alert.
 *
 * PARAM:
 * @param[in] directive: the set alert directive.
 *
 * @RETURN: DUER_OK if success,
 *          DUER_MSG_RSP_BAD_REQUEST if the directive is invalid,
 *          DUER_ERR_FAILED if other error happened.
 */
duer_status_t duer_dcs_tone_alert_set_handler(const baidu_json *directive);

/**
 * DESC:
 * Developer needs to implement this interface to delete alert.
 *
 * PARAM:
 * @param[in] token: the token of the alert need to delete.
 *
 * @RETURN: none.
 */
void duer_dcs_alert_delete_handler(const char *token);

/**
 * DESC:
 * Add a alert info to the alert list, it should be used in duer_dcs_get_all_alert callback.
 * This API could be used in duer_dcs_get_all_alert, so DCS can get all alerts info.
 *
 * PARAM:
 * @param[out] alert_list: used to store the alert info.
 * @param[in] alert_info: the info about the alert, such as token, schedule time.
 * @param[in] is_active: whether this alert is sounding.
 *
 * @return: 0 when success, negative value when failed.
 */
int duer_insert_alert_list(baidu_json *alert_list,
                            duer_dcs_alert_info_type *alert_info,
                            duer_bool is_active);

/**
 * DESC:
 * It's used to get all alert info by DCS.
 * Developer can implement this interface by call duer_dcs_report_alert to report all alert info.
 *
 * PARAM:
 * @param[out] alert_list: a json array used to store all alert info.
 *
 * @RETURN: none.
 */
void duer_dcs_get_all_alert(baidu_json *alert_list);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_DCS_ALERT_H*/

