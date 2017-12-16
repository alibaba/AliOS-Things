/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "test_bed.h"

extern void CK_Platform_Init(void);
void test_bed_init(void);
extern int TEEC_Init(void);
extern void test_entry(void);
int main(void)
{
    CK_Platform_Init();
    test_bed_init();

    printk("Start...\n");
    test_entry();

    while(1);
}

void test_bed_init(void)
{
    kmem_init();
}
