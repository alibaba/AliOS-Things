/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SST_H_
#define _SST_H_



#define RET_SST_SUCCESS                 (0x00000000)
#define RET_SST_ERROR_GENERIC           (0xFFFF0000)
#define RET_SST_ACCESS_DENIED           (0xFFFF0001)
#define RET_SST_FILE_NOT_FOUND          (0xFFFF0002)
#define RET_SST_BAD_PARAMETERS          (0xFFFF0003)
#define RET_SST_OUT_OF_MEMORY           (0xFFFF0004)
#define RET_SST_STORAGE_NO_SPACE        (0xFFFF0005)
#define RET_SST_ACCESS_CONFLICT         (0xFFFF0006)
#define RET_SST_STORAGE_NOT_AVAILABLE   (0xFFFF0007)
#define RET_SST_ERROR_BUSY              (0xFFFF0008)
#define RET_SST_ERROR_OVERFLOW          (0xFFFF0009)
#define RET_SST_ERROR_OBJ_ID            (0xFFFF000A)


#define  SST_OBJ_ID_LEN    16

uint32_t sst_init(void);

uint32_t sst_store_file(uint8_t *name, uint8_t* obj_id, uint8_t* data_in, 
                            size_t in_size, uint32_t flag);

uint32_t sst_get_file(uint8_t *name, uint8_t* obj_id, uint8_t* data_out);

uint32_t sst_get_file_size(uint8_t *name, size_t* out_size);

#endif /* _SST_H_ */

