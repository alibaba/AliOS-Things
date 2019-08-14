/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include "hal/wifi.h"
#include "aos/hal/flash.h"
#include "ls_hal.h"
#include "plat_comm.h"

#define DEV_ID_LEN 6
#define MIN_SECTOR_SIZE 4096
#define MIN_SECTOR_SHIFT 12

#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

#define pl_malloc ls_osa_malloc
#define pl_free ls_osa_free

int ls_hal_get_dev_id(uint8_t *dev_id, uint32_t *id_len)
{
    int ret = 0;

    if (*id_len < DEV_ID_LEN) {
        PL_ERR("short buffer id len is %d\n", (unsigned int)*id_len);
        *id_len = DEV_ID_LEN;
        return -1;
    }

    ret = hal_wifi_get_mac_addr(NULL, dev_id);
    if (ret) {
        PL_ERR("get mac addr failed\n");
        return -1;
    }

    *id_len = DEV_ID_LEN;

    return 0;
}

/* if not support, return 0 directly */
int ls_hal_open_rsvd_part(int flag)
{
    return 0;
}

int ls_hal_write_rsvd_part(int fd, uint32_t offset, void *data, uint32_t data_len)
{
    int ret = 0;
    uint32_t erase_size = 0;
    uint32_t pre_len = 0;
    uint8_t *pre_buf = NULL;
    uint32_t suf_len = 0;
    uint8_t *suf_buf = NULL;
    uint32_t first_off = 0;
    uint32_t last_off = 0;
    uint32_t fin_off = offset + data_len;
    uint32_t tmp_off = 0;

    if (data_len != 0 && data == NULL) {
        PL_ERR("bad param \n");
        return -1;
    }

    if (data_len == 0) {
        return 0;
    }

    first_off = ROUNDDOWN(offset, MIN_SECTOR_SIZE);
    pre_len = offset - first_off;
    if (pre_len) {
        pre_buf = (uint8_t *)pl_malloc(pre_len);
        if (pre_buf == NULL) {
            PL_ERR("malloc pre buf failed\n");
            return -1;
        }

        //store the data of first block that will be erased
        tmp_off = first_off;
        ret = hal_flash_read(HAL_PARTITION_PARAMETER_4, &tmp_off,
                             pre_buf, pre_len);
        if (ret || (tmp_off - first_off != pre_len)) {
            PL_ERR("hal flash pre read failed\n");
            ret = -1;
            goto clean;
        }

    }

    last_off = ROUNDUP(fin_off, MIN_SECTOR_SIZE);
    suf_len = last_off - fin_off;
    if (suf_len) {
        suf_buf = (uint8_t *)pl_malloc(suf_len);
        if (suf_buf == NULL) {
            PL_ERR("malloc suf buf failed\n");
            ret = -1;
            goto clean;
        }

        //read the data of last block that will be erased
        tmp_off = fin_off;
        ret = hal_flash_read(HAL_PARTITION_PARAMETER_4, &tmp_off,
                             suf_buf, suf_len);
        if (ret || (tmp_off - fin_off != suf_len)) {
            PL_ERR("hal flash pre read failed\n");
            goto clean;
        }
    }

    erase_size = last_off - first_off;
    ret = hal_flash_erase(HAL_PARTITION_PARAMETER_4, first_off, erase_size);
    if (ret) {
        PL_ERR("flash erase failed ret %d\n", ret);
        goto clean;
    }
    PL_INF("pre_len is %d, suf len is %d, erase len is %d, first_off is %d\n",
            (unsigned int)pre_len, (unsigned int)suf_len, (unsigned int)erase_size, (unsigned int)first_off);

    /* write first sector data that be erased*/
    tmp_off = first_off;
    if (pre_len) {
        ret = hal_flash_write(HAL_PARTITION_PARAMETER_4, &tmp_off, pre_buf, pre_len);
        if (ret || (tmp_off - first_off) != pre_len) {
            PL_ERR("write pre buf failed %d, len %d\n", ret, (unsigned int)tmp_off);
            goto clean;
        }
    }

    /* write the data need to write */
    ret = hal_flash_write(HAL_PARTITION_PARAMETER_4, &tmp_off, data, data_len);
    if (ret) {
        PL_ERR("write pre buf failed %d\n", ret);
        goto clean;
    }

   /* write last sector data that be erased */
    if (suf_len) {
        ret = hal_flash_write(HAL_PARTITION_PARAMETER_4, &tmp_off, suf_buf, suf_len);
        if (ret) {
            PL_ERR("write pre buf failed %d\n", ret);
            goto clean;
        }
    }

clean:
    if (suf_buf) {
        pl_free(suf_buf);
        suf_buf = NULL;
    }

    if (pre_buf) {
        pl_free(pre_buf);
        pre_buf = NULL;
    }

    return ret;
}


int ls_hal_read_rsvd_part(int fd, uint32_t offset, void *buffer, uint32_t read_len)
{
    uint32_t off_set = offset;
    uint32_t len = 0;
    int ret = 0;

    ret = hal_flash_read(HAL_PARTITION_PARAMETER_4, &off_set, buffer, read_len);
    if (ret) {
        PL_ERR("hal flash read failed\n");
        return -1;
    }

    len = off_set - offset;
    if (len != read_len) {
        PL_ERR("read failed read_len is %d", (unsigned int)len);
        return -1;
    }

    return 0;
}

int ls_hal_close_rsvd_part(int fd)
{
    return 0;
}
