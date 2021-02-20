/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SOUND_PCM_H
#define SOUND_PCM_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <aos/list.h>
#include <aos/kernel.h>
#ifndef HAAS_AUDIO_DEMO
#include <sys/ioctl.h>
#endif
#include "audio_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long aos_pcm_uframes_t;
typedef signed long aos_pcm_sframes_t;

#define AOS_PCM_NONBLOCK            0x00000001       /** Non blocking mode (flag for open mode) */
#define AOS_PCM_ASYNC               0x00000002       /** Async notification (flag for open mode) */
#define AOS_PCM_ABORT               0x00008000       /** In an abort state (internal, not allowed for open) */
#define AOS_PCM_NO_AUTO_RESAMPLE    0x00010000       /** Disable automatic (but not forced!) rate resamplinig */
#define AOS_PCM_NO_AUTO_CHANNELS    0x00020000       /** Disable automatic (but not forced!) channel conversion */
#define AOS_PCM_NO_AUTO_FORMAT      0x00040000       /** Disable automatic (but not forced!) format conversion */
#define AOS_PCM_NO_SOFTVOL          0x00080000       /** Disable soft volume control */

#define AOS_PCM_EVT_WRITE           (1 << 0)         /** playback resource available event */
#define AOS_PCM_EVT_READ            (1 << 1)         /** capture data available event */
#define AOS_PCM_EVT_XRUN            (1 << 2)         /** underrun (playback) or overrun (capture) detected */

typedef enum {
	/** Playback stream */
	AOS_PCM_STREAM_PLAYBACK = 0,
	/** Capture stream */
	AOS_PCM_STREAM_CAPTURE,
	AOS_PCM_STREAM_LAST = AOS_PCM_STREAM_CAPTURE
} aos_pcm_stream_t;

typedef enum {
	/** IDLE */
	AOS_PCM_STATE_IDLE = 0,
	/** Open */
	AOS_PCM_STATE_OPEN,
	/** Ready to start */
	AOS_PCM_STATE_PREPARED,
	/** Running */
	AOS_PCM_STATE_RUNNING,
	/** Stopped: underrun (playback) or overrun (capture) detected */
	AOS_PCM_STATE_XRUN,
	/** Draining: running (playback) or stopped (capture) */
	AOS_PCM_STATE_DRAINING,
	/** Paused */
	AOS_PCM_STATE_PAUSED,
	/** Hardware is suspended */
	AOS_PCM_STATE_SUSPENDED,
	/** Hardware is disconnected */
	AOS_PCM_STATE_DISCONNECTED,
	AOS_PCM_STATE_LAST = AOS_PCM_STATE_DISCONNECTED,
	/** Private - used internally in the library - do not use*/
	AOS_PCM_STATE_PRIVATE1 = 1024
} aos_pcm_state_t;

typedef enum {
	/** Signed 8-bit */
	AOSRV_PCM_FORMAT_S8 = 1,
	/** Signed 16-bit, little endian */
	AOSRV_PCM_FORMAT_S16_LE = 2,
	/** Signed 24-bit, little endian */
	AOSRV_PCM_FORMAT_S24_LE = 3,
	/** Signed 32-bit, little endian */
	AOSRV_PCM_FORMAT_S32_LE = 4,
	AOSRV_PCM_FORMAT_ALL
} aos_pcm_format_t;

typedef enum {
	/** MMAP RW interleaved access */
	AOS_PCM_ACCESS_MMAP_INTERLEAVED = 0,
	/** MMAP RW non-interleaved access */
	AOS_PCM_ACCESS_MMAP_NONINTERLEAVED,
	/** RW interleaved access, e.g. writei/readi */
	AOS_PCM_ACCESS_RW_INTERLEAVED,
	/** RW non-interleaved access, e.g. writen/readn */
	AOS_PCM_ACCESS_RW_NONINTERLEAVED
} aos_pcm_access_t;

typedef enum {
	/** Kernel level PCM */
	AOS_PCM_TYPE_HW = 0,
	/** One or more linked PCM with exclusive access to selected channels */
	AOS_PCM_TYPE_MULTI,
	/** IMA-ADPCM format conversion PCM */
	AOS_PCM_TYPE_ADPCM,
	/** Rate conversion PCM */
	AOS_PCM_TYPE_RATE,
	/** Mixing PCM */
	AOS_PCM_TYPE_MIX,
	/** Direct Mixing plugin */
	AOS_PCM_TYPE_DMIX,
	/** Jack Audio Connection Kit plugin */
	AOS_PCM_TYPE_JACK,
	/** Soft volume plugin */
	AOS_PCM_TYPE_SOFTVOL,
	AOS_PCM_TYPE_LAST = AOS_PCM_TYPE_SOFTVOL
} aos_pcm_type_t;

