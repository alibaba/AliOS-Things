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
#include <sys/time.h>
#include <stdarg.h>
#include <k_api.h>

extern const k_mm_region_t g_mm_region[];
extern int statfs(const char *path, struct statfs *buf);

#define KB ((unsigned long long)1024)
#define MB (KB * 1024)
#define GB (KB * 1024)

#define DEFAULT_RUN_TIMES 3
#define DEFAULT_ORG "rwspeed.tmp"
#define DEFAULT_BUF_SIZE (4 * 1024)

#define VERSION "v0.1.0"
#define COMPILE "Compiled in " __DATE__ " at " __TIME__

#define min(x, y) ({                    \
        typeof(x) min1 = (x);           \
        typeof(y) min2 = (y);           \
        (void) (&min1 == &min2);        \
        min1 < min2 ? min1 : min2; })
#define max(x, y) ({                    \
        typeof(x) max1 = (x);           \
        typeof(y) max2 = (y);           \
        (void) (&max1 == &max2);        \
        max1 < max2 ? max2 : max1; })

struct rwtask {
    char *file;
    char *dir;
    char *path;
    unsigned int avg;
    unsigned long long size;
    unsigned long long r_ms;
    unsigned long long w_ms;

    struct sys {
        uint32_t free_ddr;
        uint32_t total_ddr;
        uint32_t total_flash;
    } sys;
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

    if (!dir)
        return 0;

    if (statfs(dir, &sfs) < 0) {
        aos_cli_printf("statfs %s failed\n", dir);
        return 0;
    }

    return sfs.f_bsize * sfs.f_blocks;
}

static inline uint32_t get_free_flash(const char *dir)
{
    struct statfs sfs;

    if (!dir)
        return 0;

    if (statfs(dir, &sfs) < 0)
        return 0;

    /* get free by f_bavail rather than f_bfree */
    return sfs.f_bsize * sfs.f_bavail;
}

static inline uint32_t get_file_size(const char *file)
{
    struct stat s;
    int ret;

    ret = stat(file, &s);
    if (ret)
        return 0;
    if (!S_ISREG(s.st_mode))
        return 0;
    return s.st_size;
}

static void show_help(void)
{
    aos_cli_printf("    Usage: rwspeed <-d dir> [-h] [-t avg] [-s size]\n");
    aos_cli_printf("\n");
    aos_cli_printf("\t-h : show this massage and exit\n");
    aos_cli_printf("\t-d : # : the diretory to check [default currect path]\n");
    aos_cli_printf("\t-t # : check times for average\n");
    aos_cli_printf("\t-s # : set file size\n");

    aos_cli_printf("\n");
    aos_cli_printf("  size trailing with k|m|g or not\n");
}

static void print_head(struct rwtask *task)
{
    time_t t = time(NULL);
    struct sys *sys = &task->sys;

    aos_cli_printf("\n\trwspeed: get seq read/write speed\n\n");
    aos_cli_printf("\tversion: %s\n", VERSION);
    aos_cli_printf("\tbuild: %s\n", COMPILE);
    aos_cli_printf("\tdate: %s\n", ctime(&t));
    aos_cli_printf("\tfree/total ddr: %llu/%llu KB\n",
            sys->free_ddr / KB, sys->total_ddr / KB);
    if (task->dir)
        aos_cli_printf("\tfree/total flash: %llu/%llu KB\n",
                get_free_flash(task->dir) / KB, sys->total_flash / KB);
    else
        aos_cli_printf("\tfile size: %u KB\n", get_file_size(task->file));
    aos_cli_printf("\tset file size to %lld KB\n", task->size / KB);
    aos_cli_printf("\tset average to %u\n", task->avg);
    if (task->file)
        aos_cli_printf("\tset check file as %s\n", task->file);
    else
        aos_cli_printf("\tset check diretory as %s\n", task->dir);
    aos_cli_printf("\tset r/w file as %s\n", task->path);
}

