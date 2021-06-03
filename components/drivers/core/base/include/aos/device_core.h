/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_DEVICE_CORE_H
#define AOS_DEVICE_CORE_H

#include <aos/kernel.h>
#include <aos/list.h>
#include <k_rbtree.h>
#include <drivers/u_ld.h>
#ifdef AOS_COMP_VFS
#include <aos/vfs.h>
#endif

typedef enum {
    AOS_DEV_TYPE_MISC           = 0,
    AOS_DEV_TYPE_TTY,
    AOS_DEV_TYPE_GPIOC,
    AOS_DEV_TYPE_GPIO,
} aos_dev_type_t;

struct aos_dev;
struct aos_dev_ref;

typedef struct {
    void (*unregister)(struct aos_dev *);
    aos_status_t (*get)(struct aos_dev_ref *);
    void (*put)(struct aos_dev_ref *);
} aos_dev_ops_t;

#ifdef AOS_COMP_VFS
#define AOS_DEV_NAME_MAX_LEN    63

typedef struct {
    char name[AOS_DEV_NAME_MAX_LEN + 1];
    const struct file_ops *ops;
} aos_dev_vfs_helper_t;
#endif

typedef struct aos_dev {
    aos_dev_type_t type;
    uint32_t id;
    const aos_dev_ops_t *ops;
#ifdef AOS_COMP_VFS
    aos_dev_vfs_helper_t vfs_helper;
#endif
    struct k_rbtree_node_t rb_node;
    aos_sem_t rb_sem;
    aos_mutex_t mutex;
    uint32_t ref_count;
} aos_dev_t;

#define aos_dev_lock(dev) \
    do { (void)aos_mutex_lock(&(dev)->mutex, AOS_WAIT_FOREVER); } while (0)
#define aos_dev_unlock(dev) \
    do { (void)aos_mutex_unlock(&(dev)->mutex); } while (0)

typedef struct aos_dev_ref {
    aos_dev_t *dev;
    void *pdata;
} aos_dev_ref_t;

#define aos_dev_ref_init(ref) \
    do { (ref)->dev = NULL; (ref)->pdata = NULL; } while (0)
#define aos_dev_ref_is_valid(ref)       (!!(ref)->dev)
#define aos_dev_ref_is_first(ref)       ((ref)->dev->ref_count == 0)
#define aos_dev_ref_is_last(ref)        ((ref)->dev->ref_count == 0)

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_dev_register(aos_dev_t *dev);
aos_status_t aos_dev_unregister(aos_dev_type_t type, uint32_t id);
aos_status_t aos_dev_ref(aos_dev_ref_t *ref, aos_dev_t *dev);
aos_status_t aos_dev_get(aos_dev_ref_t *ref, aos_dev_type_t type, uint32_t id);
void aos_dev_put(aos_dev_ref_t *ref);

#ifdef __cplusplus
}
#endif

#endif /* AOS_DEVICE_CORE_H */
