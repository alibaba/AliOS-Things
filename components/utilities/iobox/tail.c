#include <stdio.h>
#include <tinatest.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>

#define LINE_MAX_SIZE (1024)

static void show_help(void)
{
    aos_cli_printf("Usage: tail [-n line_cnt] <file>\n");
}

int tail_do(const char *file, unsigned int lcnt)
{
    char linebuf[LINE_MAX_SIZE];
    unsigned int num;
    int ret, fd;

    fd = open(file, O_RDONLY);
    if(fd < 0) {
        aos_cli_printf("Can not open the file %s\n", file);
        return -1;
    }

    lseek(fd, -1, SEEK_END);

    num = 0;
    ret = -1;
    while (true) {
        off_t off;
        char c;

        if (read(fd, &c, 1) != 1) {
            aos_cli_printf("read %s failed\n", file);
            goto err;
        }
        if(c == '\n')
             num++;

        if(num > lcnt)
             break;

        off = lseek(fd, -2, SEEK_CUR);
        if (off < 0) {
            aos_cli_printf("lseek %s failed\n", file);
            goto err;
        } else if(off == 0) {
            break;
        }
    }

    while (true) {
        memset(linebuf, 0, LINE_MAX_SIZE);
        ret = read(fd, linebuf, LINE_MAX_SIZE);
        if (ret > 0)
            aos_cli_printf("%s", linebuf);
        else if (ret == 0)
            break;
        else
            goto err;
    }

    ret = 0;
err:
    close(fd);
    return ret;
}

int tail_main(int argc, char **argv)
{
    int opts = 0, flags = 0;
    unsigned int lcnt = (unsigned int)(-1);

    optind = 0;
    while ((opts = getopt(argc, argv, ":hn:")) != EOF) {
        switch (opts) {
        case 'n': lcnt = atoi(optarg); break;
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
        aos_cli_printf("Please enter file\n");
        show_help();
        return -1;
    }

    return tail_do(argv[0], lcnt);
}
ALIOS_CLI_CMD_REGISTER(tail_main, tail, read file from tail);
