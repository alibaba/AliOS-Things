/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 *
 */
#include "uai_load.h"

static int uai_flash_info_parse(char *info, uint32_t *part, uint32_t *offset, int *size)
{
    char *ptr = NULL;

    if (!info || !part || !offset || !size) {
        UAI_LOGE("arg null !\n");
        return UAI_FAIL;
    }

    ptr = strstr(info, "pt:");
    if (ptr) {
        ptr += strlen("pt:");
        *part = strtoul(ptr, 0, 0);
    } else {
        UAI_LOGE("part not found !\n");
        return UAI_FAIL;
    }

    ptr = strstr(info, "offset:");
    if (ptr) {
        ptr += strlen("offset:");
        *offset = strtoul(ptr, 0, 0);
    } else {
        UAI_LOGE("offset not found !\n");
        return UAI_FAIL;
    }

    ptr = strstr(info, "size:");
    if (ptr) {
        ptr += strlen("size:");
        *size = atoi(ptr);
    } else {
        UAI_LOGE("size not found !\n");
        return UAI_FAIL;
    }

    return UAI_SUCCESS;
}

static int uai_load_from_flash(int8_t *weight, int8_t *bias, int32_t weight_size, int32_t bias_size, int32_t offset, char *trained_data_src)
{
    uint32_t ptno;
    uint32_t off_set;
    int      size;
    int      ret;

    if(UAI_SUCCESS != uai_flash_info_parse(trained_data_src, &ptno, &off_set, &size)) {
        UAI_LOGE("flash info parse fail !\n");
        return UAI_FAIL;
    }

    if(weight_size + bias_size > size - offset) {
        UAI_LOGE("size error !\n");
        return UAI_FAIL;
    }

    off_set += offset;

    ret = hal_flash_read(ptno, &off_set, weight, weight_size);
    if(ret != 0) {
        UAI_LOGE("read from flash error !\n");
        return UAI_FAIL;
    }

    off_set += weight_size;
    ret = hal_flash_read(ptno, &off_set, bias, bias_size);
    if(ret != 0) {
        UAI_LOGE("read from flash error !\n");
        return UAI_FAIL;
    }

    UAI_LOGD("load trained data from flash success !\n");

    return UAI_SUCCESS;
}

static int uai_mem_info_parse(char *info, uint32_t *addr, int *size)
{
    char *ptr = NULL;

    if (!info || !addr || !size) {
        UAI_LOGE("arg null !\n");
        return UAI_FAIL;
    }

    ptr = strstr(info, "mem:");
    if (ptr) {
        ptr += strlen("mem:");
        *addr = strtoul(ptr, 0, 0);
    } else {
        UAI_LOGE("part not found !\n");
        return UAI_FAIL;
    }

    ptr = strstr(info, "size:");
    if (ptr) {
        ptr += strlen("size:");
        *size = atoi(ptr);
    } else {
        UAI_LOGE("size not found !\n");
        return UAI_FAIL;
    }

    return UAI_SUCCESS;
}

static int uai_load_from_mem(uint32_t *weight_addr, uint32_t *bias_addr, int32_t weight_size, int32_t bias_size, int32_t offset, char *trained_data_src)
{
    uint32_t addr;
    int      size;

    if(UAI_SUCCESS != uai_mem_info_parse(trained_data_src, &addr, &size)) {
        UAI_LOGE("flash info parse fail !\n");
        return UAI_FAIL;
    }

    if(weight_size + bias_size > size - offset) {
        UAI_LOGE("size error !\n");
        return UAI_FAIL;
    }

    *weight_addr = addr + offset;
    *bias_addr   = addr + offset + weight_size;

    UAI_LOGD("load trained data from mem addr %p success !\n", addr + offset);

    return UAI_SUCCESS;
}

static int uai_file_info_parse(char *info, char **filename)
{
    char *ptr;

    if (!info || !filename) {
        UAI_LOGE("arg null !\n");
        return UAI_FAIL;
    }

    ptr = strstr(info, "fs:");
    if (ptr) {
        ptr += strlen("fs:");
        *filename = ptr;
    } else {
        UAI_LOGE("part not found !\n");
        return UAI_FAIL;
    }

    UAI_LOGD("load trained data from file %s success !\n", filename);

    return UAI_SUCCESS;
}

static int uai_load_from_file(int8_t *weight, int8_t *bias, int32_t weight_size, int32_t bias_size, int32_t offset, char *trained_data_src)
{
    char *filename = NULL;
    FILE *fd       = NULL;
    int size       = 0;
    int ret        = 0;

    if(UAI_SUCCESS != uai_file_info_parse(trained_data_src, &filename)) {
        UAI_LOGE("file info parse fail !\n");
        return UAI_FAIL;
    }

    fd = fopen(filename, "rb");
    if(fd == NULL) {
        UAI_LOGE("open file fail !\n");
        return UAI_FAIL;
    }

    fseek(fd, 0, SEEK_END);
    size = ftell(fd);

    if(weight_size + bias_size > size - offset) {
        UAI_LOGE("size error !\n");
        fclose(fd);
        return UAI_FAIL;
    }

    fseek(fd, offset, SEEK_SET);

    ret = fread(weight, 1, weight_size, fd);
    if(ret <= 0) {
        UAI_LOGE("read from file error, ret %d !\n", ret);
        fclose(fd);
        return UAI_FAIL;
    }

    ret = fread(bias, 1, bias_size, fd);
    if(ret <= 0) {
        UAI_LOGE("read from file error, ret %d !\n", ret);
        fclose(fd);
        return UAI_FAIL;
    }

    fclose(fd);
    return UAI_SUCCESS;
}

uai_src_type_e uai_src_type_parse(char *source)
{
    if (!source) {
        return UAI_SRC_TYPE_NULL;
    }

    if (!strncmp(source, "pt:", strlen("pt:"))) {
        return UAI_SRC_TYPE_FLASH;
    }
    else if (!strncmp(source, "mem:", strlen("mem:"))) {
        return UAI_SRC_TYPE_MEM;
    }
    else if (!strncmp(source, "fs:", strlen("fs:"))) {
        return UAI_SRC_TYPE_FILE;
    }

    return UAI_SRC_TYPE_NULL;
}

int uai_load_trained_data(int8_t *weight, int8_t *bias, int32_t weight_size, int32_t bias_size, int32_t offset, char *trained_data_src)
{
    int ret             = UAI_SUCCESS;
    uai_src_type_e type = UAI_SRC_TYPE_NULL;

    type = uai_src_type_parse(trained_data_src);
    if(type == UAI_SRC_TYPE_NULL) {
        UAI_LOGE("source type error !\n");
        return UAI_FAIL;
    }

    switch (type) {
        case UAI_SRC_TYPE_FLASH:
            ret = uai_load_from_flash(weight, bias, weight_size, bias_size, offset, trained_data_src);
        break;

        case UAI_SRC_TYPE_MEM:
            ret = uai_load_from_mem((uint32_t *)weight, (uint32_t *)bias, weight_size, bias_size, offset, trained_data_src);
        break;

        case UAI_SRC_TYPE_FILE:
            ret = uai_load_from_file(weight, bias, weight_size, bias_size, offset, trained_data_src);
        break;

        default:
            UAI_LOGE("source type not support !\n");
            return UAI_FAIL;
        break;
    }

    return ret;
}
