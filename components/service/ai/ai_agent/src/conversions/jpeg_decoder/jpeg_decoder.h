/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef JPEG_DECODER_H
#define JPEG_DECODER_H

int tjpeg_header(unsigned char* jpeg_buffer, int jpeg_size, int* width, int* height, int* subsample, int* colorspace);
int tjpeg2rgb(unsigned char* jpeg_buffer, int jpeg_size, unsigned char** rgb_buffer, int* size);
int trgb2jpeg(unsigned char* rgb_buffer, int width, int height, int quality, unsigned char** jpeg_buffer, unsigned long* jpeg_size);

typedef struct _jpeg_frame_t {
    char *data;
    int len;
} jpeg_frame_t;

#endif //JPEG_DECODER_H