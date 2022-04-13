/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_bin.h"
#include "k_proc.h"
#include "aos/cli.h"
#include "utask.h"
#include "iomap.h"
#include <stdbool.h>
#include <fcntl.h>
#include "fs/vfs_types.h"
#include "fs/vfs_api.h"

#ifdef AOS_COMP_DEBUG
    #include "debug_api.h"
#endif

#define RHINO_CONFIG_PROC_DEBUG 1

#ifdef RHINO_CONFIG_PROC_DEBUG
    #define DEBUG(fmt, arg...) printk(fmt, ##arg)
#else
    #define DEBUG(fmt, arg...)
#endif

/* reserve g_app[0] as pid 0 for kernel */
app_info_t g_app;

/* for each bit: 0 free, 1: used */
static uint32_t g_pid_set;

static kmutex_t proc_mutex;

static int proc_inited = 0;

/* should be called with proc_mutex locked */
static uint32_t alloc_pid(void)
{
    int i;

    for (i = 1; i <= MAX_APP_BINS; i++) {
        if (!(g_pid_set & (1 << i))) {
            g_pid_set |= (1 << i);
            return i;
        }
    }

    return 0;
}

/* should be called with proc_mutex locked */
static void free_pid(uint32_t pid)
{
    g_pid_set &= ~(1 << pid);
}

void *os_proc_file2ram(const char *path)
{
    int   file_len;
    int   read_count;
    int   fd;
    void *pbuff;

    fd = vfs_open(path, O_RDONLY);
    if (fd < 0) {
        printf("vfs_open failed\r\n");
        return NULL;
    }

    file_len = vfs_lseek(fd, 0, SEEK_END);
    vfs_lseek(fd, 0, SEEK_SET);

    if (file_len == 0) {
        vfs_close(fd);
        return NULL;
    }

    pbuff = krhino_mm_alloc(file_len);
    if (pbuff == NULL) {
        vfs_close(fd);
        return NULL;
    }

    read_count = vfs_read(fd, pbuff, file_len);

    vfs_close(fd);

    if (read_count != file_len) {
        free(pbuff);
        return NULL;
    }

    fd = -1;

    return pbuff;
}


static int proc_load_copy_mem(preamble_t *app_info, void *bin_buf)
{
    uint8_t *bin_dst;
    /* copy preamble */
    memcpy(app_info, bin_buf, sizeof(preamble_t));

    DEBUG("%s: read from preamble flash_start %p, flash_end %p, bss_start %p, maigc 0x%x\r\n", __func__,
          app_info->flash_start, app_info->flash_end, app_info->bss_start, app_info->magic);

    /* copy the all bin image */
    bin_dst = app_info->flash_start;

    DEBUG("%s: memcpy image dst %p, src %p, len 0x%x\r\n", __func__, bin_dst, bin_buf,
          (uint8_t *)app_info->ram_end - (uint8_t *)app_info->flash_start);

    memcpy(bin_dst, bin_buf, (uint8_t *)app_info->ram_end - (uint8_t *)app_info->flash_start);

    /* clear bss section */
    DEBUG("%s: clear bss, dst %p, len 0x%x\r\n", __func__, (void *)(app_info->bss_start),
          (size_t)app_info->bss_end - (size_t)app_info->bss_start);

    memset((void *)(app_info->bss_start), 0, (size_t)app_info->bss_end - (size_t)app_info->bss_start);

    DEBUG("%s: load image finished\r\n", __func__);

    return 0;
}

static int proc_load_app_from_file(preamble_t *app_info, const char *file, const char *name)
{
    uint8_t *bin_buf;

    bin_buf = (uint8_t *)os_proc_file2ram(file);
    if (bin_buf == NULL) {
        return -1;
    }

    proc_load_copy_mem(app_info, bin_buf);
    krhino_mm_free(bin_buf);

    return 0;
}

static int proc_load_app_from_ram(preamble_t *app_info, const void *addr, const char *name)
{
    uint8_t *bin_buf;

    bin_buf = (uint8_t*)addr;
    if (bin_buf == NULL) {
        return -1;
    }

    proc_load_copy_mem(app_info, bin_buf);
    return 0;
}

static int proc_create_task(app_t *app)
{
    preamble_t *app_info;
    kstat_t     stat;
    uint32_t    pid;

    app_info = app->preamble;
    pid      = app->pid;

    DEBUG("%s: start to create app %s\r\n", __func__, app_info->app_name);

    stat = krhino_uprocess_create(app_info->app_name,
                                  app_info->app_name, 0,
                                  app_info->priority, (tick_t)0,
                                  app_info->ustack,
                                  app_info->ustack_size, // ustasck size
                                  app_info->kstack_size, // kstasck size
                                  (task_entry_t)app_info->main_entry, app_info->ret_entry,
                                  pid, 9);
    if (stat != RHINO_SUCCESS) {
        DEBUG("create process %s failed, ret 0x%x\r\n", app_info->app_name, stat);
        return -1;
    }

    return 0;
}

static int proc_init_region(preamble_t *info,
                            mpu_region_t *text,
                            mpu_region_t *data)
{
    int      ret;
    uint32_t start;
    uint32_t size;
    //uint32_t mpusize;

    start   = (uint32_t)info->flash_start;
    size    = (uint32_t)info->flash_end - (uint32_t)info->flash_start;

    ret = mpu_region_check(start, size);
    if (!ret) {
        text->start   = start;
        text->size    = size;
    } else {
        goto err;
    }

    start   = (uint32_t)info->ram_start;
    size    = (uint32_t)info->ram_end - (uint32_t)info->ram_start;

    ret = mpu_region_check(start, size);
    if (!ret) {
        data->start   = start;
        data->size    = size;
    } else {
        goto err;
    }

    return 0;

err:
    DEBUG("app_mem_region[%d] data start 0x%x, size 0x%x\r\n",
          start, size);

    return ret;
}

int k_proc_init(void *phyaddr_dummy, size_t size_dummp)
{
    int ret;

    /* reserve bit 0, app starts from bit 1 */
    g_pid_set = 0x01;

    memset(&g_app, 0, sizeof(g_app));

    krhino_mutex_create(&proc_mutex, "proc_mutex");

    ret = mpu_init();

    if (!ret) {
        proc_inited = 1;
    } else {
        return -2;
    }

    return 0;
}

static int32_t proc_run_app(preamble_t *app_info)
{
    app_t    *app;
    uint32_t  pid;
    int       ret;

    krhino_mutex_lock(&proc_mutex, RHINO_WAIT_FOREVER);

    pid = alloc_pid();

    if (pid == 0) {
        DEBUG("err: no available pid\r\n");
        /* no available pid */
        ret = -3;
        goto out;
    }

    app = &(g_app.procs[pid]);
    app->pid = pid;
    app->preamble = app_info;

    DEBUG("%s: init app region\r\n", __func__);
    if (proc_init_region(app->preamble, &app->region.text_region,
                         &app->region.data_region)) {
        ret = -4;
        goto out;
    }

    DEBUG("%s: start to create app task\r\n", __func__);
    if (proc_create_task(app)) {
        ret = -5;
        goto out;
    }

    DEBUG("%s: load app end\r\n", __func__);

    /* success */
    ret = pid;

out:
    krhino_mutex_unlock(&proc_mutex);
    DEBUG("%s: load app end\r\n", __func__);

    return ret;
}


int32_t k_proc_load(const char *file, const char *name)
{
    preamble_t *app_info = krhino_mm_alloc(sizeof(preamble_t));

    if ((proc_inited == 0) || (!app_info)) {
        return -1;
    }

    if (proc_load_app_from_file(app_info, file, name) != 0) {
        return -2;
    }

    return proc_run_app(app_info);
}

int32_t k_proc_ramload(const void *addr, const char *name)
{
    preamble_t *app_info = krhino_mm_alloc(sizeof(preamble_t));

    if ((proc_inited == 0) || (!app_info)) {
        return -1;
    }

    if (proc_load_app_from_ram(app_info, addr, name) != 0) {
        return -2;
    }

    return proc_run_app(app_info);
}

extern void mpu_free_region(uint32_t pid, uint32_t region);
static void proc_mm_region_release(app_t *app)
{
    io_region_t *io_regions;
    io_region_t *tmp_regions;

    io_regions = app->io_regions;
    while (io_regions) {
        tmp_regions = io_regions->next;
        mpu_free_region(app->pid, io_regions->region_id);
        free(io_regions);
        io_regions = tmp_regions;
    }
    app->io_regions = NULL;
    return;
}

void k_proc_unload(uint32_t pid)
{
    app_t *app;

    if (pid > MAX_APP_BINS || pid == 0) {
        return;
    }

    krhino_mutex_lock(&proc_mutex, RHINO_WAIT_FOREVER);

    app = &(g_app.procs[pid]);
    proc_mm_region_release(app);
    krhino_mm_free(app->preamble);
    memset(app, 0, sizeof(app_t));

    free_pid(pid);

    krhino_mutex_unlock(&proc_mutex);
}

