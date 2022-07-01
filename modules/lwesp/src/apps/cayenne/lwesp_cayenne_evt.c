/**
 * \file            lwesp_cayenne_evt.c
 * \brief           Event helper functions
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include "lwesp/apps/lwesp_cayenne_evt.h"

/**
 * \brief           Get event type
 * \param[in]       evt: Cayenne event handle
 * \return          Event type
 */
lwesp_cayenne_evt_type_t
lwesp_cayenne_evt_get_type(lwesp_cayenne_evt_t* evt) {
    return evt->type;
}

/**
 * \brief           Get event data message
 * \param[in]       evt: Cayenne event handle
 * \return          Message pointer
 */
lwesp_cayenne_msg_t*
lwesp_cayenne_evt_data_get_msg(lwesp_cayenne_evt_t* evt) {
    return evt->evt.data.msg;
}
