/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>

#include "aos/vfs.h"
#include "fatfs/aos_fatfs.h"

#include "fatfs_diskio.h"
#include "ff.h"

#if FF_USE_LFN == 0
#define MAX_NAME_LEN 12
#else
#define MAX_NAME_LEN FF_MAX_LFN
#endif

typedef struct _fsid_map_t
{
    int         index; /* Device index */
    const char *root;  /* The mount point of the physical device */
    const char *id;    /* The partition id of the device in fatfs */
} fsid_map_t;

typedef struct _fat_dir_t
{
    aos_dir_t    dir;
    FF_DIR       ffdir;
    FILINFO      filinfo;
    aos_dirent_t cur_dirent;
} fat_dir_t;

static fsid_map_t g_fsid[] = { { DEV_MMC, MMC_MOUNTPOINT, MMC_PARTITION_ID },
                               { DEV_USB, USB_MOUNTPOINT, USB_PARTITION_ID },
                               { DEV_RAM, RAM_MOUNTPOINT, RAM_PARTITION_ID },
                               { DEV_FLASH1, FLASH1_MOUNTPOINT, FLASH1_PARTITION_ID },
                               { DEV_FLASH2, FLASH2_MOUNTPOINT, FLASH2_PARTITION_ID }};

static FATFS *g_fatfs[FF_VOLUMES] = { 0 };

#if FF_USE_LFN == 3 /* Dynamic memory allocation */

/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/

void *ff_memalloc(/* Returns pointer to the allocated memory block (null on not
                     enough core) */
                  UINT msize /* Number of bytes to allocate */
)
{
    return (void *)aos_malloc(
      msize); /* Allocate a new memory block with POSIX API */
}


/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree(void *mblock /* Pointer to the memory block to free */
)
{
    aos_free(mblock); /* Free the memory block with POSIX API */
}

#endif


#if FF_FS_REENTRANT /* Mutal exclusion */

/*------------------------------------------------------------------------*/
/* Create a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object for the volume, such as semaphore and mutex.
/  When a 0 is returned, the f_mount() function fails with FR_INT_ERR.
*/

// const osMutexDef_t Mutex[FF_VOLUMES]; /* CMSIS-RTOS */


int ff_cre_syncobj(/* 1:Function succeeded, 0:Could not create the sync object
                    */
                   BYTE vol, /* Corresponding volume (logical drive number) */
                   FF_SYNC_t
                     *sobj /* Pointer to return the created sync object */
)
{
    int ret = aos_mutex_new(sobj);
    return (ret == FR_OK) ? 1 : 0;
}


/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/

int ff_del_syncobj(/* 1:Function succeeded, 0:Could not delete due to an error
                    */
                   FF_SYNC_t sobj /* Sync object tied to the logical drive to be
                                     deleted */
)
{
    aos_mutex_free(&sobj);
    return 1;
}


/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant(/* 1:Got a grant to access the volume, 0:Could not get a grant
                  */
                 FF_SYNC_t sobj /* Sync object to wait */
)
{
    int ret = aos_mutex_lock(&sobj, FF_FS_TIMEOUT);
    return (ret == FR_OK) ? 1 : 0;
}


/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
 */

void ff_rel_grant(FF_SYNC_t sobj /* Sync object to be signaled */
)
{
    aos_mutex_unlock(&sobj);
}

#endif

static int get_disk_index(int pdrv)
{
    int index;
    for (index = 0; index < FF_VOLUMES; index++) {
        if (g_fsid[index].index == pdrv) {
            return index;
        }
    }

    return -1;
}

static char *translate_relative_path(const char *path)
{
    int   len, prefix_len;
    char *relpath, *p;
    BYTE  pdrv;

    if (!path) {
        return NULL;
    }

    len = strlen(path);
    for (pdrv = 0; pdrv < FF_VOLUMES; pdrv++) {
        prefix_len = strlen(g_fsid[pdrv].root);
        if (strncmp(g_fsid[pdrv].root, path, prefix_len) == 0) {
            break;
        }
    }

    if (pdrv == FF_VOLUMES) {
        return NULL;
    }

    relpath = (char *)aos_malloc(len + 1);
    if (!relpath) {
        return NULL;
    }

    memset(relpath, 0, len + 1);
    memcpy(relpath, g_fsid[pdrv].id, strlen(g_fsid[pdrv].id));

    if (len > prefix_len) {
        p = (char *)(path + strlen(g_fsid[pdrv].root) + 1);
        memcpy(relpath + strlen(g_fsid[pdrv].id), p, len - prefix_len - 1);
    }
    relpath[len] = '\0';

    return relpath;
}

