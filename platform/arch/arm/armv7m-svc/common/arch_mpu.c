/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <uapp.h>
#include <mpu.h>

#define MPU_DEBUG

#ifdef MPU_DEBUG
#define DEBUG(fmt, arg...) printf(fmt, ##arg)
#else
#define DEBUG(fmt, arg...)
#endif

#define  ARCH_CODE_REGION_START         0x00000000UL
#define  ARCH_CODE_REGION_SIZE          0x20000000UL
#define  ARCH_SRAM_REGION_START         0x20000000UL
#define  ARCH_SRAM_REGION_SIZE          0x20000000UL
#define  ARCH_PERIPHERAL_REGION_START   0x40000000UL
#define  ARCH_PERIPHERAL_REGION_SZIE    0x20000000UL
#define  ARCH_REST_REGION_START         0xE0000000UL
#define  ARCH_REST_REGION_SIZE          0x20000000UL

typedef struct {
    unsigned long start;
    unsigned long size;
    unsigned long mpusize;
} mem_region_t;

typedef struct {
    mem_region_t text_region;
    mem_region_t data_region;
} app_mem_region_t;

enum {
    APP_CODE_REGION = 0,
    APP_DATA_REGION,
    SYSTEM_CODE_REGION,
    SYSTEM_SRAM_REGION,
    SYSTEM_PERIPHERAL_REGION,
    SYSTEM_REST_REGION,
    SYSTEM_REGION_NUM,
};

#define APP_RNG_NO      APP_CODE_REGION

#define MAX_RNG_NO      7

static mem_region_t system_region[] = {
    {ARCH_CODE_REGION_START, ARCH_CODE_REGION_SIZE, 0},
    {ARCH_SRAM_REGION_START, ARCH_SRAM_REGION_SIZE, 0},
    {ARCH_PERIPHERAL_REGION_START, ARCH_PERIPHERAL_REGION_SZIE, 0},
    {ARCH_REST_REGION_START, ARCH_REST_REGION_SIZE, 0}
};

static app_mem_region_t app_mem_region[MAX_APP_BINS];

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


static int system_mem_region_init(void)
{
    int           ret;
    unsigned long start;
    unsigned long size;
    unsigned long mpusize;
    int           i;
    int           sys_region;

    for (i = SYSTEM_CODE_REGION; i < SYSTEM_REGION_NUM; i++) {
        sys_region = i - SYSTEM_CODE_REGION;
        start   = system_region[sys_region].start;
        size    = system_region[sys_region].size;
        mpusize = size_to_mpusize(size);

        DEBUG("system_region[%d] start 0x%x, size 0x%x, mpusize 0x%x\r\n",
              sys_region, start, size, mpusize);

        ret = mpu_region_check(start, size);
        if (!ret) {
            system_region[sys_region].mpusize = mpusize;
        } else {
            return ret;
        }
    }

    return 0;
}

static int app_mem_region_init(uapp_info_t *app_info, int id)
{
    int           ret;
    unsigned long start;
    unsigned long size;
    unsigned long mpusize;

    if ((app_info == NULL) || (id >= MAX_APP_BINS)) {
        DEBUG("%s: invalid arg, app_info %p, id %d\r\n",
                __func__, app_info, id);
        return -1;
    }

    start   = app_info->text_flash_begin;
    size    = app_info->text_flash_end - app_info->text_flash_begin;
    mpusize = size_to_mpusize(size);

    DEBUG("app_mem_region[%d] text start 0x%x, size 0x%x, mpusize 0x%x\r\n",
          id, start, size, mpusize);

    ret = mpu_region_check(start, size);
    if (!ret) {
        app_mem_region[id].text_region.start   = start;
        app_mem_region[id].text_region.size    = size;
        app_mem_region[id].text_region.mpusize = mpusize;
    } else {
        return ret;
    }

    start   = app_info->data_ram_start;
    size    = app_info->heap_end - app_info->data_ram_start;
    mpusize = size_to_mpusize(size);

    DEBUG("app_mem_region[%d] data start 0x%x, size 0x%x, mpusize 0x%x\r\n",
          id, start, size, mpusize);

    ret = mpu_region_check(start, size);
    if (!ret) {
        app_mem_region[id].data_region.start   = start;
        app_mem_region[id].data_region.size    = size;
        app_mem_region[id].data_region.mpusize = mpusize;
    } else {
        return ret;
    }

    return 0;
}

