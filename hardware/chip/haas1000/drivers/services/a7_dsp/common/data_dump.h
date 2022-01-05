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
#ifndef _DATA_DUMP_H_
#define _DATA_DUMP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*data_dump_cb_t)(unsigned char *data, unsigned int len, void *param);
typedef void (*data_dump_cb_register_t)(data_dump_cb_t cbf, void *param);
typedef void (*data_dump_cb_unregister_t)(data_dump_cb_t cbf, void *param);

void *data_dump_open(data_dump_cb_register_t register_cbf, int kfifo_len_KB, int samplerate, int auto_start);
void *data_dump_open_withfifo(data_dump_cb_register_t register_cbf, void *val_fifo, int samplerate, int auto_start);
int data_dump_set_timeout(void *handle, int timeout);
int data_dump_start(void *handle);
int data_dump_get_size(void *handle);
int data_dump_write(void *handle, void *buf, unsigned int size);
int data_dump_read(void *handle, void *buf, unsigned int size);
int data_dump_stop(void *handle);
void data_dump_close(void *handle, data_dump_cb_unregister_t unregister_cbf);

#ifdef __cplusplus
}
#endif

#endif /* _DATA_DUMP_H_ */
