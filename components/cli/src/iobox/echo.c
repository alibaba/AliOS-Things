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

static void print_help()
{
    aos_cli_printf("Usage:\r\n"
               "  - To stdout:\r\n"
               "      echo \"string\"\r\n"
               "  - To file:\r\n"
               "      echo \"string\" > file\r\n");
}

/**
 * Echo string to stdout or file. String must be quoted with '"' pair.
 *
 * echo to stdout:
 *   # echo "hello world"
 * echo to file:
 *   # echo "hello world" > /tmp/test.txt
 */
static int echo_main(int argc, char **argv)
{
    int fd, flag = O_CREAT | O_WRONLY, rc;
    bool append = false;
    char *str, *file;

    /* debug use */
    // for (int i = 0; i < argc; i++) printf("argv[%2d]: %s\r\n", i, argv[i]);

    if (argc < 2 || argc > 4) {
        print_help();
        return -1;
    }

    str = argv[1];

    if (argc == 2) {
        /* echo to stdout */
        printf("%s\r\n", str);
    } else {
        if (argv[2][0] != '>') {
            aos_cli_printf("Error: invalid input!\r\n");
            print_help();
            return -1;
        }

        /* in case of 'echo "hello" >' or 'echo "hello" >>' error */
        if ((argc == 3) && ((strlen(argv[2]) == 1) || ((strlen(argv[2]) == 2) && (argv[2][1] == '>')))) {
            aos_cli_printf("Error: no file provided!\r\n");
            print_help();
            return -1;
        }

        if (argv[2][1] == '>') {
            append = true;
            flag |= O_APPEND;
        } else {
            flag |= O_TRUNC;
        }

        if (argc == 3) {
            if (append)
                file = argv[2] + 2;
            else
                file = argv[2] + 1;
        } else {
            file = argv[3];
        }

        char abspath[256] = {0};
        file = get_realpath(file, abspath, sizeof(abspath));
        if (!file) {
            aos_cli_printf("Failed to get real path!\r\n");
            return -1;
        }

        fd = open(file, flag);
        if (fd < 0) {
            aos_cli_printf("Failed to open file %s\r\n", file);
            return errno;
        }

        int cnt = strlen(str);
        while (cnt > 0) {
            rc = write(fd, str, cnt);
            if (rc < 0) {
                aos_cli_printf("Failed to write to file!\r\n");
                close(fd);
                return errno;
            } else {
                cnt -= rc;
            }
        }

        close(fd);
    }

    return 0;
}

ALIOS_CLI_CMD_REGISTER(echo_main, echo, echo strings);
