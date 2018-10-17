/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <uapp.h>
#include <mpu.h>

//#define MPU_DEBUG

#ifdef MPU_DEBUG
#define DEBUG(fmt, arg...) printf(fmt, ##arg)
#else
#define DEBUG(fmt, arg...)
#endif

#define KERNEL_RNG_NO   0
#define APP_RNG_NO      2

#define MAX_RNG_NO      7

typedef struct {
    unsigned long text_start_addr;
    unsigned long text_size;
    unsigned long data_start_addr;
    unsigned long data_size;
} mem_region_t;

mem_region_t *g_active_app_mem_region = NULL;

// defined in linker script
extern char text_start, text_size;
extern char data_start, data_size;

// the first one is kernel
static mem_region_t g_mm_map[MAX_APP_BINS + 1];

static int mpu_region_check(unsigned int addr,
                            unsigned int size)
{
    if ((size == 0) || (addr % size != 0)) {
        DEBUG("error: mpu region addr 0x%x, size 0x%x\r\n",
               addr, size);
        return -1;
    }

    return 0;
}


static int kernel_mem_region_init(void)
{
    int ret;
    unsigned long addr, size;

    DEBUG("init kernel region\r\n");
    addr = &text_start;
    size = &text_size;
    ret = mpu_region_check(addr, size);
    if (!ret) {
        g_mm_map[0].text_start_addr = addr;
        g_mm_map[0].text_size = size_to_mpusize(size);
    } else {
        return ret;
    }

    addr = &data_start;
    size = &data_size;
    ret = mpu_region_check(addr, size);
    if (!ret) {
        g_mm_map[0].data_start_addr = addr;
        g_mm_map[0].data_size = size_to_mpusize(size);
    } else {
        return ret;
    }

    DEBUG("kernel text addr 0x%x, mpusize 0x%x, data addr 0x%x, mpusize 0x%x\r\n",
            g_mm_map[0].text_start_addr,
            g_mm_map[0].text_size,
            g_mm_map[0].data_start_addr,
            g_mm_map[0].data_size);



    return 0;
}

static int app_mem_region_init(uapp_info_t *app_info, int id)
{
    int ret;

    unsigned long addr, size;
    unsigned long text_start_addr, text_mpusize;
    unsigned long data_start_addr, data_mpusize;

    if ((app_info == NULL)
        || (id == 0) || (id >= MAX_APP_BINS)) {
        DEBUG("%s: invalid arg, app_info %p, id %d\r\n",
                __func__, app_info, id);
        return -1;
    }

    addr = app_info->text_flash_begin;
    size = app_info->text_flash_end - app_info->text_flash_begin;
    ret = mpu_region_check(addr, size);
    if (!ret) {
        text_start_addr = addr;
        text_mpusize = size_to_mpusize(size);
    } else {
        return ret;
    }

    addr = app_info->data_ram_start;
    size = app_info->heap_end - app_info->data_ram_start;
    ret = mpu_region_check(addr, size);
    if (!ret) {
        data_start_addr = addr;
        data_mpusize = size_to_mpusize(size);
    } else {
        return ret;
    }

    g_mm_map[id].text_start_addr = text_start_addr;
    g_mm_map[id].text_size = text_mpusize;
    g_mm_map[id].data_start_addr = data_start_addr;
    g_mm_map[id].data_size = data_mpusize;

    DEBUG("%s: text addr 0x%x, mpusize 0x%x, data addr 0x%x, mpusize 0x%x\r\n",
            __func__,
            g_mm_map[id].text_start_addr,
            g_mm_map[id].text_size,
            g_mm_map[id].data_start_addr,
            g_mm_map[id].data_size);

    return 0;
}

static int enable_kernel_region(void)
{
    int ret = -1;
    MPU_Region_Init_t init;
    mem_region_t *mem_region;
    int rng_no;

    rng_no = KERNEL_RNG_NO;
    mem_region = &g_mm_map[0];

    init.range_no = rng_no;
    init.base_addr = mem_region->text_start_addr;
    init.size = mem_region->text_size;
    init.subregion_disable = 0;
    init.ext_type = 0;
    init.access_permission = MPU_AP_RO_NA;
    init.disable_exec = 0;
    init.shareable = 0;
    init.cacheable = 1;
    init.bufferable = 1;
    init.enable = 1;
    mpu_config_region(&init);

    init.range_no = rng_no + 1;
    init.base_addr = mem_region->data_start_addr;
    init.size = mem_region->data_size;
    init.subregion_disable = 0;
    init.ext_type = 0;
    init.access_permission = MPU_AP_RW_NA;
    init.disable_exec = 0;
    init.shareable = 0;
    init.cacheable = 1;
    init.bufferable = 1;
    init.enable = 1;
    mpu_config_region(&init);

    mpu_enable();

    return 0;
}

static int enable_app_region(int id, int rng_no)
{
    MPU_Region_Init_t init;
    mem_region_t *mem_region;

    if ((rng_no > MAX_RNG_NO - 1)
        || (id > MAX_APP_BINS - 1)) {
        return -1;
    }

    mem_region = &g_mm_map[id];

    mpu_disable();

    init.range_no = rng_no;
    init.base_addr = mem_region->text_start_addr;
    init.size = mem_region->text_size;
    init.subregion_disable = 0;
    init.ext_type = 0;
    init.access_permission = MPU_AP_RO_RO;
    init.disable_exec = 0;
    init.shareable = 0;
    init.cacheable = 1;
    init.bufferable = 1;
    init.enable = 1;
    mpu_config_region(&init);

    init.range_no = rng_no + 1;
    init.base_addr = mem_region->data_start_addr;
    init.size = mem_region->data_size;
    init.subregion_disable = 0;
    init.ext_type = 0;
    init.access_permission = MPU_AP_RW_RW;
    init.disable_exec = 0;
    init.shareable = 0;
    init.cacheable = 1;
    init.bufferable = 1;
    init.enable = 1;
    mpu_config_region(&init);

    mpu_enable();

    g_active_app_mem_region = mem_region;

    return 0;
}

static int disable_region(int rng_no)
{
    MPU_Region_Init_t init;

    if (rng_no > MAX_RNG_NO - 1)
        return -1;

    mpu_disable();

    init.range_no = rng_no;
    init.enable = 0;
    mpu_config_region(&init);

    init.range_no = rng_no + 1;
    init.enable = 0;
    mpu_config_region(&init);

    mpu_enable();

    g_active_app_mem_region = NULL;

    return 0;
}

int arch_app_init(void)
{
    int ret;;
    unsigned int type;

    memset((void*)g_mm_map, 0, sizeof(g_mm_map));

    ret = kernel_mem_region_init();
    if (ret) {
        DEBUG("%s: kernel region init failed\r\n",
              __func__);
        return ret;
    }

    type = mpu_get_type();
    if (!type) {
        DEBUG("%s: system doesn't support mpu\r\n",
              __func__);
        return -1;
    } else {
        DEBUG("%s: system mpu type 0x%x\r\n",
              __func__, type);
    }

    return enable_kernel_region();
}


int arch_app_prepare(uapp_info_t *app_info, int id)
{
    int ret;

    ret = app_mem_region_init(app_info, id);
    if (ret) {
        DEBUG("%s: app mem region init failed, id %d\r\n",
              __func__, id);
    }

    return ret;
}

void cpu_mm_hook(ktask_t *new, ktask_t *old)
{
    if (new->pid == old->pid)
        return;

    if (new->pid == 0) {
        disable_region(APP_RNG_NO);
    } else {
        disable_region(APP_RNG_NO);
        enable_app_region(new->pid, APP_RNG_NO);
    }
}

