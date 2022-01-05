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

#ifndef _INPUT_KEY_COM_USER_ID_H_
#define _INPUT_KEY_COM_USER_ID_H_

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief input key user user-defined id
 */
typedef enum {
    INPUT_KEY_USER_ID_UNKNOWN                   = -1,   /*!< unknown user id */
    INPUT_KEY_USER_ID_REC                       = 0x01, /*!< user id for recording */
    INPUT_KEY_USER_ID_SET                       = 0x02, /*!< user id for settings */
    INPUT_KEY_USER_ID_PLAY                      = 0x03, /*!< user id for playing */
    INPUT_KEY_USER_ID_MODE                      = 0x04, /*!< user id for mode */
    INPUT_KEY_USER_ID_VOLDOWN                   = 0x05, /*!< user id for volume down */
    INPUT_KEY_USER_ID_VOLUP                     = 0x06, /*!< user id for volume up */
    INPUT_KEY_USER_ID_MUTE                      = 0x07, /*!< user id for mute */
    INPUT_KEY_USER_ID_CAPTURE                   = 0x08, /*!< user id for capture photo */
    INPUT_KEY_USER_ID_MSG                       = 0x09, /*!< user id for message */
    INPUT_KEY_USER_ID_BATTERY_CHARGING          = 0x0A, /*!< user id for battery charging */
    INPUT_KEY_USER_ID_WAKEUP                    = 0x0B, /*!< user id for GPIO wakeup */

    INPUT_KEY_USER_ID_MAX                       = 0x101,
} input_KEY_user_id_t;

#ifdef __cplusplus
}
#endif

#endif
