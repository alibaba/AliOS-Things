#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>
#include <path_helper.h>

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

static int cat_main(int argc, char **argv)
{
    int index;
    char abspath[256];

    for (index = 1; index < argc; index++) {
        char *file = argv[index];
        struct stat s;
        int ret, fd, fsize, total = 0;
        char buf[128];

        memset(abspath, 0 , sizeof(abspath));
        file = get_realpath(file, abspath, sizeof(abspath));
        if (!file) {
            aos_cli_printf("Failed to get real path!\r\n");
            return -1;
        }

        ret = stat(file, &s);
        if (ret) {
            aos_cli_printf("stat %s failed - %s\n", file, strerror(errno));
            continue;
        }

        fsize = s.st_size;

        fd = open(file, O_RDONLY);
        if (fd < 0) {
            aos_cli_printf("open %s failed - %s\n", file, strerror(errno));
            continue;
        }

        while ((ret = read(fd, buf, 128))) {
            int wlen;

            if (ret < 0) {
                aos_cli_printf("read %s failed - %s\n", file, strerror(errno));
                goto close;
            }

            wlen = write(STDOUT_FILENO, buf, ret);
            if (wlen != ret) {
                aos_cli_printf("write %s failed - %s\n", file, strerror(errno));
                goto close;
            }

            total += ret;
            if (total >= fsize) {
                break;
            }
        }
        write(STDOUT_FILENO, "\r\n", 2);
close:
        close(fd);
    }
    return 0;
}
ALIOS_CLI_CMD_REGISTER(cat_main, cat, read file);
