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

#define MKDIR_FLAGS_PARENTS (1 << 0)
static void show_help(void)
{
    aos_cli_printf("Usage: mkdir [-p] <dir>\n");
    aos_cli_printf("  -p: make the upper directory if needed.\n");
}

static int mkdir_do(char *path, int flags)
{
    char *s = NULL;
    char abspath[256] = {0};

    path = get_realpath(path, abspath, sizeof(abspath));
    if (!path) {
        aos_cli_printf("Failed to get real path!\r\n");
        return -1;
    }


    /*
     * All of operations must base on root directory
     * As alios has not root dierctory, we can operate '/data' but not '/'
     */
#define MOUNT_BASE_DIR "/"
    if (strncmp(path, MOUNT_BASE_DIR, strlen(MOUNT_BASE_DIR))) {
        aos_cli_printf("make directory must base on %s\n", MOUNT_BASE_DIR);
        return -1;
    }

    if (path[0] == '.') {
        if (path[1] == '\0')
            return 0;
        if (path[1] == '.' && path[2] == '\0')
            return 0;
    }

    if (flags & MKDIR_FLAGS_PARENTS)
        s = path + strlen(MOUNT_BASE_DIR);
    while (1) {
        struct stat st;

        if (flags & MKDIR_FLAGS_PARENTS) {
            /* in case of tailing '/', such as '/data/a/' */
            if (*(s++) == '\0')
                break;
            s = strchr(s, '/');
            if (s)
                *s = '\0';
        }

        if (!stat(path, &st)) {
            if (S_ISDIR(st.st_mode))
                goto next;
            aos_cli_printf("make failed - %s already existed and not direcotry\n", path);
            return -1;
        }

        if (mkdir(path, 0777) < 0) {
            aos_cli_printf("mkdir %s failed\n", path);
            return -1;
        }

next:
        if (!s)
            break;
        *s = '/';
    }
    return 0;
}

static int mkdir_main(int argc, char **argv)
{
    int opts = 0, ret = 0, flags = 0, index;

    optind = 0;
    while ((opts = getopt(argc, argv, ":ph")) != EOF) {
        switch (opts) {
            case 'p':
                flags |= MKDIR_FLAGS_PARENTS;
                break;
            case 'h':
                show_help();
                return 0;
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
        char *path;

        path = strdup(argv[index]);
        ret = mkdir_do(path, flags);
        free(path);
        if (ret)
            return ret;
    }
    return 0;
}
ALIOS_CLI_CMD_REGISTER(mkdir_main, mkdir, make directory);
