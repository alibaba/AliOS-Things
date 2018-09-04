/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ATHOST_IO_H_
#define _ATHOST_IO_H_

typedef int (*athost_atcmd_handle_cb_t)();

typedef struct athost_io_s
{

    int (*at_read)(char *outbuf, uint32_t len);

    int (*at_write)(const char *header, const uint8_t *data, uint32_t len,
                    const char *tailer);

    int (*at_handle_register_cb)(const char              *prefix,
                                 athost_atcmd_handle_cb_t fn);

} athost_io_t;

int athost_io_register(athost_io_t *module);

int athost_at_read(char *outbuf, uint32_t len);

int athost_at_write(const char *header, const uint8_t *data, uint32_t len,
                    const char *tailer);

int athost_handle_register_cb(const char *prefix, athost_atcmd_handle_cb_t fn);

#endif
