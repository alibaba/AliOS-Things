/*
 * Media device node
 *
 * Copyright (C) 2010 Nokia Corporation
 *
 * Contacts: Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 *	     Sakari Ailus <sakari.ailus@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * --
 *
 * Common functions for media-related drivers to register and unregister media
 * device nodes.
 */

#ifndef _MEDIA_DEVNODE_H
#define _MEDIA_DEVNODE_H

#if 0
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#endif


/*
 * Flag to mark the media_devnode struct as registered. Drivers must not touch
 * this flag directly, it will be set and cleared by media_devnode_register and
 * media_devnode_unregister.
 */
#define MEDIA_FLAG_REGISTERED	0

struct media_file_operations {
	//struct module *owner;
#if 0	
	ssize_t (*read) (char __user *, size_t, loff_t *);
	ssize_t (*write) (const char __user *, size_t, loff_t *);
	unsigned int (*poll) (struct poll_table_struct *);
#endif
	long (*ioctl) (unsigned int, unsigned long);
	long (*compat_ioctl) (unsigned int, unsigned long);
	int (*open) ();
	int (*release) ();
};

/**
 * struct media_devnode - Media device node
 * @parent:	parent device
 * @minor:	device node minor number
 * @flags:	flags, combination of the MEDIA_FLAG_* constants
 *
 * This structure represents a media-related device node.
 *
 * The @parent is a physical device. It must be set by core or device drivers
 * before registering the node.
 */
struct media_devnode {
	/* device ops */
	const struct media_file_operations *fops;

	/* sysfs */
#if 0	
	struct device dev;		/* media device */
	struct cdev cdev;		/* character device */
	struct device *parent;		/* device parent */
#endif
	/* device info */
	int minor;
	unsigned long flags;		/* Use bitops to access flags */

	/* callbacks */
	void (*release)(struct media_devnode *mdev);
};

/* dev to media_devnode */
#define to_media_devnode(minor) container_of(minor, struct media_devnode, dev)

int media_devnode_register(struct media_devnode *mdev);
void media_devnode_unregister(struct media_devnode *mdev);
#if 0
static inline struct media_devnode *media_devnode_data()
{
	return private_data;
}
#endif
static inline int media_devnode_is_registered(struct media_devnode *mdev)
{
	return test_bit(MEDIA_FLAG_REGISTERED, &mdev->flags);
}

#endif /* _MEDIA_DEVNODE_H */
