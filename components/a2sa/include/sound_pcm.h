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

/** @defgroup a2sa_pcm_api pcm
 *  @ingroup a2sa_api
 * @{
 */

typedef unsigned long aos_pcm_uframes_t;
typedef signed long aos_pcm_sframes_t;

/* AOS_PCM常用宏定义 */
#define AOS_PCM_BLOCK               0x00000000       /**< Blocking mode (flag for open mode) */
#define AOS_PCM_NONBLOCK            0x00000001       /**< Non blocking mode (flag for open mode) */
#define AOS_PCM_ASYNC               0x00000002       /**< Async notification (flag for open mode) */
#define AOS_PCM_ABORT               0x00008000       /**< In an abort state (internal, not allowed for open) */
#define AOS_PCM_NO_AUTO_RESAMPLE    0x00010000       /**< Disable automatic (but not forced!) rate resamplinig */
#define AOS_PCM_NO_AUTO_CHANNELS    0x00020000       /**< Disable automatic (but not forced!) channel conversion */
#define AOS_PCM_NO_AUTO_FORMAT      0x00040000       /**< Disable automatic (but not forced!) format conversion */
#define AOS_PCM_NO_SOFTVOL          0x00080000       /**< Disable soft volume control */
#define AOS_PCM_EVT_WRITE           (1 << 0)         /**< playback resource available event */
#define AOS_PCM_EVT_READ            (1 << 1)         /**< capture data available event */
#define AOS_PCM_EVT_XRUN            (1 << 2)         /**< underrun (playback) or overrun (capture) detected */

/* PCM stream 类型定义 */
typedef enum {
	AOS_PCM_STREAM_PLAYBACK = 0,                     /**< Playback stream */
	AOS_PCM_STREAM_CAPTURE,                          /**< Capture stream */
	AOS_PCM_STREAM_LAST = AOS_PCM_STREAM_CAPTURE
} aos_pcm_stream_t;

/* PCM stream 状态定义 */
typedef enum {
	AOS_PCM_STATE_IDLE = 0,                          /**< IDLE */
	AOS_PCM_STATE_OPEN,                              /**< Open */
	AOS_PCM_STATE_PREPARED,                          /**< Ready to start */
	AOS_PCM_STATE_RUNNING,                           /**< Running */
	AOS_PCM_STATE_XRUN,                              /**< Stopped: underrun (playback) or overrun (capture) detected */
	AOS_PCM_STATE_DRAINING,                          /**< Draining: running (playback) or stopped (capture) */
	AOS_PCM_STATE_PAUSED,                            /**< Paused */
	AOS_PCM_STATE_SUSPENDED,                         /**< Hardware is suspended */
	AOS_PCM_STATE_DISCONNECTED,                      /**< Hardware is disconnected */
	AOS_PCM_STATE_LAST = AOS_PCM_STATE_DISCONNECTED, /**< Last state */
	AOS_PCM_STATE_PRIVATE1 = 1024                    /**< Private - used internally in the library - do not use*/
} aos_pcm_state_t;

/* PCM stream 格式定义 */
typedef enum {
	AOSRV_PCM_FORMAT_S8 = 1,                         /**< Signed 8-bit */
	AOSRV_PCM_FORMAT_S16_LE = 2,                     /**< Signed 16-bit, little endian */
	AOSRV_PCM_FORMAT_S24_LE = 3,                     /**< Signed 24-bit, little endian */
	AOSRV_PCM_FORMAT_S32_LE = 4,                     /**< Signed 32-bit, little endian */
	AOSRV_PCM_FORMAT_ALL
} aos_pcm_format_t;

/* PCM stream 访问模式定义 */
typedef enum {
	AOS_PCM_ACCESS_MMAP_INTERLEAVED = 0,             /**< MMAP RW interleaved access */
	AOS_PCM_ACCESS_MMAP_NONINTERLEAVED,              /**< MMAP RW non-interleaved access */
	AOS_PCM_ACCESS_RW_INTERLEAVED,                   /**< RW interleaved access, e.g. writei/readi */
	AOS_PCM_ACCESS_RW_NONINTERLEAVED                 /**< RW non-interleaved access, e.g. writen/readn */
} aos_pcm_access_t;

/* PCM stream 硬件参数类型 */
typedef struct {
	aos_pcm_access_t access;                         /**< aos_pcm_access_t 类型 */
    aos_pcm_format_t format;                         /**< aos_pcm_format_t 类型 */
    unsigned int sample_bits;                        /**< 采样精度 */
    unsigned int frame_bits;                         /**< frame 位宽大小 */
    unsigned int channels;                           /**< channel 通道数 */
    unsigned int rate;                               /**< 采样率 */
    unsigned int period_time;
    unsigned int period_size;
    unsigned int period_bytes;
    unsigned int periods;
    unsigned int buffer_time;
    unsigned int buffer_size;
    unsigned int buffer_bytes;
    unsigned int tick_time;
} aos_pcm_hw_params_t;