static void proc_disable_region(uint32_t pid)
{
    app_t *app = &(g_app.procs[pid]);
    io_region_t *io_regions;

    mpu_disable_region(MPU_APP_DATA_RNG);
    mpu_disable_region(MPU_APP_TEXT_RNG);

    /* free io region */
    io_regions = app->io_regions;
    while (io_regions) {
        mpu_disable_region(io_regions->region_id);
        io_regions = io_regions->next;
    }
}

static void proc_enable_region(uint32_t pid)
{
    app_t *app = &(g_app.procs[pid]);
    io_region_t *io_regions;
    int cacheattr;
    int attr;

    mpu_enable_region(&app->region.text_region,
                      MPU_APP_TEXT_RNG, 0, 0,
                      MPU_AP_RO_RO, 0, 0, 1, 0);

    mpu_enable_region(&app->region.data_region,
                      MPU_APP_DATA_RNG, 0, 0,
                      MPU_AP_RW_RW, 0, 0, 1, 0);

    /* map io region */
    io_regions = app->io_regions;
    while (io_regions) {
        attr = io_regions->memattr;
        if(attr == NORMAL_MEM){
            cacheattr = 1;
        }else if(attr == DEVICE_MEM){
            cacheattr = 0;
        }else{
            io_regions = io_regions->next;
            continue;
        }
        mpu_enable_region(&(io_regions->io_region), io_regions->region_id, 0, 0, MPU_AP_RW_RW, 0, 0, cacheattr, 0);
        io_regions = io_regions->next;
    }
}

uint32_t k_proc_ioregion_exist(uint32_t pid, uintptr_t addr, size_t len)
{
    mpu_region_t *pregion;
    app_t *app = &(g_app.procs[pid]);
    io_region_t *tmp_region = app->io_regions;
    if (NULL == tmp_region) {
        return false;
    }
    while (tmp_region) {
        pregion = &tmp_region->io_region;
        if ((addr <= pregion->start && (addr + len) > pregion->start) ||
            (addr >= pregion->start && addr < (pregion->start + pregion->size))) {
            return true;
        }

        tmp_region = tmp_region->next;
    }

    return false;
}

void k_proc_ioregion_reg(uint32_t pid, io_region_t *pregion)
{
    app_t *app = &(g_app.procs[pid]);
    io_region_t *tmp_region = app->io_regions;
    if (NULL == tmp_region) {
        app->io_regions = pregion;
    } else {
        pregion->next = tmp_region;
        app->io_regions = pregion;
    }
    return;
}

io_region_t *k_proc_ioregion_unreg(uint32_t pid, uintptr_t addr)
{

    app_t *app = &(g_app.procs[pid]);
    io_region_t *tmp_region = app->io_regions;

    if (NULL == tmp_region) {
        return NULL;
    }
    if (tmp_region->io_region.start == addr) {
        return tmp_region;
    }
    tmp_region = tmp_region->next;

    while (tmp_region) {
        if (tmp_region->io_region.start == addr) {
            return tmp_region;
        }

        tmp_region = tmp_region->next;
    }

    return NULL;
}

void k_debug_dump_mpu(uint32_t pid)
{
    app_t *app = &(g_app.procs[pid]);
    mpu_region_t *pregion;
    io_region_t *io_regions;
    printk("Cur pid:%d\r\n", pid);
    /* text */
    printk("Info: text region:\r\n");
    pregion = &(app->region.text_region);
    printk("ID\t addr\t    len\r\n");
    printk("%d\t0x%x\t 0x%x\r\n", MPU_APP_TEXT_RNG, pregion->start, pregion->size);
    /* data */
    printk("Info: data region:\r\n");
    pregion = &(app->region.data_region);
    printk("ID\t addr\t    len\r\n");
    printk("%d\t0x%x\t 0x%x\r\n", MPU_APP_DATA_RNG, pregion->start, pregion->size);
    /* io */
    printk("Info: IO region:\r\n");
    io_regions = app->io_regions;
    //pregion = &(io_regions->io_region);
    printk("ID\t addr\t    len\r\n");
    while (io_regions) {
        printk("%d\t0x%x\t 0x%x\r\n", io_regions->region_id, io_regions->io_region.start, io_regions->io_region.size);
        io_regions = io_regions->next;
    }

    return;
}

void k_proc_switch(void *new, void *old)
{
    ktask_t *new_task = (ktask_t *)new;
    ktask_t *old_task = (ktask_t *)old;

    if (new_task->pid == old_task->pid) {
        return;
    }

    if (new_task->pid == 0) {
        proc_disable_region(old_task->pid);
    } else {
        proc_enable_region(new_task->pid);
    }
    k_asid_set(new_task->pid);
}

