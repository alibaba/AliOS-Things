/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/kernel.h>

extern int vfs_init(void);
extern int vfs_device_init(void);
extern int aos_kv_init(void);
extern void ota_service_init(void);
extern int aos_framework_init(void);
extern void trace_start(void);
extern int application_start(int argc, char **argv);
extern void aos_components_init(void);

#ifdef WITH_SAL
extern int sal_device_init(void);
#endif

#ifdef AOS_BINS
extern void *syscall_ktbl[];
extern char  app_info_addr;
extern char  framework_info_addr;
extern k_mm_head  *g_kmm_head;

struct framework_info_t *framework_info = (struct framework_info_t *)&framework_info_addr;
struct app_info_t *app_info = (struct app_info_t *)&app_info_addr;

static void app_pre_init(void)
{
    memcpy((void *)(app_info->data_ram_start), (void *)(app_info->data_flash_begin),
           app_info->data_ram_end - app_info->data_ram_start);
    memset((void *)(app_info->bss_start), 0, app_info->bss_end - app_info->bss_start);

    krhino_add_mm_region(g_kmm_head, (void *)(app_info->heap_start),
                        app_info->heap_end - app_info->heap_start);

    krhino_mm_leak_region_init((void *)(app_info->data_ram_start), (void *)(app_info->data_ram_end));
    krhino_mm_leak_region_init((void *)(app_info->bss_start), (void *)(app_info->bss_end));
}

static void framework_pre_init(void)
{
    memcpy((void *)(framework_info->data_ram_start), (void *)(framework_info->data_flash_begin),
           framework_info->data_ram_end - framework_info->data_ram_start);
    memset((void *)(framework_info->bss_start), 0, framework_info->bss_end - framework_info->bss_start);

    krhino_add_mm_region(g_kmm_head, (void *)(framework_info->heap_start),
                        framework_info->heap_end - framework_info->heap_start);

    krhino_mm_leak_region_init((void *)(framework_info->data_ram_start), (void *)(framework_info->data_ram_end));
    krhino_mm_leak_region_init((void *)(framework_info->bss_start), (void *)(framework_info->bss_end));
}
#endif

int aos_kernel_init(kinit_t *kinit)
{
#ifdef AOS_VFS
    vfs_init();
    vfs_device_init();
#endif
    
#ifdef CONFIG_AOS_CLI
    if (kinit->cli_enable)
        aos_cli_init();
#endif
    
#ifdef AOS_KV
    aos_kv_init();
#endif

#ifdef WITH_SAL
    sal_device_init();
#endif

#ifdef AOS_LOOP
    aos_loop_init();
#endif

#ifdef VCALL_RHINO
    trace_start();
#endif

#ifdef AOS_FOTA 
    ota_service_init();
#endif

// auto_component generated by the compiler system, now gcc support
#if defined (__GNUC__)
//    aos_components_init();
#endif

#ifdef AOS_BINS
    app_pre_init();
    framework_pre_init();

    if (framework_info->framework_entry) {
        framework_info->framework_entry((void *)syscall_ktbl, kinit->argc, kinit->argv);
    }
#else

#ifdef AOS_FRAMEWORK_COMMON
    aos_framework_init();
#endif

    application_start(kinit->argc, kinit->argv);
#endif

    return 0;
}


