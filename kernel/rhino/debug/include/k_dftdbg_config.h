/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_DFTDBG_CONFIG_H
#define K_DFTDBG_CONFIG_H

#ifndef RHINO_CONFIG_BACKTRACE
#define RHINO_CONFIG_BACKTRACE 1
#endif

/* If AliOS task over the Exception/Fatal Error */
#ifndef RHINO_CONFIG_PANIC
#define RHINO_CONFIG_PANIC 1
#endif

#if (RHINO_CONFIG_PANIC > 0)
/* If the mcu printf depends on isr */
#ifndef RHINO_CONFIG_PANIC_PRT_INT
#define RHINO_CONFIG_PANIC_PRT_INT 1
#endif

#ifndef RHINO_CONFIG_PANIC_OVERVIEW
#define RHINO_CONFIG_PANIC_OVERVIEW 1
#endif
#endif

#ifndef RHINO_CONFIG_NORMAL_PRT
#define RHINO_CONFIG_NORMAL_PRT 1
#endif


#endif /* K_DFTDBG_CONFIG_H */
