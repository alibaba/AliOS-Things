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
 * File: lightduer_dcs_infrared_control.h
 * Auth: Chang Li (changli@baidu.com)
 * Desc: Light duer DCS APIS for infrared control.
 */

#ifndef BAIDU_DUER_LIGHTDUER_DCS_INFRARED_CONTROL_H
#define BAIDU_DUER_LIGHTDUER_DCS_INFRARED_CONTROL_H

#include "lightduer_types.h"
#include <stdint.h>

typedef enum {
    DCS_INFRARED_SEND_COMPLETED,
    DCS_INFRARED_SEND_FAILED,
    DCS_INFRARED_RECEIVED,
    DCS_INFRARED_RECEIVE_TIMEOUT
} duer_infrared_event_t;

typedef struct _ray_item_t {
    struct _ray_item_t *next;
    uint32_t carrier_freq;
    uint32_t interval_ms;
    uint16_t pattern_cnt;
    uint16_t pattern[];
} duer_ray_item_t;

typedef struct {
    duer_ray_item_t *head;
} duer_ray_item_list_t;

/**
 * DESC:
 * Destroy the ray item list and reclaim memory.
 *
 * PARAM[in] list: Infrared ray data.
 *
 * @RETURN: None
 */
void duer_destroy_ray_items(duer_ray_item_list_t *list);

/**
 * DESC:
 * Developer needs to implement this interface to send infrared data.
 * Note, it's asynchronized.
 *
 * PARAM[in] list: Infrared ray data. Note: need to be reclaimed after use.
 *
 * @RETURN: None
 */
void duer_dcs_infrared_send_handler(duer_ray_item_list_t *list);

/**
 * DESC:
 * Developer needs to implement this interface to receive infrared data.
 *
 * PARAM[in] timeout_ms: the time device wait for the infrared data.
 *
 * @RETURN: None
 */
void duer_dcs_infrared_receive_handler(long timeout_ms);

/**
 * DESC:
 * report infrared event to cloud.
 *
 * PARAM[in] event: infrared event.
 *
 * PARAM[in] array: pattern array for DCS_INFRARED_RECEIVED event.
 *
 * PARAM[in] cnt: pattern count for DCS_INFRARED_RECEIVED event.
 *
 * @RETURN: DUER_OK for sucess, others for failure.
 */
int duer_dcs_infrared_control_report(duer_infrared_event_t event, uint16_t *array, int cnt);

/**
 * DESC:
 * Initialize infrared control interface.
 *
 * PARAM: none
 *
 * @RETURN: none
 */
void duer_dcs_infrared_control_init(void);

#endif // BAIDU_DUER_LIGHTDUER_DCS_INFRARED_CONTROL_H
