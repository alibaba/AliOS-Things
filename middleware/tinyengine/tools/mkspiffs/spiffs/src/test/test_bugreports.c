/*
 * test_bugreports.c
 *
 *  Created on: Mar 8, 2015
 *      Author: petera
 */



#include "testrunner.h"
#include "test_spiffs.h"
#include "spiffs_nucleus.h"
#include "spiffs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

/* The follow defines control details of how the fuzzer can exercise the API. If you
 * undef any of these, then the fuzzer is less brutal. FOr example, if you undef
 * HAVE_REMOVE_OPEN, then the fuzzer will not attempt to remove (or rename) an open file
 */
#define HAVE_REMOVE_OPEN
#define HAVE_MULTIPLE_OPEN
#define NO_FORCE_CHECK

SUITE(bug_tests)
static void setup() {
  _setup_test_only();
}
static void teardown() {
  _teardown();
}

TEST(nodemcu_full_fs_1) {
  fs_reset_specific(0, 0, 4096*20, 4096, 4096, 256);

  int res;
  spiffs_file fd;

  printf("  fill up system by writing one byte a lot\n");
  fd = SPIFFS_open(FS, "test1.txt", SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
  TEST_CHECK(fd > 0);
  int i;
  spiffs_stat s;
  res = SPIFFS_OK;
  for (i = 0; i < 100*1000; i++) {
    u8_t buf = 'x';
    res = SPIFFS_write(FS, fd, &buf, 1);
  }

  int errno = SPIFFS_errno(FS);
  int res2 = SPIFFS_fstat(FS, fd, &s);
  TEST_CHECK(res2 == SPIFFS_OK);
  printf("  >>> file %s size: %i\n", s.name, s.size);

  TEST_CHECK(errno == SPIFFS_ERR_FULL);
  SPIFFS_close(FS, fd);

  printf("  remove big file\n");
  res = SPIFFS_remove(FS, "test1.txt");

  printf("res:%i errno:%i\n",res, SPIFFS_errno(FS));

  TEST_CHECK(res == SPIFFS_OK);
  res2 = SPIFFS_fstat(FS, fd, &s);
  TEST_CHECK(res2 < 0);
  TEST_CHECK(SPIFFS_errno(FS) == SPIFFS_ERR_FILE_CLOSED);
  res2 = SPIFFS_stat(FS, "test1.txt", &s);
  TEST_CHECK(res2 < 0);
  TEST_CHECK(SPIFFS_errno(FS) == SPIFFS_ERR_NOT_FOUND);

  printf("  create small file\n");
  fd = SPIFFS_open(FS, "test2.txt", SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
  TEST_CHECK(fd > 0);
  res = SPIFFS_OK;
  for (i = 0; res >= 0 && i < 1000; i++) {
    u8_t buf = 'x';
    res = SPIFFS_write(FS, fd, &buf, 1);
  }
  TEST_CHECK(res >= SPIFFS_OK);

  res2 = SPIFFS_fstat(FS, fd, &s);
  TEST_CHECK(res2 == SPIFFS_OK);
  printf("  >>> file %s size: %i\n", s.name, s.size);

  TEST_CHECK(s.size == 1000);
  SPIFFS_close(FS, fd);

  return TEST_RES_OK;

} TEST_END

TEST(nodemcu_full_fs_2) {
  fs_reset_specific(0, 0, 4096*22, 4096, 4096, 256);

  int res;
  spiffs_file fd;

  printf("  fill up system by writing one byte a lot\n");
  fd = SPIFFS_open(FS, "test1.txt", SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
  TEST_CHECK(fd > 0);
  int i;
  spiffs_stat s;
  res = SPIFFS_OK;
  for (i = 0; i < 100*1000; i++) {
    u8_t buf = 'x';
    res = SPIFFS_write(FS, fd, &buf, 1);
  }

  int errno = SPIFFS_errno(FS);
  int res2 = SPIFFS_fstat(FS, fd, &s);
  TEST_CHECK(res2 == SPIFFS_OK);
  printf("  >>> file %s size: %i\n", s.name, s.size);

  TEST_CHECK(errno == SPIFFS_ERR_FULL);
  SPIFFS_close(FS, fd);

  res2 = SPIFFS_stat(FS, "test1.txt", &s);
  TEST_CHECK(res2 == SPIFFS_OK);

  SPIFFS_clearerr(FS);
  printf("  create small file\n");
  fd = SPIFFS_open(FS, "test2.txt", SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
#if 0
  // before gc in v3.1
  TEST_CHECK(SPIFFS_errno(FS) == SPIFFS_OK);
  TEST_CHECK(fd > 0);

  for (i = 0; i < 1000; i++) {
    u8_t buf = 'x';
    res = SPIFFS_write(FS, fd, &buf, 1);
  }

  TEST_CHECK(SPIFFS_errno(FS) == SPIFFS_ERR_FULL);
  res2 = SPIFFS_fstat(FS, fd, &s);
  TEST_CHECK(res2 == SPIFFS_OK);
  printf("  >>> file %s size: %i\n", s.name, s.size);
  TEST_CHECK(s.size == 0);
  SPIFFS_clearerr(FS);
#else
  TEST_CHECK(SPIFFS_errno(FS) == SPIFFS_ERR_FULL);
  SPIFFS_clearerr(FS);
#endif
  printf("  remove files\n");
  res = SPIFFS_remove(FS, "test1.txt");
  TEST_CHECK(res == SPIFFS_OK);
#if 0
  res = SPIFFS_remove(FS, "test2.txt");
  TEST_CHECK(res == SPIFFS_OK);
#endif

  printf("  create medium file\n");
  fd = SPIFFS_open(FS, "test3.txt", SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
  TEST_CHECK(SPIFFS_errno(FS) == SPIFFS_OK);
  TEST_CHECK(fd > 0);

  for (i = 0; i < 20*1000; i++) {
    u8_t buf = 'x';
    res = SPIFFS_write(FS, fd, &buf, 1);
  }
  TEST_CHECK(SPIFFS_errno(FS) == SPIFFS_OK);

  res2 = SPIFFS_fstat(FS, fd, &s);
  TEST_CHECK(res2 == SPIFFS_OK);
  printf("  >>> file %s size: %i\n", s.name, s.size);
  TEST_CHECK(s.size == 20*1000);

  return TEST_RES_OK;

} TEST_END

TEST(magic_test) {
  // this test only works on default sizes
  TEST_ASSERT(sizeof(spiffs_obj_id) == sizeof(u16_t));

  // one obj lu page, not full
  fs_reset_specific(0, 0, 4096*16, 4096, 4096*1, 128);
  TEST_CHECK(SPIFFS_CHECK_MAGIC_POSSIBLE(FS));
  // one obj lu page, full
  fs_reset_specific(0, 0, 4096*16, 4096, 4096*2, 128);
  TEST_CHECK(!SPIFFS_CHECK_MAGIC_POSSIBLE(FS));
  // two obj lu pages, not full
  fs_reset_specific(0, 0, 4096*16, 4096, 4096*4, 128);
  TEST_CHECK(SPIFFS_CHECK_MAGIC_POSSIBLE(FS));

  return TEST_RES_OK;

} TEST_END

TEST(nodemcu_309) {
  fs_reset_specific(0, 0, 4096*20, 4096, 4096, 256);

  int res;
  spiffs_file fd;
  int j;

  for (j = 1; j <= 3; j++) {
    char fname[32];
    sprintf(fname, "20K%i.txt", j);
    fd = SPIFFS_open(FS, fname, SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_DIRECT, 0);
    TEST_CHECK(fd > 0);
    int i;
    res = SPIFFS_OK;
    u8_t err = 0;
    for (i = 1; i <= 1280; i++) {
      char *buf = "0123456789ABCDE\n";
      res = SPIFFS_write(FS, fd, buf, strlen(buf));
      if (!err && res < 0) {
        printf("err @ %i,%i\n", i, j);
        err = 1;
      }
    }
  }

  int errno = SPIFFS_errno(FS);
  TEST_CHECK(errno == SPIFFS_ERR_FULL);

  u32_t total;
  u32_t used;

  SPIFFS_info(FS, &total, &used);
  printf("total:%i\nused:%i\nremain:%i\nerrno:%i\n", total, used, total-used, errno);
  //TEST_CHECK(total-used < 11000); // disabled, depends on too many variables

  spiffs_DIR d;
  struct spiffs_dirent e;
  struct spiffs_dirent *pe = &e;

  SPIFFS_opendir(FS, "/", &d);
  int spoon_guard = 0;
  while ((pe = SPIFFS_readdir(&d, pe))) {
    printf("%s [%04x] size:%i\n", pe->name, pe->obj_id, pe->size);
    TEST_CHECK(spoon_guard++ < 3);
  }
  TEST_CHECK(spoon_guard == 3);
  SPIFFS_closedir(&d);

  return TEST_RES_OK;

} TEST_END


TEST(robert) {
  // create a clean file system starting at address 0, 2 megabytes big,
  // sector size 65536, block size 65536, page size 256
  fs_reset_specific(0, 0, 1024*1024*2, 65536, 65536, 256);

  int res;
  spiffs_file fd;
  char fname[32];

  sprintf(fname, "test.txt");
  fd = SPIFFS_open(FS, fname, SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
  TEST_CHECK(fd > 0);
  res = SPIFFS_OK;
  char buf[500];
  memset(buf, 0xaa, 500);
  res = SPIFFS_write(FS, fd, buf, 500);
  TEST_CHECK(res >= SPIFFS_OK);
  SPIFFS_close(FS, fd);

  int errno = SPIFFS_errno(FS);
  TEST_CHECK(errno == SPIFFS_OK);

  //SPIFFS_vis(FS);
  // unmount
  SPIFFS_unmount(FS);

  // remount
  res = fs_mount_specific(0, 1024*1024*2, 65536, 65536, 256);
  TEST_CHECK(res== SPIFFS_OK);

  //SPIFFS_vis(FS);

  spiffs_stat s;
  TEST_CHECK(SPIFFS_stat(FS, fname, &s) == SPIFFS_OK);
  printf("file %s stat size %i\n", s.name, s.size);
  TEST_CHECK(s.size == 500);

  return TEST_RES_OK;

} TEST_END


TEST(spiffs_12) {
  fs_reset_specific(0x4024c000, 0x4024c000 + 0, 192*1024, 4096, 4096*2, 256);

  int res;
  spiffs_file fd;
  int j = 1;

  while (1) {
    char fname[32];
    sprintf(fname, "file%i.txt", j);
    fd = SPIFFS_open(FS, fname, SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_DIRECT, 0);
    if (fd <=0) break;

    int i;
    res = SPIFFS_OK;
    for (i = 1; i <= 100; i++) {
      char *buf = "0123456789ABCDE\n";
      res = SPIFFS_write(FS, fd, buf, strlen(buf));
      if (res < 0) break;
    }
    SPIFFS_close(FS, fd);
    j++;
  }

  int errno = SPIFFS_errno(FS);
  TEST_CHECK(errno == SPIFFS_ERR_FULL);

  u32_t total;
  u32_t used;

  SPIFFS_info(FS, &total, &used);
  printf("total:%i (%iK)\nused:%i (%iK)\nremain:%i (%iK)\nerrno:%i\n", total, total/1024, used, used/1024, total-used, (total-used)/1024, errno);

  spiffs_DIR d;
  struct spiffs_dirent e;
  struct spiffs_dirent *pe = &e;

  SPIFFS_opendir(FS, "/", &d);
  while ((pe = SPIFFS_readdir(&d, pe))) {
    printf("%s [%04x] size:%i\n", pe->name, pe->obj_id, pe->size);
  }
  SPIFFS_closedir(&d);

  //SPIFFS_vis(FS);

  //dump_page(FS, 0);
  //dump_page(FS, 1);

  return TEST_RES_OK;

} TEST_END


TEST(zero_sized_file_44) {
  fs_reset();

  spiffs_file fd = SPIFFS_open(FS, "zero", SPIFFS_RDWR | SPIFFS_CREAT, 0);
  TEST_CHECK_GE(fd, 0);

  int res = SPIFFS_close(FS, fd);
  TEST_CHECK_GE(res, 0);

  fd = SPIFFS_open(FS, "zero", SPIFFS_RDWR, 0);
  TEST_CHECK_GE(fd, 0);

  u8_t buf[8];
  res = SPIFFS_read(FS, fd, buf, 8);
  TEST_CHECK_EQ(res, 0);
  TEST_CHECK_EQ(SPIFFS_errno(FS), SPIFFS_ERR_END_OF_OBJECT);

  res = SPIFFS_read(FS, fd, buf, 0);
  TEST_CHECK_GE(res, 0);

  res = SPIFFS_read(FS, fd, buf, 0);
  TEST_CHECK_GE(res, 0);

  buf[0] = 1;
  buf[1] = 2;

  res = SPIFFS_write(FS, fd, buf, 2);
  TEST_CHECK_EQ(res, 2);

  res = SPIFFS_lseek(FS, fd, 0, SPIFFS_SEEK_SET);
  TEST_CHECK_GE(res, 0);

  u8_t b;
  res = SPIFFS_read(FS, fd, &b, 1);
  TEST_CHECK_EQ(res, 1);
  TEST_CHECK_EQ(b, 1);

  res = SPIFFS_read(FS, fd, &b, 1);
  TEST_CHECK_EQ(res, 1);
  TEST_CHECK_EQ(b, 2);

  res = SPIFFS_read(FS, fd, buf, 8);
  TEST_CHECK_EQ(res, 0);
  TEST_CHECK_EQ(SPIFFS_errno(FS), SPIFFS_ERR_END_OF_OBJECT);

  return TEST_RES_OK;
} TEST_END

#if !SPIFFS_READ_ONLY
TEST(truncate_48) {
  fs_reset();

  u32_t len = SPIFFS_DATA_PAGE_SIZE(FS)/2;

  s32_t res = test_create_and_write_file("small", len, len);
  TEST_CHECK_GE(res, 0);

  spiffs_file fd = SPIFFS_open(FS, "small", SPIFFS_RDWR, 0);
  TEST_CHECK_GE(fd, 0);

  spiffs_fd *desc;
#if SPIFFS_FILEHDL_OFFSET
  res = spiffs_fd_get(FS, fd - TEST_SPIFFS_FILEHDL_OFFSET, &desc);
#else
  res = spiffs_fd_get(FS, fd, &desc);
#endif

  TEST_CHECK_GE(res, 0);

  TEST_CHECK_EQ(desc->size, len);

  u32_t new_len = len/2;
  res = spiffs_object_truncate(desc, new_len, 0);
  TEST_CHECK_GE(res, 0);

  TEST_CHECK_EQ(desc->size, new_len);

  res = SPIFFS_close(FS, fd);
  TEST_CHECK_GE(res, 0);

  spiffs_stat s;
  res = SPIFFS_stat(FS, "small", &s);
  TEST_CHECK_GE(res, 0);
  TEST_CHECK_EQ(s.size, new_len);

  res = SPIFFS_remove(FS, "small");
  TEST_CHECK_GE(res, 0);

  fd = SPIFFS_open(FS, "small", SPIFFS_RDWR, 0);
  TEST_CHECK_LT(fd, 0);
  TEST_CHECK_EQ(SPIFFS_errno(FS), SPIFFS_ERR_NOT_FOUND);

  return TEST_RES_OK;
} TEST_END
#endif

TEST(eof_tell_72) {
  fs_reset();

  s32_t res;

  spiffs_file fd = SPIFFS_open(FS, "file", SPIFFS_CREAT | SPIFFS_RDWR | SPIFFS_APPEND, 0);
  TEST_CHECK_GT(fd, 0);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 1);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 0);

  res = SPIFFS_write(FS, fd, "test", 4);
  TEST_CHECK_EQ(res, 4);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 1);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 4);

  res = SPIFFS_fflush(FS, fd);
  TEST_CHECK_EQ(res, SPIFFS_OK);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 1);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 4);

  res = SPIFFS_lseek(FS, fd, 2, SPIFFS_SEEK_SET);
  TEST_CHECK_EQ(res, 2);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 0);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 2);

  res = SPIFFS_write(FS, fd, "test", 4);
  TEST_CHECK_EQ(res, 4);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 1);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 8);

  res = SPIFFS_fflush(FS, fd);
  TEST_CHECK_EQ(res, SPIFFS_OK);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 1);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 8);

  res = SPIFFS_close(FS, fd);
  TEST_CHECK_EQ(res, SPIFFS_OK);
  TEST_CHECK_LT(SPIFFS_eof(FS, fd), SPIFFS_OK);
  TEST_CHECK_LT(SPIFFS_tell(FS, fd), SPIFFS_OK);

  fd = SPIFFS_open(FS, "file", SPIFFS_RDWR, 0);
  TEST_CHECK_GT(fd, 0);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 0);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 0);

  res = SPIFFS_lseek(FS, fd, 2, SPIFFS_SEEK_SET);
  TEST_CHECK_EQ(res, 2);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 0);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 2);

  res = SPIFFS_write(FS, fd, "test", 4);
  TEST_CHECK_EQ(res, 4);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 0);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 6);

  res = SPIFFS_fflush(FS, fd);
  TEST_CHECK_EQ(res, SPIFFS_OK);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 0);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 6);

  res = SPIFFS_lseek(FS, fd, 0, SPIFFS_SEEK_END);
  TEST_CHECK_EQ(res, 8);
  TEST_CHECK_EQ(SPIFFS_eof(FS, fd), 1);
  TEST_CHECK_EQ(SPIFFS_tell(FS, fd), 8);

  return TEST_RES_OK;
} TEST_END

