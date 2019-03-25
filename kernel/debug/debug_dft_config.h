/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_DFTDBG_CONFIG_H
#define DEBUG_DFTDBG_CONFIG_H

/* If the mcu printf depends on isr */
#ifndef DEBUG_CONFIG_PANIC_PRT_INT
#define DEBUG_CONFIG_PANIC_PRT_INT      1
#endif

#ifndef DEBUG_CONFIG_ERRDUMP
#define DEBUG_CONFIG_ERRDUMP            1
#endif

#endif /* DEBUG_DFTDBG_CONFIG_H */
