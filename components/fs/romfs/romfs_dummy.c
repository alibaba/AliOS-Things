#include "romfs_def.h"

const static unsigned char _dummy_dummy_txt[] =
{
    0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x66, 0x69, 0x6c, 0x65, 0x21, 0x0d, 0x0a,
};

const static struct romfs_dirent _dummy[] =
{
    {ROMFS_DIRENT_FILE, "dummy.txt", _dummy_dummy_txt, sizeof(_dummy_dummy_txt)},
};

const static unsigned char _dummy_txt[] =
{
    0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x66, 0x69, 0x6c, 0x65, 0x21, 0x0d, 0x0a,
};

const static struct romfs_dirent _root_dirent[] =
{
    {ROMFS_DIRENT_DIR, "dummy", (uint8_t *)_dummy, sizeof(_dummy) / sizeof(_dummy[0])},
    {ROMFS_DIRENT_FILE, "dummy.txt", _dummy_txt, sizeof(_dummy_txt)},
    {ROMFS_DIRENT_MOUNTPOINT, "tmp", NULL, 0},
    {ROMFS_DIRENT_MOUNTPOINT, "mnt", NULL, 0},
    {ROMFS_DIRENT_MOUNTPOINT, "data", NULL, 0},
    {ROMFS_DIRENT_MOUNTPOINT, "system", NULL, 0},
};

const struct romfs_dirent romfs_root =
{
    ROMFS_DIRENT_DIR, "/", (uint8_t *)_root_dirent, sizeof(_root_dirent) / sizeof(_root_dirent[0])
};
