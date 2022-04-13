#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include <aos/hal/flash.h>
#include <sys/types.h>

typedef enum {
    IO_TYPE_DEV = 1,
    IO_TYPE_FILE = 2,
    IO_TYPE_STD = 4,
    IO_TYPE_NULL = 8,
    IO_TYPE_MAX,
} io_type_t;

typedef enum {
    OP_TYPE_D2F = IO_TYPE_FILE | (IO_TYPE_DEV << 1),
    OP_TYPE_F2D = IO_TYPE_DEV | (IO_TYPE_FILE << 1),
    OP_TYPE_S2F = IO_TYPE_FILE | (IO_TYPE_STD << 1),
    OP_TYPE_F2N = IO_TYPE_NULL | (IO_TYPE_FILE << 1),
} dd_op_t;

#define GET_OP_TYPE(in, out) ((out) | ((in) << 1))

static void print_dd_help()
{
    aos_cli_printf("\r\n"
    "NAME\r\n"
    "       dd - convert and copy a file\r\n"
    "\r\n"
    "SYNOPSIS\r\n"
    "       dd [OPERAND]...\r\n"
    "       dd OPTION\r\n"
    "\r\n"
    "DESCRIPTION\r\n"
    "       Copy a file, converting and formatting according to the operands.\r\n"
    "\r\n"
    "Operands are:\r\n"
    "       bs=M/K/B\r\n"
    "              read and write up to Byte/KB/MB at a time\r\n"
    "\r\n"
    "       count=N\r\n"
    "              copy only N input blocks\r\n"
    "\r\n"
    "       if=FILE\r\n"
    "              read from FILE instead of stdin\r\n"
    "\r\n"
    "       of=FILE/dev\r\n"
    "              write to FILE or device instead of stdout\r\n"
    "\r\n"
    "Options are:\r\n"
    "       -h/--help display this help and exit\r\n"
    "\r\n"
    );
}

static io_type_t find_type(const char *path, int *idx)
{
    char *p;
    io_type_t type = IO_TYPE_MAX;

    if (path) {
        if (strncmp(path, "/dev/", strlen("/dev/")) == 0) {
            p = path + strlen("/dev/");

            if (strcmp(p, "null") == 0) {
                return IO_TYPE_NULL;
            }

            if (strncmp(p, "MTD", strlen("MTD")) != 0) {
                aos_cli_printf("Invalid dev %s provided! Currently only MTD "
                           "device is supported. Please check MTD partitions "
                            "using 'mtdparts' command.\r\n", path);
                return IO_TYPE_MAX;
            }

            char *tmp;
            *idx = (int)strtol(path + strlen("/dev/MTD"), &tmp, 10) - 1;
            if (*tmp != '\0') {
                aos_cli_printf("Invalid MTD index %s provided!\r\n", tmp);
                return IO_TYPE_MAX;
            }

            type = IO_TYPE_DEV;
        } else {
            type = IO_TYPE_FILE;
        }
    } else {
        type = IO_TYPE_STD;
    }

    return type;
}

static int get_mtd_partition_info(hal_mtdpart_info_t **a, int **s, int *c)
{
    int cnt, *seq, *p;
    int32_t ret;
    char name[32];
    uint32_t size, offset;
    hal_mtdpart_info_t *arr = NULL;

    ret = hal_flash_mtdpart_info_get(&arr, &cnt);
    if (ret) {
        printf("Failed to get mtd partition info!\r\n");
        return -1;
    }

    seq = (uint32_t *)malloc(cnt * sizeof(int));
    if (!seq) {
        printf("Failed to malloc in %s!\r\n", __func__);
        free(arr);
        return -1;
    }

    memset(seq, -1, cnt * sizeof(int));
    int idx, i, j;
    for (i = 0; i < cnt; i++) {
        offset = arr[i].offset;

        for (j = 0; j < i; j++) {
            if (arr[seq[j]].offset <= offset) continue;
            memmove(seq+j+1, seq+j, sizeof(seq[0]) * (i-j));
            break;
        }

        seq[j] = i;
    }

    *a = arr;
    *s = seq;
    *c = cnt;

    return 0;
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))
static int dd_mtd2file(hal_partition_t inp, const char *outfile,
                       uint32_t bsize, uint32_t count, bool fsync_flag)
{
    int fd, ret;
    unsigned char buf[256];
    long long t1, t2;

    (void)fsync_flag;

    aos_cli_printf("%s: parition %d -> file %s\r\n", __func__, inp, outfile);

    t1 = aos_now_ms();

    fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd < 0) {
        aos_cli_printf("Failed to open file %s, errno %d\r\n", outfile, errno);
        return fd;
    }

    uint32_t off, toread;
    int left = bsize;
    while (left > 0) {
        off = bsize * count - left;
        toread = MIN(left, sizeof(buf));

        // read from flash
        memset(buf, 0, sizeof(buf));
        ret = hal_flash_read(inp, &off, buf, toread);
        if (ret) {
            aos_cli_printf("Failed to read from flash: partition %d, "
                       "offset %d, len: %d\r\n", inp, off, toread);
            close(fd);
            return -1;
        }

        // write to file
        int write_done = 0;
        while (write_done < toread) {
            ret = write(fd, buf + write_done, toread - write_done);
            if (ret <= 0) {
                aos_cli_printf("Failed to write!");
                close(fd);
                return -1;
            }
            write_done += ret;
        }

        left -= toread;
    }

    close(fd);

    t2 = aos_now_ms();
    aos_cli_printf("Totally %ld bytes finished in %lld ms, "
               "average speed %lldKB/s\r\n", bsize * count,
               t2 - t1, ((bsize * count) / (t2 - t1) * 1000) >> 10);

    return 0;
}