static void print_end(struct rwtask *task)
{

    aos_cli_printf("\n");
    aos_cli_printf("\tread average speed: %.2f KB/s\n",
            ((double)task->size * task->avg / KB) / ((double)task->r_ms / 1000));
    aos_cli_printf("\twrite average speed: %.2f KB/s\n",
            ((double)task->size * task->avg / KB) / ((double)task->w_ms / 1000));

#ifdef AW_TINA_TEST
    ttips("read %.2f KB/s write %.2f KB/s\n",
            ((double)task->size * task->avg / KB) / ((double)task->r_ms / 1000),
            ((double)task->size * task->avg / KB) / ((double)task->w_ms / 1000));
#endif
}

static int do_remove(struct rwtask *task)
{
    int ret = 0;

    if (task->dir) {
        ret = unlink(task->path);
        if (!ret)
            aos_cli_printf("\tremove\t: %s ... OK\n", task->path);
        else
            aos_cli_printf("\tremove\t: %s ... Failed - %s\n", task->path, strerror(errno));
    }
    return ret;
}

static inline unsigned long long auto_size(struct rwtask *task)
{
    if (task->dir)
        return get_free_flash(task->dir) * 85 / 100;
    else
        return get_file_size(task->file);
}

static int do_write(struct rwtask *task)
{
    int fd, ret = -1;
    char *buf;
    unsigned long long size = 0, start_ms = 0, end_ms = 0, ms = 0;
    struct timeval start = {0}, end = {0};

    aos_cli_printf("\r\twrite\t: %s ... ", task->path);

    buf = malloc(DEFAULT_BUF_SIZE);
    if (!buf) {
        aos_cli_printf("malloc failed - %s\n", strerror(errno));
        return -ENOMEM;
    }
    randbuf(buf, DEFAULT_BUF_SIZE);

    fd = open(task->path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        aos_cli_printf("open failed - %s\n", strerror(errno));
        goto out;
    }

    ret = gettimeofday(&start, NULL);
    if (ret) {
        aos_cli_printf("gettimeofday failed - %s\n", strerror(errno));
        close(fd);
        goto out;
    }

    ret = -1;
    size = task->size;
    while (size > 0) {
        unsigned long long done;
        unsigned long long sz;

        sz = min(size, (unsigned long long)DEFAULT_BUF_SIZE);
        done = write(fd, buf, sz);
        if (done != sz) {
            aos_cli_printf("write failed - %s\n", strerror(errno));
            close(fd);
            goto out;
        }
        size -= sz;
    }
    fsync(fd);
    close(fd);

    ret = gettimeofday(&end, NULL);
    if (ret) {
        aos_cli_printf("gettimeofday failed - %s\n", strerror(errno));
        goto out;
    }

    /* calculate the speed */
    start_ms = start.tv_sec * 1000 + start.tv_usec / 1000;
    end_ms = end.tv_sec * 1000 + end.tv_usec / 1000;
    ms = end_ms - start_ms;
    task->w_ms += ms;
    aos_cli_printf("OK (%.2f KB/s)\n", ((double)task->size / KB) / ((double)ms / 1000));

    ret = 0;
out:
    free(buf);
    return ret;
}

