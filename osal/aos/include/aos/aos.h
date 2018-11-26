/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_AOS_H
#define AOS_AOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <aos/types.h>

#ifdef CONFIG_AOS_CLI
#include <aos/cli.h>
#endif

#include <aos/debug.h>
#include <aos/kernel.h>

#ifdef AOS_KV
#include <aos/kv.h>
#endif

#include <aos/list.h>
#include <aos/log.h>

#ifdef AOS_VFS
#include <vfs.h>
#endif

#include <aos/version.h>

#ifdef AOS_LOOP
#include <aos/yloop.h>
#endif

#include <aos/errno.h>
#include <aos/init.h>

#ifdef __cplusplus
}
#endif

#endif /* AOS_AOS_H */