static int dd_file2mtd(const char *infile, hal_partition_t outp, uint32_t siz)
{
    int fd, ret, err;
    uint32_t off, finished = 0;
    unsigned char buf[256];
    long long t1, t2;
    struct stat s;

    aos_cli_printf("%s: file %s -> parition %d siz %u\r\n",
               __func__, infile, outp, siz);

    if(stat(infile, &s)) {
        aos_cli_printf("Failed to stat file %s\r\n", infile);
        return -1;
    }

    err = hal_flash_erase(outp, 0, siz);
    if (err) {
        aos_cli_printf("Failed to erase!\r\n");
        return -1;
    }

    fd = open(infile, O_RDONLY);
    if (fd < 0) {
        aos_cli_printf("Failed to open file %s, errno %d\r\n", infile, errno);
        return fd;
    }

    t1 = aos_now_ms();

    memset(buf, 0, sizeof(buf));
    while ((ret = read(fd, buf, sizeof(buf))) > 0) {
        off = finished;
        err = hal_flash_write(outp, &off, buf, ret);
        if (err) {
            aos_cli_printf("Failed to write to flash, finished: %u!\r\n", finished);
            close(fd);
            return -1;
        }
        finished += ret;
        memset(buf, 0, sizeof(buf));
    }

    if (ret < 0) {
        aos_cli_printf("Failed to read from file!\r\n");
        close(fd);
        return -1;
    }

    close(fd);

    t2 = aos_now_ms();
    aos_cli_printf("Totally %u bytes finished in %lld ms, "
               "average speed %lldKB/s\r\n", finished,
               t2 - t1, (finished / (t2 - t1) * 100) >> 1);

    return 0;
}

#define DD_BUFFER_SIZE (256*1024)
static int dd_file2null(const char *infile)
{
    int fd, ret;
    unsigned char *buf;
    long long t1, t2;
    uint32_t total = 0;

    aos_cli_printf("%s: file %s -> null\r\n", __func__, infile);

    buf = malloc(DD_BUFFER_SIZE);
    if (!buf) {
        aos_cli_printf("malloc failed!\r\n");
        return -1;
    }

    fd = open(infile, O_RDONLY);
    if (fd < 0) {
        aos_cli_printf("Failed to open file %s, errno %d\r\n", infile, errno);
        free(buf);
        return fd;
    }

    t1 = aos_now_ms();

    while ((ret = read(fd, buf, DD_BUFFER_SIZE)) > 0) total += ret;

    if (ret < 0) {
        aos_cli_printf("Failed to read from file!\r\n");
        goto end;
    }

    t2 = aos_now_ms();
    aos_cli_printf("Totally %u bytes finished in %lld ms, "
               "average speed %lld KB/s\r\n", total,
               t2 - t1, (total / (t2 - t1) * 1000) >> 10);

    ret = 0;

end:
    if (buf) free(buf);
    close(fd);

    return ret;
}

static int read_from_stdio(unsigned char *buf, long len)
{
    memset(buf, (unsigned char)atoi(__LINE__), len);
    return len;
}

static int dd_stdio2file(const char *outfile, uint32_t bsize,
                         uint32_t count, bool fsync_flag)
{
    int readsiz, ret, fd, err = 0;
    uint32_t finished = 0;
    unsigned char *buf;
    long long t1, t2;

    (void)fsync_flag;

    aos_cli_printf("%s: stdio -> file %s, bs %ld, count %ld\r\n",
               __func__, outfile, bsize, count);

    buf = malloc(DD_BUFFER_SIZE);
    if (!buf) {
        aos_cli_printf("malloc failed!\r\n");
        return -1;
    }

    t1 = aos_now_ms();

    fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd < 0) {
        aos_cli_printf("Failed to open file %s, errno %d\r\n", outfile, errno);
        free(buf);
        return fd;
    }

    while (finished < (bsize * count)) {
        // read from stdio
        memset(buf, 0, DD_BUFFER_SIZE);
        if ((readsiz = read_from_stdio(buf, DD_BUFFER_SIZE)) < 0) {
            aos_cli_printf("Failed to read from stdio!\r\n");
            err = -1;
            goto end;
        }

        // write to file
        int write_done = 0;
        while (write_done < readsiz) {
            ret = write(fd, buf + write_done, readsiz - write_done);
            if (ret <= 0) {
                aos_cli_printf("Failed to write!");
                err = -1;
                goto end;
            }
            write_done += ret;
        }

        finished += readsiz;
    }

    t2 = aos_now_ms();
    aos_cli_printf("Totally %ld bytes finished in %lld ms, "
               "average speed %lld KB/s\r\n", bsize * count,
               t2 - t1, ((bsize * count) / (t2 - t1) * 1000) >> 10);

    err = 0;

