/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <aos/list.h>
#include "control_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

struct audio_kcontrol;
struct audio_mixer_control;

#define	AOS_CTL_ELEM_TYPE_NONE	        (1 << 0) /* invalid */
#define	AOS_CTL_ELEM_TYPE_BOOLEAN	    (1 << 1) /* boolean type */
#define	AOS_CTL_ELEM_TYPE_INTEGER	    (1 << 2) /* integer type */
#define	AOS_CTL_ELEM_TYPE_ENUMERATED	(1 << 3) /* enumerated type */
#define	AOS_CTL_ELEM_TYPE_BYTES	        (1 << 4) /* byte array */
#define	AOS_CTL_ELEM_TYPE_IEC958	    (1 << 5) /* IEC958 (S/PDIF) setup */
#define	AOS_CTL_ELEM_TYPE_INTEGER64	    (1 << 6) /* 64-bit integer type */
#define	AOS_CTL_ELEM_TYPE_LAST	        AOS_CTL_ELEM_TYPE_INTEGER64

#define	AOS_CTL_ELEM_IFACE_MIXER        (1 << 2) /* virtual mixer device */
#define	AOS_CTL_ELEM_IFACE_LAST	        AOS_CTL_ELEM_IFACE_MIXER

#define AOS_CTL_ELEM_ACCESS_READ		    (1<<0)
#define AOS_CTL_ELEM_ACCESS_WRITE		    (1<<1)
#define AOS_CTL_ELEM_ACCESS_READWRITE		(AOS_CTL_ELEM_ACCESS_READ|AOS_CTL_ELEM_ACCESS_WRITE)

#define AOS_DOUBLE_VALUE(sreg, left, right, smax, sinvert, sautodisable) \
	((unsigned long)&(struct audio_mixer_control) \
	{.reg = sreg, .rreg = sreg, .shift = left, \
	.rshift = right, .max = smax, .platform_max = smax, \
	.invert = sinvert, .autodisable = sautodisable})

#define AOS_SINGLE_VALUE(reg, shift, max, invert, autodisable) \
	    AOS_DOUBLE_VALUE(reg, shift, shift, max, invert, autodisable)

#define SOC_SINGLE_EXT(sname, reg, shift, max, invert, handler_get, handler_put) \
{	.iface = AOS_CTL_ELEM_IFACE_MIXER, \
	.name = sname, \
	.info = get_integer_info, \
	.get = handler_get, \
	.put = handler_put, \
	.private_value = AOS_SINGLE_VALUE(reg, shift, max, invert, 0) \
}

#define SOC_DOUBLE_EXT(sname, reg, left, right, max, invert, handler_get, handler_put) \
{	.iface = AOS_CTL_ELEM_IFACE_MIXER, \
	.name = (sname),\
	.info = get_integer_info, \
	.get = handler_get, \
	.put = handler_put, \
	.private_value = AOS_DOUBLE_VALUE(reg, left, right, max, invert, 0) \
}

struct audio_ctl_card_info {
	int card;			              /* card number */
	unsigned char driverName[16];	  /* Driver name */
	unsigned char shortName[32];      /* Short name of card */
	unsigned char longName[64];	      /* long name about card */
	unsigned char mixerName[64];	  /* visual mixer name */
	unsigned char components[64];     /* card components info */
};

struct audio_ctl_elem_id {
	unsigned int id;		          /* element id, zero = invalid */
	int iface;	                      /* interface id */
	unsigned int deviceId;		      /* device number */
	unsigned int subdeviceId;		  /* subdevice (substream) number */
	char name[64];		              /* name of item */
	unsigned int index;		          /* index of item */
};

struct audio_ctl_elem_list {
	struct audio_ctl_elem_id  *pids;  /* R: IDs */
	unsigned int offset;		      /* W: first element offset */
	unsigned int space;		          /* W: element count to get */
	unsigned int used;		          /* R: element count been set */
	unsigned int count;		          /* R: total elements count */
};

