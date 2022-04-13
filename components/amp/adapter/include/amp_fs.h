/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

/* AMP System Abstract Layer */
#ifndef AMP_FILESYS_H
#define AMP_FILESYS_H

#include "infra_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct hal_stat{
    uint32_t st_mode;
    uint32_t st_size;
};

/**
 * @brief       filesystem initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Finit(void);

/**
 * @brief       file open
 * 
 * @return      void *
 */
void *HAL_Fopen(const char *path, const char *mode);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Fclose(void *stream);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Fseek(void *stream, int32_t offset, int32_t whence, int32_t *curpos);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Fread(void *buff, uint32_t size, uint32_t count, void *stream);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Fwrite(void *buff, uint32_t size, uint32_t count, void *stream);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Fsync(void *stream);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_stat(const char *path, struct hal_stat *st);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Funlink(const char *path);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Fmkdir(const char *path);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int32_t HAL_Frmdir(const char *path);


long HAL_Ftell(void *stream);


char HAL_S_ISREG(uint32_t mode);

#if defined(__cplusplus)
}
#endif
#endif /* AMP_FILESYS_H */
