/**
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _CHAR_DEV_H_
#define _CHAR_DEV_H_

#include "aos/list.h"

#define CDEV_LOG_DEBUG 0
#define CDEV_LOG_INFO  1
#define CDEV_LOG_WARN  2
#define CDEV_LOG_ERROR 3

extern int g_cdev_log_level;

static char cdev_level_tag[] = {
	'D',
	'I',
	'W',
	'E'
};

#define cdev_log(level, format, ...) do { \
	if (level >= g_cdev_log_level) \
		printf("<%c> [%s:%u]" format, cdev_level_tag[level], __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while (0)

#define cdev_loud(format, ...) cdev_log(CDEV_LOG_DEBUG, format, ##__VA_ARGS__)
#define cdev_dbg(format, ...) cdev_log(CDEV_LOG_DEBUG, format, ##__VA_ARGS__)
#define cdev_info(format, ...) cdev_log(CDEV_LOG_INFO, format, ##__VA_ARGS__)
#define cdev_warn(format, ...) cdev_log(CDEV_LOG_WARN, format, ##__VA_ARGS__)
#define cdev_err(format, ...) cdev_log(CDEV_LOG_ERROR, format, ##__VA_ARGS__)

#ifndef MINORBITS

#define MINORBITS	20
#define MINORMASK	((1U << MINORBITS) - 1)

#define MAJOR(dev)	((unsigned int) ((dev) >> MINORBITS))

#define MINOR(dev)	((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma, mi)	(((ma) << MINORBITS) | (mi))
#endif //MINORBITS

typedef struct u_cdev {
	int dev_id;
	unsigned int count;
	dlist_t node;
	const void *ops;
	void *thread_ctx;
} u_cdev_t;

int u_cdev_module_init(void);
int u_cdev_init(u_cdev_t *dev, /*struct u_file_operations*/void *ops);
int u_cdev_add(u_cdev_t *dev, unsigned int devid, unsigned int count);
int u_cdev_del(u_cdev_t *dev);
int alloc_u_cdev_region(unsigned int *dev_id, unsigned int minor, unsigned int count);
int register_u_cdev_region(unsigned dev_id, unsigned count);
int __register_u_cdev_region(unsigned int major,
                             unsigned int minor,
                             int count);
int unregister_u_cdev_region(unsigned dev_id, unsigned count);
u_cdev_t * u_cdev_find_node_by_name(char *pathname);
u_cdev_t * u_cdev_find_node_by_devid(unsigned int devid);

#endif //_CHAR_DEV_H_
