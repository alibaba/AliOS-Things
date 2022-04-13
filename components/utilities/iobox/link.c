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

static void show_help(void)
{
    aos_cli_printf("Usage: link <from> <to>\n");
}

static int link_main(int argc, char **argv)
{
    int ret;

    if (argc != 3) {
        show_help();
        return -1;
    }

    ret = link(argv[1], argv[2]);
    if (ret < 0) {
        aos_cli_printf("link %s to %s failed - %s\n", argv[1], argv[2], strerror(errno));
        return -1;
    }
    return 0;
}
ALIOS_CLI_CMD_REGISTER(link_main, link, link file);