static int fatfs_mode_conv(int m)
{
    int res      = 0;
    int acc_mode = m & O_ACCMODE;
    if (acc_mode == O_RDONLY) {
        res |= FA_READ;
    } else if (acc_mode == O_WRONLY) {
        res |= FA_WRITE;
    } else if (acc_mode == O_RDWR) {
        res |= FA_READ | FA_WRITE;
    }

    if (m & O_CREAT) {
        res |= FA_OPEN_ALWAYS;
    }

    if (m & O_TRUNC) {
        res |= FA_CREATE_ALWAYS;
    }

    if (m & O_EXCL) {
        res |= FA_CREATE_NEW;
    }

    if (m & O_APPEND) {
        res |= FA_OPEN_APPEND;
    }

    if (res == 0) {
        res |= FA_OPEN_EXISTING;
    }
    return res;
}


static int fatfs_open(file_t *fp, const char *path, int flags)
{
    int   ret     = -EPERM;
    FIL  *f       = NULL;
    char *relpath = NULL;

    f = (FIL *)aos_malloc(sizeof(FIL));
    if (!f) {
        return -ENOMEM;
    }

    relpath = translate_relative_path(path);
    if (!relpath) {
        aos_free(f);
        return -EINVAL;
    }

    ret = f_open(f, relpath, fatfs_mode_conv(flags));
    if (ret == FR_OK) {
        fp->f_arg = (void *)f;
        aos_free(relpath);
        return ret;
    }

    aos_free(relpath);
    aos_free(f);
    return ret;
}

static int fatfs_close(file_t *fp)
{
    int  ret = -EPERM;
    FIL *f   = (FIL *)(fp->f_arg);

    if (f) {
        ret = f_close(f);
        if (ret == FR_OK) {
            aos_free(f);
            fp->f_arg = NULL;
        }
    }

    return ret;
}

static ssize_t fatfs_read(file_t *fp, char *buf, size_t len)
{
    ssize_t nbytes;
    int     ret = -EPERM;
    FIL    *f   = (FIL *)(fp->f_arg);

    if (f) {
        if ((ret = f_read(f, (void *)buf, (UINT)len, (UINT *)&nbytes)) ==
            FR_OK) {
            return nbytes;
        }
    }

    return ret;
}

static ssize_t fatfs_write(file_t *fp, const char *buf, size_t len)
{
    ssize_t nbytes;
    int     ret = -EPERM;
    FIL    *f   = (FIL *)(fp->f_arg);

    if (f) {
        if ((ret = f_write(f, (void *)buf, (UINT)len, (UINT *)&nbytes)) ==
            FR_OK) {
            return nbytes;
        }
    }

    return ret;
}

static int fatfs_access(file_t *fp, const char *path, int amode)
{
    char   *relpath = NULL;
    FILINFO info;
    int     len, ret;

#if FF_FS_READONLY
    if (amode == W_OK) {
        return -EACCES;
    }
#endif

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }

    len = strlen(relpath);
    if (relpath[len - 1] == ':' && relpath[len] == '\0') {
        ret = FR_OK;
    } else {
        ret = f_stat(relpath, &info);
    }

    aos_free(relpath);
    return ret;
}

static off_t fatfs_lseek(file_t *fp, off_t off, int whence)
{
    off_t new_pos = 0;
    int   ret     = -EPERM;
    FIL  *f       = (FIL *)(fp->f_arg);

    if (f) {
        if (whence == SEEK_SET) {
            new_pos = off;
        } else if (whence == SEEK_CUR) {
            off_t cur_pos = f_tell(f);
            new_pos       = cur_pos + off;
        } else if (whence == SEEK_END) {
            off_t size = f_size(f);
            new_pos    = size + off;
        } else {
            return -EINVAL;
        }

        if ((ret = f_lseek(f, new_pos)) != FR_OK) {
            return ret;
        }
    }

    return new_pos;
}

static int fatfs_sync(file_t *fp)
{
    int  ret = -EPERM;
    FIL *f   = (FIL *)(fp->f_arg);

    if (f) {
        ret = f_sync(f);
    }

    return ret;
}