/* PCM stream 软件参数类型 */
typedef struct {
	int tstamp_mode;			                     /*< timestamp mode */
	unsigned int period_step;
	unsigned int sleep_min;			                 /*< min ticks to sleep */
	aos_pcm_uframes_t avail_min;		             /*< min avail frames for wakeup */
	aos_pcm_uframes_t xfer_align;		             /*< obsolete: xfer size need to be a multiple */
	aos_pcm_uframes_t start_threshold;	             /*< min hw_avail frames for automatic start */
	aos_pcm_uframes_t stop_threshold;	             /*< min avail frames for automatic stop */
	aos_pcm_uframes_t silence_threshold;             /*< min distance from noise for silence filling */
	aos_pcm_uframes_t silence_size;		             /*< silence block size */
	aos_pcm_uframes_t boundary;		                 /*< pointers wrap point */
	unsigned int proto;			                     /*< protocol version */
	unsigned int tstamp_type;		                 /*< timestamp type (req. proto >= 2.0.12) */
	unsigned char reserved[56];		                 /*< reserved for future */
} aos_pcm_sw_params_t;

/* PCM stream 类型 */
typedef struct {
	/* mandatory */
	int fd;                                          /*< 该pcm设备节点的fd，通过open()获取 */
	aos_pcm_stream_t stream;                         /*< aos_pcm_stream_t 类型 */
	aos_pcm_state_t state;                           /*< aos_pcm_state_t 类型 */
	char *name;                                      /*< pcm stream name */
	int mode;
	int card;                                        /*< pcm stream 所属的card id */
	int device;                                      /*< pcm stream device id */
	aos_hdl_t mutex;
    aos_hdl_t evt;
	aos_pcm_hw_params_t *hw_params;                  /*< pcm stream 硬件参数 */
    aos_pcm_sw_params_t *sw_params;                  /*< pcm stream 软件参数 */

	/* options */
    void *open_func;
	long minperiodtime;
	int poll_fd_count;
	unsigned short poll_events;
	int setup: 1,
	    compat: 1;
	unsigned int mmap_rw: 1;
	unsigned int mmap_shadow: 1;
	unsigned int donot_close: 1;
	unsigned int own_state_check:1;
	void *private_data;
} aos_pcm_t;

/* 所有声卡的PCM stream汇总 */
typedef struct {
	dlist_t list;                                    /*< 列表 */
	unsigned int count;                              /*< 列表中PCM Stream个数 */
	unsigned int allocated;
	const char *siface;                              /*< 默认: AOS_CTL_ELEM_IFACE_MIXER */
	int card;                                        /*< 当前声卡ID */
	int device;                                      /*< 在当前声中的设备ID */
	long device_input;
	long device_output;
	int stream;                                      /*< 0: Capture Stream 类型，1: Playback Stream 类型 */
	int show_all;
	char name[100];                                  /*< 当前设备名 */
} hint_list_t;

/**
 * 获取声卡card下属所有的PCM Stream
 *
 * @param[in]  card    声卡ID
 * @param[out]  hints   hint_list_t类型列表
 *
 * @return  0 on success, negative error on failure.
 */
int aos_device_name_hint(int card, void *hints);

