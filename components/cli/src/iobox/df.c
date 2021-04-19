#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <fcntl.h>
#include <aos/vfs.h>
#include <path_helper.h>

extern int32_t vfs_get_node_name(const char *path, char names[][64], uint32_t* size);

#define KB (1024ULL)
static void df_do_dir(const char *dir)
{
    struct aos_statfs sfs;
    unsigned long long total, used, free;
    char abspath[256] = {0}, *dir1;

    dir1 = get_realpath(dir, abspath, sizeof(abspath));
    if (!dir1) {
        aos_cli_printf("Failed to get real path!\r\n");
        return;
    }

    if (access(dir1, F_OK) != 0) {
        aos_cli_printf("Failed to access path:%s\r\n", dir1);
        return;
    }

    if (aos_statfs(dir1, &sfs) < 0) {
        aos_cli_printf("statfs %s failed\n", dir);
        return;
    }

    total = ((unsigned long long)sfs.f_bsize * (unsigned long long)sfs.f_blocks) >> 10;
    free = ((unsigned long long)sfs.f_bsize * (unsigned long long)sfs.f_bavail) >> 10;
    used = total - free;

    if (total != 0 || !strcmp(dir, "/")) {
        aos_cli_printf("%10llu%10llu%10llu%6llu%%    %s\n", total, used, free,
                used * 100 / total, dir);
    } else {
        aos_cli_printf("%10llu%10llu%10llu     0%%    %s\n", total, used, free, dir);
    }
}

static void print_help()
{
    aos_cli_printf(
        "usage:\r\n"
        "  df <dir>\r\n"
        "eg:\r\n"
        "  1.show /etc/config disk space info:\r\n"
        "    df /etc/cofig\r\n");
}

static int df_main(int argc, char **argv)
{
    int i;
    char node_names[8][64];
    uint32_t count = 0;
    uint32_t index;


    if (argc >= 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_help();
        return 0;
    }

    aos_cli_printf("%10s%10s%10s%7s    %s\n", "Total", "Used", "Free", "Use%", "Mount");

    if (argc <= 1) {
        vfs_get_node_name("/", node_names, &count);
        for (index = 0; index < count; index++) {
            df_do_dir(node_names[index]);
        }

        return 0;
    }

    for (i = 1; i < argc; i++)
        df_do_dir(argv[i]);
    return 0;
}
ALIOS_CLI_CMD_REGISTER(df_main, df, show fs usage info);
