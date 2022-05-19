/*
 * test_spiffs.c
 *
 *  Created on: Jun 19, 2013
 *      Author: petera
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "params_test.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"

#include "testrunner.h"

#include "test_spiffs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#define AREA(x) _area[(x) - addr_offset]

static u32_t _area_sz;
static unsigned char *_area = NULL;
static u32_t addr_offset = 0;

static int *_erases;
static char _path[256];
static u32_t bytes_rd = 0;
static u32_t bytes_wr = 0;
static u32_t reads = 0;
static u32_t writes = 0;
static u32_t error_after_bytes_written = 0;
static u32_t error_after_bytes_read = 0;
static char error_after_bytes_written_once_only = 0;
static char error_after_bytes_read_once_only = 0;
static char log_flash_ops = 1;
static u32_t fs_check_fixes = 0;
static u32_t _fs_locks;

spiffs __fs;
static u8_t *_work = NULL;
static u8_t *_fds = NULL;
static u32_t _fds_sz;
static u8_t *_cache = NULL;
static u32_t _cache_sz;

static int check_valid_flash = 1;

#ifndef TEST_PATH
#define TEST_PATH "/dev/shm/spiffs/test-data/"
#endif

// taken from http://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
// thanks Jonathan Leffler

static int do_mkdir(const char *path, mode_t mode)
{
  struct stat st;
  int status = 0;

  if (stat(path, &st) != 0) {
    /* Directory does not exist. EEXIST for race condition */
    if (mkdir(path, mode) != 0 && errno != EEXIST) {
      status = -1;
    }
  } else if (!S_ISDIR(st.st_mode)) {
    errno = ENOTDIR;
    status = -1;
  }

  return status;
}

/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
static int mkpath(const char *path, mode_t mode) {
  char *pp;
  char *sp;
  int status;
  char *copypath = strdup(path);

  status = 0;
  pp = copypath;
  while (status == 0 && (sp = strchr(pp, '/')) != 0) {
    if (sp != pp)  {
      /* Neither root nor double slash in path */
      *sp = '\0';
      status = do_mkdir(copypath, mode);
      *sp = '/';
    }
    pp = sp + 1;
  }
  if (status == 0) {
      status = do_mkdir(path, mode);
  }
  free(copypath);
  return status;
}

// end take
//
//
char *make_test_fname(const char *name) {
  sprintf(_path, "%s/%s", TEST_PATH, name);
  return _path;
}

void create_test_path(void) {
  if (mkpath(TEST_PATH, 0755)) {
    printf("could not create path %s\n", TEST_PATH);
    exit(1);
  }
}

void clear_test_path() {
  DIR *dp;
  struct dirent *ep;
  dp = opendir(TEST_PATH);

  if (dp != NULL) {
    while ((ep = readdir(dp))) {
      if (ep->d_name[0] != '.') {
        sprintf(_path, "%s/%s", TEST_PATH, ep->d_name);
        remove(_path);
      }
    }
    closedir(dp);
  }
}

static s32_t _read(
#if SPIFFS_HAL_CALLBACK_EXTRA
    spiffs *fs,
#endif
    u32_t addr, u32_t size, u8_t *dst) {
  //printf("rd @ addr %08x => %p\n", addr, &AREA(addr));
  if (log_flash_ops) {
    bytes_rd += size;
    reads++;
    if (error_after_bytes_read > 0 && bytes_rd >= error_after_bytes_read) {
      if (error_after_bytes_read_once_only) {
        error_after_bytes_read = 0;
      }
      return SPIFFS_ERR_TEST;
    }
  }
  if (addr < SPIFFS_CFG_PHYS_ADDR(&__fs)) {
    printf("FATAL read addr too low %08x < %08x\n", addr, SPIFFS_PHYS_ADDR);
    ERREXIT();
    return -1;
  }
  if (addr + size > SPIFFS_CFG_PHYS_ADDR(&__fs) + SPIFFS_CFG_PHYS_SZ(&__fs)) {
    printf("FATAL read addr too high %08x + %08x > %08x\n", addr, size, SPIFFS_PHYS_ADDR + SPIFFS_FLASH_SIZE);
    ERREXIT();
    return -1;
  }
  memcpy(dst, &AREA(addr), size);
  return 0;
}

