/**
 * @file fbdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "fbdev.h"
#if USE_FBDEV

#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "lvgl/lv_core/lv_vdb.h"

/*********************
 *      DEFINES
 *********************/
#ifndef FBDEV_PATH
#define FBDEV_PATH  "/dev/fb0"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static char *fbp = 0;
static long int screensize = 0;
static int fbfd = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void fbdev_init(void)
{
    // Open the file for reading and writing
    fbfd = open(FBDEV_PATH, O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return;
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        return;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        return;
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return;
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

}

/**
 * Flush a buffer to the marked area
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color_p an array of colors
 */
void fbdev_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    if(fbp == NULL) return;

    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > vinfo.xres - 1) return;
    if(y1 > vinfo.yres - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > vinfo.xres - 1 ? vinfo.xres - 1 : x2;
    int32_t act_y2 = y2 > vinfo.yres - 1 ? vinfo.yres - 1 : y2;

    long int location = 0;

    /*32 or 24 bit per pixel*/
    if(vinfo.bits_per_pixel == 32 || vinfo.bits_per_pixel == 24) {
        uint32_t *fbp32 = (uint32_t*)fbp;
        uint32_t x;
        uint32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp32[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }
    /*16 bit per pixel*/
    else if(vinfo.bits_per_pixel == 16) {
        uint16_t *fbp16 = (uint16_t*)fbp;
        uint32_t x;
        uint32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp16[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }
    /*8 bit per pixel*/
    else if(vinfo.bits_per_pixel == 8) {
        uint8_t *fbp8 = (uint8_t*)fbp;
        uint32_t x;
        uint32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp8[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    } else {
        /*Not supported bit per pixel*/
    }

    //May be some direct update command is required
    //ret = ioctl(state->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)rect));

    lv_flush_ready();
}

/**
 * Fill out the marked area with a color
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color fill color
 */
void fbdev_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    if(fbp == NULL) return;

    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > vinfo.xres - 1) return;
    if(y1 > vinfo.yres - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > vinfo.xres - 1 ? vinfo.xres - 1 : x2;
    int32_t act_y2 = y2 > vinfo.yres - 1 ? vinfo.yres - 1 : y2;

    uint32_t x;
    uint32_t y;

    long int location = 0;

    /*32 or 24 bit per pixel*/
    if(vinfo.bits_per_pixel == 32 || vinfo.bits_per_pixel == 24) {
        uint32_t *fbp32 = (uint32_t*)fbp;
        for(x = act_x1; x <= act_x2; x++) {
            for(y = act_y1; y <= act_y2; y++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp32[location] = color.full;
            }
        }
    }
    else if(vinfo.bits_per_pixel == 16) {
        uint16_t *fbp16 = (uint16_t*)fbp;
        for(x = act_x1; x <= act_x2; x++) {
            for(y = act_y1; y <= act_y2; y++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp16[location] = color.full;
            }
        }
    }
    else if(vinfo.bits_per_pixel == 8) {
        uint8_t *fbp8 = (uint8_t*)fbp;
        for(x = act_x1; x <= act_x2; x++) {
            for(y = act_y1; y <= act_y2; y++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp8[location] = color.full;
            }
        }
    } else {
        /*Not supported bit per pixel*/
    }

    //May be some direct update command is required
    //ret = ioctl(state->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)rect));

}

/**
 * Put a color map to the marked area
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color_p an array of colors
 */
void fbdev_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{

    if(fbp == NULL) return;

    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > vinfo.xres - 1) return;
    if(y1 > vinfo.yres - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > vinfo.xres - 1 ? vinfo.xres - 1 : x2;
    int32_t act_y2 = y2 > vinfo.yres - 1 ? vinfo.yres - 1 : y2;

    long int location = 0;

    /*32 or 24 bit per pixel*/
    if(vinfo.bits_per_pixel == 32 || vinfo.bits_per_pixel == 24) {
        uint32_t *fbp32 = (uint32_t*)fbp;
        uint32_t x;
        uint32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp32[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }
    /*16 bit per pixel*/
    else if(vinfo.bits_per_pixel == 16) {
        uint16_t *fbp16 = (uint16_t*)fbp;
        uint32_t x;
        uint32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp16[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }
    /*8 bit per pixel*/
    else if(vinfo.bits_per_pixel == 8) {
        uint8_t *fbp8 = (uint8_t*)fbp;
        uint32_t x;
        uint32_t y;
        for(y = act_y1; y <= act_y2; y++) {
            for(x = act_x1; x <= act_x2; x++) {
                location = (x+vinfo.xoffset) + (y+vinfo.yoffset) * vinfo.xres;
                fbp8[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    } else {
        /*Not supported bit per pixel*/
    }

    //May be some direct update command is required
    //ret = ioctl(state->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)rect));
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif



