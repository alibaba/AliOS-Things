/*
 *
 *	V 4 L 2   D R I V E R   H E L P E R   A P I
 *
 * Moved from videodev2.h
 *
 *	Some commonly needed functions for drivers (v4l2-common.o module)
 */
#ifndef _V4L2_IOCTL_H
#define _V4L2_IOCTL_H

#if 0
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/compiler.h> /* need __user */
#endif
#include "v4l2-osdep.h"
#include "videodev2.h"

struct v4l2_fh;

struct v4l2_ioctl_ops {
	/* ioctl callbacks */

	/* VIDIOC_QUERYCAP handler */
	int (*vidioc_querycap)(void *fh, struct v4l2_capability *cap);

	/* Priority handling */
	int (*vidioc_g_priority)   (void *fh,
				    enum v4l2_priority *p);
	int (*vidioc_s_priority)   (void *fh,
				    enum v4l2_priority p);

	/* VIDIOC_ENUM_FMT handlers */
	int (*vidioc_enum_fmt_vid_cap)     (void *fh,
					    struct v4l2_fmtdesc *f);
	int (*vidioc_enum_fmt_vid_overlay) (void *fh,
					    struct v4l2_fmtdesc *f);
	int (*vidioc_enum_fmt_vid_out)     (void *fh,
					    struct v4l2_fmtdesc *f);
	int (*vidioc_enum_fmt_vid_cap_mplane)(void *fh,
					      struct v4l2_fmtdesc *f);
	int (*vidioc_enum_fmt_vid_out_mplane)(void *fh,
					      struct v4l2_fmtdesc *f);

	/* VIDIOC_G_FMT handlers */
	int (*vidioc_g_fmt_vid_cap)    (void *fh,
					struct v4l2_format *f);
	int (*vidioc_g_fmt_vid_overlay)(void *fh,
					struct v4l2_format *f);
	int (*vidioc_g_fmt_vid_out)    (void *fh,
					struct v4l2_format *f);
	int (*vidioc_g_fmt_vid_out_overlay)(void *fh,
					struct v4l2_format *f);
	int (*vidioc_g_fmt_vbi_cap)    (void *fh,
					struct v4l2_format *f);
	int (*vidioc_g_fmt_vbi_out)    (void *fh,
					struct v4l2_format *f);
	int (*vidioc_g_fmt_sliced_vbi_cap)(void *fh,
					struct v4l2_format *f);
	int (*vidioc_g_fmt_sliced_vbi_out)(void *fh,
					struct v4l2_format *f);
	int (*vidioc_g_fmt_vid_cap_mplane)(void *fh,
					   struct v4l2_format *f);
	int (*vidioc_g_fmt_vid_out_mplane)(void *fh,
					   struct v4l2_format *f);

	/* VIDIOC_S_FMT handlers */
	int (*vidioc_s_fmt_vid_cap)    (void *fh,
					struct v4l2_format *f);
	int (*vidioc_s_fmt_vid_overlay)(void *fh,
					struct v4l2_format *f);
	int (*vidioc_s_fmt_vid_out)    (void *fh,
					struct v4l2_format *f);
	int (*vidioc_s_fmt_vid_out_overlay)(void *fh,
					struct v4l2_format *f);
	int (*vidioc_s_fmt_vbi_cap)    (void *fh,
					struct v4l2_format *f);
	int (*vidioc_s_fmt_vbi_out)    (void *fh,
					struct v4l2_format *f);
	int (*vidioc_s_fmt_sliced_vbi_cap)(void *fh,
					struct v4l2_format *f);
	int (*vidioc_s_fmt_sliced_vbi_out)(void *fh,
					struct v4l2_format *f);
	int (*vidioc_s_fmt_vid_cap_mplane)(void *fh,
					   struct v4l2_format *f);
	int (*vidioc_s_fmt_vid_out_mplane)(void *fh,
					   struct v4l2_format *f);

	/* VIDIOC_TRY_FMT handlers */
	int (*vidioc_try_fmt_vid_cap)    (void *fh,
					  struct v4l2_format *f);
	int (*vidioc_try_fmt_vid_overlay)(void *fh,
					  struct v4l2_format *f);
	int (*vidioc_try_fmt_vid_out)    (void *fh,
					  struct v4l2_format *f);
	int (*vidioc_try_fmt_vid_out_overlay)(void *fh,
					  struct v4l2_format *f);
	int (*vidioc_try_fmt_vbi_cap)    (void *fh,
					  struct v4l2_format *f);
	int (*vidioc_try_fmt_vbi_out)    (void *fh,
					  struct v4l2_format *f);
	int (*vidioc_try_fmt_sliced_vbi_cap)(void *fh,
					  struct v4l2_format *f);
	int (*vidioc_try_fmt_sliced_vbi_out)(void *fh,
					  struct v4l2_format *f);
	int (*vidioc_try_fmt_vid_cap_mplane)(void *fh,
					     struct v4l2_format *f);
	int (*vidioc_try_fmt_vid_out_mplane)(void *fh,
					     struct v4l2_format *f);

	/* Buffer handlers */
	int (*vidioc_reqbufs) (void *fh, struct v4l2_requestbuffers *b);
	int (*vidioc_querybuf)(void *fh, struct v4l2_buffer *b);
	int (*vidioc_qbuf)    (void *fh, struct v4l2_buffer *b);
	int (*vidioc_expbuf)  (void *fh,
				struct v4l2_exportbuffer *e);
	int (*vidioc_dqbuf)   (void *fh, struct v4l2_buffer *b);

	int (*vidioc_create_bufs)(void *fh, struct v4l2_create_buffers *b);
	int (*vidioc_prepare_buf)(void *fh, struct v4l2_buffer *b);

	int (*vidioc_overlay) (void *fh, unsigned int i);
	int (*vidioc_g_fbuf)   (void *fh,
				struct v4l2_framebuffer *a);
	int (*vidioc_s_fbuf)   (void *fh,
				const struct v4l2_framebuffer *a);

	/* Stream on/off */
	int (*vidioc_streamon) (void *fh, enum v4l2_buf_type i);
	int (*vidioc_streamoff)(void *fh, enum v4l2_buf_type i);

	/* Standard handling
		ENUMSTD is handled by videodev.c
	 */
	int (*vidioc_g_std) (void *fh, v4l2_std_id *norm);
	int (*vidioc_s_std) (void *fh, v4l2_std_id norm);
	int (*vidioc_querystd) (void *fh, v4l2_std_id *a);

	/* Input handling */
	int (*vidioc_enum_input)(void *fh,
				 struct v4l2_input *inp);
	int (*vidioc_g_input)   (void *fh, unsigned int *i);
	int (*vidioc_s_input)   (void *fh, unsigned int i);

	/* Output handling */
	int (*vidioc_enum_output) (void *fh,
				  struct v4l2_output *a);
	int (*vidioc_g_output)   (void *fh, unsigned int *i);
	int (*vidioc_s_output)   (void *fh, unsigned int i);

	/* Control handling */
	int (*vidioc_queryctrl)        (void *fh,
					struct v4l2_queryctrl *a);
	int (*vidioc_g_ctrl)           (void *fh,
					struct v4l2_control *a);
	int (*vidioc_s_ctrl)           (void *fh,
					struct v4l2_control *a);
	int (*vidioc_g_ext_ctrls)      (void *fh,
					struct v4l2_ext_controls *a);
	int (*vidioc_s_ext_ctrls)      (void *fh,
					struct v4l2_ext_controls *a);
	int (*vidioc_try_ext_ctrls)    (void *fh,
					struct v4l2_ext_controls *a);
	int (*vidioc_querymenu)        (void *fh,
					struct v4l2_querymenu *a);

	/* Audio ioctls */
	int (*vidioc_enumaudio)        (void *fh,
					struct v4l2_audio *a);
	int (*vidioc_g_audio)          (void *fh,
					struct v4l2_audio *a);
	int (*vidioc_s_audio)          (void *fh,
					const struct v4l2_audio *a);

	/* Audio out ioctls */
	int (*vidioc_enumaudout)       (void *fh,
					struct v4l2_audioout *a);
	int (*vidioc_g_audout)         (void *fh,
					struct v4l2_audioout *a);
	int (*vidioc_s_audout)         (void *fh,
					const struct v4l2_audioout *a);
	int (*vidioc_g_modulator)      (void *fh,
					struct v4l2_modulator *a);
	int (*vidioc_s_modulator)      (void *fh,
					const struct v4l2_modulator *a);
	/* Crop ioctls */
	int (*vidioc_cropcap)          (void *fh,
					struct v4l2_cropcap *a);
	int (*vidioc_g_crop)           (void *fh,
					struct v4l2_crop *a);
	int (*vidioc_s_crop)           (void *fh,
					const struct v4l2_crop *a);
	int (*vidioc_g_selection)      (void *fh,
					struct v4l2_selection *s);
	int (*vidioc_s_selection)      (void *fh,
					struct v4l2_selection *s);
	/* Compression ioctls */
	int (*vidioc_g_jpegcomp)       (void *fh,
					struct v4l2_jpegcompression *a);
	int (*vidioc_s_jpegcomp)       (void *fh,
					const struct v4l2_jpegcompression *a);
	int (*vidioc_g_enc_index)      ( void *fh,
					struct v4l2_enc_idx *a);
	int (*vidioc_encoder_cmd)      (void *fh,
					struct v4l2_encoder_cmd *a);
	int (*vidioc_try_encoder_cmd)  (void *fh,
					struct v4l2_encoder_cmd *a);
	int (*vidioc_decoder_cmd)      (void *fh,
					struct v4l2_decoder_cmd *a);
	int (*vidioc_try_decoder_cmd)  (void *fh,
					struct v4l2_decoder_cmd *a);

