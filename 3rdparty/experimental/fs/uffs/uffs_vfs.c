/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "uffs/uffs_fd.h"
#include "uffs/uffs_mtb.h"
#include "uffs/uffs_flash.h"

#include "uffs_port.h"

#include "aos/hal/nand.h"

#define UFFS_NAME_LEN   256

static const char *uffs_mnt_path = "/uffs";

typedef struct {
    nand_dev_t *dev;
    struct uffs_StorageAttrSt *storage;
    uffs_Device *udev;
    uffs_MountTable *mtb;
} uffs_mgr_t;

static uffs_mgr_t *g_uffs_mgr = NULL;

typedef struct {
    vfs_dir_t dir;
    uffs_DIR *d;
    vfs_dirent_t cur_dirent;
} uffs_dir_t;

static int32_t nand_erase_block(uffs_Device *dev, uint32_t block)
{
    nand_addr_t addr;

    addr.zone = block / (((nand_dev_t *)(dev->_private))->config.zone_size);
    addr.block = block - (addr.zone) * ((((nand_dev_t *)(dev->_private))->config.zone_size));
    hal_nand_erase_block(dev->_private, &addr);
    return 0;
}

static int32_t nand_read_page(uffs_Device *dev, uint32_t block, uint32_t page, uint8_t *data, 
                              int32_t data_len, uint8_t *ecc, uint8_t *spare, int32_t spare_len)
{
    nand_addr_t addr;
    int32_t page_count, page_size, ret;
    uint8_t *spare_buffer, *data_buffer = NULL;

    addr.zone = block / (((nand_dev_t *)(dev->_private))->config.zone_size);
    addr.block = block - (addr.zone) * ((((nand_dev_t *)(dev->_private))->config.zone_size));
    addr.page = page;

    page_size = ((nand_dev_t *)(dev->_private))->config.page_size;
    page_count = data_len / page_size + ((data_len % page_size) > 0 ? 1 : 0);

    if (data == NULL && spare == NULL) {
        spare_buffer = (uint8_t *)krhino_mm_alloc(dev->attr->spare_size);
        if (!spare_buffer) {
            return -EIO;
        }

        memset(spare_buffer, 0, dev->attr->spare_size);
        hal_nand_read_spare(dev->_private, &addr, spare_buffer, dev->attr->spare_size);
        
        ret = *(spare_buffer + dev->attr->block_status_offs) == 0xFF ?
            UFFS_FLASH_NO_ERR : UFFS_FLASH_BAD_BLK;

        krhino_mm_free(spare_buffer);
        return ret;

    }

    if (data && data_len > 0) {
        data_buffer = (uint8_t *)krhino_mm_alloc(page_size);
        if (!data_buffer)
            return -EIO;
        memset(data_buffer, 0, page_size);
        hal_nand_read_page(dev->_private, &addr, data_buffer, page_count);
        memcpy(data, data_buffer, data_len);
        krhino_mm_free(data_buffer);
    }

    if (spare && spare_len > 0)
        hal_nand_read_spare(dev->_private, &addr, spare, spare_len);
    return 0;
}

static int32_t nand_write_page(uffs_Device *dev, uint32_t block, uint32_t page,
                               const uint8_t *data, int32_t data_len, const uint8_t *spare, uint32_t spare_len)
{
    nand_addr_t addr;
    uint32_t page_count, page_size;
    uint8_t *spare_buffer, *data_buffer = NULL;

    addr.zone = block / (((nand_dev_t *)(dev->_private))->config.zone_size);
    addr.block = block - (addr.zone) * ((((nand_dev_t *)(dev->_private))->config.zone_size));
    addr.page = page;

    page_size = ((nand_dev_t *)(dev->_private))->config.page_size;
    page_count = data_len / page_size + ((data_len % page_size) > 0 ? 1 : 0);

    if (data == NULL && spare == NULL) {
        spare_buffer = (uint8_t *)krhino_mm_alloc(UFFS_MAX_SPARE_SIZE);
        if (!spare_buffer)
            return -EIO;
        memset(spare_buffer, 0, UFFS_MAX_SPARE_SIZE);
        *(spare_buffer + dev->attr->block_status_offs) = 0x00;
        hal_nand_write_spare(dev->_private, &addr, spare_buffer, dev->attr->spare_size);
        krhino_mm_free(spare_buffer);
    }

    if (data && data_len > 0) {
        data_buffer = (uint8_t *)krhino_mm_alloc(page_size);
        if (!data_buffer)
            return -EIO;
        memset(data_buffer, 0xff, page_size);
        memcpy(data_buffer, data, data_len);
        hal_nand_write_page(dev->_private, &addr, data_buffer, page_count);
        krhino_mm_free(data_buffer);
    }

    if (spare && spare_len > 0) {
        hal_nand_write_spare(dev->_private, &addr, (uint8_t *)spare, spare_len);
    }
    return 0;
}

