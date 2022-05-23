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
 * File: lightduer_key.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: Key Event Handle Interface
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_KEY_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_KEY_H

enum KeyType {
    START_KEY    = 1,
    STOP_KEY     = 2,
    END_KEY      = 3,
    NEXT_KEY     = 4,
    PREVIOUS_KEY = 5,
    REPEAT_KEY   = 6,
};

/*
 * Init LightDuer key handler framework
 *
 * @param void: None
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED.
 */
extern int duer_init_key_handler(void);

/*
 * Report key event to event framework
 * Call this API in IRQ context as the last func
 *
 * @param void: enum KeyType key
 *               START_KEY
 *               STOP_KEY
 *               END_KEY
 *               NEXT_KEY
 *               PREVIOUS_KEY
 *               REPEAT_KEY
 *
 *               int value
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED.
 *
 */
extern int duer_report_key_event(enum KeyType key, int value);

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_KEY_H
