/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <stddef.h>

#ifdef USER_SPACE_DRIVER
#include <rpc.h>
#endif
#include <drivers/char/u_device.h>
#include <drivers/u_ld.h>
#include <drivers/char/u_device.h>
#include <drivers/char/u_cdev_msg.h>

extern long __core_driver_start__;
extern long __core_driver_end__;
extern long __bus_driver_start__;
extern long __bus_driver_end__;
extern long __vfs_driver_start__;
extern long __vfs_driver_end__;
extern long __early_driver_start__;
extern long __early_driver_end__;
extern long __level0_driver_start__;
extern long __level0_driver_end__;
extern long __level1_driver_start__;
extern long __level1_driver_end__;
extern long __level2_driver_start__;
extern long __level2_driver_end__;
extern long __level3_driver_start__;
extern long __level3_driver_end__;
extern long __post_driver_start__;
extern long __post_driver_end__;

struct section_info {
    OS_DRIVER_ENTRY *s;
    OS_DRIVER_ENTRY *e;
};

#define DODE(x) (OS_DRIVER_ENTRY *)(x)

/**
 * all driver entry declared with XXXX_DRIVER_ENTRY or XXXX_DRIVER_FULL_ENTRY (POST is excluded) will be called with specified
 * sequence
 *
 * @return 0 for success; negative error no. for failure
 */
int _os_driver_entry(void) {
    int i = 0;
    OS_DRIVER_ENTRY *s = NULL;
    OS_DRIVER_ENTRY *e = NULL;

    OS_DRIVER_ENTRY *drv_entry = NULL;

    struct section_info g_drv_section_entry[] = {
        {DODE(&__core_driver_start__), DODE(&__core_driver_end__)},
        {DODE(&__bus_driver_start__), DODE(&__bus_driver_end__)},
        {DODE(&__early_driver_start__), DODE(&__early_driver_end__)},
        {DODE(&__vfs_driver_start__), DODE(&__vfs_driver_end__)},
        {DODE(&__level0_driver_start__), DODE(&__level0_driver_end__)},
        {DODE(&__level1_driver_start__), DODE(&__level1_driver_end__)},
        {DODE(&__level2_driver_start__), DODE(&__level2_driver_end__)},
        {DODE(&__level3_driver_start__), DODE(&__level3_driver_end__)},
    };

    for (i = 0; i < sizeof(g_drv_section_entry)/sizeof(g_drv_section_entry[0]); i++) {
        s = g_drv_section_entry[i].s;
        e = g_drv_section_entry[i].e;
        drv_entry = s;
        while (drv_entry < e) {
            ddkc_dbg("drv_entry:%p, *drv_entry:%p\r\n", drv_entry, *drv_entry);
            (*((OS_DRIVER_ENTRY *)drv_entry))();
            drv_entry++;
        }

    }
    return 0;
}

/**
 * all driver entry declared with POST_DRIVER_ENTRY or POST_DRIVER_FULL_ENTRY will be called with specified
 * sequence
 *
 * @return 0 for success; negative error no. for failure
 */
int _os_post_driver_entry(void) {
    int i = 0;
    OS_DRIVER_ENTRY *s = NULL;
    OS_DRIVER_ENTRY *e = NULL;

    OS_DRIVER_ENTRY *drv_entry = NULL;

    struct section_info g_drv_section_entry[] = {
        {DODE(&__post_driver_start__), DODE(&__post_driver_end__)},
    };

    for (i = 0; i < sizeof(g_drv_section_entry)/sizeof(g_drv_section_entry[0]); i++) {
        s = g_drv_section_entry[i].s;
        e = g_drv_section_entry[i].e;
        drv_entry = s;
        while (drv_entry < e) {
            ddkc_dbg("drv_entry:%p, *drv_entry:%p\r\n", drv_entry, *drv_entry);
            (*((OS_DRIVER_ENTRY *)drv_entry))();
            drv_entry++;
        }

    }
    return 0;
}
