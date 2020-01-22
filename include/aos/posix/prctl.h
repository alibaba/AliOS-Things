/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef PRCTL_H
#define PRCTL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "k_api.h"
#include "aos/posix/posix_config.h"

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

#endif /* PRCTL_H */

