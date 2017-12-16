/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 *
 * Filename     : device.h
 * Author       : Jun Lin
 * Date Created : 08/12/2015
 * Description  : driver interface header
 */

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "tee_types.h"
#include "tee_list.h"
#include "tee_table.h"

#define DEV_NAME_LEN    8

#define DEV_TREE_MAGIC  0x44657654  /* DevT */

enum status {
    SUCCESS     = 0,
    FAIL        = -1,
    ETIMEOUT    = -2,
    EARG        = -3,
    EBUSY       = -4,
    ESYSIO      = -5,
    EMEM        = -6,
 };

typedef struct _dev_t {
    uint32_t magic;
    tee_list_t node;
    int8_t name[DEV_NAME_LEN];
    struct _dev_ops_t *ops;
    uint32_t dev_ref;           /* device open reference */
#ifdef MUTLI_THREAD
    tee_osa_sem_t lock;               /* for dev_ref */
#endif
    void *priv;                 /* private, could be set in device open */
} dev_t;

typedef struct _dev_ops_t {
    int32_t (*open)(dev_t *dev);
    int32_t (*read)(dev_t *dev, int32_t pos, void *buf, int32_t size);
    int32_t (*write)(dev_t *dev, int32_t pos, void *buf, int32_t size);
    int32_t (*ioctl)(dev_t *dev, int32_t cmd, void *arg);
    int32_t (*suspend)(dev_t *dev, int32_t state);
    int32_t (*resume)(dev_t *dev);
    int32_t (*close)(dev_t *dev);
} dev_ops_t;

typedef struct _dev_tree_t {
    uint32_t magic;
    uint8_t  *name;
    int32_t (*probe)(void);
} dev_tree_t;

#define DEV_INIT(func, name_str)   \
    dev_tree_t _##func##_ TABLE_ATTR(devtree) = { DEV_TREE_MAGIC,    \
                                                  name_str,  \
                                                  func }

extern void device_init(void); /* should be call only once in bs */
extern dev_t *dev_register(int8_t *name, dev_ops_t *ops);
extern int32_t dev_unregister(dev_t *dev);
extern void *dev_open(int8_t *name);
extern int32_t dev_close(dev_t *dev);
extern int32_t dev_ioctl(dev_t *dev, int32_t cmd, void *arg);
extern int32_t dev_read(dev_t *dev, int32_t pos, void *buf, int32_t size);
extern int32_t dev_write(dev_t *dev, int32_t pos, void *buf, int32_t size);
extern int32_t dev_suspend(int32_t state);
extern int32_t dev_resume(void);

#endif /*_DEVICE_H_ */