TEST(spiffs_dup_file_74) {
  fs_reset_specific(0, 0, 64*1024, 4096, 4096*2, 256);
  {
    spiffs_file fd = SPIFFS_open(FS, "/config", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_WRONLY, 0);
    TEST_CHECK(fd >= 0);
    char buf[5];
    strncpy(buf, "test", sizeof(buf));
    SPIFFS_write(FS, fd, buf, 4);
    SPIFFS_close(FS, fd);
  }
  {
    spiffs_file fd = SPIFFS_open(FS, "/data", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_WRONLY, 0);
    TEST_CHECK(fd >= 0);
    SPIFFS_close(FS, fd);
  }
  {
    spiffs_file fd = SPIFFS_open(FS, "/config", SPIFFS_RDONLY, 0);
    TEST_CHECK(fd >= 0);
    char buf[5];
    int cb = SPIFFS_read(FS, fd, buf, sizeof(buf));
    TEST_CHECK(cb > 0 && cb < sizeof(buf));
    TEST_CHECK(strncmp("test", buf, cb) == 0);
    SPIFFS_close(FS, fd);
  }
  {
    spiffs_file fd = SPIFFS_open(FS, "/data", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_WRONLY, 0);
    TEST_CHECK(fd >= 0);
    spiffs_stat stat;
    SPIFFS_fstat(FS, fd, &stat);
    if (strcmp((const char*) stat.name, "/data") != 0) {
      // oops! lets check the list of files...
      spiffs_DIR dir;
      SPIFFS_opendir(FS, "/", &dir);
      struct spiffs_dirent dirent;
      while (SPIFFS_readdir(&dir, &dirent)) {
        printf("%s\n", dirent.name);
      }
      // this will print "/config" two times
      TEST_CHECK(0);
    }
    SPIFFS_close(FS, fd);
  }
  return TEST_RES_OK;
} TEST_END

