/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/flashpart_core.h>
#ifdef AOS_COMP_DEVFS
#include <stdio.h>
#include <inttypes.h>
#endif

aos_status_t aos_flashpart_get(aos_flashpart_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_FLASHPART, id);
}

void aos_flashpart_put(aos_flashpart_ref_t *ref)
{
    aos_dev_put(ref);
}

aos_status_t aos_flashpart_get_info(aos_flashpart_ref_t *ref, aos_flashpart_info_t *info, aos_flash_info_t *flash_info)
{
    aos_flashpart_t *flashpart;

    if (!ref || !aos_dev_ref_is_valid(ref) || !info)
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    info->flash_id = flashpart->flash_id;
    info->block_start = flashpart->block_start;
    info->block_count = flashpart->block_count;

    if (flash_info)
        *flash_info = flashpart->flash_info;

    return 0;
}

aos_status_t aos_flashpart_read(aos_flashpart_ref_t *ref, uint64_t offset, void *buf, size_t count)
{
    aos_flashpart_t *flashpart;
    aos_flash_info_t *flash_info;
    uint64_t part_offset;
    uint64_t part_size;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    flash_info = &flashpart->flash_info;
    part_offset = flashpart->block_start * flash_info->pages_per_block * flash_info->page_data_size;
    part_size = flashpart->block_count * flash_info->pages_per_block * flash_info->page_data_size;

    if (offset >= part_size || count > part_size - offset)
        return -EINVAL;

    return aos_flash_read(&flashpart->flash_ref, part_offset + offset, buf, count);
}

aos_status_t aos_flashpart_write(aos_flashpart_ref_t *ref, uint64_t offset, const void *buf, size_t count)
{
    aos_flashpart_t *flashpart;
    aos_flash_info_t *flash_info;
    uint64_t part_offset;
    uint64_t part_size;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    flash_info = &flashpart->flash_info;
    part_offset = flashpart->block_start * flash_info->pages_per_block * flash_info->page_data_size;
    part_size = flashpart->block_count * flash_info->pages_per_block * flash_info->page_data_size;

    if (offset >= part_size || count > part_size - offset)
        return -EINVAL;

    return aos_flash_write(&flashpart->flash_ref, part_offset + offset, buf, count);
}

aos_status_t aos_flashpart_erase(aos_flashpart_ref_t *ref, uint64_t offset, size_t size)
{
    aos_flashpart_t *flashpart;
    aos_flash_info_t *flash_info;
    uint64_t part_offset;
    uint64_t part_size;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    flash_info = &flashpart->flash_info;
    part_offset = flashpart->block_start * flash_info->pages_per_block * flash_info->page_data_size;
    part_size = flashpart->block_count * flash_info->pages_per_block * flash_info->page_data_size;

    if (offset >= part_size || size > part_size - offset)
        return -EINVAL;

    return aos_flash_erase(&flashpart->flash_ref, part_offset + offset, size);
}

aos_status_t aos_flashpart_read_page(aos_flashpart_ref_t *ref, uint64_t page,
                                     size_t data_offset, void *data_buf, size_t data_count,
                                     size_t spare_offset, void *spare_buf, size_t spare_count)
{
    aos_flashpart_t *flashpart;
    aos_flash_info_t *flash_info;
    uint64_t page_start;
    uint64_t num_pages;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    flash_info = &flashpart->flash_info;
    page_start = flashpart->block_start * flash_info->pages_per_block;
    num_pages = flashpart->block_count * flash_info->pages_per_block;

    if (page >= num_pages)
        return -EINVAL;

    return aos_flash_read_page(&flashpart->flash_ref, page_start + page, data_offset, data_buf, data_count,
                               spare_offset, spare_buf, spare_count);
}

aos_status_t aos_flashpart_write_page(aos_flashpart_ref_t *ref, uint64_t page,
                                      size_t data_offset, const void *data_buf, size_t data_count,
                                      size_t spare_offset, const void *spare_buf, size_t spare_count)
{
    aos_flashpart_t *flashpart;
    aos_flash_info_t *flash_info;
    uint64_t page_start;
    uint64_t num_pages;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    flash_info = &flashpart->flash_info;
    page_start = flashpart->block_start * flash_info->pages_per_block;
    num_pages = flashpart->block_count * flash_info->pages_per_block;

    if (page >= num_pages)
        return -EINVAL;

    return aos_flash_write_page(&flashpart->flash_ref, page_start + page, data_offset, data_buf, data_count,
                                spare_offset, spare_buf, spare_count);
}

