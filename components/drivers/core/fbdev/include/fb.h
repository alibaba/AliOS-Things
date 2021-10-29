/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef FB_H
#define FB_H
/**
 *  @addtogroup aos_fbdev fbdev
 *  This is the brief description of the component.
 *
 *  This is the detailed description, and it's optional.
 *  @{
 */

#include <fb_define.h>
#include <ulog/ulog.h>
#include <aos/kernel.h>

#ifdef CONFIG_BACKLIGHT_MODE_ENABLE
#include "backlight.h"
#endif

#include "aos/vfs.h"

/* Here is Macro and struct definition*/
#define FB_MAX 3
#define FB_STR "FB"

#define FB_MAX_NUM  16
#define FB_NAME_LEN 16
#define fb_node_path       "/dev/fb"

#define FBDEV_STEP_DEBUG printf("[%s][%d]excute to here\n", __func__, __LINE__);

#define FBINFO_STATE_RUNNING    0
#define FBINFO_STATE_SUSPENDED    1

#define PAGE_SIZE 4096

#define fb_memcpy_fromfb memcpy
#define fb_memcpy_tofb memcpy

#ifndef unlikely
#if defined(__CC_ARM)
#define unlikely(x) __builtin_expect(!!(x), 0)
#elif defined(__ICCARM__)
#define unlikely(x) (x)
#elif defined(__GNUC__)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define unlikely(x) (x)
#endif
#endif

/* FBINFO_* = fb_info.flags bit flags */
#define FBINFO_DEFAULT        0
#define FBINFO_HWACCEL_DISABLED    0x0002

#define FBINFO_HWACCEL_NONE        0x0000
#define FBINFO_HWACCEL_COPYAREA        0x0100 /* required */
#define FBINFO_HWACCEL_FILLRECT        0x0200 /* required */
#define FBINFO_HWACCEL_IMAGEBLIT    0x0400 /* required */
#define FBINFO_HWACCEL_ROTATE        0x0800 /* optional */
#define FBINFO_HWACCEL_XPAN        0x1000 /* optional */
#define FBINFO_HWACCEL_YPAN        0x2000 /* optional */
#define FBINFO_HWACCEL_YWRAP        0x4000 /* optional */

struct _fb_ops_t;
#ifdef CONFIG_BACKLIGHT_MODE_ENABLE
struct _bl_device_t;
#endif
struct _fb_info_t;

typedef struct _fb_videomode_t {
    const char *name;    /* optional */
    uint32_t refresh;        /* optional */
    uint32_t xres;
    uint32_t yres;
    uint32_t pixclock;
    uint32_t left_margin;
    uint32_t right_margin;
    uint32_t upper_margin;
    uint32_t lower_margin;
    uint32_t hsync_len;
    uint32_t vsync_len;
    uint32_t sync;
    uint32_t vmode;
    uint32_t flag;
} fb_videomode_t;

struct fb_pixmap {
    uint8_t  *addr;        /* pointer to memory            */
    uint32_t size;        /* size of buffer in bytes        */
    uint32_t offset;        /* current offset to buffer        */
    uint32_t buf_align;        /* byte alignment of each bitmap    */
    uint32_t scan_align;        /* alignment per scanline        */
    uint32_t access_align;    /* alignment per read/write (bits)    */
    uint32_t flags;        /* see FB_PIXMAP_*            */
    uint32_t blit_x;             /* supported bit block dimensions (1-32)*/
    uint32_t blit_y;             /* Format: blit_x = 1 << (width - 1)    */
                            /*         blit_y = 1 << (height - 1)   */
                            /* if 0, will be set to 0xffffffff (all)*/
    /* access methods */
    void (*writeio)(struct _fb_info_t *info, void *dst, void *src, unsigned int size);
    void (*readio) (struct _fb_info_t *info, void *dst, void *src, unsigned int size);
};

typedef struct _fb_info_t {
    char path[FB_NAME_LEN];
    int ref;
    int node;
    int flags;

    aos_mutex_t lock;            /* Lock for open/release/ioctl funcs */
    aos_mutex_t mm_lock;        /* Lock for fb_mmap and smem_* fields */
    fb_var_screeninfo_t var;    /* Current var */
    fb_fix_screeninfo_t fix;    /* Current fix */
    struct fb_pixmap pixmap;    /* Image hardware mapper */
    struct fb_pixmap sprite;    /* Cursor hardware mapper */
    // struct fb_cmap cmap;        /* Current cmap */
    uint8_t *prefb;                /* Pre-frame buffer from user space*/
    void *par;                    /* From here on everything is device dependent */

#ifdef CONFIG_BACKLIGHT_MODE_ENABLE
    struct _bl_device_t *bl_dev;        /*backlight device*/

    uint8_t bl_curve[FB_BACKLIGHT_LEVELS];
#endif

    uint32_t state;            /* Hardware state i.e suspend */
    const struct _fb_ops_t *fbops;

    union {
        uint8_t *screen_base;    /* Virtual address */
        uint8_t *screen_buffer;
    };
    unsigned long screen_size;    /* Amount of ioremapped VRAM or 0 */
} fb_info_t;

typedef struct _fb_ops_t {
    /* open/release and usage marking */
    int (*fb_open)(fb_info_t *info, int user);
    int (*fb_release)(fb_info_t *info, int user);

    /* For framebuffers with strange non linear layouts or that do not
     * work with normal memory mapped access
     */
    ssize_t (*fb_read)(fb_info_t *info, char *buf,
               size_t count);
    ssize_t (*fb_write)(fb_info_t *info, const char *buf,
                size_t count);

    /* blank display */
    int (*fb_blank)(int blank, fb_info_t *info);

    /* pan display */
    int (*fb_pan_display)(fb_var_screeninfo_t *var, fb_info_t *info);

    /* set the video mode according to info->var */
    int (*fb_set_par)(struct fb_info *info);

    /* Rotates the display */
    void (*fb_rotate)(fb_info_t *info, int angle);

    /* wait for blit idle, optional */
    int (*fb_sync)(fb_info_t *info);

    /* perform fb specific ioctl (optional) */
    int (*fb_ioctl)(fb_info_t *info, unsigned int cmd,
            unsigned long arg);

    /* perform fb specific mmap */
    void *(*fb_mmap)(fb_info_t *info, size_t size);

#ifdef CONFIG_HW_ACCELERATION
    /* Draws a rectangle */
    void (*fb_fillrect) (fb_info_t *info, const fb_fillrect_t *rect);
    /* Copy data from area to another */
    void (*fb_copyarea) (fb_info_t *info, const fb_copyarea_t *region);
    /* Draws a image to the display */
    void (*fb_imageblit) (fb_info_t *info, const fb_image_t *image);
#endif

    /* teardown any resources to do with this framebuffer */
    void (*fb_destroy)(fb_info_t *info);
} fb_ops_t;

/* Here is API and callback definition*/

static inline void lock_fb_info(fb_info_t *info)
{
    if (aos_mutex_lock(&info->lock, AOS_WAIT_FOREVER) != 0) {
        LOGE(FB_STR, "Failed to lock mutex (%s).", __func__);
        return;
    }
}

static inline void unlock_fb_info(fb_info_t *info)
{
    aos_mutex_unlock(&info->lock);
}

static inline inode_t *file_inode(const file_t *file)
{
    if (!file)
        return NULL;

    return file->node;
}

int fb_init(void);
fb_info_t *framebuffer_alloc(size_t size);
int register_framebuffer(fb_info_t *info);
void unregister_framebuffer(fb_info_t *info);
void framebuffer_release(fb_info_t *info);
/** @} */
#endif

