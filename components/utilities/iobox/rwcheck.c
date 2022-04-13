#include <stdio.h>
#include <dirent.h>
#include <sys/statfs.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <aos/vfs.h>
#include <stdarg.h>
#include <k_api.h>

extern const k_mm_region_t g_mm_region[];

#define KB ((unsigned long long)1024)
#define MB (KB * 1024)
#define GB (KB * 1024)

#define DEFAULT_RUN_TIMES 1
#define DEFAULT_TEST_PERCENT 90
#define DEFAULT_TEST_SIZE (128 * KB)
#define DEFAULT_ORG_SIZE (4 * KB)
#define DEFAULT_BUF_SIZE (512)
#define DEFAULT_ORG "rwcheck.org"
#define DEFAULT_TMP "rwcheck.tmp"
#define MAX_PATH_LEN (256)
#define DEFAULT_TEST_DEV "/dev/UDISK"
#define DEFAULT_TEST_DIR "/data"
#define sum_t int32_t

#define VERSION "v0.1.0"
#define COMPILE "Compiled in " __DATE__ " at " __TIME__

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

extern int statfs(const char *path, struct statfs *buf);

static void show_help(void)
{
    aos_cli_printf("    Usage: rwcheck <-d dir> [-h] [-t times] [-s size] [-p percent]\n");
    aos_cli_printf("\n");
    aos_cli_printf("\t-h : show this massage and exit\n");
    aos_cli_printf("\t-d : # : the diretory to check [default currect path]\n");
    aos_cli_printf("\t-t # : check times\n");
    aos_cli_printf("\t-s # : set file size\n");
    aos_cli_printf("\t-p # : set maximum ratio of total flash size to check. Eg. -p 95\n");

    aos_cli_printf("\n");
    aos_cli_printf("  size trailing with k|m|g or not\n");
}

struct rwtask {
    const char *dir;
    unsigned int times;
    unsigned int percent;
    unsigned long long size;
    unsigned long long min_free;

    struct sys {
        uint32_t free_ddr;
        uint32_t total_ddr;
        uint32_t total_flash;
    } sys;
    char *org;
    char path[MAX_PATH_LEN];
};

static unsigned long long str_to_bytes(const char *str)
{
    unsigned long long size;
    char c;

    c = str[strlen(str) - 1];
    size = atoll(str);
    if (size == 0)
        return 0;

    switch (c) {
    case '0'...'9': return size;
    case 'k':
    case 'K': return size * KB;
    case 'm':
    case 'M': return size * MB;
    case 'g':
    case 'G': return size * GB;
    default:
          return 0;
    }

}

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

static inline uint32_t get_free_ddr(void)
{
    return g_kmm_head->free_size;
}

static inline uint32_t get_total_ddr(void)
{
    return g_mm_region[0].len;
}

static inline uint32_t get_total_flash(const char *dir)
{
    struct statfs sfs;

    if (statfs(dir, &sfs) < 0) {
        aos_cli_printf("statfs %s failed\n", dir);
        return 0;
    }

    return sfs.f_bsize * sfs.f_blocks;
}

static inline uint32_t get_free_flash(const char *dir)
{
    struct statfs sfs;

    if (statfs(dir, &sfs) < 0)
        return 0;

    /* get free by f_bavail rather than f_bfree */
    return sfs.f_bsize * sfs.f_bavail;
}

static int print_head(struct rwtask *task)
{
    time_t t = time(NULL);
    struct sys *sys = &task->sys;

    aos_cli_printf("\n\trwcheck: do read and write check\n\n");
    aos_cli_printf("\tversion: %s\n", VERSION);
    aos_cli_printf("\tbuild: %s\n", COMPILE);
    aos_cli_printf("\tdate: %s\n", ctime(&t));
    aos_cli_printf("\tfree/total ddr: %llu/%llu KB\n",
            sys->free_ddr / KB, sys->total_ddr / KB);
    aos_cli_printf("\tfree/total flash: %llu/%llu KB\n",
            get_free_flash(task->dir) / KB, sys->total_flash / KB);
    aos_cli_printf("\tset file size to %lld KB\n", task->size / KB);
    aos_cli_printf("\tset times to %u\n", task->times);
    aos_cli_printf("\tset max percent of total space to %u%%\n", task->percent);
    aos_cli_printf("\tset check diretory as %s\n", task->dir);
    aos_cli_printf("\tset orgin file as %s\n", task->org);

    return 0;
}

