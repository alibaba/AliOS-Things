#include <stdlib.h>
#include <stdio.h>
#include <aos/device_core.h>
#include <aos/devfs.h>

static const char path_prefix[] = "/dev/";

typedef struct {
    aos_dev_ref_t ref;
} devfs_file_data_t;

static int devfs_open(inode_t *inode, file_t *file)
{
    aos_devfs_node_t *node = (aos_devfs_node_t *)inode->i_arg;
    aos_dev_t *dev = aos_container_of(node, aos_dev_t, devfs_node);
    devfs_file_data_t *fdata;
    int ret;

    fdata = malloc(sizeof(*fdata));
    if (!fdata)
        return -ENOMEM;

    ret = aos_dev_ref(&fdata->ref, dev);
    if (ret) {
        free(fdata);
        return ret;
    }

    file->f_arg = fdata;

    return 0;
}

static int devfs_close(file_t *file)
{
    devfs_file_data_t *fdata = (devfs_file_data_t *)file->f_arg;

    aos_dev_put(&fdata->ref);
    free(fdata);
    file->f_arg = NULL;

    return 0;
}

static ssize_t devfs_read(file_t *file, void *buf, size_t count)
{
    aos_devfs_node_t *node = (aos_devfs_node_t *)file->node->i_arg;

    if (!node->ops->read)
        return -ENOTSUP;

    if (!aos_devfs_file_is_readable(file))
        return -EPERM;

    if (!buf || !count)
        return -EINVAL;

    return node->ops->read(file, buf, count);
}

static ssize_t devfs_write(file_t *file, const void *buf, size_t count)
{
    aos_devfs_node_t *node = (aos_devfs_node_t *)file->node->i_arg;

    if (!node->ops->write)
        return -ENOTSUP;

    if (!aos_devfs_file_is_writable(file))
        return -EPERM;

    if (!buf || !count)
        return -EINVAL;

    return node->ops->write(file, buf, count);
}

static int devfs_ioctl(file_t *file, int cmd, unsigned long arg)
{
    aos_devfs_node_t *node = (aos_devfs_node_t *)file->node->i_arg;

    if (!node->ops->ioctl)
        return -ENOTSUP;

    return node->ops->ioctl(file, cmd, arg);
}

static int devfs_poll(file_t *file, int flag, poll_notify_t notify, void *fd, void *arg)
{
    aos_devfs_node_t *node = (aos_devfs_node_t *)file->node->i_arg;
    struct pollfd *poll_fd = (struct pollfd *)fd;
    aos_devfs_poll_request_t req = 0;
    aos_status_t revents;
    aos_irqsave_t flags;

    revents = node->ops->poll ? node->ops->poll(file, &req) : -ENOTSUP;
    revents = (revents < 0) ? POLLERR : revents;

    if (poll_fd) {
        if (poll_fd->events & revents & (POLLIN | POLLOUT | POLLERR)) {
            poll_fd->revents |= revents & (POLLIN | POLLOUT | POLLERR);
            notify(poll_fd, arg);
        } else {
            flags = aos_spin_lock_irqsave(&node->poll_lock);
            node->poll_notify = notify;
            node->poll_fd = poll_fd;
            node->poll_arg = arg;
            aos_spin_unlock_irqrestore(&node->poll_lock, flags);
        }
    } else {
        flags = aos_spin_lock_irqsave(&node->poll_lock);
        if (node->poll_fd)
            node->poll_fd->revents |= revents & (POLLIN | POLLOUT | POLLERR);
        node->poll_notify = NULL;
        node->poll_fd = NULL;
        node->poll_arg = NULL;
        aos_spin_unlock_irqrestore(&node->poll_lock, flags);
    }

    return 0;
}

static uint32_t devfs_lseek(file_t *file, int64_t offset, int32_t whence)
{
    aos_devfs_node_t *node = (aos_devfs_node_t *)file->node->i_arg;
    int64_t pos;

    if (!node->ops->lseek)
        return (uint32_t)(-ENOTSUP);

    switch (whence) {
    case SEEK_SET:
    case SEEK_CUR:
    case SEEK_END:
        break;
    default:
        return (uint32_t)(-EINVAL);
    }

    pos = node->ops->lseek(file, offset, whence);
    if (pos < 0)
        return (uint32_t)(int32_t)pos;

    aos_devfs_file_set_position(file, pos);

    return (uint32_t)aos_devfs_file_get_position(file);
}

static file_ops_t devfs_fops = {
    .open       = devfs_open,
    .close      = devfs_close,
    .read       = devfs_read,
    .write      = devfs_write,
    .ioctl      = devfs_ioctl,
    .poll       = devfs_poll,
    .lseek      = devfs_lseek,
    .stat       = NULL,
    .mmap       = NULL,
    .access     = NULL,
};

aos_status_t aos_devfs_add_node(aos_devfs_node_t *node)
{
    char path[sizeof(path_prefix) - 1 + AOS_DEVFS_NODE_NAME_MAX_LEN + 1];
    int path_len;

    if (!node || !aos_devfs_node_is_valid(node))
        return -EINVAL;

    path_len = snprintf(path, sizeof(path), "%s%s", path_prefix, node->name);
    if (path_len < 0 || path_len >= sizeof(path))
        return -EINVAL;

    aos_spin_lock_init(&node->poll_lock);
    node->poll_notify = NULL;
    node->poll_fd = NULL;
    node->poll_arg = NULL;

    return aos_register_driver(path, &devfs_fops, node);
}

aos_status_t aos_devfs_remove_node(aos_devfs_node_t *node)
{
    char path[sizeof(path_prefix) - 1 + AOS_DEVFS_NODE_NAME_MAX_LEN + 1];
    int path_len;

    if (!node || !aos_devfs_node_is_valid(node))
        return -EINVAL;

    path_len = snprintf(path, sizeof(path), "%s%s", path_prefix, node->name);
    if (path_len < 0 || path_len >= sizeof(path))
        return -EINVAL;

    (void)aos_unregister_driver(path);

    return 0;
}

aos_dev_ref_t *aos_devfs_file2ref(aos_devfs_file_t *file)
{
    devfs_file_data_t *fdata = (devfs_file_data_t *)file->f_arg;

    return &fdata->ref;
}

mode_t aos_devfs_file_get_mode(aos_devfs_file_t *file)
{
    return file->node->i_flags;
}

uint64_t aos_devfs_file_get_position(aos_devfs_file_t *file)
{
    return file->offset;
}

void aos_devfs_file_set_position(aos_devfs_file_t *file, uint64_t pos)
{
    file->offset = pos;
}

void aos_devfs_poll_add(aos_devfs_node_t *node, aos_devfs_wait_queue_t wq, aos_devfs_poll_request_t *req)
{
}

void aos_devfs_poll_wakeup(aos_devfs_node_t *node, aos_devfs_wait_queue_t wq)
{
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&node->poll_lock);
    if (node->poll_notify)
        node->poll_notify(node->poll_fd, node->poll_arg);
    node->poll_notify = NULL;
    node->poll_fd = NULL;
    node->poll_arg = NULL;
    aos_spin_unlock_irqrestore(&node->poll_lock, flags);
}
