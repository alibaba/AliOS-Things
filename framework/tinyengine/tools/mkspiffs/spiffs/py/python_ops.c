#include <stdlib.h>
#include <stdio.h>

#include "spiffs.h"

void *my_spiffs_mount(int phys_size,
                      int phys_addr,
                      int phys_erase_block,
                      int log_page_size,
                      int log_block_size,
                      s32_t (read_cb)(u32_t addr, u32_t size, u8_t *dst),
                      s32_t (write_cb)(u32_t addr, u32_t size, u8_t *src),
                      s32_t (erase_cb)(u32_t addr, u32_t size)
                      )
{

#define WORK_BUF_SIZE (log_page_size*2)
#define SPIFFS_FDS_SIZE (32*4)
#define SPIFFS_CACHE_BUF_SIZE (phys_size) //(log_page_size+32)*128)

	struct alloced {
		spiffs fs;
		uint8_t spiffs_work_buf[WORK_BUF_SIZE];
		uint8_t spiffs_fds[SPIFFS_FDS_SIZE];
		uint8_t spiffs_cache_buf[SPIFFS_CACHE_BUF_SIZE];
	};

	struct alloced *d = malloc(sizeof(struct alloced));
	spiffs *pfs = &d->fs;

	spiffs_config cfg;
	cfg.phys_size = phys_size; // use all spi flash
	cfg.phys_addr = phys_addr; // start spiffs at start of spi flash
	cfg.phys_erase_block = phys_erase_block; // according to datasheet
	cfg.log_block_size = log_block_size; // let us not complicate things
	cfg.log_page_size = log_page_size; // as we said

	cfg.hal_read_f = read_cb;
	cfg.hal_write_f = write_cb;
	cfg.hal_erase_f = erase_cb;

	int res = SPIFFS_mount(pfs,
	                       &cfg,
	                       d->spiffs_work_buf,
	                       d->spiffs_fds,
	                       SPIFFS_FDS_SIZE,
	                       d->spiffs_cache_buf,
	                       SPIFFS_CACHE_BUF_SIZE,
	                       0);

	return res?NULL:(void *)pfs;
}

int my_spiffs_umount(spiffs *fs)
{
	SPIFFS_clearerr(fs);
	SPIFFS_unmount(fs);

	int ret = SPIFFS_errno(fs);
	free(fs);

	return ret;
}

int my_dir(spiffs *fs,
           void (entry_cb)(char *name, int size, int id))
{

	spiffs_DIR d;
	struct spiffs_dirent e;
	struct spiffs_dirent *pe = &e;

	int ret;

	SPIFFS_clearerr(fs);

	if (NULL==SPIFFS_opendir(fs, "/", &d)) goto done;

	while ((pe = SPIFFS_readdir(&d, pe))) {
		entry_cb(pe->name, pe->size, pe->obj_id);
	}

	if (SPIFFS_closedir(&d)) goto done;
	SPIFFS_clearerr(fs);

 done:
	return SPIFFS_errno(fs);
}