end:
    if (buf) free(buf);
    close(fd);

    return err;
}

static int dd_main(int argc, char **argv)
{
    int cnt;
    int32_t ret;
    char name[32];
    uint32_t size, offset;
    hal_mtdpart_info_t *arr;
    char *infile = NULL, *outfile = NULL, *p;
    uint32_t bsize = 0, count = 0;
    bool fsync_flag = false;
    io_type_t intype = IO_TYPE_MAX, outtype = IO_TYPE_MAX;
    hal_partition_t inp, outp;
    int in_mtdidx, out_mtdidx;
    uint32_t mtdsiz = 0;

    if (argc == 1) {
        print_dd_help();
        return 0;
    }

    /* parse arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_dd_help();
            return 0;
        } else if (strncmp(argv[i], "if=", strlen("if="))== 0) {
            p = argv[i] + strlen("if=");
            if ((*p == '\0') || (*p != '/')) {
                aos_cli_printf("Invalid if argument %s! Please specify file "
                           "and use absolute path.\r\n", p ? p : "NULL");
                return -1;
            }

            infile = p;
        } else if (strncmp(argv[i], "of=", strlen("of=")) == 0) {
            p = argv[i] + strlen("of=");
            if ((*p == '\0') || (*p != '/')) {
                aos_cli_printf("Invalid of argument %s! Please specify file "
                           "and use absolute path.\r\n", p ? p : "NULL");
                return -1;
            }

            outfile = p;
        } else if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            p = argv[i] + strlen("bs=");
            if (!p) {
                aos_cli_printf("No valid bs argument provided!\r\n");
                return -1;
            }

            switch (*p) {
                case 'B': bsize = 1; break;
                case 'K': bsize = 1024; break;
                case 'M': bsize = 1024 * 1024; break;
                default:
                    aos_cli_printf("Invalid bs argument %s! Please specify it "
                               "using 'B', 'K', or 'M'.\r\n", p ? p : "NULL");
                    return -1;
            }
        } else if (strncmp(argv[i], "count=", strlen("count=")) == 0) {
            p = argv[i] + strlen("count=");
            if (!p) {
                aos_cli_printf("No valid count argument provided!\r\n");
                return -1;
            }

            char *tmp;
            count = strtol(p, &tmp, 10);
            if (*tmp != '\0') {
                aos_cli_printf("Not a valid count number provided!\r\n");
                return -1;
            }
        } else if (strncmp(argv[i], "conv=", strlen("conv=")) == 0) {
            p = argv[i] + strlen("conv=");
            if (!p || strcmp(p, "fsync") != 0) {
                aos_cli_printf("Not valid conv argument %s. Only 'fsync' is "
                           "supported so far.\r\n", p ? p : "NULL");
                return -1;
            }

            fsync_flag = true;
        } else {
            aos_cli_printf("Invalid argument %s\r\n", argv[i]);
            return -1;
        }
    }

    intype = find_type(infile, &in_mtdidx);
    if (intype >= IO_TYPE_MAX) intype = IO_TYPE_STD;
    outtype = find_type(outfile, &out_mtdidx);
    if (outtype >= IO_TYPE_MAX) outtype = IO_TYPE_STD;

    if (intype == IO_TYPE_DEV || outtype == IO_TYPE_DEV) {
        hal_mtdpart_info_t *part = NULL;
        int *seq = NULL, cnt = 0;
        if (get_mtd_partition_info(&part, &seq, &cnt) != 0) return -1;

        if (in_mtdidx == 0 || out_mtdidx == 0) {
            aos_cli_printf("Invalid MTD index (starting from 1, not 0)!\r\n");
            if (part) free(part);
            if (seq) free(seq);
            return -1;
        }

        if (in_mtdidx > cnt || out_mtdidx > cnt) {
            aos_cli_printf("MTD index too large.");
            if (part) free(part);
            if (seq) free(seq);
            return -1;
        }

        if (intype == IO_TYPE_DEV) {
            inp = part[seq[in_mtdidx]].p;
        }

        if (outtype == IO_TYPE_DEV) {
            outp = part[seq[out_mtdidx]].p;
            mtdsiz = part[seq[out_mtdidx]].siz;
        }

        if (part) free(part);
        if (seq) free(seq);
    }

    dd_op_t op = GET_OP_TYPE(intype, outtype);
    switch (op) {
        case OP_TYPE_D2F: dd_mtd2file(inp, outfile, bsize, count, fsync_flag); break;
        case OP_TYPE_F2D: dd_file2mtd(infile, outp, mtdsiz); break;
        case OP_TYPE_F2N: dd_file2null(infile); break;
        case OP_TYPE_S2F: dd_stdio2file(outfile, bsize, count, fsync_flag); break;
        default: aos_cli_printf("Unsupported operation!\r\n"); break;
    }

    return 0;
}
ALIOS_CLI_CMD_REGISTER(dd_main, dd, convert and copy a file);