static s32_t _write(
#if SPIFFS_HAL_CALLBACK_EXTRA
    spiffs *fs,
#endif
    u32_t addr, u32_t size, u8_t *src) {
  int i;
  //printf("wr %08x %i\n", addr, size);
  if (log_flash_ops) {
    bytes_wr += size;
    writes++;
    if (error_after_bytes_written > 0 && bytes_wr >= error_after_bytes_written) {
      if (error_after_bytes_written_once_only) {
        error_after_bytes_written = 0;
      }
      return SPIFFS_ERR_TEST;
    }
  }

  if (addr < SPIFFS_CFG_PHYS_ADDR(&__fs)) {
    printf("FATAL write addr too low %08x < %08x\n", addr, SPIFFS_PHYS_ADDR);
    ERREXIT();
    return -1;
  }
  if (addr + size > SPIFFS_CFG_PHYS_ADDR(&__fs) + SPIFFS_CFG_PHYS_SZ(&__fs)) {
    printf("FATAL write addr too high %08x + %08x > %08x\n", addr, size, SPIFFS_PHYS_ADDR + SPIFFS_FLASH_SIZE);
    ERREXIT();
    return -1;
  }

  for (i = 0; i < size; i++) {
    if (((addr + i) & (SPIFFS_CFG_LOG_PAGE_SZ(&__fs)-1)) != offsetof(spiffs_page_header, flags)) {
      if (check_valid_flash && ((AREA(addr + i) ^ src[i]) & src[i])) {
        printf("trying to write %02x to %02x at addr %08x (as part of writing %d bytes to addr %08x)\n", src[i], AREA(addr + i), addr+i, size, addr);
        spiffs_page_ix pix = (addr + i) / SPIFFS_CFG_LOG_PAGE_SZ(&__fs);
        dump_page(&__fs, pix);
	ERREXIT();
        return -1;
      }
    }
    AREA(addr + i) &= src[i];
  }
  return 0;
}
static s32_t _erase(
#if SPIFFS_HAL_CALLBACK_EXTRA
    spiffs *fs,
#endif
    u32_t addr, u32_t size) {
  if (addr & (SPIFFS_CFG_PHYS_ERASE_SZ(&__fs)-1)) {
    printf("trying to erase at addr %08x, out of boundary\n", addr);
    ERREXIT();
    return -1;
  }
  if (size & (SPIFFS_CFG_PHYS_ERASE_SZ(&__fs)-1)) {
    printf("trying to erase at with size %08x, out of boundary\n", size);
    ERREXIT();
    return -1;
  }
  _erases[(addr-SPIFFS_CFG_PHYS_ADDR(&__fs))/SPIFFS_CFG_PHYS_ERASE_SZ(&__fs)]++;
  memset(&AREA(addr), 0xff, size);
  return 0;
}

void hexdump_mem(u8_t *b, u32_t len) {
  while (len--) {
    if ((((intptr_t)b)&0x1f) == 0) {
      printf("\n");
    }
    printf("%02x", *b++);
  }
  printf("\n");
}

void hexdump(u32_t addr, u32_t len) {
  int remainder = (addr % 32) == 0 ? 0 : 32 - (addr % 32);
  u32_t a;
  for (a = addr - remainder; a < addr+len; a++) {
    if ((a & 0x1f) == 0) {
      if (a != addr) {
        printf("  ");
        int j;
        for (j = 0; j < 32; j++) {
          if (a-32+j < addr)
            printf(" ");
          else {
            printf("%c", (AREA(a-32+j) < 32 || AREA(a-32+j) >= 0x7f) ? '.' : AREA(a-32+j));
          }
        }
      }
      printf("%s    %08x: ", a<=addr ? "":"\n", a);
    }
    if (a < addr) {
      printf("  ");
    } else {
      printf("%02x", AREA(a));
    }
  }
  int j;
  printf("  ");
  for (j = 0; j < 32; j++) {
    if (a-32+j < addr)
      printf(" ");
    else {
      printf("%c", (AREA(a-32+j) < 32 || AREA(a-32+j) >= 0x7f) ? '.' : AREA(a-32+j));
    }
  }
  printf("\n");
}