static int fatfs_stat(file_t *fp, const char *path, struct aos_stat *st)
{
    char   *relpath = NULL;
    FILINFO info;
    int     len, ret;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }

    len = strlen(relpath);
    if (relpath[len - 1] == ':' && relpath[len] == '\0') {
        st->st_size = 0;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO | S_IFDIR;
        ret         = 0;
    } else {

        if ((ret = f_stat(relpath, &info)) == FR_OK) {
            st->st_size = info.fsize;
            st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                          ((info.fattrib & AM_DIR) ? S_IFDIR : S_IFREG);
        }
    }

    aos_free(relpath);
    return ret;
}

static int fatfs_unlink(file_t *fp, const char *path)
{
    char *relpath = NULL;
    int   ret;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }

    ret = f_unlink(relpath);

    aos_free(relpath);
    return ret;
}

static int fatfs_rename(file_t *fp, const char *oldpath, const char *newpath)
{
    int   ret;
    char *oldname = NULL;
    char *newname = NULL;

    oldname = translate_relative_path(oldpath);
    if (!oldname) {
        return -EINVAL;
    }

    newname = translate_relative_path(newpath);
    if (!newname) {
        aos_free(oldname);
        return -EINVAL;
    }

    ret = f_rename(oldname, newname);

    aos_free(oldname);
    aos_free(newname);
    return ret;
}

static aos_dir_t *fatfs_opendir(file_t *fp, const char *path)
{
    fat_dir_t *dp      = NULL;
    char      *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return NULL;
    }

    dp = (fat_dir_t *)aos_malloc(sizeof(fat_dir_t) + MAX_NAME_LEN + 1);
    if (!dp) {
        aos_free(relpath);
        return NULL;
    }

    memset(dp, 0, sizeof(fat_dir_t) + MAX_NAME_LEN + 1);
    if (f_opendir(&dp->ffdir, relpath) == FR_OK) {
        aos_free(relpath);
        return (aos_dir_t *)dp;
    }

    aos_free(relpath);
    aos_free(dp);
    return NULL;
}

static aos_dirent_t *fatfs_readdir(file_t *fp, aos_dir_t *dir)
{
    fat_dir_t    *dp = (fat_dir_t *)dir;
    aos_dirent_t *out_dirent;

    if (!dp) {
        return NULL;
    }

    if (f_readdir(&dp->ffdir, &dp->filinfo) != FR_OK) {
        return NULL;
    }

    if (dp->filinfo.fname[0] == 0) {
        return NULL;
    }

    dp->cur_dirent.d_ino = 0;
    if (dp->filinfo.fattrib & AM_DIR) {
        dp->cur_dirent.d_type = AM_DIR;
    }

    strncpy(dp->cur_dirent.d_name, dp->filinfo.fname, MAX_NAME_LEN);
    dp->cur_dirent.d_name[MAX_NAME_LEN] = '\0';

    out_dirent = &dp->cur_dirent;
    return out_dirent;
}

static int fatfs_closedir(file_t *fp, aos_dir_t *dir)
{
    int        ret = -EPERM;
    fat_dir_t *dp  = (fat_dir_t *)dir;

    if (!dp) {
        return -EINVAL;
    }

    ret = f_closedir(&dp->ffdir);
    if (ret == FR_OK) {
        aos_free(dp);
    }

    return ret;
}

static int fatfs_mkdir(file_t *fp, const char *path)
{
    int   ret     = -EPERM;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }

    ret = f_mkdir(relpath);

    aos_free(relpath);
    return ret;
}

static int fatfs_rmdir(file_t *fp, const char *path)
{
    int   ret     = -EPERM;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }

    ret = f_rmdir(relpath);

    aos_free(relpath);

    return ret;
}

static void fatfs_rewinddir(file_t *fp, aos_dir_t *dir)
{
    fat_dir_t *dp = (fat_dir_t *)dir;

    if (!dp) {
        return;
    }

    f_rewinddir(&dp->ffdir);

    return;
}

static long fatfs_telldir(file_t *fp, aos_dir_t *dir)
{
    fat_dir_t *dp = (fat_dir_t *)dir;

    if (!dp) {
        return -1;
    }

    return (long)(dp->ffdir.dptr);
}

static void fatfs_seekdir(file_t *fp, aos_dir_t *dir, long loc)
{
    fat_dir_t *dp = (fat_dir_t *)dir;

    if (!dp) {
        return;
    }

    dp->ffdir.dptr = loc;

    return;
}

