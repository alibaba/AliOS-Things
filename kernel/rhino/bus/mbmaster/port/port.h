/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _PORT_H
#define _PORT_H

#include <k_api.h>
#include <assert.h>
#include <mbmaster_api.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MB_PORT_HAS_CLOSE   1     /* can close the port Transmission Channel? */

#define MB_CRITICAL_ALLOC() CPSR_ALLOC()
#define MB_CRITICAL_ENTER() RHINO_CRITICAL_ENTER()
#define MB_CRITICAL_EXIT()  RHINO_CRITICAL_EXIT()

#ifndef htobe
#define htobe(X) (((X >> 8) & 0x00ff) | ((X<<8) & 0xff00))
#endif

#ifdef __cplusplus
}
#endif
#endif
