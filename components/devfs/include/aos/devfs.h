/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef AOS_DEVFS_H
#define AOS_DEVFS_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <aos/kernel.h>
#include <aos/device.h>
#ifdef AOS_COMP_VFS
#include <poll.h>
#include <aos/vfs.h>
#else
#error Unsupported OS.
#endif

#define AOS_DEVFS_NODE_NAME_MAX_LEN     63

struct aos_devfs_file_ops;

typedef enum {
    AOS_DEVFS_WQ_RD     = 0,
    AOS_DEVFS_WQ_WR,
    AOS_DEVFS_WQ_MAX,
} aos_devfs_wait_queue_t;

typedef struct {
    char name[AOS_DEVFS_NODE_NAME_MAX_LEN + 1];
    const struct aos_devfs_file_ops *ops;
#ifdef AOS_COMP_VFS
    aos_spinlock_t poll_lock;
    poll_notify_t poll_notify;
    struct pollfd *poll_fd;
    void *poll_arg;
#else
#error Unsupported OS.
#endif
} aos_devfs_node_t;

#define AOS_DEVFS_NODE_INIT_VAL         { .name = { '\0', }, .ops = NULL, }
#define aos_devfs_node_init(node)       do { *(node) = (aos_devfs_node_t)AOS_DEVFS_NODE_INIT_VAL; } while (0)
#define aos_devfs_node_is_valid(node)   ((node)->name[0] != '\0')

#ifdef AOS_COMP_VFS
typedef file_t aos_devfs_file_t;
typedef int aos_devfs_poll_request_t;
#else
#error Unsupported OS.
#endif

typedef struct aos_devfs_file_ops {
    aos_status_t (*ioctl)(aos_devfs_file_t *file, int cmd, uintptr_t arg);
    aos_status_t (*poll)(aos_devfs_file_t *file, aos_devfs_poll_request_t *req);
    /* TODO: complete the argument list of mmap() */
    aos_status_t (*mmap)(aos_devfs_file_t *file, ...);
    ssize_t (*read)(aos_devfs_file_t *file, void *buf, size_t count);
    ssize_t (*write)(aos_devfs_file_t *file, const void *buf, size_t count);
    int64_t (*lseek)(aos_devfs_file_t *file, int64_t offset, int whence);
} aos_devfs_file_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_devfs_add_node(aos_devfs_node_t *node);
aos_status_t aos_devfs_remove_node(aos_devfs_node_t *node);
aos_dev_ref_t *aos_devfs_file2ref(aos_devfs_file_t *file);
mode_t aos_devfs_file_get_mode(aos_devfs_file_t *file);
uint64_t aos_devfs_file_get_position(aos_devfs_file_t *file);
void aos_devfs_file_set_position(aos_devfs_file_t *file, uint64_t pos);
void aos_devfs_poll_add(aos_devfs_node_t *node, aos_devfs_wait_queue_t wq, aos_devfs_poll_request_t *req);
void aos_devfs_poll_wakeup(aos_devfs_node_t *node, aos_devfs_wait_queue_t wq);

static inline bool aos_devfs_file_is_readable(aos_devfs_file_t *file)
{
    bool ret;

    switch (aos_devfs_file_get_mode(file) & O_ACCMODE) {
    case O_RDONLY:
    case O_RDWR:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

static inline bool aos_devfs_file_is_writable(aos_devfs_file_t *file)
{
    bool ret;

    switch (aos_devfs_file_get_mode(file) & O_ACCMODE) {
    case O_WRONLY:
    case O_RDWR:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

static inline int64_t aos_devfs_file_lseek_sized(aos_devfs_file_t *file, uint64_t size, int64_t offset, int whence)
{
    int64_t ret;

    if (size > (uint64_t)INT64_MAX)
        return -EINVAL;

    switch (whence) {
    case SEEK_SET:
        if (offset < 0 || (uint64_t)offset > size)
            return -EINVAL;
        ret = offset;
        break;
    case SEEK_CUR:
        {
            uint64_t cur = aos_devfs_file_get_position(file);
            if ((offset > 0 && cur + (uint64_t)offset > size) || (offset < 0 && (uint64_t)(-offset) > cur))
                return -EINVAL;
            ret = (int64_t)cur + offset;
        }
        break;
    case SEEK_END:
        if (offset > 0 || (uint64_t)(-offset) > size)
            return -EINVAL;
        ret = (int64_t)size + offset;
        break;
    default:
        return -EINVAL;
    }

    aos_devfs_file_set_position(file, (uint64_t)ret);

    return ret;
}

#ifdef __cplusplus
}
#endif

#endif /* AOS_DEVFS_H */
