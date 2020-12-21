#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vfs_types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "romfs_def.h"
#include <fs/romfs.h>

#define O_DIRECTORY 0200000

static struct romfs_dirent *g_romfs_data = &romfs_root;

static inline int check_dirent(struct romfs_dirent *dirent)
{
    if ((dirent->type != ROMFS_DIRENT_FILE &&
         dirent->type != ROMFS_DIRENT_DIR &&
         dirent->type != ROMFS_DIRENT_MOUNTPOINT)
        || dirent->size == ~0)
        return -1;
    return 0;
}

static char* exclude_slash(char *path)
{
    char *p = path, *q = path + 1;
    int len;

    if (path) {
        len = strlen(path);

        while (q < (path + len)) {
            if ((*p == '/') && (*q == '/')) {
                q++; continue;
            } else {
                *(++p) = *q++;
            }
        }

        *(++p) = '\0';
    }

    return path;
}

static struct romfs_dirent *dfs_romfs_lookup(struct romfs_dirent *root_dirent, const char *path, size_t *size)
{
    size_t index, found;
    const char *subpath, *subpath_end;
    struct romfs_dirent *dirent;
    size_t dirent_size;

    /* Check the root_dirent. */
    if (check_dirent(root_dirent) != 0)
        return NULL;

    char *new_path = (char *)malloc(strlen(path) + 1);
    if (!new_path) {
        return NULL;
    }

    memset(new_path, 0, strlen(path) + 1);
    strncpy(new_path, path, strlen(path));
    exclude_slash(new_path);

    if (new_path[0] == '/' && new_path[1] == '\0')
    {
        *size = root_dirent->size;
        free(new_path);
        return root_dirent;
    }

    /* goto root directy entries */
    dirent = (struct romfs_dirent *)root_dirent->data;
    dirent_size = root_dirent->size;

    /* get the end position of this subpath */
    subpath_end = new_path;
    /* skip /// */
    while (*subpath_end && *subpath_end == '/')
        subpath_end ++;
    subpath = subpath_end;
    while ((*subpath_end != '/') && *subpath_end)
        subpath_end ++;

    while (dirent != NULL)
    {
        found = 0;

        /* search in folder */
        for (index = 0; index < dirent_size; index ++)
        {
            if (check_dirent(&dirent[index]) != 0) {
                free(new_path);
                return NULL;
            }
            if (strlen(dirent[index].name) == (subpath_end - subpath) &&
                    strncmp(dirent[index].name, subpath, (subpath_end - subpath)) == 0)
            {
                dirent_size = dirent[index].size;

                /* skip /// */
                while (*subpath_end && *subpath_end == '/')
                    subpath_end ++;
                subpath = subpath_end;
                while ((*subpath_end != '/') && *subpath_end)
                    subpath_end ++;

                if (!(*subpath))
                {
                    free(new_path);
                    *size = dirent_size;
                    return &dirent[index];
                }

                if (dirent[index].type == ROMFS_DIRENT_DIR)
                {
                    /* enter directory */
                    dirent = (struct romfs_dirent *)dirent[index].data;
                    found = 1;
                    break;
                }
                else
                {
                    /* return file dirent */
                    if (subpath != NULL)
                        break; /* not the end of path */

                    free(new_path);
                    return &dirent[index];
                }
            }
        }

        if (!found)
            break; /* not found */
    }

    free(new_path);
    /* not found */
    return NULL;
}

typedef struct {
    char *path;
    size_t size;
    off_t pos;
    int flags;
    struct romfs_dirent *dirent;
} romfs_file_t;

