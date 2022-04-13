#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <blkpart.h>
#include <awlog.h>
#include <time.h>
#include <stdarg.h>

#define DEBUG
#define DEFAULT_BLKDEV "/dev/UDISK"
#define DEFAULT_TIMES (10000)
#define BUF_SIZE (4 * 1024)
#define PRINT_ALIGN (32 * 1024)

#define min(x, y) ({                \
        typeof(x) min1 = (x);        \
        typeof(y) min2 = (y);        \
        (void) (&min1 == &min2);    \
        min1 < min2 ? min1 : min2; })
#define max(x, y) ({                \
        typeof(x) max1 = (x);        \
        typeof(y) max2 = (y);        \
        (void) (&max1 == &max2);    \
        max1 < max2 ? max2 : max1; })

struct blktask {
    const char *fdev;
    unsigned int fdevsize;
    unsigned int times;

    char *buf;
    unsigned int bufsize;
};

static int randbuf(char *buf, int len)
{
    int x;

    srand((unsigned int)time(NULL));
    while (len) {
        int l = min(len, (int)sizeof(int));

        x = rand();
        memcpy(buf, &x, l);

        len -= l;
        buf += l;
    }
    return 0;
}

static int init_buf(struct blktask *task)
{
    task->bufsize = BUF_SIZE;
    task->buf = malloc(task->bufsize);
    if (!task->buf) {
        printf("malloc for buf failed\n");
        return -1;
    }

    return randbuf(task->buf, task->bufsize);
}

static int do_write(struct blktask *task)
{
    unsigned int size, wlen;
    int ret = -1, fd;

    fd = open(task->fdev, O_WRONLY);
    if (fd < 0) {
        printf("open %s failed - (%d)\n", task->fdev, fd);
        goto out;
    }

    for (size = task->fdevsize, wlen = 0; size; size -= wlen) {
#ifdef DEBUG
        if (!((task->fdevsize - size) % PRINT_ALIGN) || size < PRINT_ALIGN)
            printf("\twriting %s offset 0x%x\n", task->fdev, task->fdevsize - size);
#endif

        wlen = min(task->bufsize, size);
        ret = write(fd, task->buf, wlen);
        if (ret != wlen) {
            printf("write %s failed - (%d)\n", task->fdev, ret);
            ret = -1;
            goto close;
        }
    }

    fsync(fd);
    ret = 0;
close:
    close(fd);
out:
    return ret;
}

static int do_check(struct blktask *task)
{
    unsigned int size, rlen;
    int fd, ret = -1;
    char *buf;

    buf = malloc(task->bufsize);
    if (!buf) {
        printf("malloc failed\n");
        goto out;
    }

    fd = open(task->fdev, O_RDONLY);
    if (fd < 0) {
        printf("open %s failed - (%d)\n", task->fdev, fd);
        goto free;
    }

    for (size = task->fdevsize, rlen = 0; size; size -= rlen) {
#ifdef DEBUG
        if (!((task->fdevsize - size) % PRINT_ALIGN) || size < PRINT_ALIGN)
            printf("\tchecking %s offset 0x%x\n", task->fdev, task->fdevsize - size);
#endif

	memset(buf, 0xA5, task->bufsize);
        rlen = min(task->bufsize, size);
        ret = read(fd, buf, rlen);
        if (ret != rlen) {
            printf("read %s failed - (%d)\n", task->fdev, ret);
            ret = -1;
            goto close;
        }

        if (memcmp(task->buf, buf, rlen)) {
            printf("offset 0x%x mismatch:\n", task->fdevsize - size);
            printf("org data:\n");
            hexdump(task->buf, rlen);
            printf("\nread data:\n");
            hexdump(buf, rlen);
            ret = -1;
            goto close;
        } else if (size == task->fdevsize) {
            printf("org data:\n");
            hexdump(task->buf, rlen);
            printf("\nread data:\n");
            hexdump(buf, rlen);
	}
    }

    ret = 0;
close:
    close(fd);
free:
    free(buf);
out:
    return ret;
}

static int begin(struct blktask *task)
{
    int ret;
    unsigned int times;

    printf("\trwdev: do read/write for block device\n");
    printf("\n\tblkdev: %s\n", task->fdev);
    printf("\tloop: %d\n", task->times);
    printf("\tblkdev size: %u\n\n", task->fdevsize);

    for (times = 0; times < task->times; times++) {
        ret = init_buf(task);
        if (ret)
            return ret;

#ifdef DEBUG
        printf("\t---- %d times ----\n", times);
#endif

#ifndef DEBUG
        printf("\twriting %d times ... ", times);
#endif
        ret = do_write(task);
        if (ret)
            return ret;
#ifndef DEBUG
        printf("OK\n");
#endif

#ifndef DEBUG
        printf("\tchecking %d times ... ", times);
#endif
        ret = do_check(task);
        if (ret)
            return ret;
#ifndef DEBUG
        printf("OK\n");
#endif
    }

    printf("rwdev OK\n");
    return 0;
}

static void show_help(void)
{
    printf("    Usage: aliblk [-b blkdev] [-h] [-t times]\n");
    printf("\n");
    printf("\t-h : show this massage and exit\n");
    printf("\t-b # : the block device to check [default %s]\n", DEFAULT_BLKDEV);
    printf("\t-t # : check times [default %d]\n", DEFAULT_TIMES);
}

static unsigned int get_dev_size(const char *fdev)
{
    struct stat s;

    if (stat(fdev, &s) != 0) {
        printf("stat %s failed\n", fdev);
        return 0;
    }
    return s.st_size;
}

static int rwdev_main(int argc, char **argv)
{
    int opts = 0;
    struct blktask task;

    memset(&task, 0, sizeof(task));
    optind = 0;
    while ((opts = getopt(argc, argv, ":ht:b:")) != EOF) {
        switch (opts) {
        case 'h': show_help(); return 0;
        case 't': {
                task.times = atoi(optarg);
                if (!task.times) {
                    printf("times %s is zero or invalid\n", optarg);
                    return -1;
                }
                break;
              }
        case 'b': {
                task.fdev = optarg;
                break;
              }
        }
    }

    task.fdev = task.fdev ? task.fdev : DEFAULT_BLKDEV;
    task.times = task.times ? task.times : DEFAULT_TIMES;
    task.fdevsize = get_dev_size(task.fdev);
    if (!task.fdevsize)
        return -1;

    return begin(&task);
}

ALIOS_CLI_CMD_REGISTER(rwdev_main, rwdev, read / write test);
