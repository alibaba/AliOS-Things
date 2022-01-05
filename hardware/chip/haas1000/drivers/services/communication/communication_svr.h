/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __COMMUNICATION_SVR_H__
#define __COMMUNICATION_SVR_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*communication_receive_func_typedef)(uint8_t *buf, uint8_t len);

void communication_init(void);
int communication_receive_register_callback(communication_receive_func_typedef p);
int communication_send_buf(uint8_t * buf, uint8_t len);


#ifdef KNOWLES_UART_DATA
void uart_audio_init();
void uart_audio_deinit();
#endif


#ifdef __cplusplus
}
#endif

#endif

