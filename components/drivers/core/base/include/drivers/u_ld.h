/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_LD_H_
#define _U_LD_H_

/**
 * hardware registers' information
 * @addr - register's base address of hw module
 * @length - register region's length counted from base address of the hw module
 */
typedef struct hw_reg_info {
    unsigned long addr;
    int length;
} hw_reg_info_t;

/**
 * hardware's register information array
 * @size - array size of hw
 * @hw  - pointer to hw_reg_info_t
 */
typedef struct hw_reg_array_info {
    int size;
    hw_reg_info_t *hw[];
} hw_reg_array_info_t;

/**
 * all driver entry declared with marcos in this file should be define with this prototype
 */
typedef int (*OS_DRIVER_ENTRY)(void);

/**
 * function prototype used to get hardware module's register information
 */
typedef hw_reg_array_info_t* (*DEVICE_HW_REG_INFO)(void);

/**
 * this is the only driver entry API, when a driver is selected, must call this API in proper place
 *
 * @string process identification
 *
 * @return 0 for success; negative error number for failure
 */
extern int u_driver_entry(char *string);

/**
 * all driver entry declared with XXXX_DRIVER_ENTRY or XXXX_DRIVER_FULL_ENTRY will be called with specified
 * sequence
 *
 * @return 0 for success; negative error no. for failure
 */
extern int _os_driver_entry(void);

/**
 * unified driver initialization entry
 * do register map prior to call driver's init entry in user space
 *
 * @param drv_name - driver's name
 * @param drv_init_entry - pointer to driver's init entry
 * @param get_hw_info - pointer to the function to get driver's register info
 * @return 0 for success; negative for failure
 *
 */
extern int _unify_driver_init(const char *drv_name, OS_DRIVER_ENTRY drv_init_entry, DEVICE_HW_REG_INFO get_hw_info);

#define TRACE_DRV_ENTRY() //printf("%s\r\n", __func__)

/* ld sections declaration */
#if defined (__CC_ARM) || defined (__GNUC__)
#define OS_CORE_DRIVER_ATT    __attribute__((used, section(".core_driver_entry")))
#define OS_BUS_DRIVER_ATT     __attribute__((used, section(".bus_driver_entry")))
#define OS_EARLY_DRIVER_ATT   __attribute__((used, section(".early_driver_entry")))
#define OS_VFS_DRIVER_ATT     __attribute__((used, section(".vfs_driver_entry")))
#define OS_LEVEL0_DRIVER_ATT  __attribute__((used, section(".level0_driver_entry")))
#define OS_LEVEL1_DRIVER_ATT  __attribute__((used, section(".level1_driver_entry")))
#define OS_LEVEL2_DRIVER_ATT  __attribute__((used, section(".level2_driver_entry")))
#define OS_LEVEL3_DRIVER_ATT  __attribute__((used, section(".level3_driver_entry")))
#define OS_POST_DRIVER_ATT    __attribute__((used, section(".post_driver_entry")))
#elif defined (__ICCARM__)
#pragma OS_CORE_DRIVER_ATT   = ".core_driver_entry"
#pragma OS_BUS_DRIVER_ATT    = ".bus_driver_entry"
#pragma OS_EARLY_DRIVER_ATT  = ".early_driver_entry"
#pragma OS_VFS_DRIVER_ATT    = ".vfs_driver_entry"
#pragma OS_LEVEL0_DRIVER_ATT = ".level0_driver_entry"
#pragma OS_LEVEL1_DRIVER_ATT = ".level1_driver_entry"
#pragma OS_LEVEL2_DRIVER_ATT = ".level2_driver_entry"
#pragma OS_LEVEL3_DRIVER_ATT = ".level3_driver_entry"
#pragma OS_POST_DRIVER_ATT   = ".post_driver_entry"
#endif

/**
 * Usage: if vendor or developer want to define a new device driver
 *  you don't need to care how to call your driver initialization,
 *  just select proper marcos here and declara your driver init function
 *  by XXXX_DRIVER_ENTRY(YOUR_DRIVER_INIT_FUNCTION) or XXXX_DRIVER_FULL_ENTRY(YOUR_DRIVER_INIT_FUNCTION)
 *
 * basic concept of the following marcos is as follows:
 * 1. define a new function named xxxx_##your_driver_init_function,
 *  and put this new API's entry into image's section named .xxxx_entry
 * 2. when _os_driver_entry is called, it will read the functions pointers
 *  from .xxxx_entry image section and call them one by one
 *
 * */
