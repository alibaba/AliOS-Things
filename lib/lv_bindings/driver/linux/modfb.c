
/*********************
 *      INCLUDES
 *********************/

#include "../../lib/lv_bindings/lvgl/lvgl.h"
#include "../include/common.h"
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <signal.h>

/*********************
 *      DEFINES
 *********************/

#ifndef FBDEV_PATH
#define FBDEV_PATH  "/dev/fb0"
#endif

#define LV_TICK_RATE 20

/**********************
 *  FORWARD DECLARATIONS
 **********************/

bool fbdev_init(void);
void fbdev_deinit(void);
void fbdev_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/

static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static char * fbp = 0;
static long int screensize = 0;
static int fbfd = -1;
static pthread_t tid;
static pthread_t mp_thread;
static bool auto_refresh = true;

/**********************
 *  MODULE DEFINITION
 **********************/

STATIC inline bool fbdev_active()
{
    return fbfd >= 0;
}

STATIC mp_obj_t mp_lv_task_handler(mp_obj_t arg)
{  
    lv_task_handler();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_lv_task_handler_obj, mp_lv_task_handler);

STATIC void* tick_thread(void * data)
{
    (void)data;

    while(fbdev_active()) {
        usleep(LV_TICK_RATE * 1000);   /*Sleep for 1 millisecond*/
        lv_tick_inc(LV_TICK_RATE); /*Tell LittelvGL that LV_TICK_RATE milliseconds were elapsed*/
        mp_sched_schedule((mp_obj_t)&mp_lv_task_handler_obj, mp_const_none);
        pthread_kill(mp_thread, SIGUSR1); // interrupt REPL blocking input. See handle_sigusr1
    }

    return NULL;
}

static void handle_sigusr1(int signo)
{
    // Let the signal pass. blocking function would return E_INTR.
    // This would cause a call to "mp_handle_pending" even when 
    // waiting for user input.
    // See https://github.com/micropython/micropython/pull/5723
}

STATIC mp_obj_t mp_init_fb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_auto_refresh };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_auto_refresh, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool init_succeeded = fbdev_init();
    if (!init_succeeded) return mp_const_false;

    int err = 0;
    auto_refresh = args[ARG_auto_refresh].u_bool;
    if (auto_refresh) {
        err = pthread_create(&tid, NULL, &tick_thread, NULL);

        mp_thread = pthread_self();
        struct sigaction sa;
        sa.sa_handler = handle_sigusr1;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGUSR1, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }
    }
    
    return err == 0? mp_const_true: mp_const_false;
}

STATIC mp_obj_t mp_deinit_fb()
{
    fbdev_deinit();
    if (auto_refresh) {
        pthread_join(tid, NULL);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_init_fb_obj, 0, mp_init_fb);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_deinit_fb_obj, mp_deinit_fb);

DEFINE_PTR_OBJ(fbdev_flush);

STATIC const mp_rom_map_elem_t fb_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_fb) },
        { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_init_fb_obj) },
        { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_deinit_fb_obj) },
        { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&PTR_OBJ(fbdev_flush))},
};
         

STATIC MP_DEFINE_CONST_DICT (
    mp_module_fb_globals,
    fb_globals_table
);

const mp_obj_module_t mp_module_fb = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_fb_globals
};

/**********************
 *   IMPLEMENTATION
 **********************/

bool fbdev_init(void)
{
    // Open the file for reading and writing
    fbfd = open(FBDEV_PATH, O_RDWR);
    if(fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return false;
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        return false;
    }

    // Get variable screen information
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        return false;
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize =  finfo.line_length * vinfo.yres;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if((intptr_t)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return false;
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
    return true;

}

void fbdev_deinit(void)
{
    close(fbfd);
    fbfd = -1;
}

/**
 * Flush a buffer to the marked area
 * @param area->x1 left coordinate
 * @param area->y1 top coordinate
 * @param area->x2 right coordinate
 * @param area->y2 bottom coordinate
 * @param color_p an array of colors
 */
void fbdev_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    if(fbp == NULL ||
            area->x2 < 0 ||
            area->y2 < 0 ||
            area->x1 > (int32_t)vinfo.xres - 1 ||
            area->y1 > (int32_t)vinfo.yres - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > (int32_t)vinfo.xres - 1 ? (int32_t)vinfo.xres - 1 : area->x2;
    int32_t act_y2 = area->y2 > (int32_t)vinfo.yres - 1 ? (int32_t)vinfo.yres - 1 : area->y2;

    long int location = 0;
    long int byte_location = 0;
    unsigned char bit_location = 0;

    /*32 or 24 bit per pixel*/
    if(vinfo.bits_per_pixel == 32 || vinfo.bits_per_pixel == 24) {
        uint32_t * fbp32 = (uint32_t *)fbp;
        int32_t x;
        int32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 4;
                fbp32[location] = color_p->full;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }
    /*16 bit per pixel*/
    else if(vinfo.bits_per_pixel == 16) {
        uint16_t * fbp16 = (uint16_t *)fbp;
        int32_t x;
        int32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 2;
                fbp16[location] = color_p->full;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }
    /*8 bit per pixel*/
    else if(vinfo.bits_per_pixel == 8) {
        uint8_t * fbp8 = (uint8_t *)fbp;
        int32_t x;
        int32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length;
                fbp8[location] = color_p->full;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }
    /*1 bit per pixel*/
    else if(vinfo.bits_per_pixel == 1) {
        uint8_t * fbp8 = (uint8_t *)fbp;
        int32_t x;
        int32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x + vinfo.xoffset) + (y + vinfo.yoffset) * vinfo.xres;
                byte_location = location / 8; /* find the byte we need to change */
                bit_location = location % 8; /* inside the byte found, find the bit we need to change */
                fbp8[byte_location] &= ~(((uint8_t)(1)) << bit_location);
                fbp8[byte_location] |= ((uint8_t)(color_p->full)) << bit_location;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    } else {
        /*Not supported bit per pixel*/
    }

    //May be some direct update command is required
    //ret = ioctl(state->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)rect));

    lv_disp_flush_ready(disp_drv);
}

