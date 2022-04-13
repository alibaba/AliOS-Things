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

extern int asprintf(char **__restrict, const char *__restrict, ...)
               _ATTRIBUTE((__format__(__printf__, 2, 3)));

static void show_help(void)
{
    aos_cli_printf("Usage: mv <from> <to>\n");
}

static int mv_main(int argc, char **argv)
{
    char *from = NULL, *to = NULL;
    int fd_from = 0, fd_to = 0;
    char buf[128] = {0};
    int rlen = 0, wlen = 0, ret = -1, isdir = false;
    struct stat s;

    if (argc != 3) {
        show_help();
        return -1;
    }

    from = argv[1];
    to = argv[2];

    char abspath_from[256] = {0}, abspath_to[256] = {0};

    from = get_realpath(from, abspath_from, sizeof(abspath_from));
    to = get_realpath(to, abspath_to, sizeof(abspath_to));
    if (!from || !to) {
        aos_cli_printf("Failed to get real path!\r\n");
        return -1;
    }

    if (!stat(to, &s)) {
        if (S_ISDIR(s.st_mode)) {
            char *p = strrchr(from, '/');

            if (!p)
                ret = asprintf(&to, "%s/%s", to, from);
            else
                ret = asprintf(&to, "%s%s", to, p);
            if (ret < 0) {
                aos_cli_printf("asprintf failed\n");
                return -1;
            }
            isdir = true;
        }
    }

    ret = rename(from, to);
    if (ret < 0 && errno != EXDEV) {
        aos_cli_printf("rename %s to %s failed - %s\n", from, to, strerror(errno));
        return -1;
    } else if (ret == 0) {
        return 0;
    }

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0) {
        aos_cli_printf("open %s failed - %s\n", from, strerror(errno));
        return -1;
    }

    fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd_to < 0) {
        aos_cli_printf("open %s failed - %s\n", to, strerror(errno));
        goto close_from;
    }

    while ((rlen = read(fd_from, buf, 128))) {
        if (rlen < 0) {
            aos_cli_printf("read %s failed - %s\n", from, strerror(errno));
            goto close_to;
        }

        wlen = write(fd_to, buf, rlen);
        if (wlen != rlen) {
            aos_cli_printf("write %s failed - %s\n", to, strerror(errno));
            goto close_to;
        }
    }

    ret = unlink(from);
    if (ret) {
        aos_cli_printf("unlink %s failed - %s\n", from, strerror(errno));
        goto close_to;
    }

    ret = 0;
close_to:
    close(fd_to);
close_from:
    close(fd_from);
    if (isdir)
        free(to);
    return ret;
}
ALIOS_CLI_CMD_REGISTER(mv_main, mv, move file);