aos_status_t aos_flashpart_erase_block(aos_flashpart_ref_t *ref, uint64_t block)
{
    aos_flashpart_t *flashpart;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    if (block >= flashpart->block_count)
        return -EINVAL;

    return aos_flash_erase_block(&flashpart->flash_ref, flashpart->block_start + block);
}

aos_status_t aos_flashpart_is_bad_block(aos_flashpart_ref_t *ref, uint64_t block)
{
    aos_flashpart_t *flashpart;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    if (block >= flashpart->block_count)
        return -EINVAL;

    return aos_flash_is_bad_block(&flashpart->flash_ref, flashpart->block_start + block);
}

aos_status_t aos_flashpart_mark_bad_block(aos_flashpart_ref_t *ref, uint64_t block)
{
    aos_flashpart_t *flashpart;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    if (block >= flashpart->block_count)
        return -EINVAL;

    return aos_flash_mark_bad_block(&flashpart->flash_ref, flashpart->block_start + block);
}

static void dev_flashpart_unregister(aos_dev_t *dev)
{
}

static aos_status_t dev_flashpart_get(aos_dev_ref_t *ref)
{
    aos_flashpart_t *flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);
    aos_status_t ret;

    if (!aos_dev_ref_is_first(ref))
        return 0;

    ret = aos_flash_get(&flashpart->flash_ref, flashpart->flash_id);
    if (ret)
        return ret;

    ret = aos_flash_get_info(&flashpart->flash_ref, &flashpart->flash_info);
    if (ret) {
        aos_flash_put(&flashpart->flash_ref);
        return ret;
    }

    if (flashpart->block_start >= flashpart->flash_info.block_count ||
        flashpart->block_count > flashpart->flash_info.block_count - flashpart->block_start) {
        aos_flash_put(&flashpart->flash_ref);
        return -EINVAL;
    }

    return 0;
}

static void dev_flashpart_put(aos_dev_ref_t *ref)
{
    aos_flashpart_t *flashpart = aos_container_of(ref->dev, aos_flashpart_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    aos_flash_put(&flashpart->flash_ref);
}

static const aos_dev_ops_t dev_flashpart_ops = {
    .unregister = dev_flashpart_unregister,
    .get        = dev_flashpart_get,
    .put        = dev_flashpart_put,
};

#ifdef AOS_COMP_DEVFS

static aos_status_t devfs_mtd_ioctl(aos_devfs_file_t *file, int cmd, uintptr_t arg)
{
    aos_flashpart_ref_t *ref = aos_devfs_file2ref(file);
    aos_status_t ret;

    switch (cmd) {
    case AOS_MTD_IOC_GET_INFO:
        {
            aos_flash_info_t flash_info;
            aos_flashpart_info_t part_info;
            aos_mtd_info_t info;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            ret = aos_flashpart_get_info(ref, &part_info, &flash_info);
            if (ret)
                break;
            info.flash_id = part_info.flash_id;
            info.flash_flags = flash_info.flags;
            info.flash_block_count = flash_info.block_count;
            info.block_start = part_info.block_start;
            info.block_count = part_info.block_count;
            info.pages_per_block = flash_info.pages_per_block;
            info.page_data_size = flash_info.page_data_size;
            info.page_spare_size = flash_info.page_spare_size;
            if (!aos_umem_check((const void *)arg, sizeof(info))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_umem_copy((void *)arg, &info, sizeof(info)) ? -EFAULT : 0;
        }
        break;
    case AOS_MTD_IOC_ERASE:
        {
            aos_mtd_erase_args_t args;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&args, (const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_flashpart_erase(ref, args.offset, args.size);
        }
        break;
    case AOS_MTD_IOC_READ_PAGE:
        {
            aos_mtd_read_page_args_t args;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&args, (const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (args.data_buf && !aos_umem_check(args.data_buf, args.data_count)) {
                ret = -EFAULT;
                break;
            }
            if (args.spare_buf && !aos_umem_check(args.spare_buf, args.spare_count)) {
                ret = -EFAULT;
                break;
            }
            ret = aos_flashpart_read_page(ref, args.page, args.data_offset, args.data_buf, args.data_count,
                                          args.spare_offset, args.spare_buf, args.spare_count);
        }
        break;
    case AOS_MTD_IOC_WRITE_PAGE:
        {
            aos_mtd_write_page_args_t args;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&args, (const void *)arg, sizeof(args))) {
                ret = -EFAULT;
                break;
            }
            if (args.data_buf && !aos_umem_check(args.data_buf, args.data_count)) {
                ret = -EFAULT;
                break;
            }
            if (args.spare_buf && !aos_umem_check(args.spare_buf, args.spare_count)) {
                ret = -EFAULT;
                break;
            }
            ret = aos_flashpart_write_page(ref, args.page, args.data_offset, args.data_buf, args.data_count,
                                           args.spare_offset, args.spare_buf, args.spare_count);
        }
        break;
    case AOS_MTD_IOC_ERASE_BLOCK:
        {
            uint64_t block;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(block))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&block, (const void *)arg, sizeof(block))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_flashpart_erase_block(ref, block);
        }
        break;
    case AOS_MTD_IOC_IS_BAD_BLOCK:
        {
            uint64_t block;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(block))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&block, (const void *)arg, sizeof(block))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_flashpart_is_bad_block(ref, block);
        }
        break;
    case AOS_MTD_IOC_MARK_BAD_BLOCK:
        {
            uint64_t block;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(block))) {
                ret = -EFAULT;
                break;
            }
            if (aos_umem_copy(&block, (const void *)arg, sizeof(block))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_flashpart_mark_bad_block(ref, block);
        }
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static ssize_t devfs_mtd_read(aos_devfs_file_t *file, void *buf, size_t count)
{
    aos_flashpart_ref_t *ref = aos_devfs_file2ref(file);
    uint64_t offset = aos_devfs_file_get_position(file);
    aos_status_t r;

    if (!aos_devfs_file_is_readable(file))
        return -EPERM;

    if (count > SIZE_MAX / 2 - 1)
        count = SIZE_MAX / 2 - 1;

    if (buf && !aos_umem_check(buf, count))
        return -EFAULT;

    r = aos_flashpart_read(ref, offset, buf, count);
    if (r)
        return r;

    aos_devfs_file_set_position(file, offset + count);

    return count;
}

