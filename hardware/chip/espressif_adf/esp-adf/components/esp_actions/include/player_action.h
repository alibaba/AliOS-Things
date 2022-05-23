/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _PLAYER_ACTION_H__
#define _PLAYER_ACTION_H__

#include "esp_action_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * brief      Player provides service of playing music
 *
 * @param instance          The player instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t player_action_play(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * brief      Player provides service of pausing music playing
 *
 * @param instance          The player instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t player_action_pause(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * brief      Player provides service of playing the next audio file
 *
 * @param instance          The player instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t player_action_next(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * brief      Player provides service of playing the previous audio file
 *
 * @param instance          The player instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t player_action_prev(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * brief      Player provides service of increasing volume
 *
 * @param instance          The player instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t player_action_vol_up(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * brief      Player provides service of decreasing volume
 *
 * @param instance          The player instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t player_action_vol_down(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * brief      Player provides service of mute on
 *
 * @param instance          The player instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t player_action_mute_on(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * brief      Player provides service of mute off
 *
 * @param instance          The player instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t player_action_mute_off(void *instance, action_arg_t *arg, action_result_t *result);

#ifdef __cplusplus
}
#endif

#endif