TEST(temporal_fd_cache) {
  fs_reset_specific(0, 0, 1024*1024, 4096, 2*4096, 256);
  (FS)->fd_count = 4;

  char *fcss = "blaha.css";

  char *fhtml[] = {
      "index.html", "cykel.html", "bloja.html", "drivmedel.html", "smorgasbord.html",
      "ombudsman.html", "fubbick.html", "paragrod.html"
  };

  const int hit_probabilities[] = {
      25, 20, 16, 12, 10, 8, 5, 4
  };

  const int runs = 10000;

  // create our webserver files
  TEST_CHECK_EQ(test_create_and_write_file(fcss, 2000, 256), SPIFFS_OK);
  TEST_CHECK_EQ(test_create_and_write_file(fhtml[0], 4000, 256), SPIFFS_OK);
  TEST_CHECK_EQ(test_create_and_write_file(fhtml[1], 3000, 256), SPIFFS_OK);
  TEST_CHECK_EQ(test_create_and_write_file(fhtml[2], 2000, 256), SPIFFS_OK);
  TEST_CHECK_EQ(test_create_and_write_file(fhtml[3], 1000, 256), SPIFFS_OK);
  TEST_CHECK_EQ(test_create_and_write_file(fhtml[4], 1500, 256), SPIFFS_OK);
  TEST_CHECK_EQ(test_create_and_write_file(fhtml[5], 3000, 256), SPIFFS_OK);
  TEST_CHECK_EQ(test_create_and_write_file(fhtml[6], 2000, 256), SPIFFS_OK);
  TEST_CHECK_EQ(test_create_and_write_file(fhtml[7], 3500, 256), SPIFFS_OK);

  clear_flash_ops_log();

  int run = 0;
  do {
    // open & read an html
    int dice = rand() % 100;
    int probability = 0;
    int html_ix = 0;
    do {
      probability += hit_probabilities[html_ix];
      if (dice <= probability) {
        break;
      }
      html_ix++;
    } while(probability < 100);

    TEST_CHECK_EQ(read_and_verify(fhtml[html_ix]), SPIFFS_OK);

    // open & read css
    TEST_CHECK_EQ(read_and_verify(fcss), SPIFFS_OK);
  } while (run ++ < runs);

  return TEST_RES_OK;
} TEST_END

