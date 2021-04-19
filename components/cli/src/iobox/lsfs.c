#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>
#include <vfs_types.h>
#include <vfs_api.h>

extern int32_t vfs_list(vfs_list_type_t t);

static void lsfs_show_help(void)
{
    aos_cli_printf("Usage: lsfs\n");
}


static int lsfs_do()
{
    return vfs_list(VFS_LIST_TYPE_FS);
}

static int lsfs_main(int argc, char **argv)
{
    if (argc > 1) {
        lsfs_show_help();
        return 0;
    } else {
        return lsfs_do();
    }
}
ALIOS_CLI_CMD_REGISTER(lsfs_main, lsfs, list the registered filesystems);
