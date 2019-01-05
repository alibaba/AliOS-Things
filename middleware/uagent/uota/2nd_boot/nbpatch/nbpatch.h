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
#define PARTITION_BACKUP_PARAM   HAL_PARTITION_RECOVERY_BACK_PARA

#define NBDIFF_PARAMS_INPUT_ERROR   1
#define NBDIFF_MEM_OP_FAIL          2
#define NBDIFF_FILE_OP_FAIL         3
#define NBDIFF_LZAM_OP_FAIL         4
#define NBDIFF_PATCH_OP_FAIL        5
#define NBDIFF_CRC_OP_FAIL          6


enum {
    NBPATCH_ST_FAIL=-4,
    NBPATCH_PARAM_FAIL=-3,
    NBPATCH_DIFF_FAIL=-2,
    NBPATCH_SIZE_FAIL=-1,
};

off_t nbpatch(unsigned long old_t, off_t old_size, const unsigned long new_t, off_t new_size, off_t splict_size);

int nbpatch_error(unsigned long src);

int nbpatch_eof(unsigned long src);

int nbpatch_read(unsigned long src, const unsigned char  *buffer, unsigned long pos, size_t size, u_char is_old);

int nbpatch_write(unsigned long dst, const unsigned char *buffer, unsigned long pos, size_t size);

int nbpatch_main();

off_t offtin(u_char *buf) ;

PatchStatus *nbpatch_get_pstatus(void);

int save_patch_status(PatchStatus *status);

int read_patch_status(PatchStatus *status);

off_t save_bakeup_data(unsigned long src, off_t bsize);

off_t load_bakeup_data(unsigned long dst, off_t size, off_t offset);

void nbpatch_ota_addr_free(off_t range);
off_t nbpatch_ota_addr_get(off_t old_offset);
int nbpatch_flash_status_check(uint32_t addr);
void nbpatch_ota_status_set(off_t offset, uint32_t status);
void nbpatch_ota_addr_set(off_t old_offset, off_t new_offset);
off_t nbpatch_find_free_space(void);
int nbpatch_flash_status_check(uint32_t addr);

void nbpatch_buffer_init();

#endif /* NBPATCH_H_ */
