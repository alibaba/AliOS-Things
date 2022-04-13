#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "realview.h"
#include "drv_clcd.h"
#include "fb.h"

#define CLCD_WIDTH  (BSP_LCD_WIDTH)
#define CLCD_HEIGHT (BSP_LCD_HEIGHT)

#define CLCD_DEVICE(dev)    (struct drv_clcd_device*)(dev)

#define PL111_CR_EN         0x001
#define PL111_CR_PWR        0x800
#define PL111_IOBASE        0x10020000
#define PL111_PALBASE       (PL111_IOBASE + 0x200)
#define PL111_CLR           (PL111_IOBASE + 0x028)
#define PL111_VC_SET        (PL111_IOBASE + 0x1c)

#define RGB_888_FORMAT
//#define RGB_565_FORMAT

typedef struct _PL111MMIO
{
    uint32_t      volatile tim0;      //0
    uint32_t      volatile tim1;      //4
    uint32_t      volatile tim2;      //8
    uint32_t      volatile tim3;      //c
    uint32_t      volatile upbase;    //10
    uint32_t      volatile f;         //14
    uint32_t      volatile control;   //18
    uint32_t      volatile g;         //1c LCDMSC
    uint32_t      volatile ris;       //20 LCDRIS
    uint32_t      volatile mask;      //24 LCDMIS
    uint32_t      volatile icr;       //28 LCDICR
} PL111MMIO;

struct drv_clcd_device
{
    uint32_t width;
    uint32_t height;

    uint8_t *fb;
};
struct drv_clcd_device _lcd;


static int pl111_open(fb_info_t *info, int user);
static int pl111_release(fb_info_t *info, int user);
static ssize_t pl111_read(fb_info_t *info, char *buf, size_t count);
static ssize_t pl111_write(fb_info_t *info, const char *buf, size_t count);
static int pl111_blank(int blank, fb_info_t *info);
static int pl111_pan_display(fb_var_screeninfo_t *var, fb_info_t *info);
static pl111_ioctl(fb_info_t *info, unsigned int cmd, unsigned long arg);
static int pl111_mmap(fb_info_t *info, size_t size);

fb_info_t *g_info = NULL;
PL111MMIO   *plio;
static aos_sem_t vsync_sem;
static aos_timer_t vsync_timer;
static bool update_flag = false;
static uint8_t *fb_addr = NULL;

static const fb_ops_t pl111_ops = {
	.fb_open	= pl111_open,
	.fb_release	= pl111_release,
	.fb_read        = pl111_read,
	.fb_write       = pl111_write,
	.fb_pan_display	= pl111_pan_display,
	.fb_mmap	= pl111_mmap,
    .fb_sync    = NULL,
	.fb_blank	= pl111_blank,
	.fb_ioctl	= pl111_ioctl,
};


static const fb_fix_screeninfo_t pl111_fix = {
    .id         = PL111_DEVICE_NAME,
	.xpanstep	= 0,
	.ypanstep	= 0,
	.ywrapstep	= 0,
	.line_length	= 0,
	.mmio_start	= 0,	/* Not implemented for char. special, so */
	.mmio_len	= 0	/* these are set to 0 */
};

static void update_screen_addr(uint8_t *addr)
{
    plio->upbase = (uint32_t)addr;
}

static void update_disp_flag(bool flag)
{
    update_flag = flag;
}

static void send_vsync_irq(void *timer, void *para)
{
    if (update_flag) {
        update_screen_addr(fb_addr);
        //printf("[%s]fb_addr: %p\n", __func__, fb_addr);
        aos_sem_signal((aos_sem_t *)&vsync_sem);
        update_flag = false;
        //printf("send vsync irq event\n");
    }
}

static int pl111_open(fb_info_t *info, int user)
{
    return 0;
}

static int pl111_release(fb_info_t *info, int user)
{
    return 0;
}

static ssize_t pl111_read(fb_info_t *info, char *buf, size_t count)
{
    return 0;
}

static ssize_t pl111_write(fb_info_t *info, const char *buf, size_t count)
{
    return 0;
}

static int pl111_blank(int blank, fb_info_t *info)
{
    return 0;
}

static int pl111_pan_display(fb_var_screeninfo_t *var, fb_info_t *info)
{
    int ret = 0;

	fb_fix_screeninfo_t *fix = &info->fix;

    //printf("[%s]info->screen_base: %p\n", __func__, info->screen_base);

    fb_addr = (info->screen_base + var->yoffset * fix->line_length
		    + var->xoffset * info->var.bits_per_pixel / 8);
    k_dcache_clean_all();

    /*update address*/
    update_disp_flag(1);
    //printf("[%s]addr: %p\n", __func__, fb_addr);

    return 0;
}

static int pl111_mmap(fb_info_t *info, size_t size)
{
    if (!info)
        return -1;

    info->screen_base = (uint8_t *)k_shmm_map_area(NULL, size);

    if (info->screen_base == NULL) {
        return -ENOMEM;
    }
    info->fix.smem_start = info->screen_base; //pyshical address
    info->fix.smem_len = size;

    plio->upbase = info->screen_base;

    return info->screen_base;
}

static pl111_ioctl(fb_info_t *info, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case FBIO_WAITFORVSYNC:
            ret = aos_sem_wait((aos_sem_t *)&vsync_sem, AOS_WAIT_FOREVER);
            if (unlikely(ret)) {
                printf("input semaphore wait err\n");
            }
            break;
    }
    return ret;
}

void clcd_interrupt()
{
    uint32_t status;
    printf("enter clcd interrupt\n");

    if (plio->ris & 0x8) {
        printf("enter VC interrupt \n");
        plio->icr & 0x8;
    }
}

int drv_clcd_hw_init(void)
{
    int ret;
    fb_info_t *info = NULL;

    if (aos_sem_new(&vsync_sem, 0) != 0) {
        printf("[%s]create vsync_sem fail\n", __func__);
        return -1;
    }

    if (aos_timer_new(&vsync_timer,
        send_vsync_irq,
        NULL,
        4, //4ms for qemu. 16ms, 60fps
        1) != 0) {
        printf("[%s]start vsync timer fail\n", __func__);
    }

    /* memset _lcd to zero */
    memset(&_lcd, 0x0, sizeof(_lcd));

    _lcd.width  = CLCD_WIDTH;
    _lcd.height = CLCD_HEIGHT;

    plio = (PL111MMIO*)PL111_IOBASE;

    plio->tim0 = 0x3F1F3C00 | ((CLCD_WIDTH/16 - 1) << 2);
    plio->tim1 = 0x080B6000 | (CLCD_HEIGHT - 1);

    plio->upbase = (uint32_t)_lcd.fb;
#ifdef RGB_565_FORMAT
    /* 16-bit RGB 565 color */
    plio->control = 0x0821 | (0x6 << 1); //start of veritical sync
#endif
#ifdef RGB_888_FORMAT
    /* 24-bit RGB 888 color */
    plio->control = 0x0821 | (0x5 << 1); //little endian
    //plio->control = 0x0921 | (0x5 << 1); //BGR
    //plio->control = 0x0a21 | (0x5 << 1); //big endian
#endif
    printf("plio->control: 0x%x\n", plio->control);
    //plio->icr &= ~(0x8);
    //plio->mask |= 0x8;

    /* disable interrupts before requesting the irq */
    plio->g |= ((0) << (2));

    info = framebuffer_alloc(0);

    /*set var screeninfo*/
    memset(&info->var, 0, sizeof(fb_var_screeninfo_t));
    info->var.xres = _lcd.width;
    info->var.yres = _lcd.height;
    info->var.xres_virtual = _lcd.width;
    info->var.yres_virtual = _lcd.height*2;
    info->var.width = _lcd.width;
    info->var.height = _lcd.height;
    info->var.rotate = FB_ROTATE_UD; //rotate 0 degrees wiseclock
#ifdef RGB_888_FORMAT
    info->var.bits_per_pixel = 4*8;
#endif
#ifdef RGB_565_FORMAT
    info->var.bits_per_pixel = 2*8;
#endif
    info->screen_size = info->var.xres*info->var.yres*info->var.bits_per_pixel/8;
    info->screen_base = _lcd.fb;
    /*set fb ops function*/
    info->fbops = &pl111_ops;

    /*set fix screeninfo*/
    info->fix = pl111_fix;
    info->fix.line_length = info->var.xres*info->var.bits_per_pixel/8;

    ret = register_framebuffer(info);
    if (ret < 0) {
        printf("register framebuffer fail\n");
    }

    g_info = info;

    (void)irq_register_isr(IRQ_VEXPRESS_A9_CLCD, clcd_interrupt);
    gic_enable_interrupt(IRQ_VEXPRESS_A9_CLCD);
    plio->g |= 0x000f;
    plio->ris |= 0x000f;
    printf("plio->g: 0x%x\n", plio->g);

    printf("lcd init done\n");
    return ret;
}

int drv_clcd_hw_uninit(void)
{
    unregister_framebuffer(g_info);
    return 0;
}