static int run_fuzz_test(FILE *f, int maxfds, int debuglog) {
  // There are a bunch of arbitrary constants in this test case. Changing them will
  // almost certainly change the effets of an input file. It *may* be worth
  // making some of these constants to come from the input file. 
  int setup = fgetc(f);

  int page_size = 128 << (setup & 3);
  setup >>= 2;
  int erase_size = 4096 << (setup & 3);
  setup >>= 2;
  int block_size = erase_size << (setup & 1);
  setup >>= 1;
  int blocks = 4 + (setup & 7);
  fs_reset_specific(0, 0, blocks * block_size, erase_size, block_size, page_size);
  int res;
  (FS)->fd_count = 4;

  int c;

  spiffs_file fd[4];
  memset(fd, -1, sizeof(fd));
  int openindex[4];
  memset(openindex, -1, sizeof(openindex));
  char *filename[8];

  int i;

  for (i = 0; i < 8; i++) {
    char buff[128];
    sprintf(buff, "%dfile%d.xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxasdasdasdadxxxxxxxxxxxxxxxxxxx", i, i);
    buff[9 + 2 * i] = 0;
    filename[i] = strdup(buff);
  }

  // The list of 8 modes that are chosen. SPIFFS_EXCL is not present -- it probably ought to be.
  int modes[8] = {SPIFFS_RDONLY, SPIFFS_RDWR, SPIFFS_RDWR|SPIFFS_TRUNC, SPIFFS_RDWR|SPIFFS_CREAT, SPIFFS_RDWR|SPIFFS_CREAT|SPIFFS_TRUNC,
      SPIFFS_WRONLY|SPIFFS_CREAT|SPIFFS_TRUNC, SPIFFS_RDWR|SPIFFS_CREAT|SPIFFS_TRUNC|SPIFFS_DIRECT, SPIFFS_WRONLY};

  char buff[2048];
  for (i = 0; i < sizeof(buff); i++) {
    buff[i] = i * 19;
  }

#define LOGOP if (debuglog) printf

  while ((c = fgetc(f)) >= 0) {
    int add;
    char rbuff[2048];
    if (c <= ' ') {
      continue;
    }
    int arg = fgetc(f);
    if (arg < 0) {
      break;
    }
    int fdn = ((arg >> 6) & 3) % maxfds;
    int rc;
    switch(c) {
    case 'O':
      if (fd[fdn] >= 0) {
        LOGOP("  close(%d)\n", fd[fdn]);
        SPIFFS_close(FS, fd[fdn]);
        openindex[fdn] = -1;
        fd[fdn] = -1;
      }
#ifndef HAVE_MULTIPLE_OPEN
      {
        int index = (arg >> 3) & 7;
        for (i = 0; i < sizeof(openindex) / sizeof(openindex[0]); i++) {
          if (openindex[i] == index) {
            break;
          }
        }
        if (i < sizeof(openindex) / sizeof(openindex[0])) {
          break;
        }
      }
#endif
      LOGOP("  open(\"%s\", 0x%x)", filename[(arg>>3) & 7], modes[arg & 7]);
      fd[fdn] = SPIFFS_open(FS, filename[(arg>>3) & 7], modes[arg & 7], 0);
      if (fd[fdn] >= 0) {
        openindex[fdn] = (arg >> 3) & 7;
      }
      LOGOP(" -> %d\n", fd[fdn]);
      break;

    case 'S':
      if (fd[fdn] >= 0) {
        int offset = (14 << (arg & 7)) + arg;
        if (arg & 16) {
          offset = -offset;
        }
        int whence = (arg & 63) % 3;
        LOGOP("  lseek(%d, %d, %d)\n", fd[fdn], offset, whence);
        SPIFFS_lseek(FS, fd[fdn], offset, whence);
      }
      break;

    case 'R':
      if (fd[fdn] >= 0) {
        LOGOP("  read(%d, , %d)", fd[fdn], (15 << (arg & 7)) + (arg & 127));
        int rlen = SPIFFS_read(FS, fd[fdn], rbuff, (15 << (arg & 7)) + (arg & 127));
        LOGOP(" -> %d\n", rlen);
      }
      break;

    case 'W':
      if (fd[fdn] >= 0) {
        LOGOP("  write(%d, , %d)", fd[fdn], (15 << (arg & 7)) + (arg & 127));
        rc = SPIFFS_write(FS, fd[fdn], buff, (15 << (arg & 7)) + (arg & 127));
        LOGOP(" -> %d\n", rc);
      }
      break;

    case 'C':
      if (fd[fdn] >= 0) {
        LOGOP("  close(%d)\n", fd[fdn]);
        SPIFFS_close(FS, fd[fdn]);
      }
      fd[fdn] = -1;
      openindex[fdn] = -1;
      break;

    case 'b':
      add = fgetc(f);
      for (i = 0; i < sizeof(buff); i++) {
        buff[i] = add + i * arg;
      }
      break;

    case 'f':
      if (fd[fdn] >= 0) {
        LOGOP("  fflush(%d)\n", fd[fdn]);
        SPIFFS_fflush(FS, fd[fdn]);
      }
      break;

#ifdef HAVE_REMOVE_OPEN
    case 'D':
      if (fd[fdn] >= 0) {
        LOGOP("  fremove(%d)\n", fd[fdn]);
        SPIFFS_fremove(FS, fd[fdn]);
      }
      break;
#endif

    case 'd':
#ifndef HAVE_REMOVE_OPEN
    {
      int index = arg & 7;
      for (i = 0; i < sizeof(openindex) / sizeof(openindex[0]); i++) {
        if (openindex[i] == index) {
          break;
        }
      }
      if (i < sizeof(openindex) / sizeof(openindex[0])) {
        break;
      }
    }
#endif
    LOGOP("  remove(\"%s\")", filename[arg & 7]);
    rc = SPIFFS_remove(FS, filename[arg & 7]);
    LOGOP(" -> %d\n", rc);
    break;

    case 'r':
#ifndef HAVE_REMOVE_OPEN
    {
      int index = arg & 7;
      for (i = 0; i < sizeof(openindex) / sizeof(openindex[0]); i++) {
        if (openindex[i] == index) {
          break;
        }
      }
      if (i < sizeof(openindex) / sizeof(openindex[0])) {
        break;
      }
    }
#endif
    LOGOP("  rename(\"%s\", \"%s\")", filename[arg & 7], filename[(arg >> 3) & 7]);
    rc = SPIFFS_rename(FS, filename[arg & 7], filename[(arg >> 3) & 7]);
    LOGOP(" -> %d\n", rc);
    break;

    case 'U':
      ungetc(arg, f);
      for (i = 0; i < 4; i++) {
        fd[i] = -1;
      }
      {
#ifdef DO_UNMOUNT
        LOGOP("  unmount\n");
        SPIFFS_unmount(FS);
#endif
        char *tmpfile = strdup("/tmp/fsdump.XXXXXX");
        LOGOP("  cycle and remount\n");
        close(mkstemp(tmpfile));
        fs_store_dump(tmpfile);
        fs_mount_dump(tmpfile, 0, 0, blocks * block_size, erase_size, block_size, page_size);
        unlink(tmpfile);
        free(tmpfile);
#ifndef NO_FORCE_CHECK
        LOGOP("  forcecheck()");
        rc = SPIFFS_check(FS);
        LOGOP(" -> %d\n", rc);
#endif
      }
      break;

    case 'c':
    {
      LOGOP("  check()");
      rc = SPIFFS_check(FS);
      LOGOP(" -> %d\n", rc);
      ungetc(arg, f);
      break;
    }

    default:
      ungetc(arg, f);
      break;
    }
  }

  for (i = 0; i < 4; i++) {
    if (fd[i] >= 0) {
      LOGOP("  close(%d)\n", fd[i]);
      SPIFFS_close(FS, fd[i]);
    }
  }

  return TEST_RES_OK;
}

