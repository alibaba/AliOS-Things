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
 * File: lightduer_interactive_class.h
 * Auth: Chen Xihao (chenxihao@baidu.com)
 * Desc: Interactive class APIs.
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_INTERACTIVE_CLASS_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_INTERACTIVE_CLASS_H

#include <stdbool.h>
#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct duer_interactive_class_handler_s {
    duer_status_t (*handle_control_light)(bool on);
    duer_status_t (*handle_notice)(baidu_json *payload);
} duer_interactive_class_handler_t;

/**
 * DESC:
 * Init interactive class.
 *
 * PARAM: none
 *
 * @RETURN: none.
 */
void duer_interactive_class_init(void);

/**
 * DESC:
 * Open interactive class.
 *
 * PARAM: none
 *
 * @RETURN: success return DUER_OK, failed return DUER_ERR_FAILED.
 */
duer_status_t duer_interactive_class_open(void);

/**
 * DESC:
 * Close interactive class.
 *
 * PARAM: none
 *
 * @RETURN: success return DUER_OK, failed return DUER_ERR_FAILED.
 */
duer_status_t duer_interactive_class_close(void);

/**
 * DESC:
 * Whether interactive class is living.
 *
 * PARAM: none
 *
 * @RETURN: living return true, not return false.
 */
bool duer_interactive_class_is_living(void);

/**
 * DESC:
 * Set the handler to handle callbacks, such as control light, handle notice
 *
 * PARAM: handler, the point of handler defined by user
 *
 * @RETURN: none.
 */
void duer_interactive_class_set_handler(duer_interactive_class_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_INTERACTIVE_CLASS_H */

