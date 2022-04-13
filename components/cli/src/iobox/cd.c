#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>

static char g_last_working_directory[512] = "/";

static int up_one_level(char *abspath)
{
    char *c, *last_slash;

    if (strcmp(abspath, "/") == 0)
        return -1;

    c = abspath + 1;
    last_slash = abspath;

    while (*c != '\0') {
        if ((*c == '/') && (*(c + 1) != '\0'))
            last_slash = c;
        c++;
    }

    *(last_slash + 1) = '\0';

    return 0;
}

#define ROOT_DIR "/"
static int cd_main(int argc, char **argv)
{
    char absolute[256] = {0}, *ret, *target, cwd_backup[256] = {0};
    struct stat s;

    if (argc > 1) {
        target = argv[1];
    } else {
        target = ROOT_DIR;
    }

    ret = getcwd(absolute, sizeof(absolute));
    if (!ret) {
        aos_cli_printf("Failed to get current working directory!\r\n");
        return -1;
    }

    strncpy(cwd_backup, absolute, sizeof(cwd_backup));

    if (target[0] != '/') {
        if (target[0] == '-') {
            memset(absolute, 0, sizeof(absolute));
            strncpy(absolute, g_last_working_directory, sizeof(absolute));
            absolute[sizeof(absolute) - 1] = '\0';
            goto check_and_cd;
        }

        if (absolute[strlen(absolute) - 1] != '/') {
            absolute[strlen(absolute)] = '/';
        }

        // exclude heading "./"
        while (strncmp(target, "./", strlen("./")) == 0) {
            target += strlen("./");
            while (target[0] == '/')
                target++;
        }

        // parse heading ".."
        while (target && strncmp(target, "..", strlen("..")) == 0) {
            if (up_one_level(absolute) != 0) {
                aos_cli_printf("up to parent dir failed. %s may " \
                           "not be a valid path!", target);
                return -1;
            }

            target += strlen("..");
            while (target[0] == '/')
                target++;
        }

        if (target)
            strncpy(absolute + strlen(absolute), target, \
                            sizeof(absolute) - strlen(absolute));
    } else {
        strncpy(absolute, target, sizeof(absolute));
    }

check_and_cd:
    if (stat(absolute, &s)) {
        aos_cli_printf("cd: %s not existed\r\n", target);
        return -1;
    }

    if (!S_ISDIR(s.st_mode)) {
        aos_cli_printf("cd: %s not a directory\r\n", target);
        return -1;
    }

    if (access(absolute, F_OK) != 0) {
        aos_cli_printf("%s is not a valid path.\r\n", target);
        return -1;
    }

    int err = chdir(absolute);
    if (err != 0) {
        aos_cli_printf("Failed to change to %s, errno: %d\r\n",
                        absolute, errno);
        return -1;
    }

    memset(g_last_working_directory,
           0,
           sizeof(g_last_working_directory));

    strncpy(g_last_working_directory,
            cwd_backup,
            sizeof(g_last_working_directory));

    return 0;
}
ALIOS_CLI_CMD_REGISTER(cd_main, cd, change current working directory);