/**
 * 获取声卡card下属所有的PCM Stream
 *
 * @param[out] **pcm    aos_pcm_t 句柄指针
 * @param[in]  *name    pcm stream name
 * @param[in]  stream   aos_pcm_stream_t 类型
 * @param[in]  mode     AOS_PCM_BLOCK 或者 AOS_PCM_NONBLOCK
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_open(aos_pcm_t **pcm, const char *name, aos_pcm_stream_t stream, int mode);

/**
 * 设置PCM Stream为prepared状态
 *
 * @param[in] *pcm      aos_pcm_t 句柄
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_prepare(aos_pcm_t *pcm);

/**
 * 设置PCM Stream为START状态
 *
 * @param[in] *pcm      aos_pcm_t 句柄
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_start(aos_pcm_t *pcm);

/**
 * 设置PCM Stream为wait状态, 最大超时时间timeout milisecond
 *
 * @param[in] *pcm      aos_pcm_t 句柄
 * @param[in] timeout   超时时间，毫秒为单位
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_wait(aos_pcm_t *pcm, int timeout);

/**
 * 停止PCM Stream
 *
 * @param[in] *pcm      aos_pcm_t 句柄
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_stop(aos_pcm_t *pcm);

/**
 * 等待Playback PCM Stream buffer中的数据播放完成
 *
 * @param[in] *pcm      aos_pcm_t 句柄
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_drain(aos_pcm_t *pcm);

/**
 * 暂停 PCM Stream
 *
 * @param[in] *pcm      aos_pcm_t 句柄
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_pause(aos_pcm_t *pcm, int enable);

/**
 * 关闭 PCM Stream
 *
 * @param[in] *pcm      aos_pcm_t 句柄
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_close(aos_pcm_t *pcm);

/**
 * 恢复 PCM Stream 底层硬件状态（可选）
 *
 * @param[in] *pcm      aos_pcm_t 句柄
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_recover(aos_pcm_t *pcm);

/**
 * 分配 aos_pcm_hw_params_t 类型缓存区
 *
 * @param[out] **p      aos_pcm_hw_params_t 类型2级指针
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_hw_params_alloca(aos_pcm_hw_params_t **p);

/**
 * 设置aos_pcm_hw_params_t 类型参数为默认参数
 *
 * @param[in] *params      aos_pcm_hw_params_t* 类型
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_hw_params_any(aos_pcm_hw_params_t *params);

/**
 * 设置pcm stream的硬件参数为 *p 指向的参数
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 * @param[in] *p        aos_pcm_hw_params_t* 类型
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_hw_params(aos_pcm_t *pcm, aos_pcm_hw_params_t *p);

/**
 * 根据输入参数设置pcm stream的硬件参数
 *
 * @param[in] *pcm        aos_pcm_t* 类型
 * @param[in] format      aos_pcm_format_t 类型
 * @param[in] access      aos_pcm_access_t 类型
 * @param[in] channels    unsigned int 类型, 通道数
 * @param[in] rate        unsigned int 类型, 采样率
 * @param[in] soft_resample   int 类型
 * @param[in] latency      unsigned int 类型
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_set_params(aos_pcm_t *pcm, aos_pcm_format_t format, aos_pcm_access_t access, unsigned int channels,
                       unsigned int rate, int soft_resample, unsigned int latency);

/**
 * 分配 aos_pcm_sw_params_t 类型缓存区
 *
 * @param[in] **p       aos_pcm_sw_params_t* 类型
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_sw_params_alloca(aos_pcm_sw_params_t **p);

/**
 * 设置 aos_pcm_sw_params_t 类型参数为默认参数
 *
 * @param[in] *params      aos_pcm_sw_params_t* 类型
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_sw_params_any(aos_pcm_sw_params_t *params);

/**
 * 设置pcm stream的软件参数为 *params 指向的参数
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 * @param[in] *params   aos_pcm_sw_params_t* 类型
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_sw_params(aos_pcm_t *pcm, aos_pcm_sw_params_t *params);

/**
 * 以interleave格式往pcm stream写数据（e.g. CH0 -> CH1 -> CH2 -> CH0 ...）
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 * @param[in] *buffer   待写入的数据buffer
 * @param[in] *size     待写入的数据buffer大小，以字节为单位
 *
 * @return  writen bytes number on success, negative error on failure.
 */
aos_pcm_sframes_t aos_pcm_writei(aos_pcm_t *pcm, const void *buffer, aos_pcm_uframes_t size);

/**
 * 以interleave格式从pcm stream读数据（e.g. CH0 -> CH1 -> CH2 -> CH0 ...）
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 * @param[in] *buffer   存储读出数据的buffer
 * @param[in] *size     待读出的数据大小，以字节为单位
 *
 * @return  read bytes number on success, negative error on failure.
 */
aos_pcm_sframes_t aos_pcm_readi(aos_pcm_t *pcm, void *buffer, aos_pcm_uframes_t size);

/**
 * 以non-interleave格式往pcm stream写数据（e.g. CH0 -> CH0 ...-> CH0 (size 单位数据全部写完) -> CH1 -> CH1 ...）
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 * @param[in] *buffer   待写入的数据buffer
 * @param[in] *size     待写入的数据buffer大小，以字节为单位
 *
 * @return  writen bytes number on success, negative error on failure.
 */
aos_pcm_sframes_t aos_pcm_writen(aos_pcm_t *pcm, void **bufs, aos_pcm_uframes_t size);

/**
 * 以non-interleave格式从pcm stream读数据（e.g.  CH0 -> CH0 ...-> CH0 (size 单位数据全部读完) -> CH1 -> CH1 ...）
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 * @param[in] *buffer   存储读出数据的buffer
 * @param[in] *size     待读出的数据大小，以字节为单位
 *
 * @return  read bytes number on success, negative error on failure.
 */
aos_pcm_sframes_t aos_pcm_readn(aos_pcm_t *pcm, void **bufs, aos_pcm_uframes_t size);

/**
 * 暂停pcm stream，buffer中的数据仍然保留
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_suspend(aos_pcm_t *pcm);

/**
 * 恢复pcm stream，buffer中遗留的数据继续读写
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 *
 * @return  0 on success, negative error on failure.
 */
int aos_pcm_resume(aos_pcm_t *pcm);

/**
 * 根据pcm stream的参数配置计算bytes个字节对应的frame帧数
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 * @param[in] bytes     字节数
 *
 * @return  aos_pcm_sframes_t on success, negative error on failure.
 */
aos_pcm_sframes_t aos_pcm_bytes_to_frames(aos_pcm_t *pcm, int bytes);

/**
 * 根据pcm stream的参数配置计算frame帧数对应的bytes字节数
 *
 * @param[in] *pcm      aos_pcm_t* 类型
 * @param[in] *frames   帧数
 *
 * @return  unsigned int on success, negative error on failure.
 */
int aos_pcm_frames_to_bytes(aos_pcm_t *pcm, aos_pcm_sframes_t frames);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* SOUND_PCM_H */