static const uffs_FlashOps uffs_nand_ops = 
{
    .ReadPage = &nand_read_page,
    .WritePage = &nand_write_page,
    .EraseBlock = &nand_erase_block 
};

static char* translate_relative_path(const char *path)
{
    int32_t len, prefix_len;
    char *relpath, *p;

    if (!path) 
        return NULL;

    len = strlen(path);
    prefix_len = strlen(uffs_mnt_path);
    if (strncmp(uffs_mnt_path, path, prefix_len) != 0)
        return NULL;

    len = len - prefix_len;
    relpath = (char *)krhino_mm_alloc(len + 1);
    if (!relpath)
        return NULL;

    memset(relpath, 0, len + 1);

    if (len > 0) {
        p = (char *)(path + prefix_len);
        memcpy(relpath, p, len);
    }

    relpath[len] = '\0';

    return relpath;
}

static int32_t _uffs_ret_to_err(int32_t ret)
{
    switch (ret) {
        case UENOERR:
            return 0;
        case UEACCES:
            return -EACCES;
        case UEEXIST:
            return -EEXIST;
        case UEINVAL:
            return -EINVAL;
        case UEMFILE:
            return -ENFILE;
        case UENOENT:
            return -ENOENT;
        case UEBADF:
            return -EBADF;
        case UENOMEM:
            return -ENOMEM;
        case UEIOERR:
        case UETIME:
            return -EIO;
        case UENOTDIR:
            return -ENOTDIR;
        case UEISDIR:
            return -EISDIR;
        case UEUNKNOWN_ERR:
        default:
            return -EIO;
    }
}

static int32_t _uffs_mode_conv(int32_t flags)
{
    int32_t acc_mode, res = 0;

    acc_mode = flags & O_ACCMODE;
    if (acc_mode == O_RDONLY) {
        res |= UO_RDONLY;
    } else if (acc_mode == O_WRONLY) {
        res |= UO_WRONLY;
    } else if (acc_mode == O_RDWR) {
        res |= UO_RDWR;
    }

    if ((flags & O_CREAT) && (flags & O_EXCL)) {
        res |= UO_CREATE | UO_EXCL;
    } else if ((flags & O_CREAT) && (flags & O_TRUNC)) {
        res |= UO_CREATE | UO_TRUNC;
    } else if (flags & O_APPEND) {
        res |= UO_CREATE | UO_APPEND;
    }
    return res;
}

static int32_t uffs_vfs_open(vfs_file_t *fp, const char *path, int32_t flags)
{
    int32_t fd;

    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    fd = uffs_open(relpath, _uffs_mode_conv(flags));
    if (fd > 0) {
        fp->f_arg = (void *)fd;
        fd = 0;
    } else {
        fd = _uffs_ret_to_err(uffs_get_error());
    }

    krhino_mm_free(relpath);
    return fd;
}

static int32_t uffs_vfs_close(vfs_file_t *fp)
{
    int32_t fd, ret;

    fd = (int32_t)(fp->f_arg);
    ret = uffs_close(fd);
    if (ret < 0) {
        ret = _uffs_ret_to_err(uffs_get_error());
    } else {
        fp->f_arg = NULL;
    }

    return ret;
}

