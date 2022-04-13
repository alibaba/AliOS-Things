/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "ota_hal_os.h"
#include "xz.h"
#include "ota_log.h"
#include "xz_export.h"

int xz_check_file_crc(hal_partition_t src_partition, unsigned int size, unsigned int crc)
{
    int ret = 0;
    unsigned char* data_buf = NULL;
    unsigned int read_size = 0;
    unsigned int offset = 0;
    uint16_t cal_crc = 0;
    ota_crc16_ctx contex;
    data_buf = ota_malloc(SECTOR_SIZE);
    if(data_buf == NULL) {
        OTA_LOG_E("xz check image crc malloc failed");
        ret = XZ_MALLOC_FAILED;
    }
    else {
        memset(data_buf, 0x00, SECTOR_SIZE);
        ota_crc16_init(&contex );
        for( ; size > 0; size -= read_size, offset += read_size ) {
            read_size = size >= SECTOR_SIZE ? SECTOR_SIZE : size;
            memset(data_buf, 0, SECTOR_SIZE);
            ret = xz_flash_read_data(src_partition, offset, data_buf, read_size);
            if (ret != 0){
                OTA_LOG_I("r d ret:%d.",ret);
            }
            ota_crc16_update(&contex, data_buf, read_size);
        }
        ota_crc16_final(&contex, &cal_crc);
        OTA_LOG_I("cal crc:0x%04x, crc:0x%x", cal_crc, crc);
        if(data_buf != NULL) {
            ota_free(data_buf);
            data_buf = NULL;
        }
        if(cal_crc != crc) {
            ret = XZ_CRC_ERROR;
        }
    }
    return ret;
}

static bool xz_identify_xz_header(hal_partition_t src_partition, unsigned int offset)
{
    unsigned char data_buf[16] = {0};
    const unsigned char xz_header[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00 };
    int ret = xz_flash_read_data(src_partition, offset, data_buf, 16);
    if (0 != ret) {
        OTA_LOG_I("r xz ret:%d.",ret);
    }
    if (!memcmp(data_buf, xz_header, 6)) {
        OTA_LOG_I("is xz file.");
        return true;
    }
    return false;
}