#define FMEMARGS(x)	x, sizeof(x) - 1

TEST(fuzzer_found_1) {
  return run_fuzz_test(fmemopen(FMEMARGS("\021OlWkd5O4W4W0O5OlWkO5OlW0O5O4W0"), "r"), 4, 1);
} TEST_END

TEST(fuzzer_found_2) {
  return run_fuzz_test(fmemopen(FMEMARGS("bO4W6W0d\036O4W6"), "r"), 4, 1);
} TEST_END

TEST(fuzzer_found_3) {
  return run_fuzz_test(fmemopen(FMEMARGS("\264O4OqWeWWWWW@O4WWW\027"), "r"), 4, 1);
} TEST_END

TEST(fuzzer_found_4) {
  unsigned char smalltest[] = {
      0x62, 0x4f, 0x24, 0x57, 0x3f, 0x57, 0x3f, 0x57, 0x3f, 0x57, 0x3f, 0x57,
      0x3f, 0x4f, 0x34, 0x57, 0x3f, 0x55, 0x4f, 0x61, 0x57, 0x61, 0x4f, 0x61,
      0x57, 0x65, 0x43, 0x61, 0x4f, 0x24, 0x57, 0x30
  };
  unsigned int smalltest_len = 32;

  return run_fuzz_test(fmemopen(smalltest, smalltest_len, "r"), 4, 1);
} TEST_END