typedef struct {
	aos_pcm_access_t access;
    aos_pcm_format_t format;
    unsigned int sample_bits;
    unsigned int frame_bits;
    unsigned int channels;
    unsigned int rate;
    unsigned int period_time; // TBD
    unsigned int period_size;
    unsigned int period_bytes;
    unsigned int periods;
    unsigned int buffer_time;
    unsigned int buffer_size;
    unsigned int buffer_bytes;
    unsigned int tick_time;
} aos_pcm_hw_params_t;

typedef struct {
	int tstamp_mode;			         /* timestamp mode */
	unsigned int period_step;
	unsigned int sleep_min;			     /* min ticks to sleep */
	aos_pcm_uframes_t avail_min;		 /* min avail frames for wakeup */
	aos_pcm_uframes_t xfer_align;		 /* obsolete: xfer size need to be a multiple */
	aos_pcm_uframes_t start_threshold;	 /* min hw_avail frames for automatic start */
	aos_pcm_uframes_t stop_threshold;	 /* min avail frames for automatic stop */
	aos_pcm_uframes_t silence_threshold; /* min distance from noise for silence filling */
	aos_pcm_uframes_t silence_size;		 /* silence block size */
	aos_pcm_uframes_t boundary;		     /* pointers wrap point */
	unsigned int proto;			         /* protocol version */
	unsigned int tstamp_type;		     /* timestamp type (req. proto >= 2.0.12) */
	unsigned char reserved[56];		     /* reserved for future */
} aos_pcm_sw_params_t;

typedef struct {
	/* mandatory */
	int fd;
	aos_pcm_type_t type;
	aos_pcm_stream_t stream;
	aos_pcm_state_t state;
	char *name;
	int mode;
	int card;
	int device;
	aos_hdl_t mutex;
    aos_hdl_t evt;
	aos_pcm_hw_params_t *hw_params;
    aos_pcm_sw_params_t *sw_params;

	/* options */
    void *open_func;
	long minperiodtime;
	int poll_fd_count;
	unsigned short poll_events;
	int setup: 1,
	    compat: 1;
	unsigned int mmap_rw: 1;	    /* use always mmapped buffer */
	unsigned int mmap_shadow: 1;	/* don't call actual mmap, use the mmaped buffer of the slave */
	unsigned int donot_close: 1;	/* don't close this PCM */
	unsigned int own_state_check:1; /* plugin has own PCM state check */
	void *private_data;
} aos_pcm_t;

typedef struct {
	dlist_t list;
	unsigned int count;
	unsigned int allocated;
	const char *siface;
	int card;
	int device;
	long device_input;
	long device_output;
	int stream;
	int show_all;
	char name[100];
} hint_list_t;

int aos_device_name_hint(int card, const char *iface, void *hints);
int aos_pcm_open(aos_pcm_t **pcm, const char *name, aos_pcm_stream_t stream, int mode);
int aos_pcm_prepare(aos_pcm_t *pcm);
int aos_pcm_start(aos_pcm_t *pcm);
int aos_pcm_wait(aos_pcm_t *pcm, int timeout);
int aos_pcm_stop(aos_pcm_t *pcm);
int aos_pcm_drain(aos_pcm_t *pcm);
int aos_pcm_pause(aos_pcm_t *pcm, int enable);
int aos_pcm_close(aos_pcm_t *pcm);
int aos_pcm_recover(aos_pcm_t *pcm);

int aos_pcm_hw_params_alloca(aos_pcm_hw_params_t **p);
int aos_pcm_hw_params_any(aos_pcm_hw_params_t *params);
int aos_pcm_hw_params(aos_pcm_t *pcm, aos_pcm_hw_params_t *p);
int aos_pcm_set_params(aos_pcm_t *pcm, aos_pcm_format_t format, aos_pcm_access_t access, unsigned int channels,
                       unsigned int rate, int soft_resample, unsigned int latency);

int aos_pcm_sw_params_alloca(aos_pcm_sw_params_t **p);
int aos_pcm_sw_params_any(aos_pcm_sw_params_t *params);
int aos_pcm_sw_params(aos_pcm_t *pcm, aos_pcm_sw_params_t *params);

aos_pcm_sframes_t aos_pcm_writei(aos_pcm_t *pcm, const void *buffer, aos_pcm_uframes_t size);
aos_pcm_sframes_t aos_pcm_readi(aos_pcm_t *pcm, void *buffer, aos_pcm_uframes_t size);
aos_pcm_sframes_t aos_pcm_writen(aos_pcm_t *pcm, void **bufs, aos_pcm_uframes_t size);
aos_pcm_sframes_t aos_pcm_readn(aos_pcm_t *pcm, void **bufs, aos_pcm_uframes_t size);

/* API for enter/exit low power mode */
int aos_pcm_suspend(aos_pcm_t *pcm);
int aos_pcm_resume(aos_pcm_t *pcm);

aos_pcm_sframes_t aos_pcm_bytes_to_frames(aos_pcm_t *pcm, int bytes);
int aos_pcm_frames_to_bytes(aos_pcm_t *pcm, aos_pcm_sframes_t frames);

#ifdef __cplusplus
}
#endif
#endif /* SOUND_PCM_H */

