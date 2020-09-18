/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
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
