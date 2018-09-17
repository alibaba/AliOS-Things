/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 **/

#ifndef _TEE_TST_H_
#define _TEE_TST_H_

#if CONFIG_TST_STUB

typedef enum
{
    TEE_SEEK_SET = 0,
    TEE_SEEK_CUR = 1,
    TEE_SEEK_END = 2
} TEE_Whence;

typedef enum _tee_open_flag_t
{
    TOF_CREAT = 0x00000001,
    TOF_EXCL  = 0x00000002,
    TOF_READ  = 0x00000004,
    TOF_WRITE = 0x00000008,
} tee_open_flag_t;

typedef void *tee_file_iter_t;
/*
 * return >0:success -1:error
 */
int32_t tee_open(const int8_t *name, int32_t flags);
/*
 * return 0:success -1:error
 */
int32_t tee_close(int32_t fd);
/*
 * return 0:success -1:error
 */
int32_t tee_close_delete(int32_t fd);
/*
 * return success: return read data size. error: -1
 */
ssize_t tee_read(int32_t fd, void *buf, size_t size);
/*
 * return success: return write data size. error: -1
 */
ssize_t tee_write(int32_t fd, const void *buf, size_t size);
/*
 * return 0:success -1:error
 */
int32_t tee_ioctl(int32_t fd, int32_t cmd, ... /* args */);
/*
 * return success: the resulting offset location. error: -1
 */
int32_t tee_lseek(int32_t fd, int32_t offset, int32_t whence);
/*
 * return 0:success -1:error
 */
int32_t tee_ltruncate(int32_t fd, int32_t length);

tee_file_iter_t tee_create_file_iter(void);
void            tee_destroy_file_iter(tee_file_iter_t iter);
/*
 * return 0:success -1:error
 */
int32_t tee_iter_file(tee_file_iter_t iter, int8_t *name);

#endif

#endif /* _TEE_TST_H_ */