void dump_page(spiffs *fs, spiffs_page_ix p) {
  printf("page %04x  ", p);
  u32_t addr = SPIFFS_PAGE_TO_PADDR(fs, p);
  if (p % SPIFFS_PAGES_PER_BLOCK(fs) < SPIFFS_OBJ_LOOKUP_PAGES(fs)) {
    // obj lu page
    printf("OBJ_LU");
  } else {
    u32_t obj_id_addr = SPIFFS_BLOCK_TO_PADDR(fs, SPIFFS_BLOCK_FOR_PAGE(fs , p)) +
        SPIFFS_OBJ_LOOKUP_ENTRY_FOR_PAGE(fs, p) * sizeof(spiffs_obj_id);
    spiffs_obj_id obj_id = *((spiffs_obj_id *)&AREA(obj_id_addr));
    // data page
    spiffs_page_header *ph = (spiffs_page_header *)&AREA(addr);
    printf("DATA %04x:%04x  ", obj_id, ph->span_ix);
    printf("%s", ((ph->flags & SPIFFS_PH_FLAG_FINAL) == 0) ? "FIN " : "fin ");
    printf("%s", ((ph->flags & SPIFFS_PH_FLAG_DELET) == 0) ? "DEL " : "del ");
    printf("%s", ((ph->flags & SPIFFS_PH_FLAG_INDEX) == 0) ? "IDX " : "idx ");
    printf("%s", ((ph->flags & SPIFFS_PH_FLAG_USED) == 0) ? "USD " : "usd ");
    printf("%s  ", ((ph->flags & SPIFFS_PH_FLAG_IXDELE) == 0) ? "IDL " : "idl ");
    if (obj_id & SPIFFS_OBJ_ID_IX_FLAG) {
      // object index
      printf("OBJ_IX");
      if (ph->span_ix == 0) {
        printf("_HDR  ");
        spiffs_page_object_ix_header *oix_hdr = (spiffs_page_object_ix_header *)&AREA(addr);
        printf("'%s'  %i bytes  type:%02x", oix_hdr->name, oix_hdr->size, oix_hdr->type);
      }
    } else {
      // data page
      printf("CONTENT");
    }
  }
  printf("\n");
  u32_t len = SPIFFS_CFG_LOG_PAGE_SZ(fs);
  hexdump(addr, len);
}

void area_write(u32_t addr, u8_t *buf, u32_t size) {
  int i;
  for (i = 0; i < size; i++) {
    AREA(addr + i) = *buf++;
  }
}

void area_set(u32_t addr, u8_t d, u32_t size) {
  int i;
  for (i = 0; i < size; i++) {
    AREA(addr + i) = d;
  }
}

void area_read(u32_t addr, u8_t *buf, u32_t size) {
  int i;
  for (i = 0; i < size; i++) {
    *buf++ = AREA(addr + i);
  }
}

void dump_erase_counts(spiffs *fs) {
  spiffs_block_ix bix;
  spiffs_block_ix bix_offs;
  printf("  BLOCK     |\n");
  printf("   AGE COUNT|\n");
  for (bix_offs = 0; bix_offs < fs->block_count; bix_offs+=8) {
    for (bix = bix_offs; bix < bix_offs+8 && bix < fs->block_count; bix++) {
      printf("----%3i ----|", bix);
    }
    printf("\n");
    for (bix = bix_offs; bix < bix_offs+8 && bix < fs->block_count; bix++) {
      spiffs_obj_id erase_mark;
      _spiffs_rd(fs, 0, 0, SPIFFS_ERASE_COUNT_PADDR(fs, bix), sizeof(spiffs_obj_id), (u8_t *)&erase_mark);
      if (_erases[bix] == 0) {
        printf("            |");
      } else {
        printf("%7i %4i|", (fs->max_erase_count - erase_mark), _erases[bix]);
      }
    }
    printf("\n");
  }
}

void dump_flash_access_stats() {
  printf("  RD: %10i reads  %10i bytes %10i avg bytes/read\n", reads, bytes_rd, reads == 0 ? 0 : (bytes_rd / reads));
  printf("  WR: %10i writes %10i bytes %10i avg bytes/write\n", writes, bytes_wr, writes == 0 ? 0 : (bytes_wr / writes));
}


static int check_cb_count;
// static u32_t old_perc = 999;
static void spiffs_check_cb_f(
#if SPIFFS_HAL_CALLBACK_EXTRA
    spiffs *fs,
#endif
    spiffs_check_type type, spiffs_check_report report,
    u32_t arg1, u32_t arg2) {
/*  if (report == SPIFFS_CHECK_PROGRESS && old_perc != arg1) {
    old_perc = arg1;
    printf("CHECK REPORT: ");
    switch(type) {
    case SPIFFS_CHECK_LOOKUP:
      printf("LU "); break;
    case SPIFFS_CHECK_INDEX:
      printf("IX "); break;
    case SPIFFS_CHECK_PAGE:
      printf("PA "); break;
    }
    printf("%i%%\n", arg1 * 100 / 256);
  }*/
  if (report != SPIFFS_CHECK_PROGRESS) {
    check_cb_count++;
    if (report != SPIFFS_CHECK_ERROR) fs_check_fixes++;
    printf("   check: ");
    switch (type) {
    case SPIFFS_CHECK_INDEX:
      printf("INDEX  "); break;
    case SPIFFS_CHECK_LOOKUP:
      printf("LOOKUP "); break;
    case SPIFFS_CHECK_PAGE:
      printf("PAGE   "); break;
    default:
      printf("????   "); break;
    }
    if (report == SPIFFS_CHECK_ERROR) {
      printf("ERROR %i", arg1);
    } else if (report == SPIFFS_CHECK_DELETE_BAD_FILE) {
      printf("DELETE BAD FILE %04x", arg1);
    } else if (report == SPIFFS_CHECK_DELETE_ORPHANED_INDEX) {
      printf("DELETE ORPHANED INDEX %04x", arg1);
    } else if (report == SPIFFS_CHECK_DELETE_PAGE) {
      printf("DELETE PAGE %04x", arg1);
    } else if (report == SPIFFS_CHECK_FIX_INDEX) {
      printf("FIX INDEX %04x:%04x", arg1, arg2);
    } else if (report == SPIFFS_CHECK_FIX_LOOKUP) {
      printf("FIX INDEX %04x:%04x", arg1, arg2);
    } else {
      printf("??");
    }
    printf("\n");
  }
}

