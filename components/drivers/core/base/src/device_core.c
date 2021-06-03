/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/device_core.h>

#define REF_COUNT_PENDING       UINT32_MAX
#define REF_COUNT_MAX           (UINT32_MAX - 1)

static struct k_rbtree_root_t device_map = RBT_ROOT;
static aos_mutex_t device_map_mutex;

static void device_map_lock(void)
{
    (void)aos_mutex_lock(&device_map_mutex, AOS_WAIT_FOREVER);
}

static void device_map_unlock(void)
{
    (void)aos_mutex_unlock(&device_map_mutex);
}

static void device_forbid_removal(aos_dev_t *dev)
{
    (void)aos_sem_wait(&dev->rb_sem, AOS_WAIT_FOREVER);
}

static void device_allow_removal(aos_dev_t *dev)
{
    (void)aos_sem_signal(&dev->rb_sem);
}

static void device_wait_removal(aos_dev_t *dev)
{
    (void)aos_sem_wait(&dev->rb_sem, AOS_WAIT_FOREVER);
}

static int compare_device(aos_dev_type_t a_type, uint32_t a_id,
                          aos_dev_type_t b_type, uint32_t b_id)
{
    if (a_type < b_type)
        return -1;

    if (a_type > b_type)
        return 1;

    if (a_id < b_id)
        return -1;

    if (a_id > b_id)
        return 1;

    return 0;
}

static aos_dev_t *find_device(aos_dev_type_t type, uint32_t id)
{
    struct k_rbtree_node_t *node = device_map.rbt_node;

    while (node) {
        aos_dev_t *dev = k_rbtree_entry(node, aos_dev_t, rb_node);
        int r = compare_device(type, id, dev->type, dev->id);

        if (r < 0)
            node = node->rbt_left;
        else if (r > 0)
            node = node->rbt_right;
        else
            break;
    }

    return node ? k_rbtree_entry(node, aos_dev_t, rb_node) : NULL;
}

static void insert_device(aos_dev_t *dev)
{
    struct k_rbtree_node_t **link = &device_map.rbt_node;
    struct k_rbtree_node_t *parent = NULL;

    while (*link) {
        aos_dev_t *tmp_dev;
        int r;

        parent = *link;
        tmp_dev = k_rbtree_entry(*link, aos_dev_t, rb_node);
        r = compare_device(dev->type, dev->id, tmp_dev->type, tmp_dev->id);
        link = (r < 0) ? &(*link)->rbt_left : &(*link)->rbt_right;
    }

    k_rbtree_link_node(&dev->rb_node, parent, link);
    k_rbtree_insert_color(&dev->rb_node, &device_map);
}

static void remove_device(aos_dev_t *dev)
{
    k_rbtree_erase(&dev->rb_node, &device_map);
}

#ifdef AOS_COMP_VFS
static aos_status_t add_to_vfs(aos_dev_t *dev)
{
    const char prefix[] = "/dev/";
    char path[sizeof(prefix) - 1 + sizeof(dev->vfs_helper.name)];

    if (dev->vfs_helper.name[0] == '\0' || !dev->vfs_helper.ops)
        return 0;

    memcpy(path, prefix, sizeof(prefix) - 1);
    strcpy(&path[sizeof(prefix) - 1], dev->vfs_helper.name);

    return aos_register_driver(path, dev->vfs_helper.ops, dev);
}

static void remove_from_vfs(aos_dev_t *dev)
{
    const char prefix[] = "/dev/";
    char path[sizeof(prefix) - 1 + sizeof(dev->vfs_helper.name)];

    if (dev->vfs_helper.name[0] == '\0' || !dev->vfs_helper.ops)
        return;

    memcpy(path, prefix, sizeof(prefix) - 1);
    strcpy(&path[sizeof(prefix) - 1], dev->vfs_helper.name);
    (void)aos_unregister_driver(path);
}
#endif /* AOS_COMP_VFS */

