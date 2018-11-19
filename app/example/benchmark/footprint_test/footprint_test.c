/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

extern void         *_etext;
extern void         *_sidata;

/*  Initialized data sections goes into RAM */
extern void         *_sdata;
extern void         *_edata;

/* bss sections */
extern void         *__bss_start__;
extern void         *__bss_end__;

int application_start(int argc, char *argv[])
{
    LOG("footprint benchmark started.");

    printf("&_etext = 0x%x\n", &_etext);
    printf("&_sidata = 0x%x\n", &_sidata);
    printf("&_sdata = 0x%x\n", &_sdata);
    printf("&_edata = 0x%x\n", &_edata);
    printf("&__bss_start__ = 0x%x\n", &__bss_start__);
    printf("&__bss_end__ = 0x%x\n", &__bss_end__);

    printf("text size = %d\n", (uint32_t)&_etext - 0x8000000);
    printf("bss sec size = %d\n", (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__);
    printf("data sec size = %d\n", (uint32_t)&_edata - (uint32_t)&_sdata);

    printf("FLASH size = %d\n", (uint32_t)&_sidata - 0x8000000);
    printf("RAM size = %d\n", (uint32_t)&__bss_end__ - 0x20000000);

    printf("dynamic mm = %d\n", g_kmm_head->maxused_size);
    
    return 0;
}


