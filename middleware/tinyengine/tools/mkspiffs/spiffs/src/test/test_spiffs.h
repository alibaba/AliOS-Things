/*
 * test_spiffs.h
 *
 *  Created on: Jun 19, 2013
 *      Author: petera
 */

#ifndef TEST_SPIFFS_H_
#define TEST_SPIFFS_H_

#include "spiffs.h"

#define FS &__fs

extern spiffs __fs;


#define CHECK(r) if (!(r)) return -1;
#define CHECK_RES(r) if (r < 0) return -1;
#define FS_PURE_DATA_PAGES(fs) \
    (SPIFFS_CFG_PHYS_SZ(fs) / SPIFFS_CFG_LOG_PAGE_SZ(fs)- (fs)->block_count * SPIFFS_OBJ_LOOKUP_PAGES(fs))
#define FS_PURE_DATA_SIZE(fs) \
    FS_PURE_DATA_PAGES(fs) * SPIFFS_DATA_PAGE_SIZE(fs)

typedef enum {
  EMPTY,
  SMALL,
  MEDIUM,
  LARGE,
} tfile_size;

typedef enum {
  UNTAMPERED,
  APPENDED,
  MODIFIED,
  REWRITTEN,
} tfile_type;

typedef enum {
  SHORT = 3,
  NORMAL = 15,
  LONG = 100,
} tfile_life;

typedef struct  {
  tfile_size tsize;
  tfile_type ttype;
  tfile_life tlife;
} tfile_conf;

typedef struct  {
  int state;
  spiffs_file fd;
  tfile_conf cfg;
  char name[32];
} tfile;

void fs_reset();
void fs_reset_specific(u32_t addr_offset, u32_t phys_addr, u32_t phys_size,
    u32_t phys_sector_size,
    u32_t log_block_size, u32_t log_page_size);
s32_t fs_mount_specific(u32_t phys_addr, u32_t phys_size,
    u32_t phys_sector_size,
    u32_t log_block_size, u32_t log_page_size);
void fs_mount_dump(char *fname,
    u32_t addr_offset, u32_t phys_addr, u32_t phys_size,
        u32_t phys_sector_size,
        u32_t log_block_size, u32_t log_page_size);

void fs_store_dump(char *fname);
void fs_load_dump(char *fname);

void fs_set_addr_offset(u32_t offset);
int read_and_verify(char *name);
int read_and_verify_fd(spiffs_file fd, char *name);
void dump_page(spiffs *fs, spiffs_page_ix p);
void hexdump(u32_t addr, u32_t len);
char *make_test_fname(const char *name);
void clear_test_path();
void area_write(u32_t addr, u8_t *buf, u32_t size);
void area_set(u32_t addr, u8_t d, u32_t size);
void area_read(u32_t addr, u8_t *buf, u32_t size);
void dump_erase_counts(spiffs *fs);
void dump_flash_access_stats();
void set_flash_ops_log(int enable);
void clear_flash_ops_log();
u32_t get_flash_ops_log_read_bytes();
u32_t get_flash_ops_log_write_bytes();
void invoke_error_after_read_bytes(u32_t b, char once_only);
void invoke_error_after_write_bytes(u32_t b, char once_only);
void fs_set_validate_flashing(int i);
int get_error_count();
int count_taken_fds(spiffs *fs);

void memrand(u8_t *b, int len);
int test_create_file(char *name);
int test_create_and_write_file(char *name, int size, int chunk_size);
u32_t get_spiffs_file_crc_by_fd(spiffs_file fd);
u32_t get_spiffs_file_crc(char *name);
void _setup();
void _setup_test_only();
void _teardown();
u32_t tfile_get_size(tfile_size s);
int run_file_config(int cfg_count, tfile_conf* cfgs, int max_runs, int max_concurrent_files, int dbg);

void test_lock(spiffs *fs);
void test_unlock(spiffs *fs);

#endif /* TEST_SPIFFS_H_ */
