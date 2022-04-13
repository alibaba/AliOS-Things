#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>

static int pwd_main(int argc, char **argv)
{
    char dir[256] = {0}, *ret;

    ret = getcwd(dir, sizeof(dir));
    if (!ret) {
        aos_cli_printf("Failed to get current working directory!\r\n");
        return -1;
    }

    aos_cli_printf("%s\r\n", dir);

    return 0;
}
ALIOS_CLI_CMD_REGISTER(pwd_main, pwd, print name of current working directory);
