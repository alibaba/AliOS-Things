/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "hal/soc/soc.h"
#include <app_mm.h>

extern unsigned int _app_text_flash_begin;
extern unsigned int _app_text_flash_end;
extern unsigned int _app_data_ram_begin;
extern unsigned int _app_data_ram_end;
extern unsigned int _app_data_flash_begin;
extern unsigned int _app_bss_start;
extern unsigned int _app_bss_end;
extern unsigned int _app_heap_start;
extern unsigned int _app_heap_end;
extern unsigned int app_type;
extern int application_start(int argc, char **argv);

#define UTASK_STACK_SIZE 0x100

static ktask_t utask_obj;
cpu_stack_t utask_ubuf[UTASK_STACK_SIZE];

#define MM_BUF_SIZE     (0x10000)

static char mm_buf[MM_BUF_SIZE];


static void utask_run(void *arg)
{
    application_start(0, NULL);
}

static void app_entry(int id, int argc, char *argv[])
{
    ktask_t *cur;
    char *buf;
    int size;

    app_mm_init(mm_buf, MM_BUF_SIZE);

    printf("%s start to run\r\n", __func__);

    printf("%s mm_buf init done\r\n", __func__);


    for (int i = 0; i < 5; i++) {
        size = 100;
        buf = app_mm_malloc(size);
        if (buf) {
            printf("app malloc buf %p, size 0x%x\r\n", buf, size);
            app_mm_free(buf);
        } else {
            printf("app_ma_malloc failed\r\n");
        }

    }
    printf("app mm test end\r\n");


    krhino_uprocess_create(&utask_obj, "utask", 0,AOS_DEFAULT_APP_PRI,
        (tick_t)0, utask_ubuf, UTASK_STACK_SIZE, UTASK_STACK_SIZE,
        (task_entry_t)utask_run, id, 1);

    cur = krhino_cur_task_get();

    krhino_task_del(cur);

}

__attribute__ ((used, section(".app_info"))) struct m_app_info_t app_info = {
    app_entry,
    &_app_text_flash_begin,
    &_app_text_flash_end,
    &_app_data_ram_begin,
    &_app_data_ram_end,
    &_app_data_flash_begin,
    &_app_bss_start,
    &_app_bss_end,
    &_app_heap_start,
    &_app_heap_end,
    APP_INFO_MAGIC,
    0x0,
    &app_type
};

