/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UFFS_VFS_H
#define UFFS_VFS_H

#include "uffs_config.h"
#include "uffs/uffs_public.h"

#include <vfs_inode.h>
#include <vfs_register.h>

#ifndef CONFIG_UFFS_ECC_MODE
#define CONFIG_UFFS_ECC_MODE UFFS_ECC_NONE
#endif

#if CONFIG_UFFS_ECC_MODE == UFFS_ECC_NONE
#define CONFIG_UFFS_LAYOUT UFFS_LAYOUT_UFFS

#elif CONFIG_UFFS_ECC_MODE == UFFS_ECC_SOFT
#define CONFIG_UFFS_LAYOUT UFFS_LAYOUT_UFFS

#elif CONFIG_UFFS_ECC_MODE == UFFS_ECC_HW_AUTO
#define CONFIG_UFFS_LAYOUT UFFS_LAYOUT_FLASH
#endif

#ifndef CONFIG_UFFS_START_BLOCK
#define CONFIG_UFFS_START_BLOCK 0
#endif

#ifndef CONFIG_UFFS_END_BLOCK
#define CONFIG_UFFS_END_BLOCK 0xffffffff
#endif

#endif