static int32_t uffs_vfs_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    int32_t nbytes;
    int32_t fd;

    fd = (int32_t)(fp->f_arg);
    nbytes = uffs_read(fd, buf, len);
    if (nbytes < 0) {
        nbytes = _uffs_ret_to_err(uffs_get_error());
    }

    return nbytes;
}

static int32_t uffs_vfs_write(vfs_file_t *fp, const char *buf, uint32_t len)
{
    int32_t nbytes;
    int32_t fd;

    fd = (int)(fp->f_arg);
    nbytes = uffs_write(fd, (void *)buf, len);
    if (nbytes < 0) {
        nbytes = _uffs_ret_to_err(uffs_get_error());
    }

    return nbytes;
}

static uint32_t uffs_vfs_lseek(vfs_file_t *fp, int64_t off, int32_t whence)
{
    int64_t ret;
    int32_t fd;

    fd = (int32_t)(fp->f_arg);
    ret = uffs_seek(fd, off, whence);
    if (ret < 0) {
        ret = _uffs_ret_to_err(uffs_get_error());
    }

    return ret;
}

static int32_t uffs_vfs_sync(vfs_file_t *fp)
{
    int32_t ret, fd;

    fd = (int32_t)(fp->f_arg);
    ret = uffs_flush(fd);
    if (ret < 0) {
        ret = _uffs_ret_to_err(uffs_get_error());
    } else {
        ret = 0;
    }

    return ret;
}

static int32_t uffs_vfs_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
{
    int32_t ret;

    struct uffs_stat s;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    ret = uffs_stat(relpath, &s);
    if (ret < 0) {
        ret = _uffs_ret_to_err(uffs_get_error());
    } else {
        st->st_size = s.st_size;
        st->st_mode = s.st_mode;
    }

    krhino_mm_free(relpath);
    return ret;
}

static int32_t uffs_vfs_unlink(vfs_file_t *fp, const char *path)
{
    int32_t ret;

    struct uffs_stat s;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    if (uffs_lstat(relpath, &s) < 0) {
        krhino_mm_free(relpath);
        return _uffs_ret_to_err(uffs_get_error());
    }

    switch(s.st_mode & US_IFMT) {
        case US_IFREG:
            ret = uffs_remove(relpath);
            break;
        case US_IFDIR:
            ret = uffs_rmdir(relpath);
            break;
        default:
            krhino_mm_free(relpath);
            return -EIO;
    }

    if (ret < 0) {
        ret = _uffs_ret_to_err(uffs_get_error());
    }

    krhino_mm_free(relpath);
    return ret;
}

static int32_t uffs_vfs_rename(vfs_file_t *fp, const char *oldpath, const char *newpath)
{
    int32_t ret;

    char *oldname = NULL;
    char *newname = NULL;

    oldname = translate_relative_path(oldpath);
    if (!oldname)
        return -EINVAL;

    newname = translate_relative_path(newpath);
    if (!newname) {
        krhino_mm_free(oldname);
        return -EINVAL;
    }

    ret = uffs_rename(oldname, newname);
    if (ret < 0) {
        ret = _uffs_ret_to_err(uffs_get_error());
    }

    krhino_mm_free(oldname);
    krhino_mm_free(newname);
    return ret;
}

static vfs_dir_t *uffs_vfs_opendir(vfs_file_t *fp, const char *path)
{
    uffs_dir_t *dp = NULL;
    char *relpath = NULL;
    uffs_DIR *dir = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return NULL;

    dp = (uffs_dir_t *)krhino_mm_alloc(sizeof(uffs_dir_t) + UFFS_NAME_LEN);
    if (!dp) {
        krhino_mm_free(relpath);
        return NULL;
    }

    memset(dp, 0, sizeof(uffs_dir_t) + UFFS_NAME_LEN);
    dir = uffs_opendir(relpath);
    if (!dir) {
        krhino_mm_free(relpath);
        krhino_mm_free(dp);
        return NULL;
    }

    dp->d = dir;
    krhino_mm_free(relpath);
    return (vfs_dir_t *)dp;
}

