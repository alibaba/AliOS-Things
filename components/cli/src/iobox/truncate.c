/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <aos/cli.h>

extern char *optarg;
extern int optind, opterr, optopt;

static void truncate_usage()
{
    aos_cli_printf("truncate -s <size> <file>\n"
                   "    -s: The new size of the file.\n"
                   "    -h: Show help message and exit.\n"
                  );
}

static int truncate_main(int argc, char **argv)
{
    int opts = 0;
    int fd;
    int ret;
    int flag = 0;
    off_t new_size = -1;

    optind = 0;
    while ((opts = getopt(argc, argv, ":s:h")) != EOF) {
        switch (opts) {
        case 's':
            new_size = strtol(optarg, NULL, 0);
            flag = 1;
            break;
        case 'h':
            truncate_usage();
            return 0;
        case ':':
            aos_cli_printf("option -%c requires an argument\n", optopt);
            truncate_usage();
            return -1;
        default:
            truncate_usage();
            return -1;
        }
    }

    if ((!flag) || (new_size < 0)) {
        aos_cli_printf("The argument -s <size> must be specified and non-negative number.\n");
        truncate_usage();
        return -1;
    }

    if (argv[optind] == NULL) {
        aos_cli_printf("The argument <file> must be specified.\n");
        truncate_usage();
        return -1;
    }

    fd = open(argv[optind], O_WRONLY);
    if (fd < 0) {
        aos_cli_printf("open %s failed, ret:%d\n", argv[optind], fd);
        return -1;
    }

    ret = ftruncate(fd, new_size);
    if (ret < 0) {
        aos_cli_printf("truncate %s failed, ret:%d\n", argv[optind]);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
ALIOS_CLI_CMD_REGISTER(truncate_main, truncate, Increase or decrease the size of the file);