static int32_t copy_xz(hal_partition_t des_partition, hal_partition_t src_partition,
                           unsigned int src_size, unsigned int *des_size)
{
    struct xz_buf b;
    struct xz_dec *s;
    enum xz_ret ret = XZ_OK;
    unsigned char* in_buf = NULL;
    unsigned char* out_buf = NULL;
    unsigned int read_size;
    unsigned int rema_size = src_size;
    unsigned int erase_offset = 0;
    unsigned int read_offset = 0;
    unsigned int write_offset = 0;
    int err = 0;

    if(des_size == NULL) {
        OTA_LOG_E("input param err");
        goto exit;
    }
    *des_size = 0;
    err = xz_flash_erase(des_partition, erase_offset, SECTOR_SIZE);
    if(0 != err) {
        OTA_LOG_E("e ret:%d",err);
    }
    xz_crc32_init();
    s = xz_dec_init(XZ_DYNALLOC, 1 << 15);
    if (s == NULL) {
        OTA_LOG_E("mem fail");
        goto exit;
    }
    in_buf = ota_malloc(XZ_BUF_SIZE);
    if(in_buf == NULL) {
        ret = XZ_MALLOC_FAILED;
        goto exit;
    }
    out_buf = ota_malloc(XZ_BUF_SIZE);
    if(out_buf == NULL) {
        ret = XZ_MALLOC_FAILED;
        goto exit;
    }
    b.in = in_buf;
    b.in_pos = 0;
    b.in_size = 0;
    b.out = out_buf;
    b.out_pos = 0;
    b.out_size = XZ_BUF_SIZE;

    while (1) {
        if (b.in_pos == b.in_size) {
            read_size = rema_size > XZ_BUF_SIZE ? XZ_BUF_SIZE : rema_size;
            err = xz_flash_read_data(src_partition, read_offset, in_buf, read_size);
            if (0 != err) {
                OTA_LOG_I("r err:%d.",err);
            }
            if(read_size > 0) {
                b.in_size = read_size;
                b.in_pos = 0;
            }
            rema_size -= read_size;
            read_offset += read_size;
        }
        ret = xz_dec_run(s, &b);
        //OTA_LOG_I("xz dec ret:%d p:0x%0x s:0x%x out p:0x%0x s:0x%x \n", ret, read_addr, b.in_size, write_addr, b.out_size);
        if (b.out_pos == b.out_size) {
            if(write_offset + b.out_pos >= erase_offset + SECTOR_SIZE) {
                erase_offset += SECTOR_SIZE;
                err = xz_flash_erase(des_partition, erase_offset, SECTOR_SIZE);
                if(0 != err) {
                    OTA_LOG_I("e f err:%d",err);
                }
            }
            err = xz_flash_write_data(des_partition, write_offset, out_buf, b.out_pos);
            if (err != 0) {
                OTA_LOG_I("w f err:%d.",err);
            }
            write_offset += b.out_pos;
            b.out_pos = 0;
        }
        if (ret == XZ_OK)
            continue;
        if(write_offset + b.out_pos >= erase_offset + SECTOR_SIZE) {
            erase_offset += SECTOR_SIZE;
            err = xz_flash_erase(des_partition, erase_offset, SECTOR_SIZE);
            if(0 != err) {
                OTA_LOG_I("er #err:%d", err);
            }
        }
        err = xz_flash_write_data(des_partition, write_offset, out_buf, b.out_pos);
        if (err != 0) {
            OTA_LOG_I("w f #err:%d.", err);
        }
        write_offset += b.out_pos;
        switch (ret) {
            case XZ_STREAM_END:
                //xz_dec_end(s);
                OTA_LOG_E("write_offset = %d", write_offset);
                *des_size = write_offset;
                ret = 0;
                goto exit;
            case XZ_MEM_ERROR:
                goto exit;
            case XZ_MEMLIMIT_ERROR:
                goto exit;
            case XZ_FORMAT_ERROR:
                goto exit;
            case XZ_OPTIONS_ERROR:
                goto exit;
            case XZ_DATA_ERROR:
            case XZ_BUF_ERROR:
                goto exit;
            default:
                goto exit;
        }
    }
exit:
    xz_dec_end(s);
    if(in_buf != NULL) {
        ota_free(in_buf);
        in_buf = NULL;
    }
    if(out_buf != NULL) {
        ota_free(out_buf);
        out_buf = NULL;
    }
    return ret;
}

static int xz_uncompress(hal_partition_t des_partition, hal_partition_t src_partition,
                             unsigned int src_size, unsigned int *des_size)
{
    if(xz_identify_xz_header(src_partition, 0x00) == true) {
        return copy_xz(des_partition, src_partition, src_size, des_size);
    }
    return XZ_UNPROCESS_FAILED;
}

