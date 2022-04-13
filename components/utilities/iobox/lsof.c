#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <aos/cli.h>
#include <stdbool.h>

static void lsfs_show_help(void)
{
    aos_cli_printf("Usage: lsfs\n");
}

#ifdef CONFIG_VFS_LSOPEN
extern void aos_dump_open();
static void lsof_show_help(void)
{
    aos_cli_printf("Usage: lsof\n");
}
static int lsof_do()
{
    aos_dump_open();
    return 0;
}

static int lsof_main(int argc, char **argv)
{
    if (argc > 1) {lsof_show_help(); return 0;}
    else return lsof_do();
}
ALIOS_CLI_CMD_REGISTER(lsof_main, lsof, list open files);
#endif

#include <fs/vfs_types.h>
#include <fs/vfs_api.h>
static int lsfs_do()
{
    return vfs_list(VFS_LIST_TYPE_FS);
}

static int lsfs_main(int argc, char **argv)
{
    if (argc > 1) {lsfs_show_help(); return 0;}
    else return lsfs_do();
}
ALIOS_CLI_CMD_REGISTER(lsfs_main, lsfs, list the registered filesystems);
