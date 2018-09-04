/*
 *
 *	V 4 L 2   D R I V E R   H E L P E R   A P I
 *
 * Moved from videodev2.h
 *
 *	Some commonly needed functions for drivers (v4l2-common.o module)
 */
#ifndef _V4L2_DEV_H
#define _V4L2_DEV_H
#if 0
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#endif
#include "v4l2-osdep.h"
#include "uapi_videodev2.h"
#include "media-entity.h"


#define VIDEO_MAJOR	81

#define VFL_TYPE_GRABBER	0
#define VFL_TYPE_VBI		1
#define VFL_TYPE_RADIO		2
#define VFL_TYPE_SUBDEV		3
#define VFL_TYPE_MAX		4

/* Is this a receiver, transmitter or mem-to-mem? */
/* Ignored for VFL_TYPE_SUBDEV. */
#define VFL_DIR_RX		0
#define VFL_DIR_TX		1
#define VFL_DIR_M2M		2

//struct v4l2_ioctl_callbacks; // can't find out from the kernel source code
struct video_device;
struct v4l2_device;
struct v4l2_ctrl_handler;
enum v4l2_priority;
/* Flag to mark the video_device struct as registered.
   Drivers can clear this flag if they want to block all future
   device access. It is cleared by video_unregister_device. */
#define V4L2_FL_REGISTERED	(0)
/* file->private_data points to struct v4l2_fh */
#define V4L2_FL_USES_V4L2_FH	(1)
/* Use the prio field of v4l2_fh for core priority checking */
#define V4L2_FL_USE_FH_PRIO	(2)

/* Priority helper functions */

struct v4l2_prio_state {
	atomic_t prios[4];
};

/* 
enum v4l2_priority { 
    V4L2_PRIORITY_UNSET       = 0,  // 不初始化 
    V4L2_PRIORITY_BACKGROUND  = 1,  // 背景 
    V4L2_PRIORITY_INTERACTIVE = 2,  // 互動 
    V4L2_PRIORITY_RECORD      = 3,  // 紀錄
    V4L2_PRIORITY_DEFAULT     = V4L2_PRIORITY_INTERACTIVE, 
}; 
优先權是為了Multiple Opens操作，當可以支持device同時被多個thread 打開(i.e open())
需要用這優先權去區分哪個操作更重要，使用的那個優先級別，的prios[4]就加一。
*/ 

void v4l2_prio_init(struct v4l2_prio_state *global);
int v4l2_prio_change(struct v4l2_prio_state *global, enum v4l2_priority *local,
		     enum v4l2_priority new);
void v4l2_prio_open(struct v4l2_prio_state *global, enum v4l2_priority *local);
void v4l2_prio_close(struct v4l2_prio_state *global, enum v4l2_priority local);
enum v4l2_priority v4l2_prio_max(struct v4l2_prio_state *global);
int v4l2_prio_check(struct v4l2_prio_state *global, enum v4l2_priority local);


struct v4l2_file_operations {
	   //struct module *owner;
	   //ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	   //ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	   //unsigned int (*poll) (struct file *, struct poll_table_struct *);
	   long (*ioctl) (unsigned int, unsigned long);
	   long (*unlocked_ioctl) (unsigned int, unsigned long);
#ifdef CONFIG_COMPAT
	   long (*compat_ioctl32) (unsigned int, unsigned long);
#endif
	   //unsigned long (*get_unmapped_area) (struct file *, unsigned long,
	   //					   unsigned long, unsigned long, unsigned long);
	   int (*mmap) ();
	   int (*open) ();
	   int (*release) ();
};



/*
 * Newer version of video_device, handled by videodev2.c
 * 	This version moves redundant code from video device code to
 *	the common handler
 */

struct video_device
{
#if defined(CONFIG_MEDIA_CONTROLLER)
	struct media_entity entity;
#endif
	/* device ops */
	const struct v4l2_file_operations *fops;

	/* sysfs */
	//struct device dev;		/* v4l device,this is mainlt for sysfs */
	unsigned long dev; 		/* replace to the reference count */
	void *dev_driver_data;
	const char *init_name; 		/* initial name of the device */
	void (*dev_release)();	/* release call back in origonal dev */
	
	//struct cdev *cdev;		/* character device, which video device register */
	//struct file_operations	*cdev_ops;/* file_operations in origonal cdev */

	struct v4l2_device *v4l2_dev;	/* v4l2_device parent */
	/* Only set parent if that can't be deduced from v4l2_dev */
	//struct device *dev_parent;	/* device parent */

	/* Control handler associated with this device node. May be NULL. 
	 * look up v4l2_ctrls.h for more detail */
	struct v4l2_ctrl_handler *ctrl_handler;

	/* vb2_queue associated with this device node. May be NULL. */
	struct vb2_queue *queue;

	/* Priority state. If NULL, then v4l2_dev->prio will be used. */
	struct v4l2_prio_state *prio;

