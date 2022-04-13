#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <aos/cli.h>
#include <path_helper.h>

#ifndef MIN
#define MIN(a, b) (a > b ? b : a)
#endif

#define HEXDUMP_LINE_CHR_CNT 16

struct hd_struct {
    const char *buf;
    int bytes;
    int baseaddr;
    char same_chr;
#define FLAGS_SAME_CHR (1 << 0)
#define FLAGS_DUMP_CHR (1 << 1)
#define FLAGS_PRINT_STAR (1 << 2)
    int flags;
};

static bool is_same_char(const unsigned char *buf, int len, unsigned char chr)
{
    for (len -= 1; len; len--) {
        if (buf[len] != chr)
            return false;
    }
    return true;
}

static int multi_hexdump(struct hd_struct *hd)
{
    unsigned char line[HEXDUMP_LINE_CHR_CNT] = {0};
    int addr = 0;

    for (addr = 0; addr < hd->bytes; addr += HEXDUMP_LINE_CHR_CNT) {
        int len = MIN(hd->bytes - addr, HEXDUMP_LINE_CHR_CNT), i;
        memcpy(line, hd->buf + addr, len);
        memset(line + len, 0, HEXDUMP_LINE_CHR_CNT - len);

        if (is_same_char(line, HEXDUMP_LINE_CHR_CNT, line[0])) {
            if (hd->flags & FLAGS_SAME_CHR) {
                if (is_same_char(line, HEXDUMP_LINE_CHR_CNT, hd->same_chr)) {
                    if (!(hd->flags & FLAGS_PRINT_STAR)) {
                        hd->flags |= FLAGS_PRINT_STAR;
                        aos_cli_printf("*\n");
                    }
                    continue;
                } else {
                    hd->flags &= ~(FLAGS_SAME_CHR | FLAGS_PRINT_STAR);
                }
            } else {
                hd->same_chr = line[0];
                hd->flags |= FLAGS_SAME_CHR;
            }
        } else {
            hd->flags &= ~(FLAGS_SAME_CHR | FLAGS_PRINT_STAR);
        }

        /* print addr */
        aos_cli_printf("0x%.8X: ", addr + hd->baseaddr);
        /* print hex */
        for (i = 0; i < HEXDUMP_LINE_CHR_CNT; i++) {
            if (i < len)
                aos_cli_printf("%.2X ", line[i]);
            else
                aos_cli_printf("   ");
        }

        /* print char */
        if (!(hd->flags & FLAGS_DUMP_CHR)) {
            aos_cli_printf("\n");
            continue;
        }
        aos_cli_printf("|");
        for (i = 0; i < HEXDUMP_LINE_CHR_CNT; i++) {
            if (i < len) {
                if (line[i] >= 0x20 && line[i] <= 0x7E)
                    aos_cli_printf("%c", line[i]);
                else
                    aos_cli_printf(".");
            } else {
                aos_cli_printf(" ");
            }
        }
        aos_cli_printf("|\n");
    }
    hd->baseaddr += addr;
    return 0;
}

static int hexdump_do(const char *file, int len, int skip, int chr)
{
    int ret = 0, fd = 0;
    char buf[128];
    struct hd_struct hd = {0};
    char abspath[256] = {0}, *file1;

    file1 = get_realpath(file, abspath, sizeof(abspath));
    if (!file) {
        aos_cli_printf("Failed to get real path!\r\n");
        return -1;
    }

    fd = open(file1, O_RDONLY);
    if (fd < 0) {
        aos_cli_printf("open %s failed - %s\n", file, strerror(errno));
        return -1;
    }

    if (skip) {
        ret = lseek(fd, skip, SEEK_SET);
        if (ret < 0) {
            aos_cli_printf("lseek %s failed - %s\n", file, strerror(errno));
            goto close;
        }
    }

    if (chr)
        hd.flags |= FLAGS_DUMP_CHR;

    len = len ? len : INT32_MAX;
    while (len > 0 && (ret = read(fd, buf, MIN(128, len)))) {

        if (ret < 0) {
            aos_cli_printf("read %s failed - %s\n", file, strerror(errno));
            goto close;
        }

        hd.buf = buf;
        hd.bytes = ret;
        multi_hexdump(&hd);
        len -= ret;
    }

close:
    close(fd);
    return 0;
}

static int show_help(void)
{
    aos_cli_printf("usage:\r\n"
               "  hexdump [-C] [-n length] [-s skip] file\r\n"
               "options:\r\n"
               "  -C          show ASCII and hexadecimal\r\n"
               "  -n length   set file length(Byte) for hexadecimal conversion\r\n"
               "  -s skip     set start position(Byte) for hexadecimal conversion\r\n");
    return 0;
}

static int hexdump_main(int argc, char **argv)
{
    int opts = 0, len = 0, skip = 0, chr = 0;

    optind = 0;
    while ((opts = getopt(argc, argv, ":n:hs:C")) != EOF) {
        switch (opts) {
            case 'n':
                len = atoi(optarg);
                break;
            case 's':
                skip = atoi(optarg);
                break;
            case 'C':
                chr = true;
                break;
            case 'h':
                show_help();
                return 0;
            case '?':
                aos_cli_printf("invalid option %c\n", optopt);
                return -1;
            case ':':
                aos_cli_printf("option -%c requires an argument\n", optopt);
                show_help();
                return -1;
        }
    }

    argc -= optind;
    argv += optind;

    for (opts = 0; opts < argc; opts++)
        hexdump_do(argv[opts], len, skip, chr);

    return 0;
}
ALIOS_CLI_CMD_REGISTER(hexdump_main, hexdump, dump binary data in decimal);

static int hd_main(int argc, char **argv)
{
    char *_argv[128 + 1] = {0};
    int i = 0;

    argc = MIN(argc, 128);
    _argv[i++] = "hexdump";
    _argv[i++] = "-C";
    for (; i < argc + 1; i++)
        _argv[i] = argv[i - 1];
    return hexdump_main(argc + 1, _argv);
}
ALIOS_CLI_CMD_REGISTER(hd_main, hd, the same as 'hexdump -C');
