#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdlib.h>
#include <path_helper.h>

static void show_help(void)
{
    aos_cli_printf("Usage:\n"
               "rm <file1> [<file2>...]\n"
               "rm -r <dir>\n");
}

int rrmdir(const char *path)
{
    struct stat s;
    DIR *pdir = NULL;
    struct dirent *entry = NULL;
    int ret = -1;
    char *dir, *p;

    if (!path)
        return -EINVAL;

    dir = strdup(path);
    p = dir + strlen(dir) - 1;
    while ((*p == '/') && (p > dir)) {
        *p = '\0';
        p--;
    }

    if (stat(dir, &s) || !S_ISDIR(s.st_mode)) {
        aos_cli_printf("%s is neither existed nor a directory\n", dir);
        goto out;
    }

    pdir = opendir(dir);
    if (!pdir) {
        aos_cli_printf("opendir %s failed - %s\n", dir, strerror(errno));
        goto out;
    }

    ret = 0;
    while ((ret == 0) && (entry = readdir(pdir))) {
        char fpath[128];

        snprintf(fpath, 128, "%s/%s", dir, entry->d_name);

        ret = stat(fpath, &s);
        if (ret) {
            aos_cli_printf("stat %s failed\n", fpath);
            break;
        }

        if (!strcmp(entry->d_name, "."))
            continue;
        if (!strcmp(entry->d_name, ".."))
            continue;

        if (S_ISDIR(s.st_mode))
            ret = rrmdir(fpath);
        else
            ret = unlink(fpath);
    }

    closedir(pdir);
    if (ret == 0) {
        ret = rmdir(dir);
        if (ret)
            aos_cli_printf("rmdir %s failed\n", dir);
    }
out:
    free(dir);
    return ret;
}

#define RM_FLAG_RECURSION (1 << 0)
static int rm_main(int argc, char **argv)
{
    int opts = 0, flags = 0, index, ret = 0;

    optind = 0;
    while ((opts = getopt(argc, argv, ":r")) != EOF) {
        switch (opts) {
            case 'r':
                flags |= RM_FLAG_RECURSION;
                break;
            case '?':
                aos_cli_printf("invalid option %c\n", optopt);
                return -1;
            case ':':
                aos_cli_printf("option -%c requires an argument\n", optopt);
                show_help();
                return -1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc < 1) {
        show_help();
        return -1;
    }

    for (index = 0; index < argc; index++) {
        struct stat s;
        char *path = argv[index];
        char abspath[256] = {0};

        path = get_realpath(path, abspath, sizeof(abspath));
        if (!path) {
            aos_cli_printf("Failed to get real path!\r\n");
            return -1;
        }

        if (!stat(path, &s) && S_ISREG(s.st_mode)) {
            aos_cli_printf("remove %s\n", path);
            ret |= unlink(path);
            continue;
        }

        if (flags & RM_FLAG_RECURSION) {
            aos_cli_printf("remove dir %s\n", path);
            ret |= rrmdir(path);
            continue;
        }

        ret |= -ENOENT;
        aos_cli_printf("%s is neither existed nor a directory\n", path);
    }

    return ret;
}
ALIOS_CLI_CMD_REGISTER(rm_main, rm, remove file);
