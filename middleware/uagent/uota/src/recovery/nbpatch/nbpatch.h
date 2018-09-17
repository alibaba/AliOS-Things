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

#define PARTITION_PARAM HAL_PARTITION_PARAMETER_1
#define PARTITION_OTA   HAL_PARTITION_OTA_TEMP 

#define NBDIFF_PARAMS_INPUT_ERROR   1
#define NBDIFF_MEM_OP_FAIL          2
#define NBDIFF_FILE_OP_FAIL         3
#define NBDIFF_LZAM_OP_FAIL         4
#define NBDIFF_PATCH_OP_FAIL        5
#define NBDIFF_CRC_OP_FAIL          6

off_t nbpatch(unsigned long old_t, off_t old_size, const unsigned long new_t, off_t new_size, off_t splict_size);

int nbpatch_error(unsigned long src);

int nbpatch_eof(unsigned long src);

int nbpatch_read(unsigned long src, const unsigned char  *buffer, unsigned long pos, size_t size, u_char is_old);

int nbpatch_write(unsigned long dst, const unsigned char *buffer, unsigned long pos, size_t size);

off_t offtin(u_char *buf) ;

int save_patch_status(PatchStatus *status);

int read_patch_status(PatchStatus *status);

off_t save_bakeup_data(unsigned long src, off_t bsize);

off_t load_bakeup_data(unsigned long dst, off_t size, off_t offset);
#endif /* NBPATCH_H_ */
