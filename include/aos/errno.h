/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_ERRNO_H
#define AOS_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

/**
 * Redefine the errno, Only use in framework/app
 *
 */
#ifdef BUILD_BIN

#undef set_errno
#define set_errno(err) do { errno = (err); } while(0)


#else
#ifdef BUILD_APP
extern int get_errno(void);
extern void set_errno(int err);

#undef errno
#define errno get_errno()

#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* AOS_ERRNO_H */