static void enable_region(mem_region_t *region, int rng_no,
                          int subregion_disable, int ext_type,
                          int access_permission, int disable_exec,
                          int shareable, int cacheable, int bufferable)
{
    MPU_Region_Init_t init;

    CPSR_ALLOC();

    init.range_no          = rng_no;
    init.base_addr         = region->start;
    init.size              = region->mpusize;
    init.subregion_disable = subregion_disable;
    init.ext_type          = ext_type;
    init.access_permission = access_permission;
    init.disable_exec      = disable_exec;
    init.shareable         = shareable;
    init.cacheable         = cacheable;
    init.bufferable        = bufferable;
    init.enable            = 1;

    RHINO_CRITICAL_ENTER();

    mpu_config_region(&init);

    RHINO_CRITICAL_EXIT();
}

static int enable_system_region(void)
{
    int ret = -1;

    mem_region_t *region;
    int           sys_region;

    mpu_enable();

    sys_region = SYSTEM_CODE_REGION - SYSTEM_CODE_REGION;
    region = &system_region[sys_region];
    enable_region(region, sys_region, 0, 0, MPU_AP_RO_NA, 0, 0, 1, 1);

    sys_region = SYSTEM_SRAM_REGION - SYSTEM_CODE_REGION;
    region = &system_region[sys_region];
    enable_region(region, sys_region, 0, 0, MPU_AP_RW_NA, 0, 0, 1, 1);

    sys_region = SYSTEM_PERIPHERAL_REGION - SYSTEM_CODE_REGION;
    region = &system_region[sys_region];
    enable_region(region, sys_region, 0, 0, MPU_AP_RW_NA, 1, 0, 1, 1);

    sys_region = SYSTEM_REST_REGION - SYSTEM_CODE_REGION;
    region = &system_region[sys_region];
    enable_region(region, sys_region, 0, 0, MPU_AP_RW_NA, 1, 0, 1, 1);

    return 0;
}

static int enable_app_region(int id)
{
    mem_region_t *region;
    int rng_no;

    rng_no = APP_CODE_REGION;

    if ((rng_no > MAX_RNG_NO - 1)
        || (id > MAX_APP_BINS - 1)) {
        return -1;
    }

    region = &app_mem_region[id].text_region;
    enable_region(region, rng_no, 0, 0, MPU_AP_RO_RO, 0, 0, 1, 1);

    region = &app_mem_region[id].data_region;
    enable_region(region, rng_no + 1, 0, 0, MPU_AP_RW_RW, 0, 0, 1, 1);

    return 0;
}

static int disable_app_region(int id)
{
    MPU_Region_Init_t init;
    int rng_no;

    CPSR_ALLOC();

    rng_no = APP_CODE_REGION;

    RHINO_CRITICAL_ENTER();

    init.range_no = rng_no;
    init.enable   = 0;
    mpu_config_region(&init);

    init.range_no = rng_no + 1;
    init.enable   = 0;
    mpu_config_region(&init);

    RHINO_CRITICAL_EXIT();

    return 0;
}

int arch_mpu_init(void)
{
    int ret;;
    unsigned int type;

    memset((void*)app_mem_region, 0, sizeof(app_mem_region));

    ret = system_mem_region_init();
    if (ret) {
        return ret;
    }

    type = mpu_get_type();
    if (!type) {
        DEBUG("%s: system doesn't support mpu\r\n", __func__);
        return -1;
    } else {
        DEBUG("%s: system mpu type 0x%x\r\n", __func__, type);
    }

    return enable_system_region();
}


int arch_app_prepare(uapp_info_t *app_info, int pid)
{
    int ret;

    ret = app_mem_region_init(app_info, PID_TO_APP_ID(pid));
    if (ret) {
        DEBUG("%s: app mem region init failed, id %d\r\n",
              __func__, PID_TO_APP_ID(pid));
    }

    return ret;
}

void cpu_mm_hook(ktask_t *new, ktask_t *old)
{
    if (new->pid == old->pid)
        return;

    if (new->pid == 0) {
        disable_app_region(PID_TO_APP_ID(old->pid));
    } else {
        disable_app_region(PID_TO_APP_ID(old->pid));
        enable_app_region(PID_TO_APP_ID(new->pid));
    }
}

