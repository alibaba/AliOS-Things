/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef __INC_PRCTL_H__
#define __INC_PRCTL_H__

#include <k_api.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * PR_SET_NAME:
 *
 * Set the name of the calling thread, using the value in the
 * location pointed to by (char *) arg2.
 */

#define PR_SET_NAME 0x00000000

int prctl(int option, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INC_PRCTL_H__ */

