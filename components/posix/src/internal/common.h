/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef __POSIX_INTERNAL_COMMON_H
#define __POSIX_INTERNAL_COMMON_H

#include <errno.h>

#define CHECK_AOS_RET(ret) do {if ((ret) < 0) {errno = -(ret); (ret) = -1;}} while (0)

#endif /*__POSIX_INTERNAL_COMMON_H*/