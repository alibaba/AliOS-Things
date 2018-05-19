/**
 * @file lv_tutorial_images.h
 *
 */

/*
 * -------------------------------------------------------------------------------------------
 * Learn how to use images stored internally (in flash) or externally (like on an SD card)
 *-------------------------------------------------------------------------------------------
 *
 * In the graphics library images can be used as plain color arrays with a header.
 * To convert an image to a compatible array which can be simply copied into your project use:
 * lv_utils/img_conv/img_conv.py (https://github.com/littlevgl/lv_utils.git)
 *
 * Then type in a terminal: python img_conv.py -f my_image.png (or .jpg, .bmp etc)
 * Optionally you can assign the "-t" flag to mark the image as chroma keyed
 * (chroma key: LV_COLOR_TRANSP colored pixels will be transparent)
 *
 * The script will result 2 files:
 *  - img_my_image.c:       contains a 'const' array with the pixels (can be added to your project to compile)
 *  - img_my_image.bin:     a binary file (can be used externally e.g. on an SD card)
 *
 * USING THE IMAGES IN THE LIBRARY
 * For maximal flexibility  images are used like files.
 * Hence they can be loaded both from external drivers (like SD card) or from the internal flash.
 * LittlevGL contains a RAM file system called UFS (lv_misc/lv_ufs) to create files from images stored in the flash.
 * To add other drivers you need to use the file system interface (lv_misc/lv_fs). It assigns a letter to each drive
 * (like 'U' for UFS). You need to declare then open, close, read, write etc functions and create driver like in lv_ufs.c
 *
 * ADDING THE C FILE
 * 1. Copy the ".c" file to any folder you want in your project
 * 2. In a file where you want to use the image, declare the image variable like this: LV_IMG_DECLARE(img_my_image);
 * 3. Create a file about this image: lv_img_create_file("file_name", img_my_image);
 * 4. Create an image object: lv_obj_t *img = lv_img_create(lv_scr_act(), NULL);
 * 5. Set the created file for the image: lv_imag_set_file(img, "U:/file_name");
 *
 * USING THE BINARY FILE
 * 1. Add a new driver to 'lv_fs'.
 * 2. Copy the file to that drive (like SD card)
 * 3. Create an image object: lv_obj_t *img = lv_img_create(lv_scr_act(), NULL);
 * 4. Set the a binary file for the image: lv_imag_set_file(img, "S:/img_my_image.bin");
 *
 * One file can be assigned to any number of image objects.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_images.h"
#include "lvgl/lvgl.h"
#include <stdio.h>
#include <errno.h>

/*********************
 *      DEFINES
 *********************/
#define PC_FILES    1       /*If you are on PC you can add PC file function to 'lv_fs'*/

/**********************
 *      TYPEDEFS
 **********************/
typedef  FILE*  pc_file_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if PC_FILES
/*Interface functions to standard C file functions (only the required ones to image handling)*/
static lv_fs_res_t pcfs_open (void * file_p, const char * fn, lv_fs_mode_t mode);
static lv_fs_res_t pcfs_close (void * file_p);
static lv_fs_res_t pcfs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t pcfs_seek (void * file_p, uint32_t pos);
static lv_fs_res_t pcfs_tell (void * file_p, uint32_t * pos_p);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
/*Declare the "source code image" which is stored in the flash*/
LV_IMG_DECLARE(img_red_flower);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create object to see how they change from the anti aliasing
 * Modify LV_ANTIALIAS and LV_FONT_ANTIALIAS to see what is changing
 */
void lv_tutorial_image(void)
{
    /*************************
     * IMAGE FROM SOURCE CODE
     *************************/
    lv_img_create_file("red_flower", img_red_flower);       /*Create a file in the RAM FS*/

    lv_obj_t *img_src = lv_img_create(lv_scr_act(), NULL);  /*Crate an image object*/
    lv_img_set_file(img_src, "U:/red_flower");              /*Set the created file as image (a red fl   ower)*/
    lv_obj_set_pos(img_src, 10, 10);      /*Set the positions*/
    lv_obj_set_click(img_src, true);
    lv_obj_set_drag(img_src, true);
#if PC_FILES
    /*************************
     * IMAGE FROM BBINARY FILE
     *************************/

    /* Add a simple drive to open images from PC*/
    lv_fs_drv_t pcfs_drv;                         /*A driver descriptor*/
    memset(&pcfs_drv, 0, sizeof(lv_fs_drv_t));    /*Initialization*/

    pcfs_drv.file_size = sizeof(pc_file_t);       /*Set up fields...*/
    pcfs_drv.letter = 'P';
    pcfs_drv.open = pcfs_open;
    pcfs_drv.close = pcfs_close;
    pcfs_drv.read = pcfs_read;
    pcfs_drv.seek = pcfs_seek;
    pcfs_drv.tell = pcfs_tell;
    lv_fs_add_drv(&pcfs_drv);


    lv_obj_t *img_bin = lv_img_create(lv_scr_act(), NULL);  /*Create an image object*/
    /* Set the image's file according to the current color depth
     * a blue flower picture*/
#if LV_COLOR_DEPTH == 8
    lv_img_set_file(img_bin, "P:/lv_examples/lv_tutorial/6_images/img_blue_flower_8.bin");
#elif LV_COLOR_DEPTH == 16
    lv_img_set_file(img_bin, "P:/lv_examples/lv_tutorial/6_images/img_blue_flower_16.bin");
#elif LV_COLOR_DEPTH == 24
    lv_img_set_file(img_bin, "P:/lv_examples/lv_tutorial/6_images/img_blue_flower_24.bin");
#endif

    lv_obj_align(img_bin, img_src, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);     /*Align next to the s'ource image'*/
    lv_obj_set_click(img_bin, true);
    lv_obj_set_drag(img_bin, true);
#endif

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Open a file from the PC
 * @param file_p pointer to a FILE* variable
 * @param fn name of the file.
 * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g. FS_MODE_WR | FS_MODE_RD)
 * @return LV_FS_RES_OK: no error, the file is opened
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t pcfs_open (void * file_p, const char * fn, lv_fs_mode_t mode)
{
    errno = 0;

    const char * flags = "";

    if(mode == LV_FS_MODE_WR) flags = "wb";
    else if(mode == LV_FS_MODE_RD) flags = "rb";
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "a+";

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    sprintf(buf, "./%s", fn);

    pc_file_t f = fopen(buf, flags);
    if((long int)f <= 0) return LV_FS_RES_UNKNOWN;
    else {
        fseek(f, 0, SEEK_SET);

        /* 'file_p' is pointer to a file descriptor and
         * we need to store our file descriptor here*/
        pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
        *fp = f;
    }

    return LV_FS_RES_OK;
}


/**
 * Close an opened file
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_close (void * file_p)
{
    pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
    fclose(*fp);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param file_p pointer to a FILE variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
    *br = fread(buf, 1, btr, *fp);
    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_seek (void * file_p, uint32_t pos)
{
    pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
    fseek(*fp, pos, SEEK_SET);
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a FILE* variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_tell (void * file_p, uint32_t * pos_p)
{
    pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
    *pos_p = ftell(*fp);
    return LV_FS_RES_OK;
}