	/* device info */
	char name[32];
	int vfl_type;	/* device type,usally assign the define VFL_TYPE_XXX value */
	int vfl_dir;	/* receiver, transmitter or m2m,usally assign the define VFL_DIR_XXX value */
	/* 'minor' is set to -1 if the registration failed */
	int minor;
	u16 num;	/* record the registered video device node number */
	/* use bitops to set/clear/test flags,usally assign the define VFL_FL_XXX value */
	unsigned long flags;
	/* attribute to differentiate multiple indices on one physical device */
	int index;

	/* V4L2 file handles */
	spinlock_t		fh_lock; /* Lock for all v4l2_fhs */
	struct list_head	fh_list; /* List of struct v4l2_fh */
	int debug;			/* Activates debug level*/

	/* Video standard vars (v4l2_std_id = u64)*/
	//v4l2_std_id tvnorms;		/* Supported tv norms,usally assign the define VFL_STD_XXX value */
	u64 tvnorms;

	/* callbacks,this must non-NULL,in usually use the video_device_release()to replace of */
	void (*release)(struct video_device *vdev);

	/* ioctl callbacks,look up v4l2-ioctl.h for more detail */
	const struct v4l2_ioctl_ops *ioctl_ops;
	//DECLARE_BITMAP(valid_ioctls, BASE_VIDIOC_PRIVATE); /* unsigned long valid_ioctls[6] */
	unsigned long valid_ioctls[6];
	/* serialization lock */
	//DECLARE_BITMAP(disable_locking, BASE_VIDIOC_PRIVATE); /* unsigned long disable_locking[6] */
	unsigned long disable_locking[6];
	//struct mutex *lock;	/* make serilized ioctl lock possible */
	Mutex	*lock;
};

/* todo need to modify */
#if 0
#define media_entity_to_video_device(__e) \
	container_of(__e, struct video_device, entity)

/* dev to video-device */
#define to_video_device(cd) container_of(cd, struct video_device, dev)
#endif

int __video_register_device(struct video_device *vdev, int type,
		int nr, int warn_if_nr_in_use/*, struct module *owner*/);

/* Register video devices. Note that if video_register_device fails,
   the release() callback of the video_device structure is *not* called, so
   the caller is responsible for freeing any data. Usually that means that
   you call video_device_release() on failure. */
static inline int video_register_device(struct video_device *vdev,
		int type, int nr)
{
	// type = VFL_TYPE_GRABBER, nr = -1, 1, "THIS_MODULE"
	return __video_register_device(vdev, type, nr, 1/*, vdev->fops->owner*/);
}

/* Same as video_register_device, but no warning is issued if the desired
   device node number was already in use. */
static inline int video_register_device_no_warn(
		struct video_device *vdev, int type, int nr)
{
	return __video_register_device(vdev, type, nr, 0/*, vdev->fops->owner*/);
}

/* Unregister video devices. Will do nothing if vdev == NULL or
   video_is_registered() returns false. */
void video_unregister_device(struct video_device *vdev);

/* helper functions to alloc/release struct video_device, the
   latter can also be used for video_device->release(). */
struct video_device *video_device_alloc(void);

/* this release function frees the vdev pointer */
void video_device_release(struct video_device *vdev);

/* this release function does nothing, use when the video_device is a
   static global struct. Note that having a static video_device is
   a dubious construction at best. */
void video_device_release_empty(struct video_device *vdev);

/* returns true if cmd is a known V4L2 ioctl */
bool v4l2_is_known_ioctl(unsigned int cmd);

/* mark that this command shouldn't use core locking */
static inline void v4l2_disable_ioctl_locking(struct video_device *vdev, unsigned int cmd)
{
	#define BASE_VIDIOC_PRIVATE 192
	if (_IOC_NR(cmd) < BASE_VIDIOC_PRIVATE)
		set_bit(_IOC_NR(cmd), vdev->disable_locking);
}

/* Mark that this command isn't implemented. This must be called before
   video_device_register. See also the comments in determine_valid_ioctls().
   This function allows drivers to provide just one v4l2_ioctl_ops struct, but
   disable ioctls based on the specific card that is actually found. */
static inline void v4l2_disable_ioctl(struct video_device *vdev, unsigned int cmd)
{
	#define BASE_VIDIOC_PRIVATE 192
	if (_IOC_NR(cmd) < BASE_VIDIOC_PRIVATE)
		set_bit(_IOC_NR(cmd), vdev->valid_ioctls);
}

/* helper functions to access driver private data. */
static inline void *video_get_drvdata(struct video_device *vdev)
{
	//return dev_get_drvdata(&vdev->dev);
	return (vdev->dev_driver_data);
}

static inline void video_set_drvdata(struct video_device *vdev, void *data)
{
	//dev_set_drvdata(&vdev->dev, data);
	vdev->dev_driver_data = data;
}

struct video_device *video_devdata();

/* Combine video_get_drvdata and video_devdata as this is
   used very often. */
static inline void *video_drvdata()
{
	return video_get_drvdata(video_devdata());
}

static inline const char *video_device_node_name(struct video_device *vdev)
{
	//return dev_name(&vdev->dev);
	return (vdev->init_name);
}

static inline int video_is_registered(struct video_device *vdev)
{
	return test_bit(V4L2_FL_REGISTERED, &vdev->flags);
}

#endif /* _V4L2_DEV_H */