static vfs_dirent_t *uffs_vfs_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    uffs_dir_t *dp;
    struct uffs_dirent *e;
    vfs_dirent_t *out_dirent;

    dp = (uffs_dir_t *)dir;
    if (!dp)
        return NULL;

    e = uffs_readdir(dp->d);
    if (!e) {
        return NULL;
    }

    if (e->d_name[0] == 0)
        return NULL;

    dp->cur_dirent.d_ino = 0;
    dp->cur_dirent.d_type = e->d_type;

    strncpy(dp->cur_dirent.d_name, (char *)e->d_name, UFFS_NAME_LEN);
    dp->cur_dirent.d_name[UFFS_NAME_LEN] = '\0';

    out_dirent = &dp->cur_dirent;
    return out_dirent;
}

static int32_t uffs_vfs_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    int32_t ret;

    uffs_dir_t *dp = (uffs_dir_t *)dir;

    if (!dp)
        return -EINVAL;

    ret = uffs_closedir(dp->d);
    if (ret < 0) {
        ret = _uffs_ret_to_err(uffs_get_error());
    }

    krhino_mm_free(dp);
    return ret;
}

static int32_t uffs_vfs_mkdir(vfs_file_t *fp, const char *path)
{
    int32_t ret;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    ret = uffs_mkdir(relpath);
    if (ret < 0) {
        ret = _uffs_ret_to_err(uffs_get_error());
    }

    krhino_mm_free(relpath);
    return ret;
}

static vfs_filesystem_ops_t uffs_ops = {
    .open       = &uffs_vfs_open,
    .close      = &uffs_vfs_close,
    .read       = &uffs_vfs_read,
    .write      = &uffs_vfs_write,
    .lseek      = &uffs_vfs_lseek,
    .sync       = &uffs_vfs_sync,
    .stat       = &uffs_vfs_stat,
    .unlink     = &uffs_vfs_unlink,
    .rename     = &uffs_vfs_rename,
    .opendir    = &uffs_vfs_opendir,
    .readdir    = &uffs_vfs_readdir,
    .closedir   = &uffs_vfs_closedir,
    .mkdir      = &uffs_vfs_mkdir,
    .ioctl      = NULL
};

static URET _uffs_device_init(uffs_Device *dev)
{
    dev->attr->_private = NULL;
    dev->ops = (struct uffs_FlashOpsSt *)&uffs_nand_ops;

    return U_SUCC;
}

static URET _uffs_device_release(uffs_Device *dev)
{
    return U_SUCC;
}

static int32_t _uffs_fs_init(uffs_mgr_t *mgr)
{
    int32_t ret;
    /* init low level nand device */
    ret = hal_nand_init(mgr->dev);
    if (ret != 0)
        return ret;

    /* init storage attribute struct */
    mgr->storage->page_data_size = mgr->dev->config.page_size;
    mgr->storage->pages_per_block = mgr->dev->config.block_size;
    mgr->storage->spare_size = mgr->dev->config.spare_area_size;
    mgr->storage->ecc_opt = CONFIG_UFFS_ECC_MODE;
    mgr->storage->ecc_size = 0;
    mgr->storage->block_status_offs = mgr->storage->ecc_size;
    mgr->storage->layout_opt = CONFIG_UFFS_LAYOUT;
    mgr->storage->total_blocks = mgr->dev->config.zone_number * mgr->dev->config.zone_size;

    /* init uffs device struct */
    mgr->udev->attr = mgr->storage;
    mgr->udev->Init = _uffs_device_init;
    mgr->udev->Release = _uffs_device_release;
    mgr->udev->_private = mgr->dev;
#if CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR > 0
    uffs_MemSetupSystemAllocator(&mgr->udev->mem);
#endif

    /* init uffs mount table */
    mgr->mtb->mount = "/";
    mgr->mtb->dev = mgr->udev;
    mgr->mtb->start_block = (mgr->storage->total_blocks > CONFIG_UFFS_START_BLOCK) ? 
                            CONFIG_UFFS_START_BLOCK : mgr->storage->total_blocks - 1;
    mgr->mtb->end_block = (mgr->storage->total_blocks > CONFIG_UFFS_END_BLOCK) ? 
                            CONFIG_UFFS_END_BLOCK : mgr->storage->total_blocks - 1;

    return ret;
}