static int do_read(struct rwtask *task)
{
    int fd, ret = -1;
    char *buf;
    unsigned long long size = 0, start_ms = 0, end_ms = 0, ms = 0;
    struct timeval start = {0}, end = {0};

    aos_cli_printf("\r\tread\t: %s ... ", task->path);

    buf = malloc(DEFAULT_BUF_SIZE);
    if (!buf) {
        aos_cli_printf("malloc failed - %s\n", strerror(errno));
        return -ENOMEM;
    }

    fd = open(task->path, O_RDONLY, 0666);
    if (fd < 0) {
        aos_cli_printf("open failed - %s\n", strerror(errno));
        goto out;
    }

    ret = gettimeofday(&start, NULL);
    if (ret) {
        aos_cli_printf("gettimeofday failed - %s\n", strerror(errno));
        close(fd);
        goto out;
    }

    ret = -1;
    size = task->size ? task->size : auto_size(task);
    while (size > 0) {
        unsigned long long done;
        unsigned long long sz;

        sz = min(size, (unsigned long long)DEFAULT_BUF_SIZE);
        done = read(fd, buf, sz);
        if (done != sz) {
            aos_cli_printf("read failed - %s\n", strerror(errno));
            close(fd);
            goto out;
        }
        size -= sz;
    }

    ret = gettimeofday(&end, NULL);
    if (ret) {
        aos_cli_printf("gettimeofday failed - %s\n", strerror(errno));
        close(fd);
        goto out;
    }

    close(fd);

    /* calculate the speed */
    start_ms = start.tv_sec * 1000 + start.tv_usec / 1000;
    end_ms = end.tv_sec * 1000 + end.tv_usec / 1000;
    ms = end_ms - start_ms;
    task->r_ms += ms;
    aos_cli_printf("OK (%.2f KB/s)\n", ((double)task->size / KB) / ((double)ms / 1000));

    ret = 0;
out:
    free(buf);
    return ret;
}

static int begin(struct rwtask *task)
{
    int avg = task->avg;
    int ret;

    print_head(task);

    while (avg-- > 0) {
        aos_cli_printf("\n");

        ret = do_write(task);
        if (ret)
            return ret;

        ret = do_read(task);
        if (ret)
            return ret;

        ret = do_remove(task);
        if (ret)
            return ret;
    }

    print_end(task);

    return 0;
}

static int rwspeed_main(int argc, char **argv)
{
    int opts = 0;
    int ret = 0;
    struct rwtask *task;

    task = malloc(sizeof(struct rwtask));
    if (!task) {
        aos_cli_printf("malloc for task failed\n");
        return -1;
    }
    memset(task, 0, sizeof(*task));

    optind = 0;
    while ((opts = getopt(argc, argv, ":hd:t:s:f:")) != EOF) {
        switch (opts) {
        case 'h': show_help(); goto out;
        case 'f': {
                if (task->dir) {
                    aos_cli_printf("-f and -d is conflicting\n");
                    goto out;
                }
                task->file = optarg;
                break;
              }
        case 'd': {
                if (task->file) {
                    aos_cli_printf("-f and -d is conflicting\n");
                    goto out;
                }
                /* Alios's spiffs do not support stat directory */
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
                task->dir = optarg;
                break;
              }
        case 't': {
                task->avg = atoi(optarg);
                if (!task->avg) {
                    aos_cli_printf("average times %s is zero or invalid\n", optarg);
                    goto out;
                }
                break;
              }
        case 's': {
                task->size = str_to_bytes(optarg);
                if (!task->size) {
                    aos_cli_printf("size %s is zero or invalid\n", optarg);
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

    if (!task->dir && !task->file) {
        aos_cli_printf("Which directory/file to check? Please tell me by '-d/-f'\n");
        show_help();
        goto out;
    }

    if (!task->avg)
        task->avg = DEFAULT_RUN_TIMES;

    if (task->file) {
        task->path = task->file;
    } else {
        task->path = malloc(strlen(task->dir) + sizeof(DEFAULT_ORG) + 2);
        if (!task->path) {
            aos_cli_printf("malloc for path failed\n");
            goto out;
        }
        snprintf(task->path, strlen(task->dir) + sizeof(DEFAULT_ORG) + 1,
                 "%s/%s", task->dir, DEFAULT_ORG);
        unlink(task->path);
    }

    if (!task->size)
        task->size = auto_size(task);

    task->sys.free_ddr = get_free_ddr();
    task->sys.total_ddr = get_total_ddr();
    task->sys.total_flash = get_total_flash(task->dir);

    ret = begin(task);
    free(task->path);
out:
    free(task);
    return ret;
}
ALIOS_CLI_CMD_REGISTER(rwspeed_main, rwspeed, get seq read/write speed);
