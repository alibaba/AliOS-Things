#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include "md5.h"

static void print_md5sum_help()
{
    aos_cli_printf("Usage: md5sum <file>\r\n");
}

#define MD5_DIGEST_LENGTH 16 // 128-bit md5
static int md5sum_main(int argc, char **argv)
{
    int fd;
    MD5_CTX c = {0};
    unsigned char buf[256];
    ssize_t bytes;
    unsigned char out[MD5_DIGEST_LENGTH];
    FILE *file;

    if (argc != 2) {
        print_md5sum_help();
        return -1;
    }

    MD5_Init(&c);

    file = fopen(argv[1], "rb");
    if (!file) {
        aos_cli_printf("Failed to open file %s\r\n", argv[1]);
        return -1;
    }

    do {
        bytes = fread(buf, 1, sizeof(buf), file);

        if ((bytes < sizeof(buf)) && !feof(file)) {
            aos_cli_printf("Failed to read file %s\r\n", argv[1]);
            fclose(file);
            return -1;
        }

        MD5_Update(&c, buf, bytes);
    } while((bytes == sizeof(buf)) && !feof(file));

    fclose(file);

    MD5_Final(out, &c);

    for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        printf("%02x", out[i]);
    }

    printf(" %s\r\n", argv[1]);

    return 0;
}
ALIOS_CLI_CMD_REGISTER(md5sum_main, md5sum, compute and check MD5 message digest);