TEST(fuzzer_found_single_1) {
  return run_fuzz_test(fmemopen(FMEMARGS("\000O\004Odr4d\356Okr0WWUO;WWWWd\035W4"), "r"), 1, 1);
} TEST_END

TEST(log_afl_test) {
  u32_t old_val = set_abort_on_error(1);
  int rc = run_fuzz_test(stdin, 4, 1);
  set_abort_on_error(old_val);
  return rc;
} TEST_END

TEST(afl_test) {
  u32_t old_val = set_abort_on_error(1);
  int rc = run_fuzz_test(stdin, 4, 0);
  set_abort_on_error(old_val);
  return rc;
} TEST_END

TEST(afl_single) {
  u32_t old_val = set_abort_on_error(1);
  int rc = run_fuzz_test(stdin, 1, 0);
  set_abort_on_error(old_val);
  return rc;
} TEST_END

TEST(small_free_space) {
  fs_reset_specific(0, 0, 400*1024, 4096, 2*4096, 256);
  spiffs_file fd;
  int res;
  (FS)->fd_count = 4;

  int tfd = SPIFFS_open(FS, "testfile", SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
  TEST_CHECK(tfd > 0);
  char *tbuf = "some data";
  res = SPIFFS_write(FS, tfd, tbuf, strlen(tbuf));

  TEST_CHECK(res == strlen(tbuf));

  res = SPIFFS_fflush(FS, tfd);
  TEST_CHECK(res >= SPIFFS_OK);

  SPIFFS_close(FS, tfd);

  const int runs = 1000;

  int fileCurrNumber = 0;
  int fileDelNumber = 1;

  int run = 0;
  do {
    u8_t buf[1000];

    sprintf(buf, "%d", fileCurrNumber);
    int i;
    for (i = 0; i < 100; i++) {
      strcat(buf, " azzaaax");
    }

    int maxFileNr = 500;
    char *filename = "awyn";
    char *fileext = ".dat";

    u32_t total;
    u32_t used;

    SPIFFS_info(FS, &total, &used);

    if (total - used < 20000) {
      maxFileNr = 1;
    }

    fileCurrNumber++;
    int fileCntr = fileCurrNumber + 1 - fileDelNumber;

    char fileCurrName[64];
    sprintf(fileCurrName, "%s%d%s", filename, fileCurrNumber, fileext);

    fd = SPIFFS_open(FS, fileCurrName, SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
    TEST_CHECK(fd > 0);

    //printf("About to write to %s\n", fileCurrName);
    res = SPIFFS_write(FS, fd, buf, strlen(buf));

    TEST_CHECK(res == strlen(buf));

    res = SPIFFS_fflush(FS, fd);
    TEST_CHECK_EQ(res, SPIFFS_OK);

    SPIFFS_close(FS, fd);

    if (fileCntr > maxFileNr) {
      char fileDelName[64];
      sprintf(fileDelName, "%s%d%s", filename, fileDelNumber, fileext);
      //printf("Deleting %s (free space %d)\n", fileDelName, total - used);
  
      res = SPIFFS_remove(FS, fileDelName);

      TEST_CHECK(res == SPIFFS_OK);
      fileDelNumber++;
    }
  } while (run ++ < runs);

  tfd = SPIFFS_open(FS, "testfile", SPIFFS_RDONLY, 0);
  TEST_CHECK(tfd > 0);
  char rbuf[32];
  res = SPIFFS_read(FS, tfd, rbuf, sizeof(rbuf));

  TEST_CHECK(res == strlen(tbuf));

  SPIFFS_close(FS, tfd);

  TEST_CHECK(memcmp(rbuf, tbuf, strlen(tbuf)) == 0);

  return TEST_RES_OK;
} TEST_END

TEST(lots_of_overwrite) {
  fs_reset_specific(0, 0, 3000*1024, 4096, 2*4096, 256);
  spiffs_file fd;
  int res;
  (FS)->fd_count = 4;

  int i;

  for (i = 0; i < 5; i++) {

    char filename[64];
    sprintf(filename, "%d-tstfile", i);
    int tfd = SPIFFS_open(FS, filename, SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
    TEST_CHECK(tfd > 0);
    char tbuf[1024];
    memset(tbuf, 'a', 700);
    tbuf[700] = 0;
    res = SPIFFS_write(FS, tfd, tbuf, strlen(tbuf));

    TEST_CHECK(res == strlen(tbuf));

    res = SPIFFS_fflush(FS, tfd);
    TEST_CHECK(res >= SPIFFS_OK);

    SPIFFS_close(FS, tfd);
  }

  const int runs = 100000;

  int run = 0;
  for (run = 0; run < runs; run++) {
    u8_t buf[2000];

    sprintf(buf, "%d", run);
    int i;
    for (i = 0; i < 100 + (run % 100); i++) {
      strcat(buf, " azzaaax");
    }

    int tfd = SPIFFS_open(FS, "file.dat", SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC, 0);
    TEST_CHECK(tfd > 0);
    res = SPIFFS_write(FS, tfd, buf, strlen(buf));

    TEST_CHECK(res == strlen(buf));

    res = SPIFFS_fflush(FS, tfd);
    TEST_CHECK(res >= SPIFFS_OK);

    SPIFFS_close(FS, tfd);

    tfd = SPIFFS_open(FS, "file.dat", SPIFFS_RDONLY, 0);
    TEST_CHECK(tfd > 0);
    char rbuf[2000];
    res = SPIFFS_read(FS, tfd, rbuf, sizeof(rbuf));

    TEST_CHECK(res == strlen(buf));

    SPIFFS_close(FS, tfd);

    TEST_CHECK(memcmp(rbuf, buf, strlen(buf)) == 0);

    char filename[64];
    sprintf(filename, "%d-tstfile", run % 5);
    tfd = SPIFFS_open(FS, filename, SPIFFS_RDONLY, 0);
    TEST_CHECK(tfd > 0);
    char tbuf[1024];
    memset(tbuf, 'a', 700);
    tbuf[700] = 0;
    res = SPIFFS_read(FS, tfd, rbuf, sizeof(rbuf));

    TEST_CHECK(res == strlen(tbuf));

    SPIFFS_close(FS, tfd);
    TEST_CHECK(memcmp(rbuf, tbuf, strlen(tbuf)) == 0);
  }

  return TEST_RES_OK;
} TEST_END


#if 0
TEST(spiffs_hidden_file_90) {
  fs_mount_dump("imgs/90.hidden_file.spiffs", 0, 0, 1*1024*1024, 4096, 4096, 128);

  SPIFFS_vis(FS);

  dump_page(FS, 1);
  dump_page(FS, 0x8fe);
  dump_page(FS, 0x8ff);

  {
    spiffs_DIR dir;
    SPIFFS_opendir(FS, "/", &dir);
    struct spiffs_dirent dirent;
    while (SPIFFS_readdir(&dir, &dirent)) {
      printf("%-32s sz:%-7i obj_id:%08x pix:%08x\n", dirent.name, dirent.size, dirent.obj_id, dirent.pix);
    }
  }

  printf("remove cli.bin res %i\n", SPIFFS_remove(FS, "cli.bin"));

  {
    spiffs_DIR dir;
    SPIFFS_opendir(FS, "/", &dir);
    struct spiffs_dirent dirent;
    while (SPIFFS_readdir(&dir, &dirent)) {
      printf("%-32s sz:%-7i obj_id:%08x pix:%08x\n", dirent.name, dirent.size, dirent.obj_id, dirent.pix);
    }
  }
  return TEST_RES_OK;

} TEST_END
#endif
#if 0
TEST(null_deref_check_93) {
  fs_mount_dump("imgs/93.dump.bin", 0, 0, 2*1024*1024, 4096, 4096, 256);

  //int res = SPIFFS_open(FS, "d43.fw", SPIFFS_TRUNC | SPIFFS_CREAT | SPIFFS_WRONLY, 0);
  //TEST_CHECK_GE(res, SPIFFS_OK);

  SPIFFS_vis(FS);

  printf("\n\n-------------------------------------------------\n\n");

  SPIFFS_check(FS);
  //fs_store_dump("imgs/93.dump.checked.bin");

  SPIFFS_vis(FS);

  printf("\n\n-------------------------------------------------\n\n");

  SPIFFS_check(FS);

  SPIFFS_vis(FS);
  printf("\n\n-------------------------------------------------\n\n");



  return TEST_RES_OK;
} TEST_END
#endif

TEST(spiffs_145) {
  int res;
  fs_reset_specific(0, 0, 1024*1024, 65536, 65536, 1024);
  {
    spiffs_file fd = SPIFFS_open(FS, "biggie", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_WRONLY, 0);
    TEST_CHECK(fd >= 0);
    char buf[1024*512];
    memset(buf, 0xee, sizeof(buf));
    TEST_CHECK_GT(SPIFFS_write(FS, fd, buf, sizeof(buf)), 0);
    TEST_CHECK_EQ(SPIFFS_close(FS, fd), SPIFFS_OK);
  }
  const int runs = 1000;
  int run = 0;
  while (run++ < runs) {
    spiffs_file fd = SPIFFS_open(FS, "clobber", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_WRONLY, 0);
    TEST_CHECK(fd >= 0);
    char buf[8192];
    memset(buf, 0xee, sizeof(buf));
    TEST_CHECK_GT(SPIFFS_write(FS, fd, buf, sizeof(buf)), 0);
    TEST_CHECK_EQ(SPIFFS_close(FS, fd), SPIFFS_OK);
    TEST_CHECK_EQ(SPIFFS_remove(FS, "clobber"), SPIFFS_OK);
  }

  // below stolen from SPIFFS_vis
  spiffs *fs = FS;
  int entries_per_page = (SPIFFS_CFG_LOG_PAGE_SZ(fs) / sizeof(spiffs_obj_id));
  spiffs_obj_id *obj_lu_buf = (spiffs_obj_id *)fs->lu_work;
  spiffs_block_ix bix = 0;

  while (bix < fs->block_count) {
    // check each object lookup page
    spiffs_obj_id erase_count;
    TEST_CHECK_EQ(_spiffs_rd(fs, SPIFFS_OP_C_READ | SPIFFS_OP_T_OBJ_LU2, 0,
        SPIFFS_ERASE_COUNT_PADDR(fs, bix),
        sizeof(spiffs_obj_id), (u8_t *)&erase_count), SPIFFS_OK);
    TEST_CHECK_NEQ(erase_count, (spiffs_obj_id)-1);
    TEST_CHECK_NEQ(erase_count, 0);
    bix++;
  } // per block

  return TEST_RES_OK;
} TEST_END


TEST(seek_bug_148) {
  int res;
#define MAGIC_SIZE_THAT_FAILS 26355 // happens to be SPIFFS_DATA_PAGE_SIZE(FS) * SPIFFS_OBJ_HDR_IX_LEN(FS)
  fs_reset_specific(0, 0, 64*1024, 4096, 4096, 256);
  u8_t buf[MAGIC_SIZE_THAT_FAILS];
  spiffs_file fd = SPIFFS_open(FS, "EVENT", SPIFFS_O_CREAT | SPIFFS_O_RDWR, 0);
  TEST_CHECK_GT(fd, 0);
  TEST_CHECK_EQ(SPIFFS_write(FS, fd, &buf, sizeof(buf)), sizeof(buf));
  TEST_CHECK_EQ(SPIFFS_close(FS, fd), SPIFFS_OK);
  fd = SPIFFS_open(FS, "EVENT", SPIFFS_O_RDONLY, 0);
  TEST_CHECK_GT(fd, 0);
  TEST_CHECK_EQ(SPIFFS_lseek(FS, fd, 0, SEEK_END), MAGIC_SIZE_THAT_FAILS);
  return TEST_RES_OK;
} TEST_END


TEST(remove_release_fd_152) {
  int res;
  fs_reset_specific(0, 0, 64*1024, 4096, 4096, 256);
  u8_t buf[1024];
  memrand(buf, sizeof(buf));
  TEST_CHECK_EQ(count_taken_fds(FS), 0);
  spiffs_file fd1 = SPIFFS_open(FS, "removemeandloseafd", SPIFFS_O_CREAT | SPIFFS_O_RDWR, 0);
  TEST_CHECK_GT(fd1, 0);
  TEST_CHECK_EQ(count_taken_fds(FS), 1);
  TEST_CHECK_EQ(SPIFFS_write(FS, fd1, &buf, sizeof(buf)), sizeof(buf));
  TEST_CHECK_EQ(SPIFFS_close(FS, fd1), SPIFFS_OK);
  TEST_CHECK_EQ(count_taken_fds(FS), 0);
  spiffs_file fd2 = SPIFFS_open(FS, "removemeandloseafd", SPIFFS_O_RDWR, 0);
  TEST_CHECK_GT(fd2, 0);
  TEST_CHECK_EQ(count_taken_fds(FS), 1);
  spiffs_file fd3 = SPIFFS_open(FS, "removemeandloseafd", SPIFFS_O_RDWR, 0);
  TEST_CHECK_GT(fd3, 0);
  TEST_CHECK_EQ(count_taken_fds(FS), 2);
  TEST_CHECK_EQ(SPIFFS_remove(FS, "removemeandloseafd"), SPIFFS_OK);
  TEST_CHECK_EQ(count_taken_fds(FS), 0);
  return TEST_RES_OK;
} TEST_END

TEST(certain_file_size_fail_165) {
  fs_reset_specific(0, 0, 512*1024, 4*1024, 64*1024, 256);
  const int NUM = 134;
  const int SIZ = 200;
  u8_t buf[SIZ];

  TEST_CHECK_EQ(SPIFFS_creat(FS, "test", 0), SPIFFS_OK);
  spiffs_file fd = SPIFFS_open(FS, "test", SPIFFS_O_CREAT | SPIFFS_O_WRONLY, 0);
  TEST_CHECK_GT(fd, 0);

  int i;
  for (i = 0; i < NUM; i++) {
    TEST_CHECK_EQ(SPIFFS_write(FS, fd, buf, SIZ), SIZ);
  }
  TEST_CHECK_EQ(SPIFFS_close(FS, fd), SPIFFS_OK);
  fd = SPIFFS_open(FS, "test", SPIFFS_O_RDONLY, 0);
  TEST_CHECK_GT(fd, 0);

  spiffs_stat s;
  TEST_CHECK_EQ(SPIFFS_fstat(FS, fd, &s), SPIFFS_OK);
  TEST_CHECK_EQ(s.size, NUM*SIZ);

  int size = 0;
  for (i = 0; i < NUM; i++) {
    size += SPIFFS_read(FS, fd, buf, SIZ);
  }
  TEST_CHECK_EQ(size, NUM*SIZ);

  return TEST_RES_OK;
} TEST_END


SUITE_TESTS(bug_tests)
  ADD_TEST(nodemcu_full_fs_1)
  ADD_TEST(nodemcu_full_fs_2)
  ADD_TEST(magic_test)
  ADD_TEST(nodemcu_309)
  ADD_TEST(robert)
  ADD_TEST(spiffs_12)
  ADD_TEST(zero_sized_file_44)
  ADD_TEST(truncate_48)
  ADD_TEST(eof_tell_72)
  ADD_TEST(spiffs_dup_file_74)
  ADD_TEST(temporal_fd_cache)
  ADD_TEST(spiffs_145)
  ADD_TEST(seek_bug_148)
  //ADD_TEST(small_free_space)
  ADD_TEST(lots_of_overwrite)
  ADD_TEST(fuzzer_found_1)
  ADD_TEST(fuzzer_found_2)
  ADD_TEST(fuzzer_found_3)
  ADD_TEST(fuzzer_found_4)
  ADD_TEST(remove_release_fd_152)
  ADD_TEST(certain_file_size_fail_165)
  ADD_TEST_NON_DEFAULT(fuzzer_found_single_1)
  ADD_TEST_NON_DEFAULT(log_afl_test)
  ADD_TEST_NON_DEFAULT(afl_test)
  ADD_TEST_NON_DEFAULT(afl_single)
#if 0
  ADD_TEST(spiffs_hidden_file_90)
#endif
#if 0
  ADD_TEST(null_deref_check_93)
#endif
SUITE_END(bug_tests)