struct audio_ctl_elem_info {
	struct audio_ctl_elem_id id;	  /* W: element ID */
	int type;	                      /* R: AOS_CTL_ELEM_TYPE_* */
	unsigned int access;		      /* R: AOS_CTL_ELEM_ACCESS_* */
	unsigned int count;		          /* count of values */
	union {
		struct {
			long min;		          /* R: min value */
			long max;		          /* R: max value */
			long step;		          /* R: step */
		} integer;
		struct {
			long long min;		      /* R: min value */
			long long max;		      /* R: max value */
			long long step;		      /* R: step */
		} integer64;
	} value;
};

struct audio_ctl_elem_value {
	struct audio_ctl_elem_id id;	  /* W: element ID */
	union {
		union {
			long value[64];
		} integer;
		union {
			long long value[64];
		} integer64;
		union {
			unsigned char data[512];
		} bytes;
	} value;		                  /* RO */
};

struct audio_ctl_tlv {
	unsigned int numid;	              /* control element numeric identification */
	unsigned int len;	              /* in bytes aligned to 4 */
	unsigned int tlv[0];	          /* first TLV */
};

/* mixer control */
typedef int (audio_kcontrol_info_t) (struct audio_kcontrol * kcontrol, struct audio_ctl_elem_info * uinfo);
typedef int (audio_kcontrol_get_t) (struct audio_kcontrol * kcontrol, struct audio_ctl_elem_value * ucontrol);
typedef int (audio_kcontrol_put_t) (struct audio_kcontrol * kcontrol, struct audio_ctl_elem_value * ucontrol);
typedef int (audio_kcontrol_tlv_rw_t)(struct audio_kcontrol *kcontrol, int op_flag, unsigned int size, unsigned int *tlv);


struct audio_mixer_control {
	int min, max, platform_max;
	int reg, rreg;
	unsigned int shift, rshift;
	unsigned int sign_bit;
	unsigned int invert:1;
	unsigned int autodisable:1;
};

struct audio_kcontrol_volatile {
	void *owner;	              /* locked */
	unsigned int access;	      /* access rights */
};

struct audio_kcontrol_new {
	unsigned int iface;	          /* interface identifier */
	const char *name;	          /* ASCII name of item */
	audio_kcontrol_info_t *info;
	audio_kcontrol_get_t *get;
	audio_kcontrol_put_t *put;
	union {
		audio_kcontrol_tlv_rw_t *c;
		const unsigned int *p;
	} tlv;
	unsigned long private_value;

	unsigned int deviceId;		  /* device ID*/
	unsigned int subdeviceId;	  /* subdevice ID */
	unsigned int index;		      /* index of item */
	unsigned int access;		  /* access rights */
	unsigned int count;		      /* count of same elements */
};

struct audio_kcontrol {
	struct dlist_s list;		      /* list of controls */
	struct audio_ctl_elem_id id;
	unsigned int count;		          /* count of same elements */
	audio_kcontrol_info_t *info;
	audio_kcontrol_get_t *get;
	audio_kcontrol_put_t *put;
	union {
		audio_kcontrol_tlv_rw_t *c;
		const unsigned int *p;
	} tlv;
	unsigned long private_value;
	void *private_data;
	void (*private_free)(struct audio_kcontrol *kcontrol);
	struct audio_kcontrol_volatile vd[0];	/* volatile data */
};

enum {
	AOS_CTL_TLV_OP_READ = 0,
	AOS_CTL_TLV_OP_WRITE = 1,
	AOS_CTL_TLV_OP_CMD = -1,
};

int get_integer_info(struct audio_kcontrol * kcontrol, struct audio_ctl_elem_info * uinfo);
int audio_ctl_card_info(ctrl_device_t *dev, struct audio_ctl_card_info *info);
int audio_ctl_elem_list(ctrl_device_t *dev, struct audio_ctl_elem_list *list);
int audio_ctl_elem_info(ctrl_device_t *dev, struct audio_ctl_elem_info *info);
int audio_ctl_elem_read(ctrl_device_t *dev, struct audio_ctl_elem_value *value);
int audio_ctl_elem_write(ctrl_device_t *dev, struct audio_ctl_elem_value *value);
int audio_ctl_tlv_ioctl(ctrl_device_t *dev, struct audio_ctl_tlv *tlv, int op_flag);
int audio_add_controls(ctrl_device_t *dev, const struct audio_kcontrol_new *controls, int num_controls, void *data);

#ifdef __cplusplus
}
#endif

#endif /* CONTROL_H */