static const fs_ops_t fatfs_ops = { .open      = &fatfs_open,
                                    .close     = &fatfs_close,
                                    .read      = &fatfs_read,
                                    .write     = &fatfs_write,
                                    .access    = &fatfs_access,
                                    .lseek     = &fatfs_lseek,
                                    .sync      = &fatfs_sync,
                                    .stat      = &fatfs_stat,
                                    .unlink    = &fatfs_unlink,
                                    .rename    = &fatfs_rename,
                                    .opendir   = &fatfs_opendir,
                                    .readdir   = &fatfs_readdir,
                                    .closedir  = &fatfs_closedir,
                                    .mkdir     = &fatfs_mkdir,
                                    .rmdir     = &fatfs_rmdir,
                                    .rewinddir = &fatfs_rewinddir,
                                    .telldir   = &fatfs_telldir,
                                    .seekdir   = &fatfs_seekdir,
                                    .ioctl     = NULL };

static int fatfs_dev_register(int pdrv)
{
    int    err, index;
    FATFS *fatfs = NULL;

    index = get_disk_index(pdrv);
    if (index < 0) {
        return -EINVAL;
    }

    if (g_fatfs[index] != NULL) {
        return FR_OK;
    }

    fatfs = (FATFS *)aos_malloc(sizeof(FATFS));
    if (!fatfs) {
        return -ENOMEM;
    }

    err = f_mount(fatfs, g_fsid[index].id, 1);

    if (err == FR_OK) {
        g_fatfs[index] = fatfs;
        return aos_register_fs(g_fsid[index].root, &fatfs_ops, NULL);
    }

#if FF_USE_MKFS && !FF_FS_READONLY
    if (err == FR_NO_FILESYSTEM) {
        char *work = (char *)aos_malloc(FF_MAX_SS);
        if (!work) {
            err = -ENOMEM;
            goto error;
        }

        BYTE opt = FM_ANY;
        disk_ioctl(g_fsid[index].index, GET_FORMAT_OPTION, &opt);

        err = f_mkfs(g_fsid[index].id, opt, 0, work, FF_MAX_SS);
        aos_free(work);

        if (err != FR_OK) {
            goto error;
        }

        f_mount(NULL, g_fsid[index].id, 1);
        err = f_mount(fatfs, g_fsid[index].id, 1);

        if (err == FR_OK) {
            g_fatfs[index] = fatfs;
            return aos_register_fs(g_fsid[index].root, &fatfs_ops, NULL);
        }
    }
error:
#endif
    aos_free(fatfs);
    return err;
}


static int fatfs_dev_unregister(int pdrv)
{
    int err = FR_OK;
    int index;

    index = get_disk_index(pdrv);
    if (index < 0) {
        return -EINVAL;
    }

    err = aos_unregister_fs(g_fsid[index].root);
    if (err == FR_OK) {
        f_mount(NULL, g_fsid[index].id, 1);
        aos_free(g_fatfs[index]);
        g_fatfs[index] = NULL;
    }

    return err;
}

int fatfs_register(void)
{
    int err = -EINVAL;

#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
    if ((err = fatfs_dev_register(DEV_MMC)) != FR_OK) {
        return err;
    }
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_USB
    if ((err = fatfs_dev_register(DEV_USB)) != FR_OK) {
        return err;
    }
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAM
    if ((err = fatfs_dev_register(DEV_RAM)) != FR_OK) {
        return err;
    }
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAW_FLASH

	if ((err = fatfs_dev_register(DEV_FLASH1)) != FR_OK) {
		return err;
	}
	

	if ((err = fatfs_dev_register(DEV_FLASH2)) != FR_OK) {
		return err;
	}
#endif



    return err;
}

int fatfs_unregister(void)
{
    int err = -EINVAL;

#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
    if ((err = fatfs_dev_unregister(DEV_MMC)) != FR_OK) {
        return err;
    }
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_USB
    if ((err = fatfs_dev_unregister(DEV_USB)) != FR_OK) {
        return err;
    }
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAM
    if ((err = fatfs_dev_unregister(DEV_RAM)) != FR_OK) {
        return err;
    }
#endif

#ifdef CONFIG_AOS_FATFS_SUPPORT_RAW_FLASH
	if ((err = fatfs_dev_unregister(DEV_FLASH1)) != FR_OK) {
		return err;
	}

	if ((err = fatfs_dev_unregister(DEV_FLASH2)) != FR_OK) {
		return err;
	}
#endif


    return err;
}
