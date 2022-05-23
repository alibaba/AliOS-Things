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
 * File: lightduer_dcs_router.h
 * Auth: Gang Chen (chengang12@baidu.com)
 * Desc: Light duer DCS APIS.
 */
#ifndef BAIDU_DUER_LIGHTDUER_DCS_ROUTER_H
#define BAIDU_DUER_LIGHTDUER_DCS_ROUTER_H

#include <stdlib.h>
#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DCS directive handler. The handler will be implemented by developer.
 *
 * @PARAM[in] directive: the directive info, include directive name, handler, etc.
 *
 * @return: return code must be one of following values:
 *      // no error.
 *      DUER_OK                         = 0,
 *      // directive format incorrect, such missing token in Play directive.
 *      DUER_MSG_RSP_BAD_REQUEST        = 128,
 *      // device doesn't support this directive,
 *      DUER_MSG_RSP_NOT_FOUND          = 132,  // 4.04
 *      // all other errors.
 *      DUER_ERR_FAILED                 = -0x0001,
 *      // directive parameter value invalid.
 *      DUER_ERR_INVALID_PARAMETER      = -0x0010,
 *      // memory overflow.
 *      DUER_ERR_MEMORY_OVERLOW         = -0x0011,
 */
typedef duer_status_t (*dcs_directive_handler)(const baidu_json *directive);
typedef baidu_json* (*dcs_client_context_handler)();

typedef struct {
    const char *directive_name;
    dcs_directive_handler cb;
} duer_directive_list;

/**
 * Add dcs directive.
 *
 * @param directive: the directive info, include directive name, handler, etc.
 * @param count, how many directives will be added.
 * @param name_space: the namespace of the directives needed to add
 * @return 0 if success, negative if failed.
 */
duer_status_t duer_add_dcs_directive(const duer_directive_list *directive,
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
duer_status_t duer_reg_client_context_cb(dcs_client_context_handler cb);

/**
 * DESC:
 * Used to create dcs event.
 *
 * @PARAM[in] namespace: the namespace of the event need to report.
 * @PARAM[in] name: the name the event need to report.
 * @PARAM[in] need_msg_id: including messageId item or not.
 * @RETURN: pinter of the created dcs event if success, or NULL if failed.
 */
baidu_json *duer_create_dcs_event(const char *name_space, const char *name, duer_bool need_msg_id);

/**
 * DESC:
 * Used to cancel a dcs dialog: the response of a dialogue will be ignored
 * if user calling this API before receiving the response.
 *
 * @PARAM[in] none.
 * @RETURN:   none.
 */
void duer_dcs_dialog_cancel(void);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_DCS_ROUTER_H*/
