/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <mpu.h>

#define KERNEL_RNG_NO   0
#define APP_RNG_NO      2

typedef struct {
    unsigned int text_start_addr;
    unsigned int text_size;
    unsigned int data_start_addr;
    unsigned int data_size;
} mem_region_t;


// defined in linker script
extern char text_start, text_size;
extern char data_start, data_size;

static mem_region_t kernel_mm_map;
static mem_region_t g_app_mm_map[MAX_APP_BINS];

static int region_validation_check(mem_region_t *mem_region)
{
    unsigned int size;

    size = mpusize_to_size(mem_region->text_size);
    if ((size == 0) || (mem_region->text_start_addr % size != 0)) {
        printf("error: region text addr 0x%x, size 0x%x\r\n",
               mem_region->text_start_addr, mem_region->text_size);
        return -1;
    }

    size = mpusize_to_size(mem_region->data_size);
    if ((size == 0) || (mem_region->data_start_addr % size != 0)) {
        printf("error: region data addr 0x%x, size 0x%x\r\n",
                mem_region->data_start_addr, mem_region->data_size);
        return -1;
    }

    return 0;
}


static void kernel_mem_region_init(void)
{
    kernel_mm_map.text_start_addr = &text_start;
    kernel_mm_map.text_size = size_to_mpusize(&text_size);
    kernel_mm_map.data_start_addr = &data_start;
    kernel_mm_map.data_size = size_to_mpusize(&data_size);

    return 0;
}

static void app_mem_region_init(struct m_app_info_t *app_info[])
{
    int i;

    memset((void*)g_app_mm_map, 0, sizeof(g_app_mm_map));

    for (i = 0; i < MAX_APP_BINS; i++) {
        if (app_info[i] && app_info[i]->magic == APP_INFO_MAGIC) {
            g_app_mm_map[i].text_start_addr =
                app_info[i]->text_flash_begin;

            g_app_mm_map[i].text_size =
                    size_to_mpusize(app_info[i]->text_flash_end -
                                app_info[i]->text_flash_begin);

            g_app_mm_map[i].data_start_addr =
                app_info[i]->data_ram_start;

            g_app_mm_map[i].data_size =
                    size_to_mpusize(app_info[i]->heap_end -
                                app_info[i]->data_ram_start);

        }
    }

    return 0;
}

static int enable_kernel_region(mem_region_t *mem_region, int rng_no)
{
    int ret = -1;
    MPU_Region_Init_t init;

    uint32_t type = mpu_get_type();

    if (!type) {
        printf("%s, system doesn't support mpu\r\n", __func__);
        return -1;
    } else {
        printf("%s, system mpu type 0x%x\r\n", __func__,  type);
    }

    ret = region_validation_check(mem_region);
    if (ret) {
        printf("enable_kernel_region failed, rgn_no %d\r\n", rng_no);
        return ret;
    }

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

static int enable_app_region(mem_region_t *mem_region, int rng_no)
{
    MPU_Region_Init_t init;

    if (rng_no > 6)
        return -1;

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

    return 0;
}

static int disable_region(int rng_no)
{
    MPU_Region_Init_t init;

    if (rng_no > 6)
        return -1;

    mpu_disable();

    init.range_no = rng_no;
    init.enable = 0;
    mpu_config_region(&init);

    init.range_no = rng_no + 1;
    init.enable = 0;
    mpu_config_region(&init);

    mpu_enable();

    return 0;
}

int arch_app_init(struct m_app_info_t *app_info[])
{
    int ret = -1;;

    kernel_mem_region_init();

    app_mem_region_init(app_info);

    ret = enable_kernel_region(&kernel_mm_map, KERNEL_RNG_NO);

    return ret;
}


int arch_app_prepare(int app_id)
{
    int ret = -1;

    if (app_id >= MAX_APP_BINS)
        return;

    ret = region_validation_check(&g_app_mm_map[app_id]);
    if (!ret) {
        enable_app_region(&g_app_mm_map[app_id], APP_RNG_NO);
    } else {
        printf("app %d validation check failed \r\n", app_id);
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
        enable_app_region(&g_app_mm_map[new->pid - 1], APP_RNG_NO);
    }
}

