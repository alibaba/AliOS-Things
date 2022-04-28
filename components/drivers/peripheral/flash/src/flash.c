/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/flash_core.h>

aos_status_t aos_flash_get(aos_flash_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_FLASH, id);
}

void aos_flash_put(aos_flash_ref_t *ref)
{
    aos_dev_put(ref);
}

aos_status_t aos_flash_get_info(aos_flash_ref_t *ref, aos_flash_info_t *info)
{
    aos_flash_t *flash;

    if (!ref || !aos_dev_ref_is_valid(ref) || !info)
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    info->flags = flash->flags;
    info->block_count = flash->block_count;
    info->pages_per_block = flash->pages_per_block;
    info->page_data_size = flash->page_data_size;
    info->page_spare_size = flash->page_spare_size;

    return 0;
}

aos_status_t aos_flash_read(aos_flash_ref_t *ref, uint64_t offset, void *buf, size_t count)
{
    aos_flash_t *flash;
    uint64_t full_size;
    aos_status_t ret;
    uint64_t i;
    size_t j;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    full_size = flash->block_count * flash->pages_per_block * flash->page_data_size;
    if (offset >= full_size || count > full_size - offset || count == 0 || !buf)
        return -EINVAL;

    for (i = offset / flash->page_data_size, j = 0, ret = 0; j < count; i++) {
        size_t page_data_offset = (size_t)(j == 0 ? offset - i * flash->page_data_size : 0);
        size_t page_data_count = count - j < flash->page_data_size - page_data_offset ?
                                 count - j : flash->page_data_size - page_data_offset;
        aos_status_t r;

        r = aos_flash_read_page(ref, i, page_data_offset, (char *)buf + j, page_data_count, 0, NULL, 0);
        if (r < 0) {
            ret = r;
            break;
        } else {
            ret = (r > ret) ? r : ret;
        }

        j += page_data_count;
    }

    return ret;
}

aos_status_t aos_flash_write(aos_flash_ref_t *ref, uint64_t offset, const void *buf, size_t count)
{
    aos_flash_t *flash;
    uint64_t full_size;
    uint64_t i;
    size_t j;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    full_size = flash->block_count * flash->pages_per_block * flash->page_data_size;
    if (offset >= full_size || count > full_size - offset || count == 0 || !buf)
        return -EINVAL;

    for (i = offset / flash->page_data_size, j = 0; j < count; i++) {
        size_t page_data_offset = (size_t)(j == 0 ? offset - i * flash->page_data_size : 0);
        size_t page_data_count = count - j < flash->page_data_size - page_data_offset ?
                                 count - j : flash->page_data_size - page_data_offset;
        aos_status_t ret;

        ret = aos_flash_write_page(ref, i, page_data_offset, (const char *)buf + j, page_data_count, 0, NULL, 0);
        if (ret)
            return ret;

        j += page_data_count;
    }

    return 0;
}

aos_status_t aos_flash_erase(aos_flash_ref_t *ref, uint64_t offset, size_t size)
{
    aos_flash_t *flash;
    uint64_t full_size;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    full_size = flash->block_count * flash->pages_per_block * flash->page_data_size;
    if (offset >= full_size || size > full_size - offset || size == 0)
        return -EINVAL;

    if (offset % ((uint64_t)flash->pages_per_block * flash->page_data_size))
        return -EINVAL;

    if (size % ((uint64_t)flash->pages_per_block * flash->page_data_size))
        return -EINVAL;

    for (uint64_t i = offset / ((uint64_t)flash->pages_per_block * flash->page_data_size);
         i * flash->pages_per_block * flash->page_data_size < offset + size; i++) {
        aos_status_t ret;

        ret = aos_flash_erase_block(ref, i);
        if (ret)
            return ret;
    }

    return 0;
}

