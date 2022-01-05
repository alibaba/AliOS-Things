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
/*
 * Auth: Gang Chen(chengang12@baidu.com)
 * Desc: dcs module log report
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_DCS_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_DCS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lightduer_types.h"
#include "baidu_json.h"

typedef enum {
    DUER_DS_LOG_DCS_DIRECTIVE_DROPPED     =  0x201,
    DUER_DS_LOG_DCS_OLD_DIRECTIVE_DROPPED =  0x202,

    DUER_DS_LOG_DCS_MEMORY_ERROR          =  0x301,
    DUER_DS_LOG_DCS_HANDLER_UNREALIZED    =  0x302,
    DUER_DS_LOG_DCS_REPORT_EVENT_FAILED   =  0x303,
    DUER_DS_LOG_DCS_PARAM_ERROR           =  0x304,
    DUER_DS_LOG_DCS_STOP_LISTEN_MISSING   =  0x305,

    DUER_DS_LOG_DCS_ADD_DIRECTIVE_FAILED  =  0x401,
} duer_ds_log_dcs_code_t;

/**
 * Report ds log for dcs module
 */
duer_status_t duer_ds_log_dcs(duer_ds_log_dcs_code_t log_code, const baidu_json *log_message);
/**
 * Report ds log with direction information:
 *        {
 *            "file" : "dcs.c",
 *            "line" : 100
 *        }
 */
duer_status_t duer_ds_log_dcs_report_with_dir(duer_ds_log_dcs_code_t log_code,
                                                   const char *file,
                                                   int line);
/**
 * Report ds log when directive is dropped:
 *        {
 *            "current_dialog_id" :   "aaaaaaabbbbbbbbb",      // current dialogId
 *            "directive_dialog_id" : "bbbbbbbbbaaaaaaa",      // dialogId of this directive
 *            "directive" : "play"                             // directive name
 *        }
 */
duer_status_t duer_ds_log_dcs_directive_drop(const char *current_id,
                                             const char *directive_id,
                                             const char *name);
/**
 * Report ds log if the handler is not realized:
 *        {
 *            "handler_name" : "duer_dcs_speak_handler"
 *        }
 */
duer_status_t duer_ds_log_dcs_handler_unrealize(const char *func);
/**
 * Report ds log if add directive failed:
 *        {
 *            "directive" : "play"
 *        }
 */
duer_status_t duer_ds_log_dcs_add_directive_fail(const char *name);
/**
 * Report ds log if event report failed:
 *        {
 *            "event_name" : "SetAlertFailed"
 *        }
 */
duer_status_t duer_ds_log_dcs_event_report_fail(const char *name);

#define DUER_DS_LOG_REPORT_DCS_MEMORY_ERROR() \
        duer_ds_log_dcs_report_with_dir(DUER_DS_LOG_DCS_MEMORY_ERROR, __FILE__, __LINE__)
#define DUER_DS_LOG_REPORT_DCS_PARAM_ERROR() \
        duer_ds_log_dcs_report_with_dir(DUER_DS_LOG_DCS_PARAM_ERROR, __FILE__, __LINE__)
#define DUER_DS_LOG_REPORT_DCS_HANDLER_UNREALIZE() duer_ds_log_dcs_handler_unrealize(__func__)
#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_DCS_H
