#include <stdio.h>
//#include <tinatest.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

static int cat_main(int argc, char **argv)
{
    int index;

    for (index = 1; index < argc; index++) {
        char *file = argv[index];
        struct stat s;
        int ret, fd;
        char buf[128];

        ret = stat(file, &s);
        if (ret) {
            aos_cli_printf("stat %s failed - %s\n", file, strerror(errno));
            continue;
        }

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
        }
close:
        close(fd);
    }
    return 0;
}
ALIOS_CLI_CMD_REGISTER(cat_main, cat, read file);
