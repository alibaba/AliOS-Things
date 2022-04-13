/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <errno.h>
#include <aos/vfs.h>
#include <string.h>
#include <fb.h>
#include <devicevfs/devicevfs.h>
#include <k_api.h>
// #include <k_shmm.h>

#define FBPIXMAPSIZE    (1024 * 8)

fb_info_t *registered_fb[FB_MAX];
static int num_registered_fb = 0;
static aos_mutex_t fb_mutex;

static int fb_open(inode_t *inode, file_t *file);
static int fb_close(file_t *file);
static ssize_t fb_read(file_t *file, void *buf, size_t len);
static ssize_t fb_write(file_t *file, const void *buf, size_t len);
static int fb_ioctl(file_t *file, int cmd, unsigned long arg);

#ifdef CONFIG_AOS_MMAP_ENABLE
static void *fb_mmap(file_t *file, size_t size);
#endif

file_ops_t fb_fops = {
    .open  = fb_open,
    .close = fb_close, // release
    .read  = fb_read,
    .write = fb_write,
    .ioctl = fb_ioctl,
#ifdef CONFIG_AOS_MMAP_ENABLE
    .mmap = fb_mmap,
#endif
};

static fb_info_t *get_fb_info(char *path)
{
    int i = 0;
    fb_info_t *fb_info;

    if (path == NULL) {
        printf("[%s]path is null\n", __func__);
        return NULL;
    }

    for (i = 0; i < FB_MAX; i++) {
        if (strlen(path) != strlen(registered_fb[i]->path))
            continue;

        if (strncmp(registered_fb[i]->path, path, strlen(path)) == 0)
            break;
    }

    if (i >= FB_MAX)
        return NULL;

    if (aos_mutex_lock(&fb_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(FB_STR, "Failed to lock mutex (%s).", __func__);
        return NULL;
    }
    fb_info = registered_fb[i];

    aos_mutex_unlock(&fb_mutex);

    return fb_info;
}

static int fb_open(inode_t *inode, file_t *file)
{
    fb_info_t *info;
    int ret = 0;

    info = get_fb_info(inode->i_name);
    if (!info)
        return -ENODEV;

    lock_fb_info(info);
    if (info->fbops->fb_open)
        ret = info->fbops->fb_open(info, 1);

    unlock_fb_info(info);

    return ret;
}

static fb_info_t *file_fb_info(file_t *file)
{
    inode_t *inode = file_inode(file);
    fb_info_t *info;

    if (!inode)
        return NULL;

    info = get_fb_info(inode->i_name);
    if (!info)
        return NULL;

    return info;
}

static ssize_t fb_read(file_t *file, void *buf, size_t len)
{
    fb_info_t *info = file_fb_info(file);
    unsigned long total_size;

    if (!info || !info->screen_base)
        return -ENODEV;

    if (info->state != FBINFO_STATE_RUNNING)
        return -EPERM;

    if (info->fbops->fb_read)
        return info->fbops->fb_read(info, buf, len);

    total_size = info->screen_size;

    if (total_size == 0)
        total_size = info->fix.smem_len;

    if (len >= total_size)
        len = total_size;

    memcpy(buf, info->screen_base, len);

    return len;
}

static ssize_t fb_write(file_t *file, const void *buf, size_t len)
{
    fb_info_t *info = file_fb_info(file);

    if (!info || !info->screen_base)
        return -ENODEV;

    if (info->fbops->fb_write)
        return info->fbops->fb_write(info, buf, len);

    if (!info->screen_base)
        return -1;

    memcpy(info->screen_base, buf, len);

    return len;
}

static int fb_set_var(fb_info_t *info, fb_var_screeninfo_t *var)
{
    int32_t ret = 0;
    memcpy(&info->var, var, sizeof(fb_var_screeninfo_t));

    if (info->fbops->fb_set_par)
        ret = info->fbops->fb_set_par(info);

    return ret;
}

static int fb_pan_display(fb_info_t *info, fb_var_screeninfo_t *var)
{
    fb_fix_screeninfo_t *fix = &info->fix;
    uint32_t yres = info->var.yres;
    int err = 0;

    if (!var)
        return -EINVAL;

    if (var->xoffset > 0 && (!fix->xpanstep ||
                 (var->xoffset % fix->xpanstep)))
        err = -EINVAL;

    if (err || !info->fbops->fb_pan_display ||
        var->yoffset > info->var.yres_virtual - yres ||
        var->xoffset > info->var.xres_virtual - info->var.xres)
        return -EINVAL;

    err = info->fbops->fb_pan_display(var, info);
    if (err != 0)
        return err;
    info->var.xoffset = var->xoffset;
    info->var.yoffset = var->yoffset;

    return 0;
}

static int fb_blank(fb_info_t *info, int blank)
{
    int ret = -EINVAL;

    if (info->fbops->fb_blank)
        ret = info->fbops->fb_blank(blank, info);

    return ret;
}

static void printf_string(int cmd)
{
    switch (cmd) {
        case FBIOGET_VSCREENINFO:
            printf("ioctl cmd : FBIOGET_VSCREENINFO\n");
            break;
        case FBIOPUT_VSCREENINFO:
            // printf("ioctl cmd : FBIOPUT_VSCREENINFO\n");
            break;
        case FBIOGET_FSCREENINFO:
            printf("ioctl cmd : FBIOGET_FSCREENINFO\n");
            break;
        case FBIOPAN_DISPLAY:
            printf("ioctl cmd : FBIOPAN_DISPLAY\n");
            break;
        case FBIOPUT_PREFB_ADDR:
            printf("ioctl cmd : FBIOPUT_PREFB_ADDR\n");
            break;
        // case FBIOPUT_PREFB_PARAMS:
        //     // printf("ioctl cmd: FBIOPUT_PREFB_PARAMS\n");
        //     break;
        case FBIO_WAITFORVSYNC:
            printf("ioctl cmd: FBIO_WAITFORVSYNC\n");
            break;
        case FBIOBLANK:
            printf("ioctl cmd : FBIOBLANK\n");
            break;
        case FBIOPUT_BRIGHTNESS:
            printf("ioctl cmd : FBIOPUT_BRIGHTNESS\n");
            break;
        case FBIOGET_BRIGHTNESS:
            printf("ioctl cmd : FBIOGET_BRIGHTNESS\n");
            break;
#ifdef CONFIG_BACKLIGHT_MODE_ENABLE
        case FBIOENABLE_BACKLIGHT:
            printf("ioctl cmd : FBIOENABLE_BACKLIGHT\n");
            break;
        case FBIODISABLE_BACKLIGHT:
            printf("ioctl cmd : FBIODISABLE_BACKLIGHT\n");
            break;
#endif
        default:
            // printf("ioctl cmd: unknown command\n");
            break;
    }
    return;
}

static int fb_ioctl(file_t *file, int cmd, unsigned long arg)
{
    int ret = 0;
    fb_info_t *info = file_fb_info(file);
    fb_var_screeninfo_t var;

    printf_string(cmd);

    switch (cmd) {
        case FBIOGET_VSCREENINFO:
            lock_fb_info(info);
            memcpy((void *)arg, &info->var, sizeof(fb_var_screeninfo_t));
            unlock_fb_info(info);
            break;
        case FBIOPUT_VSCREENINFO:
            lock_fb_info(info);
            ret = fb_set_var(info, (fb_var_screeninfo_t *)arg);
            unlock_fb_info(info);
            if (ret < 0)
                ret = -EFAULT;
            break;
        case FBIOGET_FSCREENINFO:
            lock_fb_info(info);
            memcpy((void *)arg, &info->fix, sizeof(fb_fix_screeninfo_t));
            unlock_fb_info(info);
            break;
        case FBIOGET_FRRAMEBUFFER:
            info->screen_buffer = malloc((int32_t)arg);
            break;
        case FBIOPAN_DISPLAY:
            lock_fb_info(info);
            if (arg)
                ret = fb_pan_display(info, (fb_var_screeninfo_t *)arg);

            unlock_fb_info(info);
            break;
        case FBIOBLANK:
            lock_fb_info(info);
            ret = fb_blank(info, arg);
            unlock_fb_info(info);
            break;
#ifdef CONFIG_BACKLIGHT_MODE_ENABLE
        case FBIOPUT_BRIGHTNESS:
            if (info->bl_dev)
                backlight_device_set_brightness(info->bl_dev, *(int32_t *)arg);

            break;
        case FBIOGET_BRIGHTNESS:
            if (info->bl_dev) {
                int32_t brightness;
                brightness = backlight_device_get_brightness(info->bl_dev);
                memcpy((void *)arg, &brightness, sizeof(brightness));
            }
            break;
        case FBIOENABLE_BACKLIGHT:
            if (info->bl_dev)
                backlight_enable(info->bl_dev);
            break;
        case FBIODISABLE_BACKLIGHT:
            if (info->bl_dev)
                backlight_disable(info->bl_dev);
            break;
#endif
#ifdef CONFIG_HW_ACCELERATION
        case FBIO_ACCEL_FILLRECT:
            lock_fb_info(info);
            if (info->fbops->fb_fillrect) {
                info->fbops->fb_fillrect(info, (const fb_fillrect_t *)arg);
                ret = 0;
            } else {
                ret = -EIO;
            }
            unlock_fb_info(info);
            break;
        case FBIO_ACCEL_IMAGEBLIT:
            lock_fb_info(info);
            if (info->fbops->fb_imageblit) {
                info->fbops->fb_imageblit(info, (const fb_image_t *)arg);
                ret = 0;
            } else {
                ret = -EIO;
            }
            unlock_fb_info(info);
            break;
        case FBIO_ACCEL_COPYAREA:
            lock_fb_info(info);
            if (info->fbops->fb_copyarea) {
                info->fbops->fb_copyarea(info, (const fb_copyarea_t *)arg);
                ret = 0;
            } else {
                ret = -EIO;
            }
            unlock_fb_info(info);
            break;
#endif
        default:
            lock_fb_info(info);
            if (info->fbops->fb_ioctl)
                ret = info->fbops->fb_ioctl(info, cmd, arg);
            else
                ret = -EIO;
            unlock_fb_info(info);
            break;
    }

    return ret;
}

#ifdef CONFIG_AOS_MMAP_ENABLE
static void *fb_mmap(file_t *file, size_t size)
{
    fb_info_t *info = file_fb_info(file);
    void *(*fb_mmap_fn)(fb_info_t *info, size_t size);
    uint64_t *start;
    uint32_t len;

    if (!info)
        return NULL;

    if (aos_mutex_lock(&info->mm_lock, AOS_WAIT_FOREVER) != 0) {
        LOGE(FB_STR, "Failed to lock mutex (%s).", __func__);
        return NULL;
    }

    fb_mmap_fn = info->fbops->fb_mmap;
    if (fb_mmap_fn) {
        void *res;
          // The framebuffer needs to be accessed decrypted, be sure
          // SME protection is removed ahead of the call
        res = fb_mmap_fn(info, size);
        aos_mutex_unlock(&info->mm_lock);
        return res;
    }

    start = (uint64_t *)&info->fix.smem_start; // pyshical address
    len = info->fix.smem_len;

    if (start == NULL || len == 0) {
        info->fix.smem_start = (uint64_t)k_shmm_map_area(NULL, size);
        if (info->screen_base == NULL) {
            aos_mutex_unlock(&info->mm_lock);
            return NULL;
        }
        info->screen_base = (uint8_t *)info->fix.smem_start;
        info->fix.smem_len = size;
    }

    aos_mutex_unlock(&info->mm_lock);

    return (void *)info->screen_base;
}
#endif

static void put_fb_info(fb_info_t *info)
{
    if (info->fbops->fb_destroy)
        info->fbops->fb_destroy(info);
}

static int fb_close(file_t *file)
{
    fb_info_t *info = file_fb_info(file);

    if (!info)
        return -ENODEV;

    lock_fb_info(info);
    if (info->fbops->fb_release)
        info->fbops->fb_release(info, 1);
    unlock_fb_info(info);
    put_fb_info(info);
    return 0;
}

static int do_register_framebuffer(fb_info_t *fb_info)
{
    int i = 0, ret = 0;

    if (num_registered_fb == FB_MAX)
        return -ENXIO;

    /*set ENDIAN mode, TODO*/

    num_registered_fb++;
    for (i = 0; i < FB_MAX; i++)
        if (!registered_fb[i])
            break;
    fb_info->node = i;

    aos_mutex_new(&fb_info->lock);
    aos_mutex_new(&fb_info->mm_lock);

    /*register framebuffer device to vfs*/
    ret = snprintf(fb_info->path, FB_NAME_LEN, "%s%d", fb_node_path, fb_info->node);
    if (ret < 0) {
        LOGE(FB_STR, "copy fb path fail (%s)\n", __func__);
        return -1;
    }

    ret = aos_register_driver(fb_info->path, &fb_fops, NULL);
    if (unlikely(ret))
        return -1;

  if (i < FB_MAX)
    registered_fb[i] = fb_info;
  else
    LOGE(FB_STR, "out of bounds (%s)\n", __func__);

    return ret;
}

static void do_unregister_framebuffer(fb_info_t *fb_info)
{
    registered_fb[fb_info->node] = NULL;
    num_registered_fb--;

    /* this may free fb info */
    put_fb_info(fb_info);
}

int register_framebuffer(fb_info_t *info)
{
    int ret;

    if (aos_mutex_lock(&fb_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(FB_STR, "Failed to lock mutex (%s).", __func__);
        return -1;
    }

    ret = do_register_framebuffer(info);
    aos_mutex_unlock(&fb_mutex);

    return ret;
}

void unregister_framebuffer(fb_info_t *info)
{
    if (aos_mutex_lock(&fb_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(FB_STR, "Failed to lock mutex (%s).", __func__);
        return;
    }
    do_unregister_framebuffer(info);
    aos_mutex_unlock(&fb_mutex);
}

int fb_init(void)
{
    int i;
    int ret = 0;

    ret = aos_mutex_new((aos_mutex_t *)&fb_mutex);
    if (unlikely(ret)) {
        printf("create fb mutex failed\n");
        return -1;
    }

    ret = aos_register_driver(fb_node_path, &fb_fops, NULL);
    if (unlikely(ret))
        return -1;

    for (i = 0; i < FB_MAX; i++)
        registered_fb[i] = NULL;

    return ret;
}

fb_info_t *framebuffer_alloc(size_t size)
{
    int fb_info_size = sizeof(fb_info_t);
    fb_info_t *info;
    char *p;

    if (size)
        fb_info_size += PADDING;

    p = aos_malloc(fb_info_size + size);

    if (!p)
        return NULL;

    info = (fb_info_t *) p;

    if (size)
        info->par = p + fb_info_size;

    return info;
}

/**
 * framebuffer_release - marks the structure available for freeing
 *
 * @info: frame buffer info structure
 *
 */
void framebuffer_release(fb_info_t *info)
{
    if (!info)
        return;

    aos_free(info);
}

LEVEL0_DRIVER_ENTRY(fb_init)
