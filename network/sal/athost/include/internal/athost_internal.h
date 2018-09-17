/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ATHOST_INTERNAL_H_
#define _ATHOST_INTERNAL_H_

/* Structure for registering at CLI commands */
typedef struct atcmd_handler
{
    const char *name;
    const char *help;

    int (*function)(void);
} atcmd_hdl_t, *atcmd_hdl_ptr_t;

typedef struct atcmd_op_s
{
    const char *name;
    const char *prefix;

    int (*init)(void);
    void (*deinit)(void);

    atcmd_hdl_ptr_t (*get_atcmd_handler)(void);
} atcmd_op_t, *atcmd_op_ptr_t;

int atcmd_register_handlers(atcmd_op_ptr_t handler);

#endif
