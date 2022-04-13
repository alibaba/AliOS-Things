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
#include <aos/hal/flash.h>

static void print_mtdparts_help()
{
    aos_cli_printf("Usage: mtdparts\r\n");
}

static int mtdparts_main(int argc, char **argv)
{
    int cnt;
    int32_t ret;
    char name[32];
    uint32_t size, offset;
    hal_mtdpart_info_t *arr;

    ret = hal_flash_mtdpart_info_get(&arr, &cnt);
    if (ret) {
        printf("Failed to get mtd partition info!\r\n");
        return -1;
    }

    int *seq = (uint32_t *)malloc(cnt * sizeof(int)), *p;
    if (!seq) {
        printf("Failed to malloc in %s!\r\n", __func__);
        free(arr);
        return -1;
    }

    p = seq;
    memset(seq, -1, cnt * sizeof(int));
    int idx, i, j;
    for (i = 0; i < cnt; i++) {
        offset = arr[i].offset;

        for (j = 0; j < i; j++) {
            if (arr[seq[j]].offset <= offset) continue;
            memmove(seq+j+1, seq+j, sizeof(seq[0]) * (i-j));
            break;
        }

        seq[j] = i;
    }

    printf("DEV:Name\tSize\tOffset\r\n");
    printf("----------------------------------\r\n");
    for (i = 0; i < cnt; i++) {
        memset(name, 0, sizeof(name));
        snprintf(name, sizeof(name), "MTD%d:%s\t", i+1, arr[seq[i]].descr);
        if (strlen(name) <= 8) {
            snprintf(name+strlen(name), sizeof(name) - strlen(name), "\t");
        }

        size = arr[seq[i]].siz >> 10; // in KB uint
        offset = arr[seq[i]].offset;

        printf("%s%dk\t0x%08x\r\n", name, size, offset);
    }

    free(arr);
    free(seq);

    return 0;
}
ALIOS_CLI_CMD_REGISTER(mtdparts_main, mtdparts, print flash partition information)