void fs_set_addr_offset(u32_t offset) {
  addr_offset = offset;
}

void test_lock(spiffs *fs) {
  if (_fs_locks != 0) {
    printf("FATAL: reentrant locks. Abort.\n");
    ERREXIT();
    exit(-1);
  }
  _fs_locks++;
}

void test_unlock(spiffs *fs) {
  if (_fs_locks != 1) {
    printf("FATAL: unlocking unlocked. Abort.\n");
    ERREXIT();
    exit(-1);
  }
  _fs_locks--;
}

s32_t fs_mount_specific(u32_t phys_addr, u32_t phys_size,
    u32_t phys_sector_size,
    u32_t log_block_size, u32_t log_page_size) {
  spiffs_config c;
  c.hal_erase_f = _erase;
  c.hal_read_f = _read;
  c.hal_write_f = _write;
#if SPIFFS_SINGLETON == 0
  c.log_block_size = log_block_size;
  c.log_page_size = log_page_size;
  c.phys_addr = phys_addr;
  c.phys_erase_block = phys_sector_size;
  c.phys_size = phys_size;
#endif
#if SPIFFS_FILEHDL_OFFSET
  c.fh_ix_offset = TEST_SPIFFS_FILEHDL_OFFSET;
#endif
  return SPIFFS_mount(&__fs, &c, _work, _fds, _fds_sz, _cache, _cache_sz, spiffs_check_cb_f);
}

static void fs_create(u32_t spiflash_size,
    u32_t phys_sector_size,
    u32_t log_page_size,
    u32_t descriptors, u32_t cache_pages) {
  _area_sz = spiflash_size;
  _area = malloc(spiflash_size);
  ASSERT(_area != NULL, "testbench area could not be malloced");

  const u32_t erase_sz = sizeof(int) * (spiflash_size / phys_sector_size);
  _erases = malloc(erase_sz);
  ASSERT(_erases != NULL, "testbench erase log could not be malloced");
  memset(_erases, 0, erase_sz);

  _fds_sz = descriptors * sizeof(spiffs_fd);
  _fds = malloc(_fds_sz);
  ASSERT(_fds != NULL, "testbench fd buffer could not be malloced");
  memset(_fds, 0, _fds_sz);

#if SPIFFS_CACHE
  _cache_sz = sizeof(spiffs_cache) + cache_pages * (sizeof(spiffs_cache_page) + log_page_size);
  _cache = malloc(_cache_sz);
  ASSERT(_cache != NULL, "testbench cache could not be malloced");
  memset(_cache, 0, _cache_sz);
#endif

  const u32_t work_sz = log_page_size * 2;
  _work = malloc(work_sz);
  ASSERT(_work != NULL, "testbench work buffer could not be malloced");
  memset(_work, 0, work_sz);
}

static void fs_free(void) {
  if (_area) free(_area);
  _area = NULL;
  if (_erases) free(_erases);
  _erases = NULL;
  if (_fds) free(_fds);
  _fds = NULL;
  if (_cache) free(_cache);
  _cache = NULL;
  if (_work) free(_work);
  _work = NULL;
}

/**
 * addr_offset
 */
void fs_reset_specific(u32_t addr_offset, u32_t phys_addr, u32_t phys_size,
    u32_t phys_sector_size,
    u32_t log_block_size, u32_t log_page_size) {
  fs_create(phys_size + phys_addr - addr_offset,
            phys_sector_size,
            log_page_size,
            DEFAULT_NUM_FD,
            DEFAULT_NUM_CACHE_PAGES);
  fs_set_addr_offset(addr_offset);
  memset(&AREA(addr_offset), 0xcc, _area_sz);
  memset(&AREA(phys_addr), 0xff, phys_size);
  memset(&__fs, 0, sizeof(__fs));

  s32_t res = fs_mount_specific(phys_addr, phys_size, phys_sector_size, log_block_size, log_page_size);

#if SPIFFS_USE_MAGIC
  if (res == SPIFFS_OK) {
    SPIFFS_unmount(&__fs);
  }
  res = SPIFFS_format(&__fs);
  if (res != SPIFFS_OK) {
    printf("format failed, %i\n", SPIFFS_errno(&__fs));
  }
  res = fs_mount_specific(phys_addr, phys_size, phys_sector_size, log_block_size, log_page_size);
  if (res != SPIFFS_OK) {
    printf("mount failed, %i\n", SPIFFS_errno(&__fs));
  }
#endif

  clear_flash_ops_log();
  log_flash_ops = 1;
  fs_check_fixes = 0;
}

