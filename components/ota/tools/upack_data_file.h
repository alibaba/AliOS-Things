/** @defgroup upack_data_file
 * @{
 *
 * This is an include file of upack js app.bin interface.
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef UPACK_DATA_FILE_H
#define UPACK_DATA_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DATA_FILE_PACK_HEAD {
    unsigned short file_numb;
    unsigned short pack_ver;
    unsigned int   pack_size;
} data_file_pack_head_t;

typedef struct DATA_FILE_INFO{
    unsigned int head_size;
    unsigned int file_size;
    char md5_vale[16];
} data_file_infor_t;

/**
 * data_file_unpack  data file unpack.
 *
 * @param[in] void *pack_file        pack file name
 * @param[in] unsigned int pack_size pack file size
 * @param[in] void *upack_path       unpack file store path
 *
 * @return  0                        upack success.
 * @return -1                        unpack failed.
 */
int data_file_unpack(void *pack_file, unsigned int pack_size, void *upack_path);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif