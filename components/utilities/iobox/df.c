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

#define KB (1024ULL)
static void df_do_dir(const char *dir)
{
    struct statfs sfs;
    unsigned long long total, used, free;

    extern int statfs(const char *path, struct statfs *buf);
    if (statfs(dir, &sfs) < 0) {
        aos_cli_printf("statfs %s failed\n", dir);
        return;
    }

    total = ((unsigned long long)sfs.f_bsize * (unsigned long long)sfs.f_blocks) >> 10;
    free = ((unsigned long long)sfs.f_bsize * (unsigned long long)sfs.f_bavail) >> 10;
    used = total - free;
    aos_cli_printf("%10llu%10llu%10llu%6llu%% %s\n", total, used, free,
            used * 100 / total, dir);
}

static int df_main(int argc, char **argv)
{
    int i;

    aos_cli_printf("%10s%10s%10s%7s %s\n", "Total", "Used", "Free", "Use%", "Mount");

    if (argc <= 1) {
        df_do_dir("/data");
        return 0;
    }

    for (i = 1; i < argc; i++)
        df_do_dir(argv[i]);
    return 0;
}
ALIOS_CLI_CMD_REGISTER(df_main, df, copy file);
