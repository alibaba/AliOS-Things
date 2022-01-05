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
#ifndef A7_CMD_H
#define A7_CMD_H

#ifdef __cplusplus
extern "C" {
#endif
#define A7_CMD_MAX_LEN  120
typedef enum {
    AUD_DUMP_SRC_BEFORE_ALG,
    AUD_DUMP_SRC_INTER_ALG,
    AUD_DUMP_SRC_AFTER_ALG,

    AUD_DUMP_SRC_NUM,
}AUD_DUMP_SRC_T;

typedef enum {
    A7_CMD_TYPE_DEBUG_MM,
    A7_CMD_TYPE_AUDIO_DUMP,
    A7_CMD_TYPE_UART2_TRACE,
    A7_CMD_TYPE_STATPRINT,
    A7_CMD_TYPE_PANIC,
    A7_CMD_TYPE_HEAP_INIT,
    A7_CMD_TYPE_HANDSHAKE,
    A7_CMD_TYPE_DUMP_CFG,
    A7_CMD_TYPE_AISPEECH,
    A7_CMD_TYPE_USER,
    A7_CMD_TYPE_NUM,
}A7_CMD_TYPE_T;

typedef struct {
    A7_CMD_TYPE_T type;
    unsigned int p1;
    unsigned int p2;
}A7_CMD_T;

typedef void (* a7_cmd_callback_t)(char *data, int len);

void a7_cmd_init(void);
void a7_cmd_send(void *data, int len);
void mcu_cmd_send(A7_CMD_T *cmd);
void a7_cmd_send2(A7_CMD_T *cmd);
void a7_cmd_recv_init(void);
void set_a7_cmd_callback_handler (a7_cmd_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif
