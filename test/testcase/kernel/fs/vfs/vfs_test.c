/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <aos/kernel.h>
#include "aos/kernel.h"

#include "dirent.h"

#include "yunit.h"

static int test_ioctl(file_t *node, int cmd, unsigned long arg)
{
    return -123;
}

static off_t test_lseek(file_t *fp, off_t off, int whence)
{
    return -123;
}

static int test_sync(file_t *fp)
{
    return -123;
}

static int test_stat(file_t *fp, const char *path, struct stat *st)
{
    return -123;
}

static int test_unlink(file_t *fp, const char *path)
{
    return -123;
}

static int test_rename(file_t *fp, const char *oldpath, const char *newpath)
{
    return -123;
}

static int test_mkdir(file_t *fp, const char *path)
{
    return -123;
}

static void test_aos_vfs_case(void)
{
    int i   = 0;
    int fd  = 0;
    int ret = 0;

    char *names[] = {
        "/tmp/abcd0",
        "/tmp/abcd1",
        "/tmp/abcd2",
        "/tmp/abcd3",
        "/tmp/abcd4",
        "/tmp/abcd5",
        "/tmp/abcd6",
        "/tmp/abcd7",
        "/tmp/abcd8",
        "/tmp/abcd9",
    };

    file_ops_t myops = {
        .open  = NULL,
        .ioctl = test_ioctl,
    };

    for (i = 0; i < 10; i++) {
        ret = aos_register_driver(names[i], &myops, NULL);
        YUNIT_ASSERT(ret == VFS_SUCCESS);
    }

    for (i = 0; i < 10; i++) {
        fd = aos_open(names[i], 0);
        YUNIT_ASSERT(fd >= 0);
        YUNIT_ASSERT(-123 == aos_ioctl(fd, 0, 0));

        aos_close(fd);
    }

    for (i = 0; i < 10; i++) {
        fd = aos_open(names[i], 0);
        YUNIT_ASSERT(fd >= 0);

        aos_close(fd);

        ret = aos_unregister_driver(names[i]);
        YUNIT_ASSERT(ret == 0);

#ifdef CONFIG_AOS_YTS_ALL
        fd = aos_open(names[i], 0);
        ret = aos_ioctl(fd, 0, 0);
        YUNIT_ASSERT(-ENOENT == ret);
#endif
    }
}

static void test_vfs_fs_case(void)
{
    int fd  = 0;
    int ret = 0;
    struct stat st;
    char *names = "/tmp/abcd0";

    fs_ops_t myops = {
        .open       = NULL,
        .lseek      = test_lseek,
        .sync       = test_sync,
        .stat       = test_stat,
        .unlink     = test_unlink,
        .rename     = test_rename,
        .mkdir      = test_mkdir,
    };

    ret = aos_register_fs(names, &myops, NULL);
    YUNIT_ASSERT(ret == VFS_SUCCESS);

    fd = aos_open(names, 0);
    YUNIT_ASSERT(fd >= 0);

    YUNIT_ASSERT(-123 == aos_lseek(fd, 0, 0));
    YUNIT_ASSERT(-123 == aos_sync(fd));
    aos_close(fd);

    YUNIT_ASSERT(-123 == aos_stat(names, &st));
    YUNIT_ASSERT(-123 == aos_unlink(names));
    YUNIT_ASSERT(-123 == aos_rename(names, names));
    YUNIT_ASSERT(-123 == aos_mkdir(names));

    ret = aos_unregister_fs(names);
    YUNIT_ASSERT(ret == 0);

    YUNIT_ASSERT(-ENODEV == aos_stat(names, &st));
    YUNIT_ASSERT(-ENODEV == aos_unlink(names));
    YUNIT_ASSERT(-ENODEV == aos_rename(names, names));
}

// static int create_socket(int port)
// {
//     struct sockaddr_in my_addr;
//     int ret = -1;
//     int sockfd;

//     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0)
//         goto out;

//     setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
//     setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int));

//     bzero(&my_addr, sizeof(my_addr));
//     my_addr.sin_family = AF_INET;
//     my_addr.sin_addr.s_addr = INADDR_ANY;
//     my_addr.sin_port = htons(port);
//     ret = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
//     if (ret < 0)
//         goto out1;

//     return sockfd;
// out1:
//     close(sockfd);
// out:
//     return -1;
// }

// static int do_poll(int fd_recv, int timeout)
// {
//     int ret;
//     struct pollfd pfd;
//     char buf2[256];

//     pfd.fd = fd_recv;
//     pfd.events = POLLIN;
//     ret = aos_poll(&pfd, 1, timeout);

//     if (ret > 0)
//         ret = recv(fd_recv, buf2, sizeof buf2, 0);

//     return ret;
// }

// #define MAXCNT 100
// static void send_seq_data(void *arg)
// {
//     int fd = *(int *)arg;
//     struct sockaddr_in addr;
//     char buf[MAXCNT]={0};

//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//     addr.sin_port = htons(12346);

//     int i;
//     for (i=1;i<MAXCNT;i++) {
//         sendto(fd, buf, i, 0, (struct sockaddr *)&addr, sizeof addr);
//         aos_msleep((rand() % 100) + 1);
//     }
// }

// static void test_aos_poll_case(void)
// {
//     int fd_send = create_socket(12345);
//     int fd_recv = create_socket(12346);
//     struct sockaddr_in addr;
//     char buf[128], buf2[256];
//     int ret;

//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//     addr.sin_port = htons(12346);

//     memset(buf, 0x5a, sizeof buf);

//     ret = sendto(fd_send, buf, sizeof buf, 0, (struct sockaddr *)&addr, sizeof addr);
//     YUNIT_ASSERT(ret == sizeof(buf));

//     ret = recv(fd_recv, buf2, sizeof buf2, 0);
//     YUNIT_ASSERT(ret == sizeof(buf));

//     ret = sendto(fd_send, buf, sizeof buf, 0, (struct sockaddr *)&addr, sizeof addr);
//     YUNIT_ASSERT(ret == sizeof(buf));

//     ret = do_poll(fd_recv, 0);
//     YUNIT_ASSERT(ret == sizeof(buf));

//     ret = do_poll(fd_recv, 0);
//     YUNIT_ASSERT(ret == 0);

//     aos_task_new("sender", send_seq_data, &fd_send, 4096);

//     int i;
//     for (i=1;i<MAXCNT;i++) {
//         ret = do_poll(fd_recv, 5000);
//         YUNIT_ASSERT(ret == i);
//     }

//     close(fd_send);
//     close(fd_recv);
// }
/*
static void test_vfs_opendir(void)
{
    int ret;
    DIR *dirp;
    // char *tmpname = "/tmp/abcd0";

    ret = mkdir(test_path, 1);
    printf("test_vfs_opendir mkdir return is %d\n", ret);
    if (mkdir(test_path, 1) != 0)
    {
        YUNIT_FAIL("mkdir fail");
        return;
        // perror ("mktemp");
        // return 1;
    }

    // if (mknod(tmpname, 0600 | S_IFIFO, 0) < 0)
    // {
    //     perror ("mknod");
    //     return 0;
    // }

    dirp = opendir (test_path);

    if (dirp == NULL)
    {
        // fputs ("`opendir' succeeded on a FIFO???\n", stdout);
        YUNIT_FAIL("opendir fail");
        closedir (dirp);
        remove (test_path);
        return;
    }

    // if (errno != ENOTDIR)
    // {
    //     // fprintf (stdout, "`opendir' return error `%s' instead of `%s'\n",strerror (errno), strerror (ENOTDIR));
    //     YUNIT_FAIL("opendir fail");
    //     return;
    // }

    closedir(dirp);
    remove(test_path);
    rmdir(test_path);

}

static void test_vfs_readdir(void)
{
    int lc, ret;
	DIR *test_dir;
	struct dirent *dptr;

	// tst_sparse_opts(ac, av, NULL, NULL);
	// setup();
    ret = mkdir(test_path, 1);
    YUNIT_ASSERT_EQUAL(ret, 0);

	for (lc = 0; lc <= 20; lc++) {

		// tst_count = 0;

		if ((test_dir = opendir(test_path)) == NULL) {
            YUNIT_FAIL("opendir(test_path) Failed\r\n");
			// tst_resm(TFAIL, "opendir(\".\") Failed, errno=%d : %s", errno, strerror(errno));
		} else {
			if (closedir(test_dir) < 0) {
                YUNIT_FAIL("closedir(test_path) Failed\r\n");
				// tst_resm(TFAIL, "closedir(\".\") Failed, errno=%d : %s",errno, strerror(errno));
			} else {
				dptr = readdir(test_dir);
				switch (errno) {
				case EBADF:
					// tst_resm(TPASS, "expected failure - errno = %d : %s", errno, strerror(errno));
					break;
				default:
					if (dptr != NULL) {
						// tst_brkm(TFAIL, cleanup,
						// 	 "call failed with an "
						// 	 "unexpected error - %d : %s",
						// 	 errno,
						// 	 strerror(errno));
					} else {
						// tst_resm(TINFO,
						// 	 "readdir() is not _required_ to fail, "
						// 	 "errno = %d  ", errno);
					}
				}
			}

		}

	}
    rmdir(test_path);
	// cleanup();
	// tst_exit();

}

static void test_vfs_seekdir(void)
{
    DIR * dirp;
    long int save3 = 0;
    long int cur;
    int i = 0;
    struct dirent *dp;
    long int save0;
    long int rewind;

    mkdir(test_path, 1);
    dirp = opendir (test_path);
    if (dirp == NULL)
    {
        YUNIT_FAIL("opendir failed\r\n");
        // printf ("opendir failed: %m\n");
        // return 1;
    }

    save0 = telldir (dirp);
    if (save0 == -1)
    {
        YUNIT_FAIL("telldir failed\r\n");
        // printf ("telldir failed: %m\n");
    }

    for (dp = readdir (dirp); dp != NULL; dp = readdir (dirp))
    {
        if (i++ == 3)
            save3 = telldir (dirp);

        // printf ("%s\n", dp->d_name);

        if (i == 400)
            break;
    }

    // printf ("going back past 4-th entry...\n");

    seekdir(dirp, save3);

    cur = telldir (dirp);
    if (cur != save3)
    {
        // printf ("seekdir (d, %ld); telldir (d) == %ld\n", save3, cur);
    }

    for (dp = readdir (dirp); dp != NULL; dp = readdir (dirp))
        printf ("%s\n", dp->d_name);

    rewinddir (dirp);
    rewind = telldir (dirp);
    if (rewind == -1)
    {
        // printf ("telldir failed: %m\n");
    }
    else if (save0 != rewind)
    {
        // prisntf ("rewinddir didn't reset directory stream\n");
    }

    closedir (dirp);
    return;
}

#define BUF "davef"
static void test_vfs_open_write_sync_close(void)
{
    // static char fname[255];

    // sprintf(fname, "/tmp/tfile_%d", getpid());
	// fd = open(test_file, 1);

    // unsigned int i;

	// for (i = 0; i < 10; i++) {
	// 	write(fd, BUF, sizeof(BUF));

	// 	ret = fsync(fd);

    //     YUNIT_ASSERT_EQUAL(0, ret);

    //     if (fd > 0)
	// 	    close(fd);

	// 	// if (TST_RET == -1)
	// 	// 	tst_res(TFAIL | TTERRNO, "fsync failed");
	// 	// else
	// 	// 	tst_res(TPASS, "fsync() returned %ld", TST_RET);
	// }
    // close(fd);
}




static struct tcases_lseek {
	off_t off;
	int whence;
	char *wname;
	off_t exp_off;
	ssize_t exp_size;
	char *exp_data;
} tcases_lseek[] = {
	{4, SEEK_SET, "SEEK_SET", 4, 3, "efg"},
	{-2, SEEK_CUR, "SEEK_CUR", 5, 2, "fg"},
	{-4, SEEK_END, "SEEK_END", 3, 4, "defg"},
	{0, SEEK_END, "SEEK_END", 7, 0, NULL},
};

#define WRITE_STR "abcdefg"
static void test_vfs_lseek()
{
	char read_buf[64];
    static int fd, ret;

    memset(read_buf, 0, sizeof(read_buf));
    printf("init read_buf is %s\n", WRITE_STR);
    fd = open(test_file, 3);
    printf("fd is %d\n", fd);

    ret = write(fd, WRITE_STR, sizeof(WRITE_STR));
    printf("write ret is %d\n", ret);

    // ret = read(fd, read_buf, sizeof(read_buf));
    // printf("read ret is %d\n", ret);
    // printf("init read_buf is %s\n", read_buf);

    for(int i = 0; i < 4; i++)
    {
        struct tcases_lseek *tc = &tcases_lseek[i];
        // reset the offset to end of file
        // close(fd);
        // fd = open(test_file, 2);

        ret = lseek(fd, tc->off, tc->whence);
        printf("tc->exp_off = %d, ret = %d\n", (int)tc->exp_off, ret);
        YUNIT_ASSERT_EQUAL(tc->exp_off, ret);

        memset(read_buf, 0, sizeof(read_buf));
        read(fd, read_buf, tc->exp_size);
        printf("read_buf is %s\n", read_buf);

        if (tc->exp_data && strcmp(read_buf, tc->exp_data)) {
            YUNIT_FAIL("lseek read incorrect data");
            // tst_res(TFAIL, "lseek(%s, %ld, %s) read incorrect data",
            // 	TFILE, tc->off, tc->wname);
        } else {
            // tst_res(TPASS, "lseek(%s, %ld, %s) read correct data",
            // 	TFILE, tc->off, tc->wname);
	    }
    }

}

static char longpathname[256 + 2];
static struct tcases_unlink {
	char *name;
	char *desc;
	int exp_errno;
} tcases_unlink[] = {
	{"nonexistfile", "non-existent file", ENOENT},
	{"", "path is empty string", ENOENT},
	{"nefile/file", "path contains a non-existent file", ENOENT},
	{NULL, "invalid address", EFAULT},
	{"file/file", "path contains a regular file", ENOTDIR},
	{longpathname, "pathname too long", ENAMETOOLONG},
};

static void test_vfs_unlink()
{
    int ret;

    for(int i = 0; i < 6; i++)
    {
        struct tcases_unlink *tc = &tcases_unlink[i];

        ret = unlink(tc->name);
        if (ret != -1) {
            // tst_res(TFAIL, "unlink(<%s>) succeeded unexpectedly",
            //     tc->desc);
            return;
        }

        if (ret == tc->exp_errno) {
            // tst_res(TPASS | TTERRNO, "unlink(<%s>) failed as expected",
            //     tc->desc);
        } else {
            // tst_res(TFAIL | TTERRNO,
            //     "unlink(<%s>) failed, expected errno: %s",
            //     tc->desc, tst_strerrno(tc->exp_errno));
            YUNIT_FAIL("errno is not expected ");
        }
    }

}
*/