static ssize_t devfs_mtd_write(aos_devfs_file_t *file, const void *buf, size_t count)
{
    aos_flashpart_ref_t *ref = aos_devfs_file2ref(file);
    uint64_t offset = aos_devfs_file_get_position(file);
    aos_status_t r;

    if (!aos_devfs_file_is_writable(file))
        return -EPERM;

    if (count > SIZE_MAX / 2 - 1)
        count = SIZE_MAX / 2 - 1;

    if (buf && !aos_umem_check(buf, count))
        return -EFAULT;

    r = aos_flashpart_write(ref, offset, buf, count);
    if (r)
        return r;

    aos_devfs_file_set_position(file, offset + count);

    return count;
}

static int64_t devfs_mtd_lseek(aos_devfs_file_t *file, int64_t offset, int whence)
{
    aos_flashpart_ref_t *ref = aos_devfs_file2ref(file);
    aos_flashpart_t *part = aos_container_of(ref->dev, aos_flashpart_t, dev);
    uint64_t part_size = part->block_count * part->flash_info.pages_per_block * part->flash_info.page_data_size;

    return aos_devfs_file_lseek_sized(file, part_size, offset, whence);
}

static const aos_devfs_file_ops_t devfs_mtd_ops = {
    .ioctl      = devfs_mtd_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = devfs_mtd_read,
    .write      = devfs_mtd_write,
    .lseek      = devfs_mtd_lseek,
};

#endif /* AOS_COMP_DEVFS */

aos_status_t aos_flashpart_register(aos_flashpart_t *flashpart)
{
#ifdef AOS_COMP_DEVFS
    int name_len;
#endif

    if (!flashpart || flashpart->block_count == 0)
        return -EINVAL;

    flashpart->dev.type = AOS_DEV_TYPE_FLASHPART;
    flashpart->dev.ops = &dev_flashpart_ops;
#ifdef AOS_COMP_DEVFS
    aos_devfs_node_init(&flashpart->dev.devfs_node);
    flashpart->dev.devfs_node.ops = &devfs_mtd_ops;
    name_len = snprintf(flashpart->dev.devfs_node.name, sizeof(flashpart->dev.devfs_node.name),
                        "mtd%" PRIu32, flashpart->dev.id);
    if (name_len < 0 || name_len >= sizeof(flashpart->dev.devfs_node.name))
        return -EINVAL;
#endif

    return aos_dev_register(&flashpart->dev);
}

aos_status_t aos_flashpart_register_argumented(aos_flashpart_t *flashpart, uint32_t id, uint32_t flash_id,
                                               uint32_t block_start, uint64_t block_count)
{
    if (!flashpart)
        return -EINVAL;

    flashpart->dev.id = id;
    flashpart->flash_id = flash_id;
    flashpart->block_start = block_start;
    flashpart->block_count = block_count;

    return aos_flashpart_register(flashpart);
}

aos_status_t aos_flashpart_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_FLASHPART, id);
}
