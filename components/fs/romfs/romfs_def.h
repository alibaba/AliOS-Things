#ifndef __ROMFS_DEF_H__
#define __ROMFS_DEF_H__

#include <stdint.h>
#include <stddef.h>
#include <fs/romfs.h>

#define ROMFS_MOUNTPATH "/"
#define ROMFS_NAME_MAX 256

enum romfs_error {
    ROMFS_ERR_OK          = 0,    // No error
    ROMFS_ERR_NOENT       = -2,   // No directory entry
    ROMFS_ERR_IO          = -5,   // Error during device operation
    ROMFS_ERR_NOTDIR      = -20,  // Entry is not a dir
    ROMFS_ERR_ISDIR       = -21,  // Entry is a dir
    ROMFS_ERR_BADF        = -9,   // Bad file number
    ROMFS_ERR_INVAL       = -22,  // Invalid parameter
    ROMFS_ERR_NOMEM       = -12,  // No more memory available
};

typedef enum {
    ROMFS_DIRENT_MOUNTPOINT = 0x000,
    ROMFS_DIRENT_FILE = 0x001,
    ROMFS_DIRENT_DIR = 0x002,
    ROMFS_DIRENT_MAX
};

// File seek flags
enum lfs_whence_flags {
    ROMFS_SEEK_SET = 0,   // Seek relative to an absolute position
    ROMFS_SEEK_CUR = 1,   // Seek relative to the current file position
    ROMFS_SEEK_END = 2,   // Seek relative to the end of the file
};

struct romfs_dirent
{
    uint32_t         type;  /* dirent type */
    const char       *name; /* dirent name */
    const uint8_t    *data; /* file date ptr */
    size_t           size;  /* file size */
};

extern const struct romfs_dirent romfs_root;

#endif