aos_status_t aos_flash_read_page(aos_flash_ref_t *ref, uint64_t page,
                                 size_t data_offset, void *data_buf, size_t data_count,
                                 size_t spare_offset, void *spare_buf, size_t spare_count)
{
    aos_flash_t *flash;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    if (page >= flash->block_count * flash->pages_per_block)
        return -EINVAL;

    if (data_count == 0 && spare_count == 0)
        return -EINVAL;

    if (data_count) {
        if (!data_buf || data_offset >= flash->page_data_size || data_count > flash->page_data_size - data_offset)
            return -EINVAL;
    } else {
        if (data_buf || data_offset)
            return -EINVAL;
    }

    if (spare_count) {
        if (!spare_buf || spare_offset >= flash->page_spare_size || spare_count > flash->page_spare_size - spare_offset)
            return -EINVAL;
    } else {
        if (spare_buf || spare_offset)
            return -EINVAL;
    }

    aos_dev_lock(ref->dev);
    ret = flash->ops->read_page(flash, page, data_offset, data_count, spare_offset, spare_count);
    if (ret < 0) {
        aos_dev_unlock(ref->dev);
        return ret;
    }

    if (data_count) {
        if (aos_umem_copy(data_buf, (const char *)flash->page_data_buf + data_offset, data_count)) {
            aos_dev_unlock(ref->dev);
            return -EFAULT;
        }
    }

    if (spare_count) {
        if (aos_umem_copy(spare_buf, (const char *)flash->page_spare_buf + spare_offset, spare_count)) {
            aos_dev_unlock(ref->dev);
            return -EFAULT;
        }
    }

    aos_dev_unlock(ref->dev);

    return ret;
}

aos_status_t aos_flash_write_page(aos_flash_ref_t *ref, uint64_t page,
                                  size_t data_offset, const void *data_buf, size_t data_count,
                                  size_t spare_offset, const void *spare_buf, size_t spare_count)
{
    aos_flash_t *flash;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    if (page >= flash->block_count * flash->pages_per_block)
        return -EINVAL;

    if (data_count == 0 && spare_count == 0)
        return -EINVAL;

    if (data_count) {
        if (!data_buf || data_offset >= flash->page_data_size || data_count > flash->page_data_size - data_offset)
            return -EINVAL;
    } else {
        if (data_buf || data_offset)
            return -EINVAL;
    }

    if (spare_count) {
        if (!spare_buf || spare_offset >= flash->page_spare_size || spare_count > flash->page_spare_size - spare_offset)
            return -EINVAL;
    } else {
        if (spare_buf || spare_offset)
            return -EINVAL;
    }

    aos_dev_lock(ref->dev);

    if (data_count) {
        if (aos_umem_copy((char *)flash->page_data_buf + data_offset, data_buf, data_count)) {
            aos_dev_unlock(ref->dev);
            return -EFAULT;
        }
    }

    if (spare_count) {
        if (aos_umem_copy((char *)flash->page_spare_buf + spare_offset, spare_buf, spare_count)) {
            aos_dev_unlock(ref->dev);
            return -EFAULT;
        }
    }

    ret = flash->ops->write_page(flash, page, data_offset, data_count, spare_offset, spare_count);
    aos_dev_unlock(ref->dev);

    return ret;
}

aos_status_t aos_flash_erase_block(aos_flash_ref_t *ref, uint64_t block)
{
    aos_flash_t *flash;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    if (block >= flash->block_count)
        return -EINVAL;

    aos_dev_lock(ref->dev);
    ret = flash->ops->erase_block(flash, block);
    aos_dev_unlock(ref->dev);

    return ret;
}

aos_status_t aos_flash_is_bad_block(aos_flash_ref_t *ref, uint64_t block)
{
    aos_flash_t *flash;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    if (block >= flash->block_count)
        return -EINVAL;

    if (!flash->ops->is_bad_block)
        return -ENOTSUP;

    aos_dev_lock(ref->dev);
    ret = flash->ops->is_bad_block(flash, block);
    aos_dev_unlock(ref->dev);

    return ret;
}

