/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_DFTDBG_CONFIG_H
#define DEBUG_DFTDBG_CONFIG_H


#if (DEBUG_CONFIG_PANIC > 0)
/* If the mcu printf depends on isr */
#ifndef DEBUG_CONFIG_PANIC_PRT_INT
#define DEBUG_CONFIG_PANIC_PRT_INT 0
#endif

#endif

#ifndef RHINO_CONFIG_NORMAL_PRT
#define RHINO_CONFIG_NORMAL_PRT 1
#endif

#endif /* DEBUG_DFTDBG_CONFIG_H */