static int init_orgin(struct rwtask *task)
{
    int fd, size, ret;

    aos_cli_printf("\t--- INIT ---\n");

    /* if file existed, we should not create new one */
    if (!access(task->org, R_OK))
        return 0;

    fd = open(task->org, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd < 0) {
        aos_cli_printf("open %s failed - (%d)\n", task->org, fd);
        return -1;
    }

    aos_cli_printf("\r\tcreate\t: %s ... ", task->org);

    size = DEFAULT_ORG_SIZE;
    while (size) {
        int len;
        char buf[DEFAULT_BUF_SIZE] = {0};

        len = min(size, DEFAULT_BUF_SIZE);
        randbuf(buf, len);
        ret = write(fd, buf, len);
        if (ret != len) {
            aos_cli_printf("write %s failed - (%d)\n", task->org, ret);
            ret = -1;
            goto out;
        }

        size -= len;
    }

    aos_cli_printf("\r\tcreate\t: %s ... OK (%lluK)\n", task->org, DEFAULT_ORG_SIZE / KB);
    ret = 0;
out:
    close(fd);
    return ret;
}

static int deinit_orgin(struct rwtask *task)
{
    return remove(task->org);
}

static int copy_file(const char *in, const char *out, unsigned long long size)
{
    int fd_in, fd_out;
    int ret = -1;
    char buf[DEFAULT_BUF_SIZE];

    fd_in = open(in, O_RDONLY);
    if (fd_in < 0) {
        aos_cli_printf("open %s failed - (%d)\n", in, fd_in);
        goto out;
    }
    fd_out = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out < 0) {
        aos_cli_printf("open %s failed - (%d)\n", out, fd_out);
        goto close_in;
    }

    while (size > 0) {
        unsigned long long sz = min(size, (unsigned long long)DEFAULT_BUF_SIZE);
        unsigned long long done = 0;

        while (sz > done) {
            int rret;

            rret = read(fd_in, buf + done, sz - done);
            if (rret < 0) {
                aos_cli_printf("read %s failed - (%d)\n", in, rret);
                goto close_out;
            }
            /* lseek to start of file to ensure read full size */
            if (rret != sz - done) {
               off_t _ret = lseek(fd_in, 0, SEEK_SET);
               if (_ret == -1) goto close_out;
            }

            done += rret;
        }
        done = write(fd_out, buf, sz);
        if (done != sz) {
            aos_cli_printf("write %s failed - (%d)\n", out, (int)done);
            goto close_out;
        }
        size -= sz;
    }

    fsync(fd_out);
    ret = 0;
close_out:
    close(fd_out);
close_in:
    close(fd_in);
out:
    return ret;
}

static sum_t get_sum_by_buf(const unsigned char *buf, int len)
{
    sum_t sum = 0;

    while (--len >= 0)
        sum += buf[len];

    return sum;
}

static int get_sum_by_file(const char *file, int len, sum_t *sum)
{
    int fd, ret = -1;
    unsigned char buf[DEFAULT_BUF_SIZE];
    struct stat s;

    ret = stat(file, &s);
    if (ret) {
        aos_cli_printf("stat %s failed - (%d)\n", file, ret);
        return -1;
    }

    fd = open(file, O_RDWR);
    if (fd < 0) {
        aos_cli_printf("open %s failed - (%d)\n", file, fd);
        return -1;
    }

    len = min(s.st_size, (off_t)len);
    *sum = 0;
    while (len) {
        int rlen = min(len, DEFAULT_BUF_SIZE);

        ret = read(fd, buf, rlen);
        if (ret != rlen) {
            aos_cli_printf("read %s failed - (%d)\n", file, ret);
            close(fd);
            return -1;
        }

        *sum += get_sum_by_buf(buf, ret);
        len -= ret;
    }

    close(fd);
    return 0;
}

