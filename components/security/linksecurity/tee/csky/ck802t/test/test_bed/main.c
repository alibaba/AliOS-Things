/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "test_bed.h"

extern void CK_Platform_Init(void);
void        test_bed_init(void);
extern int  TEEC_Init(void);
extern void test_entry(void);
int         main(void)
{
    CK_Platform_Init();
    test_bed_init();
#if CONFIG_TST_STUB
    int            ret;
    extern int32_t hal_stub_init(void);
    ret = hal_stub_init();
    if (ret) {
        printk("ERR: hal tst stub init fail 0x%08x\n", ret);
    }
#endif
    printk("Start...\n");

    test_entry();

#if CONFIG_TST_STUB
    extern void hal_stub_cleanup(void);
    hal_stub_cleanup();
#endif
    while (1)
        ;
}

void test_bed_init(void)
{
    kmem_init();
}