void fs_reset() {
  fs_reset_specific(0, SPIFFS_PHYS_ADDR, SPIFFS_FLASH_SIZE, SECTOR_SIZE, LOG_BLOCK, LOG_PAGE);
}

void fs_store_dump(char *fname) {
  int pfd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  ASSERT(pfd > 0, "could not open dump file");
  write(pfd, _area, _area_sz);
  close(pfd);
}

void fs_load_dump(char *fname) {
  int pfd = open(fname, O_RDONLY, S_IRUSR | S_IWUSR);
  ASSERT(pfd > 0, "could not load dump");
  read(pfd, _area, _area_sz);
  close(pfd);
}

void fs_mount_dump(char *fname,
    u32_t addr_offset, u32_t phys_addr, u32_t phys_size,
        u32_t phys_sector_size,
        u32_t log_block_size, u32_t log_page_size) {
  fs_create(phys_size + phys_addr - addr_offset,
            phys_sector_size,
            log_page_size,
            DEFAULT_NUM_FD,
            DEFAULT_NUM_CACHE_PAGES);
  fs_set_addr_offset(addr_offset);
  memset(&AREA(addr_offset), 0xcc, _area_sz);
  memset(&AREA(phys_addr), 0xff, phys_size);
  memset(&__fs, 0, sizeof(__fs));

  fs_load_dump(fname);

  s32_t res = fs_mount_specific(phys_addr, phys_size, phys_sector_size, log_block_size, log_page_size);

  ASSERT(res == SPIFFS_OK, "failed mounting dump, check settings");

  clear_flash_ops_log();
  log_flash_ops = 1;
  fs_check_fixes = 0;
}

void set_flash_ops_log(int enable) {
  log_flash_ops = enable;
}

void clear_flash_ops_log() {
  bytes_rd = 0;
  bytes_wr = 0;
  reads = 0;
  writes = 0;
  error_after_bytes_read = 0;
  error_after_bytes_written = 0;
}

u32_t get_flash_ops_log_read_bytes() {
  return bytes_rd;
}

u32_t get_flash_ops_log_write_bytes() {
  return bytes_wr;
}

void invoke_error_after_read_bytes(u32_t b, char once_only) {
  error_after_bytes_read = b;
  error_after_bytes_read_once_only = once_only;
}
void invoke_error_after_write_bytes(u32_t b, char once_only) {
  error_after_bytes_written = b;
  error_after_bytes_written_once_only = once_only;
}

void fs_set_validate_flashing(int i) {
  check_valid_flash = i;
}

void real_assert(int c, const char *n, const char *file, int l) {
  if (c == 0) {
    printf("ASSERT: %s %s @ %i\n", (n ? n : ""), file, l);
    printf("fs errno:%i\n", __fs.err_code);
    exit(0);
  }
}

int read_and_verify(char *name) {
  int fd = SPIFFS_open(&__fs, name, SPIFFS_RDONLY, 0);
  if (fd < 0) {
    printf("  read_and_verify: could not open file %s\n", name);
    return fd;
  }
  return read_and_verify_fd(fd, name);
}

int read_and_verify_fd(spiffs_file fd, char *name) {
  s32_t res;
  int pfd = open(make_test_fname(name), O_RDONLY);
  spiffs_stat s;
  res = SPIFFS_fstat(&__fs, fd, &s);
  if (res < 0) {
    printf("  read_and_verify: could not stat file %s\n", name);
    return res;
  }

  off_t fsize = lseek(pfd, 0, SEEK_END);
  if (s.size != fsize) {
    printf("  read_and_verify: size differs, %s spiffs:%d!=fs:%ld\n", name, s.size, fsize);
    return -1;
  }
  lseek(pfd, 0, SEEK_SET);

  if (s.size == 0) {
    SPIFFS_close(&__fs, fd);
    close(pfd);
    return 0;
  }

  //printf("verifying %s, len %i\n", name, s.size);
  int offs = 0;
  u8_t buf_d[256];
  u8_t buf_v[256];
  while (offs < s.size) {
    int read_len = MIN(s.size - offs, sizeof(buf_d));
    res = SPIFFS_read(&__fs, fd, buf_d, read_len);
    if (res < 0) {
      printf("  read_and_verify: could not read file %s offs:%i len:%i filelen:%i\n", name, offs, read_len, s.size);
      return res;
    }
    int pres = read(pfd, buf_v, read_len);
    (void)pres;
    //printf("reading offs:%i len:%i spiffs_res:%i posix_res:%i\n", offs, read_len, res, pres);
    int i;
    int veri_ok = 1;
    for (i = 0; veri_ok && i < read_len; i++) {
      if (buf_d[i] != buf_v[i]) {
        printf("file verification mismatch @ %i, %02x %c != %02x %c\n", offs+i, buf_d[i], buf_d[i], buf_v[i], buf_v[i]);
        int j = MAX(0, i-16);
        int k = MIN(sizeof(buf_d), i+16);
        k = MIN(s.size-offs, k);
        int l;
        for (l = j; l < k; l++) {
          printf("%c", buf_d[l] > 31 ? buf_d[l] : '.');
        }
        printf("\n");
        for (l = j; l < k; l++) {
          printf("%c", buf_v[l] > 31 ? buf_v[l] : '.');
        }
        printf("\n");
        veri_ok = 0;
      }
    }
    if (!veri_ok) {
      SPIFFS_close(&__fs, fd);
      close(pfd);
      printf("data mismatch\n");
      return -1;
    }

    offs += read_len;
  }

  SPIFFS_close(&__fs, fd);
  close(pfd);

  return 0;
}

