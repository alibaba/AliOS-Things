/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef COMP_FB_H
#define COMP_FB_H

#include <stdint.h>
#include <stddef.h>

#define FB_BACKLIGHT_LEVELS    128
#define FB_BACKLIGHT_MAX    0xFF

#ifdef CONFIG_64BIT
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* CONFIG_64BIT */

#define FB_SYNC    0x0001
#define FB_ASYNC   0x0010

// show normally
#define FB_ROTATE_UR    0
// rotate 90 degrees clockwise
#define FB_ROTATE_CW    1
// upside down
#define FB_ROTATE_UD    2
// rotate 90 degrees counterclockwise
#define FB_ROTATE_CCW    3


#define BYTES_PER_LONG (BITS_PER_LONG / 8)
#define PADDING (BYTES_PER_LONG - (sizeof(fb_info_t) % BYTES_PER_LONG))

#define FBIOGET_VSCREENINFO    0x4600
#define FBIOPUT_VSCREENINFO    0x4601
#define FBIOGET_FSCREENINFO    0x4602
#define FBIOGET_FRRAMEBUFFER   0x4603

#define FBIOGETCMAP        0x4604
#define FBIOPUTCMAP        0x4605
#define FBIOPAN_DISPLAY        0x4606
#define FBIO_WAITFORVSYNC    0x4607
#define FBIOPUT_PREFB_ADDR    0x4608

#define FBIOBLANK        0x4611

/*brightness ioctl*/
#define FBIOPUT_BRIGHTNESS 0x4612
#define FBIOGET_BRIGHTNESS 0x4613
#define FBIOENABLE_BACKLIGHT 0x4614
#define FBIODISABLE_BACKLIGHT 0x4615

typedef struct _fb_fix_screeninfo_t {
    char id[16];         /* Identification string */
    uint64_t smem_start;    /* Start of frame buffer mem */
                    /* (physical address) */
    uint32_t smem_len;            /* Length of frame buffer mem */
    uint16_t xpanstep;   /* zero if no hardware panning  */
    uint16_t ypanstep;   /* zero if no hardware panning  */
    uint16_t ywrapstep;  /* zero if no hardware ywrap    */
    uint32_t line_length;
    uint64_t mmio_start;  /* Start of Memory Mapped I/O   */
                               /* (physical address) */
    size_t mmio_len;           /* Length of Memory Mapped I/O  */
    uint16_t reserved[15];
} fb_fix_screeninfo_t;

typedef struct _fb_bitfield_t {
    uint32_t offset;
    uint32_t length;
    uint32_t msb_right;
} fb_bitfield_t;

typedef struct _fb_var_screeninfo_t {
    uint32_t xres;        /* visible resolution */
    uint32_t yres;
    uint32_t xres_virtual;    /* virtual resolution */
    uint32_t yres_virtual;
    uint32_t xoffset;        /* offset from virtual to visible */
    uint32_t yoffset;
    uint32_t bits_per_pixel;
    fb_bitfield_t red;    /* bitfield in fb mem if true color */
    fb_bitfield_t green;
    fb_bitfield_t blue;
    fb_bitfield_t transp;    /* transparency */

    uint32_t height;            /* height of picture in mm    */
    uint32_t width;            /* width of picture in mm     */

    uint32_t rotate;            /* angle we rotate counter clockwise */
    uint16_t reserved[10];
} fb_var_screeninfo_t;

#endif /* COMP_FB_H */