#define CORE_DRIVER_ENTRY(x) int core_drv_##x(void) { \
    TRACE_DRV_ENTRY(); \
    return x(); \
} \
OS_CORE_DRIVER_ATT OS_DRIVER_ENTRY core_##x = core_drv_##x;

#define BUS_DRIVER_FULL_ENTRY(init, get_hw_info) int bus_drv_##init(void) { \
    return _unify_driver_init(__func__, init, get_hw_info); \
} \
OS_BUS_DRIVER_ATT OS_DRIVER_ENTRY bus_##init = bus_drv_##init;

#define BUS_DRIVER_ENTRY(init) BUS_DRIVER_FULL_ENTRY(init, NULL)


#define VFS_DRIVER_FULL_ENTRY(init, get_hw_info) int vfs_drv_##init(void) { \
    return _unify_driver_init(__func__, init, get_hw_info); \
} \
OS_VFS_DRIVER_ATT OS_DRIVER_ENTRY vfs_##init = vfs_drv_##init;

#define VFS_DRIVER_ENTRY(init) VFS_DRIVER_FULL_ENTRY(init, NULL)

#define EARLY_DRIVER_FULL_ENTRY(init, get_hw_info) int early_drv_##init(void) { \
    return _unify_driver_init(__func__, init, get_hw_info); \
} \
OS_EARLY_DRIVER_ATT OS_DRIVER_ENTRY early_##init = early_drv_##init;

#define EARLY_DRIVER_ENTRY(init) EARLY_DRIVER_FULL_ENTRY(init, NULL)

#define LEVEL0_DRIVER_FULL_ENTRY(init, get_hw_info) int level0_drv_##init(void) { \
    return _unify_driver_init(__func__, init, get_hw_info); \
} \
OS_LEVEL0_DRIVER_ATT OS_DRIVER_ENTRY level0_##init = level0_drv_##init;

#define LEVEL0_DRIVER_ENTRY(init) LEVEL0_DRIVER_FULL_ENTRY(init, NULL)

#define LEVEL1_DRIVER_FULL_ENTRY(init, get_hw_info) int level1_drv_##init(void) { \
    return _unify_driver_init(__func__, init, get_hw_info); \
} \
OS_LEVEL1_DRIVER_ATT OS_DRIVER_ENTRY level1_##init = level1_drv_##init;

#define LEVEL1_DRIVER_ENTRY(init) LEVEL1_DRIVER_FULL_ENTRY(init, NULL)


#define LEVEL2_DRIVER_FULL_ENTRY(init, get_hw_info) int level2_drv_##init(void) { \
    return _unify_driver_init(__func__, init, get_hw_info); \
} \
OS_LEVEL2_DRIVER_ATT OS_DRIVER_ENTRY level2_##init = level2_drv_##init;

#define LEVEL2_DRIVER_ENTRY(init) LEVEL2_DRIVER_FULL_ENTRY(init, NULL)

#define LEVEL3_DRIVER_FULL_ENTRY(init, get_hw_info) int level3_drv_##init(void) { \
    return _unify_driver_init(__func__, init, get_hw_info); \
} \
OS_LEVEL3_DRIVER_ATT OS_DRIVER_ENTRY level3_##init = level3_drv_##init;

#define LEVEL3_DRIVER_ENTRY(init) LEVEL3_DRIVER_FULL_ENTRY(init, NULL)

#define POST_DRIVER_FULL_ENTRY(init, get_hw_info) int post_drv_##init(void) { \
    return _unify_driver_init(__func__, init, get_hw_info); \
} \
OS_POST_DRIVER_ATT OS_DRIVER_ENTRY post_##init = post_drv_##init;

#define POST_DRIVER_ENTRY(init) POST_DRIVER_FULL_ENTRY(init, NULL)

#endif //_U_LD_H_