static void test_on_stop(test *t) {
  printf("  spiffs errno:%i\n", SPIFFS_errno(&__fs));
#if SPIFFS_TEST_VISUALISATION
  if (_area) SPIFFS_vis(FS);
#endif

}

void memrand(u8_t *b, int len) {
  int i;
  for (i = 0; i < len; i++) {
    b[i] = rand();
  }
}

int test_create_file(char *name) {
  spiffs_stat s;
  spiffs_file fd;
  int res = SPIFFS_creat(FS, name, 0);
  CHECK_RES(res);
  fd = SPIFFS_open(FS, name, SPIFFS_RDONLY, 0);
  CHECK(fd >= 0);
  res = SPIFFS_fstat(FS, fd, &s);
  CHECK_RES(res);
  CHECK(strcmp((char*)s.name, name) == 0);
  CHECK(s.size == 0);
#if SPIFFS_OBJ_META_LEN
  {
    int i;
    for (i = 0; i < SPIFFS_OBJ_META_LEN; i++) {
      CHECK(s.meta[i] == 0xff);
    }
  }
#endif
  SPIFFS_close(FS, fd);
  return 0;
}

int test_create_and_write_file(char *name, int size, int chunk_size) {
  int res;
  spiffs_file fd;
  printf("    create and write %s", name);
  res = test_create_file(name);
  if (res < 0) {
    printf(" failed creation, %i\n",res);
  }
  CHECK(res >= 0);
  fd = SPIFFS_open(FS, name, SPIFFS_APPEND | SPIFFS_RDWR, 0);
  if (fd < 0) {
    printf(" failed open, %i\n",res);
  }
  CHECK(fd >= 0);
  int pfd = open(make_test_fname(name), O_APPEND | O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  int offset = 0;
  int mark = 0;
  while (offset < size) {
    int len = MIN(size-offset, chunk_size);
    if (offset > mark) {
      mark += size/16;
      printf(".");
      fflush(stdout);
    }
    u8_t *buf = malloc(len);
    memrand(buf, len);
    res = SPIFFS_write(FS, fd, buf, len);
    write(pfd, buf, len);
    free(buf);
    if (res < 0) {
      printf("\n  error @ offset %i, res %i\n", offset, res);
    }
    offset += len;
    CHECK(res >= 0);
  }
  printf("\n");
  close(pfd);

  spiffs_stat stat;
  res = SPIFFS_fstat(FS, fd, &stat);
  if (res < 0) {
    printf(" failed fstat, %i\n",res);
  }
  CHECK(res >= 0);
  if (stat.size != size) {
    printf(" failed size, %i != %i\n", stat.size, size);
  }
  CHECK(stat.size == size);

  SPIFFS_close(FS, fd);
  return 0;
}

static u32_t crc32_tab[] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
  0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
  0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
  0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
  0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
  0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
  0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
  0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
  0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
  0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
  0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
  0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
  0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
  0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
  0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
  0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
  0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
  0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
  0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
  0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
  0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
  0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static u32_t crc32(u32_t crc, const void *buf, size_t size)
{
  const u8_t *p;

  p = buf;
  crc = crc ^ ~0U;

  while (size--)
    crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

  return crc ^ ~0U;
}

u32_t get_spiffs_file_crc_by_fd(spiffs_file fd) {
  s32_t res;
  u32_t crc = 0;
  u8_t buf[256];

  ASSERT(SPIFFS_lseek(FS, fd, 0, SPIFFS_SEEK_SET) >= 0, "could not seek to start of file");

  while ((res = SPIFFS_read(FS, fd, buf, sizeof(buf))) > SPIFFS_OK) {
    crc = crc32(crc, buf, res);
  }
  ASSERT(SPIFFS_errno(FS) == SPIFFS_ERR_END_OF_OBJECT || SPIFFS_errno(FS) == SPIFFS_OK, "failed reading file");

  return crc;
}

u32_t get_spiffs_file_crc(char *name) {
  s32_t res;
  spiffs_file fd;
  fd = SPIFFS_open(FS, name, SPIFFS_O_RDONLY, 0);
  ASSERT(fd >= 0, "Could not open file");
  u32_t crc = get_spiffs_file_crc_by_fd(fd);
  res = SPIFFS_close(FS, fd);
  ASSERT(res >= SPIFFS_OK, "failing closing file");
  return crc;
}

#if SPIFFS_CACHE
#if SPIFFS_CACHE_STATS
static u32_t chits_tot = 0;
static u32_t cmiss_tot = 0;
#endif
#endif

void _setup_test_only() {
  create_test_path();
  fs_set_validate_flashing(1);
  test_init(test_on_stop);
}

void _setup() {
  _fs_locks = 0;
  fs_reset();
  _setup_test_only();
}

void _teardown() {
  printf("  free blocks     : %i of %i\n", (FS)->free_blocks, (FS)->block_count);
  printf("  pages allocated : %i\n", (FS)->stats_p_allocated);
  printf("  pages deleted   : %i\n", (FS)->stats_p_deleted);
#if SPIFFS_GC_STATS
  printf("  gc runs         : %i\n", (FS)->stats_gc_runs);
#endif
#if SPIFFS_CACHE
#if SPIFFS_CACHE_STATS
  chits_tot += (FS)->cache_hits;
  cmiss_tot += (FS)->cache_misses;
  printf("  cache hits      : %i (sum %i)\n", (FS)->cache_hits, chits_tot);
  printf("  cache misses    : %i (sum %i)\n", (FS)->cache_misses, cmiss_tot);
  printf("  cache utiliz    : %f\n", ((float)chits_tot/(float)(chits_tot + cmiss_tot)));
  chits_tot = 0;
  cmiss_tot = 0;
#endif
#endif
  if (_area) {
    dump_flash_access_stats();
    clear_flash_ops_log();
#if SPIFFS_GC_STATS
    if ((FS)->stats_gc_runs > 0)
#endif
    dump_erase_counts(FS);
    printf("  fs consistency check output begin\n");
    check_cb_count = 0;
    SPIFFS_check(FS);
    printf("  fs consistency check output end\n");
    if (check_cb_count) {
      ERREXIT();
    }
  }
  clear_test_path();
  fs_free();
  printf("  locks : %i\n", _fs_locks);
  if (_fs_locks != 0) {
    printf("FATAL: lock asymmetry. Abort.\n");
    ERREXIT();
    exit(-1);
  }
}

u32_t tfile_get_size(tfile_size s) {
  switch (s) {
  case EMPTY:
    return 0;
  case SMALL: // half a data page
    return SPIFFS_DATA_PAGE_SIZE(FS)/2;
  case MEDIUM: // one block
    return SPIFFS_DATA_PAGE_SIZE(FS) * (SPIFFS_PAGES_PER_BLOCK(FS) - SPIFFS_OBJ_LOOKUP_PAGES(FS));
  case LARGE: // third of fs
    return SPIFFS_DATA_PAGE_SIZE(FS) * (SPIFFS_PAGES_PER_BLOCK(FS) - SPIFFS_OBJ_LOOKUP_PAGES(FS)) * (FS)->block_count/3;
  }
  return 0;
}

int run_file_config(int cfg_count, tfile_conf* cfgs, int max_runs, int max_concurrent_files, int dbg) {
  int res;
  tfile *tfiles = malloc(sizeof(tfile) * max_concurrent_files);
  memset(tfiles, 0, sizeof(tfile) * max_concurrent_files);
  int run = 0;
  int cur_config_ix = 0;
  char name[32];
  while (run < max_runs)  {
    if (dbg) printf(" run %i/%i\n", run, max_runs);
    int i;
    for (i = 0; i < max_concurrent_files; i++) {
      sprintf(name, "file%i_%i", (1+run), i);
      tfile *tf = &tfiles[i];
      if (tf->state == 0 && cur_config_ix < cfg_count) {
// create a new file
        strcpy(tf->name, name);
        tf->state = 1;
        tf->cfg = cfgs[cur_config_ix];
        int size = tfile_get_size(tf->cfg.tsize);
        if (dbg) printf("   create new %s with cfg %i/%i, size %i\n", name, (1+cur_config_ix), cfg_count, size);

        if (tf->cfg.tsize == EMPTY) {
          res = SPIFFS_creat(FS, name, 0);
          CHECK_RES(res);
          int pfd = open(make_test_fname(name), O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
          close(pfd);
          int extra_flags = tf->cfg.ttype == APPENDED ? SPIFFS_APPEND : 0;
          spiffs_file fd = SPIFFS_open(FS, name, extra_flags | SPIFFS_RDWR, 0);
          CHECK(fd > 0);
          tf->fd = fd;
        } else {
          int extra_flags = tf->cfg.ttype == APPENDED ? SPIFFS_APPEND : 0;
          spiffs_file fd = SPIFFS_open(FS, name, extra_flags | SPIFFS_TRUNC | SPIFFS_CREAT | SPIFFS_RDWR, 0);
          CHECK(fd > 0);
          extra_flags = tf->cfg.ttype == APPENDED ? O_APPEND : 0;
          int pfd = open(make_test_fname(name), extra_flags | O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
          tf->fd = fd;
          u8_t *buf = malloc(size);
          memrand(buf, size);
          res = SPIFFS_write(FS, fd, buf, size);
          CHECK_RES(res);
          write(pfd, buf, size);
          close(pfd);
          free(buf);
          res = read_and_verify(name);
          CHECK_RES(res);
        }

        cur_config_ix++;
      } else if (tf->state > 0) {
// hande file lifecycle
        switch (tf->cfg.ttype) {
        case UNTAMPERED: {
          break;
        }
        case APPENDED: {
          if (dbg) printf("   appending %s\n", tf->name);
          int size = SPIFFS_DATA_PAGE_SIZE(FS)*3;
          u8_t *buf = malloc(size);
          memrand(buf, size);
          res = SPIFFS_write(FS, tf->fd, buf, size);
          CHECK_RES(res);
          int pfd = open(make_test_fname(tf->name), O_APPEND | O_RDWR);
          write(pfd, buf, size);
          close(pfd);
          free(buf);
          res = read_and_verify(tf->name);
          CHECK_RES(res);
          break;
        }
        case MODIFIED: {
          if (dbg) printf("   modify %s\n", tf->name);
          spiffs_stat stat;
          res = SPIFFS_fstat(FS, tf->fd, &stat);
          CHECK_RES(res);
          int size = stat.size / tf->cfg.tlife + SPIFFS_DATA_PAGE_SIZE(FS)/3;
          int offs = (stat.size / tf->cfg.tlife) * tf->state;
          res = SPIFFS_lseek(FS, tf->fd, offs, SPIFFS_SEEK_SET);
          CHECK_RES(res);
          u8_t *buf = malloc(size);
          memrand(buf, size);
          res = SPIFFS_write(FS, tf->fd, buf, size);
          CHECK_RES(res);
          int pfd = open(make_test_fname(tf->name), O_RDWR);
          lseek(pfd, offs, SEEK_SET);
          write(pfd, buf, size);
          close(pfd);
          free(buf);
          res = read_and_verify(tf->name);
          CHECK_RES(res);
          break;
        }
        case REWRITTEN: {
          if (tf->fd > 0) {
            SPIFFS_close(FS, tf->fd);
          }
          if (dbg) printf("   rewriting %s\n", tf->name);
          spiffs_file fd = SPIFFS_open(FS, tf->name, SPIFFS_TRUNC | SPIFFS_CREAT | SPIFFS_RDWR, 0);
          CHECK(fd > 0);
          int pfd = open(make_test_fname(tf->name), O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
          tf->fd = fd;
          int size = tfile_get_size(tf->cfg.tsize);
          u8_t *buf = malloc(size);
          memrand(buf, size);
          res = SPIFFS_write(FS, fd, buf, size);
          CHECK_RES(res);
          write(pfd, buf, size);
          close(pfd);
          free(buf);
          res = read_and_verify(tf->name);
          CHECK_RES(res);
          break;
        }
        }
        tf->state++;
        if (tf->state > tf->cfg.tlife) {
// file outlived its time, kill it
          if (tf->fd > 0) {
            SPIFFS_close(FS, tf->fd);
          }
          if (dbg) printf("   removing %s\n", tf->name);
          res = read_and_verify(tf->name);
          CHECK_RES(res);
          res = SPIFFS_remove(FS, tf->name);
          CHECK_RES(res);
          remove(make_test_fname(tf->name));
          memset(tf, 0, sizeof(tfile));
        }

      }
    }

    run++;
  }
  free(tfiles);
  return 0;
}

int count_taken_fds(spiffs *fs) {
  int i;
  spiffs_fd *fds = (spiffs_fd *)fs->fd_space;
  int taken = 0;
  for (i = 0; i < fs->fd_count; i++) {
    spiffs_fd *cur_fd = &fds[i];
    if (cur_fd->file_nbr) taken++;
  }
  return taken;
}