static int32_t xz_verify_uncompress_file(hal_partition_t des_partition, hal_partition_t src_partition, unsigned int size)
{
    struct xz_buf b;
    struct xz_dec *s;
    enum xz_ret ret = XZ_OK;
    unsigned char* data_buf = NULL;
    unsigned char* in_buf = NULL;
    unsigned char* out_buf = NULL;
    unsigned int read_size;
    unsigned int rema_size = size;
    unsigned int read_src_offset = 0;
    unsigned int read_des_offset = 0;
    int err = 0;
    xz_crc32_init();
    s = xz_dec_init(XZ_DYNALLOC, 1 << 15);
    if (s == NULL) {
	    OTA_LOG_I("xz fail");
	    goto exit;
    }
    data_buf = ota_malloc(SECTOR_SIZE);
    if(data_buf == NULL) {
        ret = XZ_MALLOC_FAILED;
        goto exit;
    }
    memset(data_buf, 0x00, SECTOR_SIZE);
    in_buf = malloc(XZ_BUF_SIZE);
    if(in_buf == NULL) {
        ret = XZ_MALLOC_FAILED;
        goto exit;
    }
    out_buf = malloc(XZ_BUF_SIZE);
    if(out_buf == NULL) {
        ret = XZ_MALLOC_FAILED;
        goto exit;
    }

    b.in = in_buf;
    b.in_pos = 0;
    b.in_size = 0;
    b.out = out_buf;
    b.out_pos = 0;
    b.out_size = XZ_BUF_SIZE;

    while (1)  {
        if (b.in_pos == b.in_size) {
            read_size = rema_size > XZ_BUF_SIZE ? XZ_BUF_SIZE : rema_size;
            err = xz_flash_read_data(src_partition, read_src_offset, in_buf, read_size);
            if (0 != err) {
                OTA_LOG_I("r v err:%d.", err);
            }
            if(read_size > 0) {
                b.in_size = read_size;
                b.in_pos = 0;
            }
            rema_size -= read_size;
            read_src_offset += read_size;
	    }
        ret = xz_dec_run(s, &b);
        //OTA_LOG_I("v xz dec ret:%d p:0x%0x s:0x%x out p:0x%0x s:0x%x \n", ret, b.in_pos, b.in_size, b.out_pos, b.out_size);
        if (b.out_pos == b.out_size) {
            memset(data_buf, 0, SECTOR_SIZE);
            err = xz_flash_read_data(des_partition, read_des_offset, data_buf, b.out_pos);
            if (0 != err) {
                OTA_LOG_I("v r err:%d.", err);
            }
            if(memcmp(data_buf, out_buf, b.out_pos) != 0)
                goto exit;
            read_des_offset += b.out_pos;
            b.out_pos = 0;
        }
        if (ret == XZ_OK)
            continue;
        err = xz_flash_read_data(des_partition, read_des_offset, data_buf, b.out_pos);
        if (0 != err) {
            OTA_LOG_I("r w err:%d.", err);
        }
        if(memcmp(data_buf, out_buf, b.out_pos) != 0)
            goto exit;
        switch (ret) {
            case XZ_STREAM_END:
                //xz_dec_end(s);
                OTA_LOG_I("verify xz OK.\n");
                ret = 0;
                goto exit;
            case XZ_MEM_ERROR:
                goto exit;
            case XZ_MEMLIMIT_ERROR:
                goto exit;
            case XZ_FORMAT_ERROR:
                goto exit;
            case XZ_OPTIONS_ERROR:
                goto exit;
            case XZ_DATA_ERROR:
            case XZ_BUF_ERROR:
                goto exit;
            default:
                goto exit;
        }
    }
exit:
    if(in_buf != NULL) {
        ota_free(in_buf);
        in_buf = NULL;
    }
    if(out_buf != NULL) {
        ota_free(out_buf);
        out_buf = NULL;
    }
    if(data_buf != NULL) {
        ota_free(data_buf);
        data_buf = NULL;
    }
    xz_dec_end(s);
    return ret;
}

static int xz_verify_unzx_file(hal_partition_t des_partition, hal_partition_t src_partition, unsigned int size)
{
    if(xz_identify_xz_header(src_partition, 0x00) == true) {
        return xz_verify_uncompress_file(des_partition, src_partition, size);
    }
    return XZ_VERIFY_FAILED;
}

int xz_file_uncompress(hal_partition_t des_partition, hal_partition_t src_partition, unsigned int xz_size, unsigned short xz_crc16)
{
    int ret = 0;
    int uncompress_size = 0;
#ifdef XZ_CHECK_FILE
    OTA_LOG_I("size:0x%08x,crc:0x%04x", xz_size, xz_crc16);
    /* check image crc16 */
    if(xz_check_file_crc(src_partition, xz_size, xz_crc16) != 0) {
        return XZ_CRC_ERROR;
    }
    ota_msleep(20);
#endif

    OTA_LOG_I("xz uncompress ...");
    /* XZ uncompress */ 
    if(xz_uncompress(des_partition, src_partition, xz_size, &uncompress_size) != 0) {
        return XZ_UNPROCESS_FAILED;
    }

#ifdef XZ_CHECK_FILE
    ota_msleep(20);
    /* XZ verify */
    if(xz_verify_unzx_file(des_partition, src_partition, xz_size) != 0) {
        return XZ_VERIFY_FAILED;
    }
#endif

    return uncompress_size;
}
