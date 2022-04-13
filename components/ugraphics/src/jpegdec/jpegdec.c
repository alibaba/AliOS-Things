/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <ulog/ulog.h>
#include <stdio.h>

#define USE_TURBOJPEG

#define TAG "JPEG_DECODER"

#ifndef USE_TURBOJPEG
#include <jpeglib.h>
#include <setjmp.h>

static unsigned jpeg_err = 0;

static void jpeg_error_exit(j_common_ptr cinfo)
{
    LOGE(TAG, "jpeg decoder error!!\n");
    jpeg_err = 1;
}

int tjpeg2rgb(unsigned char *jpeg_buffer, int jpeg_size, unsigned char **rgb_buffer, int *size)
{
    int ret = 0;
    struct jpeg_decompress_struct jpeg_info;
    static struct jpeg_error_mgr jpeg_error_mgr;

    jpeg_create_decompress(&jpeg_info);
    jpeg_info.err = jpeg_std_error(&jpeg_error_mgr);
    jpeg_error_mgr.error_exit = jpeg_error_exit;

    jpeg_mem_src(&jpeg_info, jpeg_buffer, jpeg_size);

    if (jpeg_read_header(&jpeg_info, TRUE) == -1) {
        LOGE(TAG, "lv_img_decoder_open_jpeg: read header err !!\n");
        return -1;
    }
    jpeg_info.out_color_space = JCS_RGB;
    if ((jpeg_start_decompress(&jpeg_info) == FALSE) || (jpeg_err == 1)) {
        LOGE(TAG, "lv_img_decoder_open_jpeg: decompress failed !!\n");
        return -1;
    }

    uint32_t out_buffer_size = jpeg_info.image_width * jpeg_info.image_height * jpeg_info.num_components;
    LOGI(TAG, "jpeg_info.image_width: %d, jpeg_info.image_height: %d, jpeg_info.num_components: %d",
                 jpeg_info.image_width, jpeg_info.image_height, jpeg_info.num_components);
    *rgb_buffer = calloc(out_buffer_size, 1);
    if (!*rgb_buffer)
        printf("rgb_buffer alloc fail\n");
    *size = out_buffer_size;
    if (*rgb_buffer == NULL) {
        LOGE(TAG, "%s : no enough memory!!\n", __func__);
        ret = -1;
        goto err;
    }

    while (jpeg_info.output_scanline < jpeg_info.output_height) {
        JSAMPROW out_str  = (JSAMPROW)(*rgb_buffer +
                             jpeg_info.output_scanline * jpeg_info.image_width *
                             jpeg_info.num_components);
        if ((jpeg_read_scanlines(&jpeg_info, &out_str, 1) != 1) || (jpeg_err == 1)) {
            LOGE(TAG, "[%s]: scan line failed!!\n", __func__);
            free((void *)*rgb_buffer);
            ret = -1;
        }
    }
err:
    if (jpeg_err == 0)
        jpeg_finish_decompress(&jpeg_info);

    jpeg_destroy_decompress(&jpeg_info);

    return ret;
}
#else
#include <turbojpeg.h>

int tjpeg_header(unsigned char *jpeg_buffer, int jpeg_size,
                 int *width, int *height,
                 int *subsample, int *colorspace)
{
    int ret = 0;
    tjhandle handle = NULL;

    handle = tjInitDecompress();
    if (!handle) {
        LOGE(TAG, "tjInitDecompress fail, ret = %d", ret);
        return -1;
    }
    ret = tjDecompressHeader3(handle, jpeg_buffer, jpeg_size,
                                width, height, subsample, colorspace);
    if (ret < 0)
        LOGE(TAG, "tjDecompressHeader3 fail, ret = %d", ret);

    tjDestroy(handle);

    return ret;
}

int tjpeg2rgb(unsigned char *jpeg_buffer, int jpeg_size,
              unsigned char **rgb_buffer, int *size)
{
    int ret = 0;
    tjhandle handle = NULL;
    int width, height, subsample, colorspace;
    int flags = 0;
    int pixelfmt = TJPF_RGB;
    int pitch = 0;

    handle = tjInitDecompress();
    if (!handle) {
        LOGE(TAG, "tjInitDecompress fail, ret = %d", ret);
        return -1;
    }
    ret = tjDecompressHeader3(handle, jpeg_buffer, jpeg_size,
                                &width, &height, &subsample, &colorspace);
    if (ret < 0) {
        LOGE(TAG, "tjDecompressHeader3 fail, ret = %d", ret);
        goto finish;
    }
    LOG("width: %d, height: %d", width, height);
    flags |= 0;
    *rgb_buffer = (unsigned char *)tjAlloc(width * height *
                                           tjPixelSize[pixelfmt]);
    if ((*rgb_buffer) == NULL) {
        LOGE(TAG, "allocating uncompressed image buffer");
        goto finish;
    }
    *size = width * height * tjPixelSize[pixelfmt];
    pitch = tjPixelSize[pixelfmt] * width;
    ret = tjDecompress2(handle, jpeg_buffer, jpeg_size,
                        *rgb_buffer, width, pitch, height, pixelfmt, flags);
     if (ret < 0) {
        LOGE(TAG, "tjDecompress2 fail, ret = %d", ret);
        tjFree(*rgb_buffer);
    }

finish:
    tjDestroy(handle);
    return ret;
}

int trgb2jpeg(unsigned char *rgb_buffer, int width,
              int height, int quality, unsigned char **jpeg_buffer,
              unsigned long *jpeg_size)
{
    int ret = 0;
    tjhandle handle = NULL;
    // unsigned long size=0;
    int flags = 0;
    int subsamp = TJSAMP_422;
    int pixelfmt = TJPF_RGB;

    handle = tjInitCompress();
    if (!handle) {
        LOGE(TAG, "tjInitDecompress fail, ret = %d", ret);
        return -1;
    }

    // size=tjBufSize(width, height, subsamp);
    ret = tjCompress2(handle, rgb_buffer, width, 0, height,
                    pixelfmt, jpeg_buffer, jpeg_size, subsamp, quality, flags);
    if (ret < 0)
        LOGE(TAG, "tjCompress2 fail, ret = %d", ret);

    tjDestroy(handle);
    return ret;
}
#endif
