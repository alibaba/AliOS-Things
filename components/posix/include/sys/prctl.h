/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef _PRCTL_H
#define _PRCTL_H

#ifdef __cplusplus
extern "C" {
#endif

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
#endif

#endif /* _PRCTL_H */
