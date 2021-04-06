/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_INIT_H
#define AOS_INIT_H

#include <stdbool.h>

/** @defgroup init_aos_api init
 * @{
 */

/* aos_component_init初始化参数 */
typedef struct {
    int argc;        /**< 参数个数 */
    char **argv;     /**< 参数内容 */

    bool cli_enable; /**< cli是否初始化生效 */
} kinit_t;

/**
 * aos 基础模块初始化.
 *
 * @param[in]  kinit  kinit_t类型的初始化参数.
 *
 * @return  固定返回0.
 */
extern int aos_components_init(kinit_t *kinit);

#ifndef AOS_BINS
/**
 * 定义了用户app的入口函数形式定义。该函数由用户在app内自定义实现。
 *
 * @param[in]  argc  aos_components_init时传入的参数数目.
 * @param[in]  argv  aos_components_init时传入的参数内容.
 *
 * @return  用户自定义.
 */
extern int application_start(int argc, char **argv);
#endif

/**
 * @}
 */

/**
 * AOS_BINS是多bin功能，HAAS单板不使用。
 */

#ifdef AOS_BINS
#include <k_api.h>

struct app_info_t {
     void (*app_entry)(void *ksyscall_tbl, void *fsyscall_tbl, int argc, char *argv[]);
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
};

struct framework_info_t {
     void (*framework_entry)(void *syscall_tbl, int argc, char *argv[]);
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
};

struct m_app_info_t {
     void (*app_entry)(void *ksyscall_tbl, int argc, char *argv[]);
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
     /* reserve for other */
     unsigned int reserve0;
     unsigned int reserve1;
     /* this bin_type must be here, 0x28, uniform with single bin & kernel bin,
        arm cortex not used */
     unsigned int bin_type;
     unsigned int reserve2;
     unsigned int reserve3;
     unsigned int reserve4;
     unsigned int reserve5;
     unsigned int reserve6;
};

#endif

#endif /* AOS_INIT_H */