static int32_t romfs_vfs_open(vfs_file_t *fp, const char *path, int flags)
{
    size_t size;
    romfs_file_t *file;
    struct romfs_dirent *dirent;
    struct romfs_dirent *root_dirent;

    file = (romfs_file_t *)malloc(sizeof(romfs_file_t));
    if (!file) {
        printf("%s malloc failed\r\n", __func__);
        return ROMFS_ERR_NOMEM;
    }

    file->path = path;
    file->flags = flags;

    root_dirent = g_romfs_data;

    if (check_dirent(root_dirent) != 0)
        return ROMFS_ERR_IO;

    if (file->flags & (O_CREAT | O_WRONLY | O_APPEND | O_TRUNC | O_RDWR))
        return ROMFS_ERR_INVAL;

    dirent = dfs_romfs_lookup(root_dirent, file->path, &size);
    if (dirent == NULL)
        return ROMFS_ERR_NOENT;

    /* entry is a directory file type */
    if (dirent->type == ROMFS_DIRENT_MOUNTPOINT)
    {
        return ROMFS_ERR_NOENT;
    }
    else if (dirent->type == ROMFS_DIRENT_DIR)
    {
        if (!(file->flags & O_DIRECTORY))
            return ROMFS_ERR_NOENT;
    }
    else
    {
        /* entry is a file, but open it as a directory */
        if (file->flags & O_DIRECTORY)
            return ROMFS_ERR_NOENT;
    }

    file->dirent = dirent;
    file->size = size;
    file->pos = 0;

    fp->f_arg = (void *)file;

    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_close(vfs_file_t *fp)
{
    romfs_file_t *file = fp->f_arg;

    file->path = NULL;
    file->size = 0;
    file->pos = 0;
    file->dirent = NULL;
    fp->f_arg = NULL;
    free(file);

    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    size_t siz;
    struct romfs_dirent *dirent;
    romfs_file_t *file = (romfs_file_t *)fp->f_arg;

    dirent = file->dirent;
    assert(dirent != NULL);

    if (check_dirent(dirent) != 0) return ROMFS_ERR_IO;

    if (len < (file->size - file->pos))
        siz = len;
    else
        siz = file->size - file->pos;

    if (siz > 0)
        memcpy(buf, &(dirent->data[file->pos]), siz);

    /* update file current position */
    file->pos += siz;

    return siz;
}

static int32_t romfs_vfs_write(vfs_file_t *fp, const char *buf, uint32_t len)
{
    printf("%s not supported!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static uint32_t romfs_vfs_lseek(vfs_file_t *fp, int64_t off, int32_t whence)
{
    romfs_file_t *file = (romfs_file_t *)fp->f_arg;

    off_t npos = file->pos;
    if (whence == ROMFS_SEEK_SET) {
        npos = off;
    } else if (whence == ROMFS_SEEK_CUR) {
        npos = file->pos + off;
    } else if (whence == ROMFS_SEEK_END) {
        npos = file->size;
    }

    if (npos <= file->size)
    {
        file->pos = npos;
        return file->pos;
    }

    return ROMFS_ERR_IO;
}

static int32_t romfs_vfs_sync(vfs_file_t *fp)
{
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_fstat(vfs_file_t *fp, vfs_stat_t *st)
{
    struct romfs_dirent *dirent;
    romfs_file_t *file = (romfs_file_t *)fp->f_arg;

    dirent = file->dirent;

    if (dirent->type != ROMFS_DIRENT_MOUNTPOINT) {
        assert(dirent != NULL);

        if (check_dirent(dirent) != 0) return ROMFS_ERR_IO;

        st->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
                      S_IWUSR | S_IWGRP | S_IWOTH;

        if (dirent->type == ROMFS_DIRENT_DIR)
        {
            st->st_mode &= ~S_IFREG;
            st->st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
        }

        st->st_size = dirent->size;
    } else {
        st->st_mode = S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
        st->st_size = 0;
    }

    st->st_modtime = 0;
    st->st_actime = 0;

    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
{
    size_t size;
    struct romfs_dirent *dirent;
    struct romfs_dirent *root_dirent;

    root_dirent = g_romfs_data;
    dirent = dfs_romfs_lookup(root_dirent, path, &size);

    if (dirent == NULL)
        return ROMFS_ERR_NOENT;

    st->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH;

    if (dirent->type == ROMFS_DIRENT_DIR)
    {
        st->st_mode &= ~S_IFREG;
        st->st_mode |= S_IFDIR;
    }

    st->st_size = dirent->size;
    st->st_modtime = 0;
    st->st_actime = 0;

    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_access(vfs_file_t *fp, const char *path, int mode)
{
    size_t size;
    vfs_stat_t s;
    struct romfs_dirent *dirent;
    struct romfs_dirent *root_dirent;

    root_dirent = g_romfs_data;
    dirent = dfs_romfs_lookup(root_dirent, path, &size);

    if (dirent == NULL)
        return ROMFS_ERR_NOENT;

    s.st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH;

    if (dirent->type == ROMFS_DIRENT_DIR)
    {
        s.st_mode &= ~S_IFREG;
        s.st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    }

    switch(mode) {
        case F_OK: return 0;
        case R_OK: return s.st_mode & S_IRUSR ? 0 : 1;
        case W_OK: return s.st_mode & S_IWUSR ? 0 : 1;
        case X_OK: return s.st_mode & S_IXUSR ? 0 : 1;
    }

    return 0;
}

static int32_t romfs_vfs_statfs(vfs_file_t *fp, const char *path, vfs_statfs_t *sfs)
{
    /* TODO: traverse romfs to get size the file number, etc. */
    memset(sfs, 0, sizeof(vfs_statfs_t));
    sfs->f_type = 0xd3fc;
    sfs->f_bsize = 1;
    sfs->f_blocks = 1;
    sfs->f_bfree = 0;
    sfs->f_bavail = sfs->f_bfree;
    sfs->f_namelen = 16;

    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_remove(vfs_file_t *fp, const char *path)
{
    printf("%s not supported!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_rename(vfs_file_t *fp, const char *oldpath, const char *newpath)
{
    printf("%s not supported!\r\n", __func__);
    return ROMFS_ERR_OK;
}

typedef struct {
    vfs_dir_t dir;
    struct romfs_dirent *rent;
    size_t siz;
    char *path;
    off_t pos;
    vfs_dirent_t vent;
} romfs_dir_t;

static vfs_dir_t *romfs_vfs_opendir(vfs_file_t *fp, const char *path)
{
    size_t size;
    struct romfs_dirent *dirent;
    struct romfs_dirent *root_dirent;
    romfs_dir_t *rdir;

    root_dirent = g_romfs_data;
    dirent = dfs_romfs_lookup(root_dirent, path, &size);

    if (dirent == NULL) {
#ifdef CONFIG_VFS_USE_ERRNO
        errno = ROMFS_ERR_NOENT;
#endif
        return NULL;
    }

    if (dirent->type != ROMFS_DIRENT_DIR) {
#ifdef CONFIG_VFS_USE_ERRNO
        errno = ROMFS_ERR_NOTDIR;
#endif
        return NULL;
    }

    rdir = (romfs_dir_t *)malloc(sizeof(romfs_dir_t) + ROMFS_NAME_MAX + 1);
    if (!rdir) {
#ifdef CONFIG_VFS_USE_ERRNO
        errno = ROMFS_ERR_NOMEM;
#endif
        return NULL;
    }

    memset(rdir, 0, sizeof(romfs_dir_t) + ROMFS_NAME_MAX + 1);

    rdir->rent = dirent->data;
    rdir->siz = dirent->size;
    rdir->path = path;
    rdir->pos = 0;
    fp->f_arg = (void *)rdir;

    return (vfs_dir_t *)rdir;
}

#if 0
typedef struct {
    int32_t d_ino;    /* file number */
    uint8_t d_type;   /* type of file */
    char    d_name[]; /* file name */
} vfs_dirent_t;

struct romfs_dirent
{
    uint32_t         type;  /* dirent type */

    const char       *name; /* dirent name */
    const uint8_t    *data; /* file date ptr */
    size_t           size;  /* file size */
};
#endif

static vfs_dirent_t *romfs_vfs_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    vfs_dirent_t *vent;
    struct romfs_dirent *rent, *ent;
    romfs_dir_t *rdir = (romfs_dir_t *)dir;

    rent = rdir->rent;

    /* maybe empy dir */
    if (!rent) return NULL;

    if (rdir->pos < rdir->siz) {
        ent = &(rent[rdir->pos]);
        vent = &(rdir->vent);
        memset(vent, 0, sizeof(vfs_dirent_t));
        vent->d_ino = 0;
        vent->d_type = (uint8_t)ent->type;
        strncpy(vent->d_name, ent->name, ROMFS_NAME_MAX);
        rdir->pos++;
    } else {
        return NULL;
    }

    return vent;
}

static int32_t romfs_vfs_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    free(dir);
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_mkdir(vfs_file_t *fp, const char *path)
{
    printf("%s not supported!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_rmdir (vfs_file_t *fp, const char *path)
{
    printf("%s not supported!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static void romfs_vfs_rewinddir(vfs_file_t *fp, vfs_dir_t *dir)
{
    printf("%s not implemented!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_telldir(vfs_file_t *fp, vfs_dir_t *dir)
{
    printf("%s not implemented!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static void romfs_vfs_seekdir(vfs_file_t *fp, vfs_dir_t *dir, int32_t loc)
{
    printf("%s not implemented!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_utime(vfs_file_t *fp, const char *path, const vfs_utimbuf_t *times)
{
    printf("%s not implemented!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg)
{
    printf("%s not implemented!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_pathconf(vfs_file_t *fp, const char *path, int name)
{
    printf("%s not implemented!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static int32_t romfs_vfs_fpathconf(vfs_file_t *fp, int name)
{
    printf("%s not implemented!\r\n", __func__);
    return ROMFS_ERR_OK;
}

static vfs_filesystem_ops_t romfs_ops = {
    .open       = &romfs_vfs_open,
    .close      = &romfs_vfs_close,
    .read       = &romfs_vfs_read,
    .write      = &romfs_vfs_write,
    .lseek      = &romfs_vfs_lseek,
    .sync       = &romfs_vfs_sync,
    .stat       = &romfs_vfs_stat,
    .unlink     = &romfs_vfs_remove,
    .remove     = &romfs_vfs_remove,
    .rename     = &romfs_vfs_rename,
    .access     = &romfs_vfs_access,
    .fstat      = &romfs_vfs_fstat,
    .statfs     = &romfs_vfs_statfs,
    .opendir    = &romfs_vfs_opendir,
    .readdir    = &romfs_vfs_readdir,
    .closedir   = &romfs_vfs_closedir,
    .mkdir      = &romfs_vfs_mkdir,
    .rmdir      = &romfs_vfs_rmdir,
    .rewinddir  = &romfs_vfs_rewinddir,
    .telldir    = &romfs_vfs_telldir,
    .seekdir    = &romfs_vfs_seekdir,
    .utime      = &romfs_vfs_utime,
    .ioctl      = &romfs_vfs_ioctl,
    .pathconf   = &romfs_vfs_pathconf,
    .fpathconf  = &romfs_vfs_fpathconf,
};

int32_t romfs_register(void)
{
    int32_t ret;
    ret = vfs_register_fs(ROMFS_MOUNTPATH, &romfs_ops, NULL);
    return ret;
}

int32_t romfs_unregister(void)
{
    int32_t ret;
    ret = vfs_unregister_fs(ROMFS_MOUNTPATH);
    return ret;
}