static yunit_test_case_t aos_vfs_testcases[] = {
    { "register", test_aos_vfs_case },
    // { "poll", test_aos_poll_case },
    { "fs_register", test_vfs_fs_case},
/*
    { "vfs_opendir", test_vfs_opendir},
    { "vfs_readdir", test_vfs_readdir},
    { "vfs_seekdir", test_vfs_seekdir},
    { "vfs_fsync", test_vfs_open_write_sync_close},
    { "vfs_lseek", test_vfs_lseek},
    { "vfs_unlink", test_vfs_unlink},
*/
    YUNIT_TEST_CASE_NULL
};

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

static yunit_test_suite_t suites[] = {
    { "vfs", init, cleanup, setup, teardown, aos_vfs_testcases },
    YUNIT_TEST_SUITE_NULL
};
/*
    // DIR *          opendir(const char *dirname);
    // struct dirent *readdir(DIR *dirp);
    // int            closedir(DIR *dirp);
    // long           telldir(DIR *dirp);
    // void           seekdir(DIR *dirp, long loc);
    // off_t          lseek(int fildes, off_t offset, int whence);
    int            stat(const char *path, struct stat *buf);
    int            statfs(const char *path, struct statfs *buf);
    // int            unlink(const char *path);
    // int            remove(const char *filename);
    int            rename(const char *oldname, const char *newname);
    // int            fsync(int fd);
    // int            mkdir(const char *path, mode_t mode);
    // int            rmdir(const char *path);
    // void           rewinddir(DIR *dirp);
    int            ioctl(int fildes, int request, ... );
    extern int ioctl(int fildes, int request, ... );
*/
void test_vfs(void)
{
    //ramfs_register();
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_vfs);