aos_status_t aos_flash_mark_bad_block(aos_flash_ref_t *ref, uint64_t block)
{
    aos_flash_t *flash;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref))
        return -EINVAL;

    flash = aos_container_of(ref->dev, aos_flash_t, dev);
    if (block >= flash->block_count)
        return -EINVAL;

    if (!flash->ops->mark_bad_block)
        return -ENOTSUP;

    aos_dev_lock(ref->dev);
    ret = flash->ops->mark_bad_block(flash, block);
    aos_dev_unlock(ref->dev);

    return ret;
}

static void dev_flash_unregister(aos_dev_t *dev)
{
    aos_flash_t *flash = aos_container_of(dev, aos_flash_t, dev);

    if (flash->ops->unregister)
        flash->ops->unregister(flash);
}

static aos_status_t dev_flash_get(aos_dev_ref_t *ref)
{
    aos_flash_t *flash = aos_container_of(ref->dev, aos_flash_t, dev);

    if (!aos_dev_ref_is_first(ref))
        return 0;

    return flash->ops->startup(flash);
}

static void dev_flash_put(aos_dev_ref_t *ref)
{
    aos_flash_t *flash = aos_container_of(ref->dev, aos_flash_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    flash->ops->shutdown(flash);
}

static const aos_dev_ops_t dev_flash_ops = {
    .unregister = dev_flash_unregister,
    .get        = dev_flash_get,
    .put        = dev_flash_put,
};

aos_status_t aos_flash_register(aos_flash_t *flash)
{
    if (!flash)
        return -EINVAL;

    if (!flash->ops || !flash->ops->startup || !flash->ops->shutdown ||
        !flash->ops->read_page || !flash->ops->write_page || !flash->ops->erase_block)
        return -EINVAL;

    switch (flash->flags & AOS_FLASH_F_TYPE_MASK) {
    case AOS_FLASH_F_TYPE_NOR:
        if (flash->ops->is_bad_block || flash->ops->mark_bad_block)
            return -EINVAL;
        if (aos_flash_flags_to_ecc_strength(flash->flags))
            return -EINVAL;
        if (flash->page_spare_size)
            return -EINVAL;
        break;
    case AOS_FLASH_F_TYPE_NAND:
        if (!flash->ops->is_bad_block || !flash->ops->mark_bad_block)
            return -EINVAL;
        break;
    default:
        return -EINVAL;
    }

    if (!flash->block_count || !flash->pages_per_block || !flash->page_data_size)
        return -EINVAL;

    if (UINT64_MAX / flash->pages_per_block < flash->page_data_size ||
        UINT64_MAX / flash->block_count < (uint64_t)flash->pages_per_block * flash->page_data_size)
        return -EINVAL;

    if (!flash->page_data_buf)
        return -EINVAL;

    if ((flash->page_spare_size && !flash->page_spare_buf) || (!flash->page_spare_size && flash->page_spare_buf))
        return -EINVAL;

    flash->dev.type = AOS_DEV_TYPE_FLASH;
    flash->dev.ops = &dev_flash_ops;
#ifdef AOS_COMP_DEVFS
    aos_devfs_node_init(&flash->dev.devfs_node);
#endif

    return aos_dev_register(&flash->dev);
}

aos_status_t aos_flash_register_argumented(aos_flash_t *flash, uint32_t id, const aos_flash_ops_t *ops,
                                           uint32_t flags, uint64_t block_count, size_t pages_per_block,
                                           size_t page_data_size, size_t page_spare_size,
                                           void *page_data_buf, void *page_spare_buf)
{
    if (!flash)
        return -EINVAL;

    flash->dev.id = id;
    flash->ops = ops;
    flash->flags = flags;
    flash->block_count = block_count;
    flash->pages_per_block = pages_per_block;
    flash->page_data_size = page_data_size;
    flash->page_spare_size = page_spare_size;
    flash->page_data_buf = page_data_buf;
    flash->page_spare_buf = page_spare_buf;

    return aos_flash_register(flash);
}

aos_status_t aos_flash_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_FLASH, id);
}