	/* Stream type-dependent parameter ioctls */
	int (*vidioc_g_parm)           (void *fh,
					struct v4l2_streamparm *a);
	int (*vidioc_s_parm)           (void *fh,
					struct v4l2_streamparm *a);

	/* Tuner ioctls */
	int (*vidioc_g_tuner)          (void *fh,
					struct v4l2_tuner *a);
	int (*vidioc_s_tuner)          (void *fh,
					const struct v4l2_tuner *a);
	int (*vidioc_g_frequency)      (void *fh,
					struct v4l2_frequency *a);
	int (*vidioc_s_frequency)      (void *fh,
					const struct v4l2_frequency *a);
	int (*vidioc_enum_freq_bands) (void *fh,
				    struct v4l2_frequency_band *band);

	/* Sliced VBI cap */
	int (*vidioc_g_sliced_vbi_cap) (void *fh,
					struct v4l2_sliced_vbi_cap *a);

	/* Log status ioctl */
	int (*vidioc_log_status)       ( void *fh);

	int (*vidioc_s_hw_freq_seek)   (void *fh,
					const struct v4l2_hw_freq_seek *a);

	/* Debugging ioctls */
#ifdef CONFIG_VIDEO_ADV_DEBUG
	int (*vidioc_g_register)       (void *fh,
					struct v4l2_dbg_register *reg);
	int (*vidioc_s_register)       (void *fh,
					const struct v4l2_dbg_register *reg);

	int (*vidioc_g_chip_info)      (void *fh,
					struct v4l2_dbg_chip_info *chip);
#endif

	int (*vidioc_enum_framesizes)   (void *fh,
					 struct v4l2_frmsizeenum *fsize);

	int (*vidioc_enum_frameintervals) (void *fh,
					   struct v4l2_frmivalenum *fival);

	/* DV Timings IOCTLs */
	int (*vidioc_s_dv_timings) (void *fh,
				    struct v4l2_dv_timings *timings);
	int (*vidioc_g_dv_timings) (void *fh,
				    struct v4l2_dv_timings *timings);
	int (*vidioc_query_dv_timings) ( void *fh,
				    struct v4l2_dv_timings *timings);
	int (*vidioc_enum_dv_timings) (void *fh,
				    struct v4l2_enum_dv_timings *timings);
	int (*vidioc_dv_timings_cap) (void *fh,
				    struct v4l2_dv_timings_cap *cap);

	int (*vidioc_subscribe_event)  (struct v4l2_fh *fh,
					const struct v4l2_event_subscription *sub);
	int (*vidioc_unsubscribe_event)(struct v4l2_fh *fh,
					const struct v4l2_event_subscription *sub);

	/* For other private ioctls */
	long (*vidioc_default)	       (void *fh,
					bool valid_prio, unsigned int cmd, void *arg);
};


/* v4l debugging and diagnostics */

/* Debug bitmask flags to be used on V4L2 */
#define V4L2_DEBUG_IOCTL     0x01
#define V4L2_DEBUG_IOCTL_ARG 0x02

/*  Video standard functions  */
extern const char *v4l2_norm_to_name(v4l2_std_id id);
extern void v4l2_video_std_frame_period(int id, struct v4l2_fract *frameperiod);
extern int v4l2_video_std_construct(struct v4l2_standard *vs,
				    int id, const char *name);
/* Prints the ioctl in a human-readable format. If prefix != NULL,
   then do printk(KERN_DEBUG "%s: ", prefix) first. */
extern void v4l_printk_ioctl(const char *prefix, unsigned int cmd);

/* Internal use only: get the mutex (if any) that we need to lock for the
   given command. */
struct video_device;
extern Mutex *v4l2_ioctl_get_lock(struct video_device *vdev, unsigned cmd);

/* names for fancy debug output */
extern const char *v4l2_field_names[];
extern const char *v4l2_type_names[];

#ifdef CONFIG_COMPAT
/* 32 Bits compatibility layer for 64 bits processors */
extern long v4l2_compat_ioctl32(unsigned int cmd, unsigned long arg);
#endif

typedef long (*v4l2_kioctl)(unsigned int cmd, void *arg);

/* Include support for obsoleted stuff */
extern long video_usercopy(unsigned int cmd, unsigned long arg, v4l2_kioctl func);

/* Standard handlers for V4L ioctl's */
extern long video_ioctl2(unsigned int cmd, unsigned long arg);

#endif /* _V4L2_IOCTL_H */