static void _uffs_deinit(void)
{
    if (!g_uffs_mgr)
        return;

    if (g_uffs_mgr->dev) {
        hal_nand_finalize(g_uffs_mgr->dev);
        krhino_mm_free(g_uffs_mgr->dev);
    }

    if (g_uffs_mgr->storage) {
        krhino_mm_free(g_uffs_mgr->storage);
    }

    if (g_uffs_mgr->udev) {
        krhino_mm_free(g_uffs_mgr->udev);
    }

    if (g_uffs_mgr->mtb) {
        krhino_mm_free(g_uffs_mgr->mtb);
    }

    krhino_mm_free(g_uffs_mgr);
    g_uffs_mgr = NULL;
}

static int32_t _uffs_init(void)
{
    if (g_uffs_mgr)
        return 0;

    g_uffs_mgr = (uffs_mgr_t *)krhino_mm_alloc(sizeof(uffs_mgr_t));
    if (!g_uffs_mgr)
        return -ENOMEM;

    memset(g_uffs_mgr, 0, sizeof(uffs_mgr_t));

    /* nand flash device */
    g_uffs_mgr->dev = (nand_dev_t *)krhino_mm_alloc(sizeof(nand_dev_t));
    if (!g_uffs_mgr->dev)
        goto err;
    memset(g_uffs_mgr->dev, 0, sizeof(nand_dev_t));

    /* uffs storage attribute struct */
    g_uffs_mgr->storage = (struct uffs_StorageAttrSt *)krhino_mm_alloc(sizeof(struct uffs_StorageAttrSt));
    if (!g_uffs_mgr->storage)
        goto err;
    memset(g_uffs_mgr->storage, 0, sizeof(struct uffs_StorageAttrSt));

    /* uffs device */
    g_uffs_mgr->udev = (uffs_Device *)krhino_mm_alloc(sizeof(uffs_Device));
    if (!g_uffs_mgr->udev)
        goto err;
    memset(g_uffs_mgr->udev, 0, sizeof(uffs_Device));

    /* uffs mount table */
    g_uffs_mgr->mtb = (uffs_MountTable *)krhino_mm_alloc(sizeof(uffs_MountTable));
    if (!g_uffs_mgr->mtb)
        goto err;
    memset(g_uffs_mgr->mtb, 0, sizeof(uffs_MountTable));

    if (_uffs_fs_init(g_uffs_mgr) == 0)
        return 0;

err:
    _uffs_deinit();
    return -1;    
}

int32_t vfs_uffs_register(void)
{
    int32_t ret = U_SUCC;
    uffs_SetupDebugOutput();

    ret = _uffs_init();
    if (ret != U_SUCC) {
        return ret;
    }

    ret = uffs_InitFileSystemObjects();
    if (ret != U_SUCC) {
        goto err;
    }

    ret = uffs_RegisterMountTable(g_uffs_mgr->mtb);
    if (ret != U_SUCC) {
        uffs_ReleaseFileSystemObjects();
        goto err;
    }

    // first try to mount
    ret = uffs_Mount(g_uffs_mgr->mtb->mount);
    if (ret != U_SUCC) {
        uffs_UnRegisterMountTable(g_uffs_mgr->mtb);
        uffs_ReleaseFileSystemObjects();
        goto err;
    }

    return vfs_register_fs(uffs_mnt_path, &uffs_ops, NULL);

err:
    _uffs_deinit();
    return ret;
}

int32_t vfs_uffs_unregister(void)
{
    uffs_UnMount(g_uffs_mgr->mtb->mount);
    uffs_UnRegisterMountTable(g_uffs_mgr->mtb);
    uffs_ReleaseFileSystemObjects();
    _uffs_deinit();
    return vfs_unregister_fs(uffs_mnt_path);
}
