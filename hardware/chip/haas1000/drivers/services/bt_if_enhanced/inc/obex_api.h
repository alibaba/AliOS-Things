/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __OBEX_API_H__
#define __OBEX_API_H__

#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BTIF_OBEX_SESSION_ROLE_CLIENT = 0,
    BTIF_OBEX_SESSION_ROLE_SERVER, // 1
} btif_obex_session_role_t;

#ifdef __cplusplus
}
#endif

#endif /* __OBEX_API_H__ */

