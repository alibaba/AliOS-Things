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
#ifdef AOS_COMP_FATFS
#include <fs/fatfs.h>
#endif

static void print_mount_help()
{
    //aos_cli_printf("Usage: mount -a [-t fstype] [dir] [mmcblk0|usb]\r\n");
    aos_cli_printf("\r\n"
       "Mount a filesystem.\r\n"
       "Usage:\r\n"
       "  mount [-h]\r\n"
       "  mount -a [-t fstype]\r\n"
       "  mount [-t fstype] dir device\r\n"
    );
}

static void print_umount_help()
{
    //aos_cli_printf("Usage: umount [-a] [dir]\r\n");
    aos_cli_printf("\r\n"
       "Unmount file systems.\r\n"
       "Usage:\r\n"
       "  umount -h\r\n"
       "  umount -a\r\n"
       "  umount {directory|device}...\r\n"
    );
}

typedef enum {
    VFAT_MOUNT_T,
    NFS_MOUNT_T
} mount_fs_t;

static int mount_main(int argc, char **argv)
{
    int32_t ret;
    int opts = 0;
    bool all = false;
    int32_t dev;
    char *dir = NULL;
    mount_fs_t type;

    optind = 0;
    while ((opts = getopt(argc, argv, ":hat:")) != EOF) {
        switch (opts) {
        case 'h': print_mount_help(); return 0;
        case 'a': all = true; break;
        case 't':
            // only support vfat so far
            if (strcmp(optarg, "vfat") == 0) {
                type = VFAT_MOUNT_T;
            } else if (strcmp(optarg, "nfs") == 0) {
                type = NFS_MOUNT_T;
            } else {
                printf("Error: invalid fstype (please choose from 'vfat', 'nfs').\r\n");
                return -1;
            }
            break;
        case '?': aos_cli_printf("invalid option %c\n", optopt); return -1;
        case ':':
            aos_cli_printf("option - %c requires an argument\n", optopt);
            print_mount_help();
            return -1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc == 0) {
        if (all) {
        #ifdef AOS_COMP_FATFS
            return fatfs_register();
        #endif
        } else {
        #ifdef AOS_COMP_NFS
            nfs_list();
        #endif
        #ifdef AOS_COMP_FATFS
            fatfs_list();
        #endif
            return 0;
        }
    } else if (argc == 2) {
        dir = argv[0];
        // strip tailing slash
        for (int i = strlen(dir); i > 0; i--) {
            if (dir[i] == '/') dir[i] = '\0';
        }

        if (type == VFAT_MOUNT_T) {
        #ifdef AOS_COMP_FATFS
            if (strcmp(argv[1], "sdmmc0") == 0) {
                dev = DEV_MMC;
            } else if (strcmp(argv[1], "usb0") == 0) {
                dev = DEV_USB;
            } else if (strcmp(argv[1], "ram0") == 0) {
                dev = DEV_RAM;
            } else {
                aos_cli_printf("Unsupported device! Please choose "
                           "from 'sdmmc0', 'usb0', 'ram0'\r\n");
                return -1;
            }
    
            return fatfs_dev_register(dev, dir);
        #endif
        } else {
        #ifdef AOS_COMP_NFS
            return nfs_mount(dir, 0, argv[1]);
        #endif
        }
    } else {
        aos_cli_printf("Invalid argument,\r\n");
        print_mount_help();
        return -1;
    }

    return 0;
}
ALIOS_CLI_CMD_REGISTER(mount_main, mount, mount partition);

static int umount_main(int argc, char **argv)
{
    int32_t ret;
    int opts = 0;
    bool all = false;
    int32_t dev;
    char *dir = NULL;

    optind = 0;
    while ((opts = getopt(argc, argv, ":ha")) != EOF) {
        switch (opts) {
        case 'h': print_umount_help(); return 0;
        case 'a': all = true; break;
        case '?': aos_cli_printf("invalid option %c\n", optopt); return -1;
        case ':':
            aos_cli_printf("option - %c requires an argument\n", optopt);
            print_umount_help();
            return -1;
        }
    }

    argc -= optind;
    argv += optind;

    if ((argc == 0) && all) {
    #ifdef AOS_COMP_FATFS
        return fatfs_unregister();
    #endif
    } else if (argc == 1) {
        #ifdef AOS_COMP_FATFS
        if (strcmp(argv[0], "sdmmc0") == 0) {
            dev = DEV_MMC;
        } else if (strcmp(argv[0], "usb0") == 0) {
            dev = DEV_USB;
        } else if (strcmp(argv[0], "ram0") == 0) {
            dev = DEV_RAM;
        } else
        #endif
        {
            dir = argv[0];

        #ifdef AOS_COMP_NFS
            if (is_valid_nfs(dir)) {
                return nfs_unregister(dir);
            }
        #endif

        #ifdef AOS_COMP_FATFS
            {
                if ((dev = fatfs_root2dev(dir)) < 0) {
                    aos_cli_printf("Invalid dev or path %s. Please choose dev "
                               "from 'sdmmc0', 'usb0', 'ram0'.\r\n", dir);
                    return -1;
                }
            }
        #endif
        }

    #ifdef AOS_COMP_FATFS
        return fatfs_dev_unregister(dev);
    #endif
    } else {
        aos_cli_printf("Invalid argument!\r\n");
        print_umount_help();
    }

    return 0;
}
ALIOS_CLI_CMD_REGISTER(umount_main, umount, umount pairtion);