static int append_sum(const char *file)
{
    int fd, ret = -1;
    sum_t sum = 0;
    struct stat s;

    fd = open(file, O_RDWR);
    if (fd < 0) {
        aos_cli_printf("open %s failed - (%d)\n", file, fd);
        return -1;
    }

    ret = stat(file, &s);
    if (ret) {
        aos_cli_printf("stat %s failed - (%d)\n", file, ret);
        close(fd);
        return -1;
    }

    ret = get_sum_by_file(file, s.st_size, &sum);
    if (ret < 0) {
        aos_cli_printf("get sum by file %s failed\n", file);
        close(fd);
        return -1;
    }

    off_t _ret = lseek(fd, 0, SEEK_END);
    if (_ret == -1) {
        close(fd);
        return _ret;
    }

    ret = write(fd, &sum, sizeof(sum));
    if (ret != sizeof(sum)) {
        aos_cli_printf("write %s failed - (%d)\n", file, ret);
        goto out;
    }

    ret = 0;
out:
    close(fd);
    return ret;

}

static int read_sum_from_file(const char *file, sum_t *sum)
{
    int fd, ret;

    fd = open(file, O_RDONLY);
    if (fd < 0) {
        aos_cli_printf("open %s failed - (%d)\n", file, fd);
        return -1;
    }

    off_t _ret = lseek(fd, -sizeof(sum_t), SEEK_END);
    if (_ret == -1) {
        close(fd);
        return _ret;
    }

    ret = read(fd, sum, sizeof(sum_t));
    if (ret != sizeof(sum_t)) {
        aos_cli_printf("read sum from %s failed - (%d)\n", file, ret);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static unsigned int get_test_size(struct rwtask *task)
{
    unsigned int free_bytes = get_free_flash(task->dir);

    if (task->size >= free_bytes)
        return 0;

    if (free_bytes - task->size < task->min_free)
        return 0;

    return task->size;
}

static inline const char *get_path(struct rwtask *task, int index)
{
    memset(task->path, 0, MAX_PATH_LEN);
    snprintf(task->path, MAX_PATH_LEN, "%s/%s.%d", task->dir,
            DEFAULT_TMP, index);
    return task->path;
}

static int do_create(struct rwtask *task)
{
    uint32_t size = 0;
    const char *path;
    int num = 0, ret = 0;

    aos_cli_printf("\t--- CREATE ---\n");

    while ((size = get_test_size(task))) {
        path = get_path(task, num);
        if (!access(path, R_OK))
            return 0;

        aos_cli_printf("\r\tcreate\t: %s ... ", path);

        /* the last 8 bytes should reserved for check sum */
        ret = copy_file(task->org, path, size - sizeof(sum_t));
        if (ret)
            return -1;

        /* the last 2 btyes are reserved for saving crc */
        ret = append_sum(path);
        if (ret)
            return -1;

        aos_cli_printf("\r\tcreate\t: %s ... OK (%lluK)\n", path, size / KB);
        num++;
    }
    return 0;
}

static int do_check(struct rwtask *task)
{
    int num = 0;
    const char *path;

    aos_cli_printf("\t--- CHECK ---\n");

    while (true) {
        struct stat s;
        sum_t sum, sum_file;
        int ret = 0;

        path = get_path(task, num);
        if (access(path, R_OK))
            return 0;

        aos_cli_printf("\tcheck\t: %s ... ", path);

        ret = stat(path, &s);
        if (ret) {
            aos_cli_printf("stat %s failed - (%d)\n", path, ret);
            return -1;
        }

        if (s.st_size < 8) {
            path = get_path(task, num + 1);
            if (!access(path, R_OK)) {
                aos_cli_printf("FAILED (empty file)\n");
                return -1;
            } else {
                aos_cli_printf("OK: ignore the last one\n");
                break;
            }
        }

        ret = get_sum_by_file(path, s.st_size - sizeof(sum_t), &sum);
        if (ret < 0)
            return -1;

        ret = read_sum_from_file(path, &sum_file);
        if (ret < 0)
            return -1;

        if (sum != sum_file) {
            path = get_path(task, num + 1);
            if (!access(path, R_OK)) {
                aos_cli_printf("FAILED (check sum error 0x%x != 0x%x)\n",
                        sum, sum_file);
                return -1;
            } else {
                aos_cli_printf("OK: ignore the last one\n");
                break;
            }
        } else {
            aos_cli_printf("OK\n");
        }

        num++;
    }

    return 0;
}

static int get_max_file_index(struct rwtask *task)
{
    int i = 0;
    const char *path;

    while (true) {
        path = get_path(task, i);
        if (access(path, R_OK))
            return max(0, i - 1);
        i++;
    }
}

static int do_remove(struct rwtask *task)
{
    int num;
    const char *path;

    aos_cli_printf("\t--- REMOVE ---\n");

    num = get_max_file_index(task);

    while (num >= 0) {
        path = get_path(task, num);
        if (access(path, R_OK))
            return 0;

        aos_cli_printf("\tremove\t: %s ... ", path);
        if (unlink(path)) {
            aos_cli_printf("FAILED: %s\n", strerror(errno));
            return -1;
        }
        aos_cli_printf("OK\n");

        num--;
    }
    return 0;
}

static int begin(struct rwtask *task)
{
    int ret;

    print_head(task);
    while (task->times-- > 0) {
        aos_cli_printf("\n");

        ret = init_orgin(task);
        if (ret)
            return ret;

        ret = do_create(task);
        if (ret)
            return ret;

        ret = do_check(task);
        if (ret)
            return ret;

        ret = do_remove(task);
        if (ret)
            return ret;

        ret = deinit_orgin(task);
        if (ret)
            return ret;
    }
    aos_cli_printf("rwcheck OK\n");
    return 0;
}

static int rwcheck_main(int argc, char **argv)
{
    int opts = 0;
    int ret = 0;
    struct rwtask task;

    memset(&task, 0, sizeof(task));
    optind = 0;
    while ((opts = getopt(argc, argv, ":hd:t:s:p:")) != EOF) {
        switch (opts) {
        case 'h': show_help(); goto out;
        case 'd': {
                /*
                struct stat s;

                ret = stat(optarg, &s);
                if (ret) {
                    printf("stat %s failed - %s\n", optarg, strerror(errno));
                    goto out;
                }

                if (!S_ISDIR(s.st_mode)) {
                    printf("%s is not directory\n", optarg);
                    goto out;
                }
                */
                task.dir = optarg;
                break;
              }
        case 't': {
                task.times = atoi(optarg);
                if (!task.times) {
                    aos_cli_printf("times %s is zero or invalid\n", optarg);
                    goto out;
                }
                break;
              }
        case 's': {
                task.size = str_to_bytes(optarg);
                if (!task.size) {
                    aos_cli_printf("size %s is zero or invalid\n", optarg);
                    goto out;
                }
                break;
              }
        case 'p': {
                task.percent = atoi(optarg);
                if (!task.percent) {
                    aos_cli_printf("percent %s is zero or invalid\n", optarg);
                    goto out;
                }
                break;
              }
        case '?':
                aos_cli_printf("invalid option %c\n", optopt);
                goto out;
        case ':':
                aos_cli_printf("option -%c requires an argument\n", optopt);
                show_help();
                goto out;
        }
    }

    if (!task.dir) {
        aos_cli_printf("Which directory to check? Please tell me by '-d'\n");
        show_help();
        goto out;
    }

    if (!task.times)
        task.times = DEFAULT_RUN_TIMES;

    if (!task.percent)
        task.percent = DEFAULT_TEST_PERCENT;

    if (!task.size)
        task.size = DEFAULT_TEST_SIZE;

    task.org = malloc(strlen(task.dir) + sizeof(DEFAULT_ORG) + 2);
    if (!task.org) {
        aos_cli_printf("malloc for org failed\n");
        goto out;
    }
    snprintf(task.org, strlen(task.dir) + sizeof(DEFAULT_ORG) + 1,
             "%s/%s", task.dir, DEFAULT_ORG);

    task.sys.free_ddr = get_free_ddr();
    task.sys.total_ddr = get_total_ddr();
    task.sys.total_flash = get_total_flash(task.dir);
    task.min_free = task.sys.total_flash * (100 - task.percent) / 100;
    if (!task.sys.free_ddr || !task.sys.total_ddr ||
            !task.sys.total_flash || !task.min_free) {
        aos_cli_printf("invalid sys info: ddr %llu/%llu KB, flash %llu KB, min free %llu KB\n",
                task.sys.free_ddr / KB, task.sys.total_ddr / KB,
                task.sys.total_flash / KB, task.min_free / KB);
        goto free;
    }

    ret = begin(&task);
#ifdef AW_TINA_TEST
    if (ret)
        ttips("rwcheck failed\n");
    else
        ttips("rwcheck ok\n");
#endif
free:
    free(task.org);
out:
    return ret;
}
ALIOS_CLI_CMD_REGISTER(rwcheck_main, rwcheck, do read/write check);
