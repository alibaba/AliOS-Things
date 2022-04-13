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

#define ROOT_DIR "/"
static int cd_main(int argc, char **argv)
{
    char absolute[256] = {0}, *ret, *target;

    if (argc > 1) {
        target = argv[1];
    } else {
        target = ROOT_DIR;
    }

    if (target[0] != '/') {
        ret = getcwd(absolute, sizeof(absolute));
        if (!ret) {
            aos_cli_printf("Failed to get current working directory!\r\n");
            return -1;
        }

        snprintf(absolute + strlen(absolute), sizeof(absolute) - strlen(absolute), "/%s", target);
    } else {
        strncpy(absolute, target, sizeof(absolute) - 1);
    }

    int err = chdir(absolute);
    if (err != 0) {
        aos_cli_printf("Failed to change to %s, errno: %d\r\n", absolute, errno);
        return -1;
    } else {
        aos_cli_printf("Now inside %s\r\n", absolute);
    }

    return 0;
}
ALIOS_CLI_CMD_REGISTER(cd_main, cd, change current working directory);
