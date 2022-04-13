#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <path_helper.h>

static int touch_do(const char *file)
{
    int fd;

    fd = open(file, O_WRONLY | O_CREAT);
    if (fd < 0) {
        aos_cli_printf("open %s failed - %s\n", file, strerror(errno));
        return -1;
    }

    close(fd);
    return 0;
}

static int touch_main(int argc, char **argv)
{
    int index;
    int ret = 0;
    char abspath[256];

    for (index = 1; index < argc; index++) {
        char *file = argv[index];

        memset(abspath, 0 , sizeof(abspath));
        file = get_realpath(file, abspath, sizeof(abspath));
        if (!file) {
            aos_cli_printf("Failed to get real path!\r\n");
            return -1;
        }

        if (access(file, F_OK))
            ret |= touch_do(file);
    }
    return ret;
}
ALIOS_CLI_CMD_REGISTER(touch_main, touch, create empty file);
