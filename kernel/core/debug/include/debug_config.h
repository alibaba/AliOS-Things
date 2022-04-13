/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

/*  If the mcu printf depends on isr */
#ifndef DEBUG_CONFIG_PANIC_PRT_INT
    #define DEBUG_CONFIG_PANIC_PRT_INT       1
#endif

#ifndef DEBUG_CONFIG_PANIC_CLI
    #define DEBUG_CONFIG_PANIC_CLI           1
#endif

#ifndef DEBUG_CONFIG_AOS_TRACE
    #define DEBUG_CONFIG_AOS_TRACE           0
#endif

#ifndef DEBUG_CONFIG_LAST_WORD
    #define DEBUG_CONFIG_LAST_WORD           0
#endif

#ifndef DEBUG_CONFIG_LOG_FILE_NAME
    #define DEBUG_CONFIG_LOG_DIR_NAME       "/data/crash_reports"
    #define DEBUG_CONFIG_LOG_FILE_NAME      "/data/crash_reports/crash_report"
    #define DEBUG_CONFIG_LOG_FILE_INDEX     "/data/crash_reports/log_file_index"
#endif

#ifndef DEBUG_CONFIG_LOG_FILE_NUM
    #define DEBUG_CONFIG_LOG_FILE_NUM        10
#endif

#ifndef DEBUG_CONFIG_LASTWORD_RAM
    #define DEBUG_CONFIG_LASTWORD_RAM        1
#endif

#ifndef DEBUG_CONFIG_LASTWORD_RAM_ADDR
    #define DEBUG_CONFIG_LASTWORD_RAM_ADDR   0
#endif

#ifndef DEBUG_CONFIG_LASTWORD_REGION_LEN
    #define DEBUG_CONFIG_LASTWORD_REGION_LEN 0
#endif

#if ((DEBUG_CONFIG_LAST_WORD >= 1) && (DEBUG_CONFIG_LASTWORD_RAM >= 1) && (DEBUG_CONFIG_LASTWORD_RAM_ADDR == 0))
    #error "Should config DEBUG_CONFIG_LASTWORD_RAM_ADDR which used to store exception log"
#endif

#if ((DEBUG_CONFIG_LAST_WORD >= 1) && (DEBUG_CONFIG_LASTWORD_RAM >= 1) && (DEBUG_CONFIG_LASTWORD_REGION_LEN == 0))
    #error "Should config DEBUG_CONFIG_LASTWORD_REGION_LEN"
#endif

#endif /* DEBUG_CONFIG_H */
