/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * A simple GIF encoder.
 */
#include "imlib.h"
#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)

#include "fb_alloc.h"
#include "ff_wrapper.h"
#define BLOCK_SIZE (126) // (2^7) - 2 // (DO NOT CHANGE!)

void gif_open(FIL *fp, int width, int height, bool color, bool loop)
{
    file_buffer_on(fp);

    write_data(fp, "GIF89a", 6);
    write_word(fp, width);
    write_word(fp, height);
    write_data(fp, (uint8_t []) {0xF6, 0x00, 0x00}, 3);

    if (color) {
        for (int i=0; i<128; i++) {
            int red =   ((((i & 0x60) >> 5) * 255) + 1.5) / 3;
            int green = ((((i & 0x1C) >> 2) * 255) + 3.5) / 7;
            int blue =   (((i & 0x3)        * 255) + 1.5) / 3;
            write_data(fp, (uint8_t []) {red, green, blue}, 3);
        }
    } else {
        for (int i=0; i<128; i++) {
            int gray = ((i * 255) + 63.5) / 127;
            write_data(fp, (uint8_t []) {gray, gray, gray}, 3);
        }
    }

    if (loop) {
        write_data(fp, (uint8_t []) {'!', 0xFF, 0x0B}, 3);
        write_data(fp, "NETSCAPE2.0", 11);
        write_data(fp, (uint8_t []) {0x03, 0x01, 0x00, 0x00, 0x00}, 5);
    }

    file_buffer_off(fp);
}

void gif_add_frame(FIL *fp, image_t *img, uint16_t delay)
{
    file_buffer_on(fp);

    if (delay) {
        write_data(fp, (uint8_t []) {'!', 0xF9, 0x04, 0x04}, 4);
        write_word(fp, delay);
        write_word(fp, 0); // end
    }

    write_byte(fp, 0x2C);
    write_long(fp, 0);
    write_word(fp, img->w);
    write_word(fp, img->h);
    write_data(fp, (uint8_t []) {0x00, 0x07}, 2); // 7-bits

    int bytes  = img->h * img->w;
    int blocks = (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (IM_IS_GS(img)) {
        for (int y=0; y<blocks; y++) {
            int block_size = IM_MIN(BLOCK_SIZE, bytes - (y*BLOCK_SIZE));
            write_byte(fp, 1 + block_size);
            write_byte(fp, 0x80); // clear code
            for (int x=0; x<block_size; x++) {
                write_byte(fp, img->pixels[(y*BLOCK_SIZE)+x]>>1);
            }
        }
    } else if (IM_IS_RGB565(img)) {
        for (int y=0; y<blocks; y++) {
            int block_size = IM_MIN(BLOCK_SIZE, bytes - (y*BLOCK_SIZE));
            write_byte(fp, 1 + block_size);
            write_byte(fp, 0x80); // clear code
            for (int x=0; x<block_size; x++) {
                int pixel = ((uint16_t *) img->pixels)[(y*BLOCK_SIZE)+x];
                int red = COLOR_RGB565_TO_R5(pixel)>>3;
                int green = COLOR_RGB565_TO_G6(pixel)>>3;
                int blue = COLOR_RGB565_TO_B5(pixel)>>3;
                write_byte(fp, (red<<5) | (green<<2) | blue);
            }
        }
    } else if (IM_IS_BAYER(img)) {
        for (int y=0; y<blocks; y++) {
            int block_size = IM_MIN(BLOCK_SIZE, bytes - (y*BLOCK_SIZE));
            write_byte(fp, 1 + block_size);
            write_byte(fp, 0x80); // clear code
            uint16_t pixels[block_size];
            imlib_debayer_line_to_rgb565(0, block_size, y, pixels, img);
            for (int x=0; x<block_size; x++) {
                int r = COLOR_RGB565_TO_R8(pixels[x]);
                int g = COLOR_RGB565_TO_G8(pixels[x]);
                int b = COLOR_RGB565_TO_B8(pixels[x]);
                r >>=3; g >>=3; b >>=3;
                write_byte(fp, (r<<5) | (g<<2) | b);
            }
        }
    }


    write_data(fp, (uint8_t []) {0x01, 0x81, 0x00}, 3); // end code

    file_buffer_off(fp);
}

void gif_close(FIL *fp)
{
    write_byte(fp, ';');
    file_close(fp);
}
#endif //IMLIB_ENABLE_IMAGE_FILE_IO
