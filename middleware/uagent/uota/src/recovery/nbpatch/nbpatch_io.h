/**
* @author E-mail: ting.guit@alibaba-inc.com
* @version time：2017年11月30日 下午8:10:58
* nbpatch_io.h
*/
#ifndef NBPATCH_IO_H_
#define NBPATCH_IO_H_

#include "rec_pub.h"
#include "flash_hal.h"
#include "fcntl.h"
#include "assert.h"

int nbpatch_read(unsigned long src, const unsigned char  *buffer, unsigned long pos, size_t size);

int nbpatch_write(unsigned long dst, const unsigned char *buffer, unsigned long pos, size_t size);

int save_patch_status(PatchStatus *status);

int read_patch_status(PatchStatus *status);

off_t save_bakeup_data(unsigned long src, off_t bsize);

off_t load_bakeup_data(unsigned long dst, off_t size, off_t offset);

int load_file(u_char **buf, off_t *buf_len, const char *file);

int save_file(u_char *buf, off_t buf_len, size_t pos, const char *file);

#endif /* NBPATCH_IO_H_ */