aos_status_t aos_dev_register(aos_dev_t *dev)
{
    aos_status_t ret;

    if (!dev)
        return -EINVAL;

    ret = aos_sem_new(&dev->rb_sem, 1);
    if (ret)
        return ret;

    ret = aos_mutex_new(&dev->mutex);
    if (ret) {
        aos_sem_free(&dev->rb_sem);
        return ret;
    }

    dev->ref_count = REF_COUNT_PENDING;
    device_map_lock();

    if (find_device(dev->type, dev->id)) {
        device_map_unlock();
        aos_mutex_free(&dev->mutex);
        aos_sem_free(&dev->rb_sem);
        return -EEXIST;
    }

    insert_device(dev);
    device_map_unlock();

#ifdef AOS_COMP_VFS
    ret = add_to_vfs(dev);
    if (ret) {
        device_map_lock();
        remove_device(dev);
        device_map_unlock();
        aos_mutex_free(&dev->mutex);
        aos_sem_free(&dev->rb_sem);
        return ret;
    }
#endif

    aos_dev_lock(dev);
    dev->ref_count = 0;
    aos_dev_unlock(dev);

    return 0;
}

aos_status_t aos_dev_unregister(aos_dev_type_t type, uint32_t id)
{
    aos_dev_t *dev;
    aos_status_t ret;

    device_map_lock();
    dev = find_device(type, id);
    if (!dev) {
        device_map_unlock();
        return -ENODEV;
    }

    aos_dev_lock(dev);

    if (dev->ref_count == REF_COUNT_PENDING) {
        aos_dev_unlock(dev);
        device_map_unlock();
        return -ENODEV;
    } else if (dev->ref_count > 0) {
        aos_dev_unlock(dev);
        device_map_unlock();
        return -EBUSY;
    } else {
        dev->ref_count = REF_COUNT_PENDING;
    }

    aos_dev_unlock(dev);
    device_map_unlock();
#ifdef AOS_COMP_VFS
    remove_from_vfs(dev);
#endif
    device_map_lock();
    device_wait_removal(dev);
    remove_device(dev);
    device_map_unlock();
    aos_mutex_free(&dev->mutex);
    aos_sem_free(&dev->rb_sem);

    if (dev->ops && dev->ops->unregister)
        dev->ops->unregister(dev);

    return 0;
}

aos_status_t aos_dev_ref(aos_dev_ref_t *ref, aos_dev_t *dev)
{
    if (!ref)
        return -EINVAL;

    if (!dev) {
        aos_dev_ref_init(ref);
        return -EINVAL;
    }

    aos_dev_lock(dev);

    if (dev->ref_count == REF_COUNT_PENDING) {
        aos_dev_unlock(dev);
        aos_dev_ref_init(ref);
        return -ENODEV;
    } else if (dev->ref_count == REF_COUNT_MAX) {
        aos_dev_unlock(dev);
        aos_dev_ref_init(ref);
        return -EBUSY;
    }

    ref->dev = dev;
    ref->pdata = NULL;

    if (dev->ops && dev->ops->get) {
        aos_status_t ret;

        ret = dev->ops->get(ref);
        if (ret) {
            aos_dev_unlock(dev);
            aos_dev_ref_init(ref);
            return ret;
        }
    }

    dev->ref_count++;
    aos_dev_unlock(dev);

    return 0;
}

aos_status_t aos_dev_get(aos_dev_ref_t *ref, aos_dev_type_t type, uint32_t id)
{
    aos_dev_t *dev;
    aos_status_t ret;

    if (!ref)
        return -EINVAL;

    device_map_lock();
    dev = find_device(type, id);
    if (!dev) {
        device_map_unlock();
        aos_dev_ref_init(ref);
        return -ENODEV;
    }

    device_forbid_removal(dev);
    device_map_unlock();
    ret = aos_dev_ref(ref, dev);
    device_allow_removal(dev);

    return ret;
}

void aos_dev_put(aos_dev_ref_t *ref)
{
    if (!ref || !aos_dev_ref_is_valid(ref))
        return;

    aos_dev_lock(ref->dev);
    ref->dev->ref_count--;

    if (ref->dev->ops && ref->dev->ops->put)
        ref->dev->ops->put(ref);

    aos_dev_unlock(ref->dev);
    aos_dev_ref_init(ref);
}

static int device_core_init(void)
{
    return (int)aos_mutex_new(&device_map_mutex);
}

CORE_DRIVER_ENTRY(device_core_init)
