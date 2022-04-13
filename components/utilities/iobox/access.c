#include <stdio.h>
#include <tinatest.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>

static void show_help(void)
{
    aos_cli_printf("Usage: access <file>...\n");
}

static int access_main(int argc, char **argv)
{
    int index;

    for (index = 1; index < argc; index++) {
        char *file= argv[index];

        aos_cli_printf("[%s] ", file);
        if (!access(file, F_OK))
            aos_cli_printf("F_OK:y ");
        else
            aos_cli_printf("F_OK:n ");

        if (!access(file, R_OK))
            aos_cli_printf("R_OK:y ");
        else
            aos_cli_printf("R_OK:n ");

        if (!access(file, W_OK))
            aos_cli_printf("W_OK:y ");
        else
            aos_cli_printf("W_OK:n ");

        if (!access(file, X_OK))
            aos_cli_printf("X_OK:y ");
        else
            aos_cli_printf("X_OK:n ");
        aos_cli_printf("\n");
    }
    return 0;
}
ALIOS_CLI_CMD_REGISTER(access_main, access, check file exist);
