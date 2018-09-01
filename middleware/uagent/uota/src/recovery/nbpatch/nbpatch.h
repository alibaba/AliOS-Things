/*
 * nbpatch.h
 *
 *  Created on: 2017年12月8日
 *      Author: ting.guit
 */

#ifndef NBPATCH_H_
#define NBPATCH_H_

#include "rec_pub.h"
#include "flash_hal.h"

#define HEADER_SIZE 48
#define DIFF_OFFSET 0x4E00

#define PARTITION_PARAM HAL_PARTITION_PARAMETER_1

//#define LOG(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define LOG(fmt, ...) printf("[%d]"fmt"\r\n", __LINE__, ##__VA_ARGS__)

#define NBDIFF_PARAMS_INPUT_ERROR   1
#define NBDIFF_MEM_OP_FAIL          2
#define NBDIFF_FILE_OP_FAIL         3
#define NBDIFF_LZAM_OP_FAIL         4
#define NBDIFF_PATCH_OP_FAIL        5
#define NBDIFF_CRC_OP_FAIL          6

off_t nbpatch(unsigned long old_t, off_t old_size, const unsigned long new_t, off_t new_size, off_t splict_size);

int nbpatch_error(unsigned long src);

int nbpatch_eof(unsigned long src);

int nbpatch_read(unsigned long src, const unsigned char  *buffer, unsigned long pos, size_t size);

int nbpatch_write(unsigned long dst, const unsigned char *buffer, unsigned long pos, size_t size);

off_t offtin(u_char *buf) ;

#endif /* NBPATCH_H_ */
