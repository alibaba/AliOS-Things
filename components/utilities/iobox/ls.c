#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>

#define LONGLIST    (1 << 0)
#define SZ_KB       (1 << 1)
#define NEW_LINE    (1 << 2)

static void show_help(void)
{
    aos_cli_printf("Usage: ls [-l] [-h] [-k] <file>...\n");
    aos_cli_printf("  -l: use long list\n");
    aos_cli_printf("  -h: show this help message and exit");
    aos_cli_printf("  -k: use 1024-byte blocks");
}

static const char *relative_path(const char *path, const char *dir)
{
    const char *p = path + strlen(dir);

    while (*p == '/')
        p++;
    return p < path + strlen(path) ? p : NULL;
}

static void ls_show(const char *file, int flags, struct stat *s)
{
    if (!(flags & LONGLIST)) {
        if (flags & NEW_LINE)
            aos_cli_printf("%s\n", file);
        else
            aos_cli_printf("%s ", file);
        return;
    }

    switch (s->st_mode & S_IFMT) {
    case S_IFBLK: aos_cli_printf("%c", 'b'); break;
    case S_IFCHR: aos_cli_printf("%c", 'c'); break;
    case S_IFDIR: aos_cli_printf("%c", 'd'); break;
    case S_IFLNK: aos_cli_printf("%c", 'l'); break;
    case S_IFREG: aos_cli_printf("%c", '-'); break;
    default: aos_cli_printf("%c", '?'); break;
    }

    if (s->st_size > 1024) {
        flags |= SZ_KB;
    }
    //aos_cli_printf
    printf("%c%c%c%c%c%c%c%c%c root root %llu%sB %s\r\n",
            s->st_mode & S_IRUSR ? 'r' : '-',
            s->st_mode & S_IWUSR ? 'w' : '-',
            s->st_mode & S_IXUSR ? 'x' : '-',
            s->st_mode & S_IRGRP ? 'r' : '-',
            s->st_mode & S_IWGRP ? 'w' : '-',
            s->st_mode & S_IXGRP ? 'x' : '-',
            s->st_mode & S_IROTH ? 'r' : '-',
            s->st_mode & S_IWOTH ? 'w' : '-',
            s->st_mode & S_IXOTH ? 'x' : '-',
            (flags & SZ_KB) ? s->st_size / 1024 : s->st_size,
            (flags & SZ_KB) ? "K" : "",
            file);
}

#ifdef LIST_DEVICE
#include <fs/vfs_types.h>
#include <fs/vfs_api.h>
#define DEVICE_PATH_PREFIX "/dev"
#define DEVICE_PATH_PREFIX_SLASH_TAILED "/dev/"
#endif

static int ls_do(int argc, char **argv, int flags)
{
    int index;

    for (index = 0; index < argc; index++) {
        char *dir = argv[index];
        DIR *pdir = NULL;
        struct dirent *entry = NULL;
        struct stat s;
        int cnt = 1;

#ifdef LIST_DEVICE
        if ((strncmp(dir, DEVICE_PATH_PREFIX_SLASH_TAILED,
            strlen(DEVICE_PATH_PREFIX_SLASH_TAILED)) == 0) ||
            (strcmp(dir, DEVICE_PATH_PREFIX) == 0)) {
            vfs_list(VFS_LIST_TYPE_DEVICE); // 0: fs, 1: device
        }
#endif

        if (stat(dir, &s)) {
            aos_cli_printf("%s not existed\n", dir);
            continue;
        }
        if (!S_ISDIR(s.st_mode)) {
            flags |= NEW_LINE;
            ls_show(dir, flags, &s);
            if (argc > 1 && index + 1 < argc)
                aos_cli_printf("\n");
            continue;
        } else {
            if (argc > 1)
                aos_cli_printf("%s:\n", dir);

            pdir = opendir(dir);
            if (!pdir) {
                aos_cli_printf("opendir %s failed - %s\n", argv[index], strerror(errno));
                continue;
            }

            while ((entry = readdir(pdir))) {
                char fpath[260];
                int len;

                memset(fpath, 0, 260);
                len = strlen(dir);
#if AOS_COMP_SPIFFS
                /*
                 * fix for spiffs
                 * readdir/opendir on spiffs will traverse all
                 * files in all directories
                 */
                snprintf(fpath, 260, "/data/%s", entry->d_name);
                if (strncmp(dir, fpath, len))
                    continue;
#else
                if (dir[len] == '/')
                    snprintf(fpath, 260, "%s%s", dir, entry->d_name);
                else
                    snprintf(fpath, 260, "%s/%s", dir, entry->d_name);
#endif

                if (stat(fpath, &s)) {
                    aos_cli_printf("stat %s failed - %s\n", fpath, strerror(errno));
                    continue;
                }

                if (!(cnt % 5))
                    flags |= NEW_LINE;
                else
                    flags &= ~NEW_LINE;
                cnt++;

                ls_show(relative_path(fpath, dir), flags, &s);
            }
            closedir(pdir);
            if (!(flags & LONGLIST) && !(flags & NEW_LINE))
                aos_cli_printf("\n");
            if (argc > 1 && index + 1 < argc)
                aos_cli_printf("\n");
        }
    }
    return 0;
}

static int get_cwd_dir(char *dir, size_t len)
{
    char *ret;

    ret = getcwd(dir, len);
    if (!ret) {
        aos_cli_printf("Failed to get current working directory!\r\n");
        return -1;
    }

    return 0;
}

static int ls_cur(int argc, char **argv, int flags)
{
    int argc2;
    char *argv2[2] = {NULL};
    char dir[256] = {0};

    if (get_cwd_dir(dir, sizeof(dir))) return -1;

    if (argc == 0) {
        argc2 = 1;
        argv2[0] = dir;
    } else {
        argc2 = 2;
        argv2[0] = argv[0];
        argv2[1] = dir;
    }

    return ls_do(argc2, argv2, flags);
}

static int ls_main(int argc, char **argv)
{
    int opts = 0, flags = 0, flag_cnt = 0;

    optind = 0;
    while ((opts = getopt(argc, argv, ":lhk")) != EOF) {
        switch (opts) {
        case 'l': flags |= LONGLIST; flag_cnt++; break;
        case 'k': flags |= SZ_KB; flag_cnt++; break;
        case 'h': show_help(); return 0;
        case '?': aos_cli_printf("invalid option %c\n", optopt); return -1;
        case ':':
            aos_cli_printf("option -%c requires an argument\n", optopt);
            show_help();
            return -1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc == 0) {
        return ls_cur(argc, argv, flags);
    }

    return ls_do(argc, argv, flags);
}
ALIOS_CLI_CMD_REGISTER(ls_main, ls, list file or directory);

static int ll_main(int argc, char **argv)
{
    argc -= 1;
    argv += 1;

    if ((argc == 0) || ((argc == 1) && (strcmp(argv[0], "-l") == 0))) {
        return ls_cur(argc, argv, LONGLIST);
    }

    return ls_do(argc, argv, LONGLIST);
}
ALIOS_CLI_CMD_REGISTER(ll_main, ll, the same as 'ls -kl');
