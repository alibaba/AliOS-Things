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
 * File: lightduer_2nd_wakeup.h
 * Auth: Sijun Li (lisijun@baidu.com)
 * Desc: apply APIs to support cloud second wakeup.
 */
#ifndef BAIDU_DUER_LIGHTDUER_2ND_WAKEUP_H
#define BAIDU_DUER_LIGHTDUER_2ND_WAKEUP_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DUER_SECOND_WAKEUP_ENABLE
/*
 * Define type of callback of second wakeup directive.
 *
 * @param is_wakeup: Success: > 0
 *                   Failed: <= 0
 *
 * @return void
 */
typedef void (*duer_on_2nd_wakeup_handler)(int is_wakeup);

/*
 * Initialize second wakeup feature.
 *
 * @param handler: callback of second wakeup directive. 
 *
 * @return: DUER_OK if successful, else failed.
 */
duer_status_t duer_2nd_wakeup_init(void);

/*
 * Set callback to get second wakeup result.
 *
 * @param handler, the point of handler defined by user.
 */
void duer_2nd_wakeup_set_handler(duer_on_2nd_wakeup_handler handler);
#endif//DUER_SECOND_WAKEUP_ENABLE

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_2ND_WAKEUP_H*/

