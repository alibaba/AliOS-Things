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
#ifndef COMMUNICATION_CMD_HANDLE_H
#define COMMUNICATION_CMD_HANDLE_H

#include "tool_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COMMUNICATION_CMD_BUFF_TO_U32(ptr,val) do{ \
                                val =( ((uint32_t) *((uint8_t*)ptr+3) << 24)   | \
                                      ((uint32_t) *((uint8_t*)ptr+2) << 16) | \
                                      ((uint32_t) *((uint8_t*)ptr+1) << 8)  | \
                                      ((uint32_t) *((uint8_t*)ptr)) ); \
                         }while(0)

#define COMMUNICATION_CMD_U32_TO_BUFF(ptr,val) do{ \
                            *(ptr+3) = (uint8_t) (val>>24); \
                            *(ptr+2) = (uint8_t) (val>>16); \
                            *(ptr+1) = (uint8_t) (val>>8); \
                            *(ptr+0) = (uint8_t) val; \
                         }while(0)

int communication_cmd_init(int (* cb)(const unsigned char *, unsigned int));

int communication_cmd_send_reply(const unsigned char *payload, unsigned int len);

enum ERR_CODE communication_cmd_check_msg_hdr(struct message_t *msg);

enum ERR_CODE communication_cmd_handle_cmd(enum COMMUNICATION_CMD_TYPE cmd, unsigned char *param, unsigned int len);


#ifdef __cplusplus
}
#endif
#endif
