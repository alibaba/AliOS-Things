#include <kernel.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ak_thread.h"
#include "ak_common.h"

#include "ak_common_video.h"

#include "ak_vi.h"
#include "ak_venc.h"
#include "dev_info.h"
#include "ak_log.h"
#include "ak_rtsp.h"
#include "ak_vpss.h"
#include "ak_video.h"
#include "ak_mem.h"

#include <sys/ioctl.h>
#include "camera_ioctl.h"
#include "HaasLog.h"
#include "videocommon.h"

#define RTSP_STREAM_TASK_STACK_SIZE (32 * 1024)

#define RTSP_CHAN_NUM_MAX           (RTSP_CHUNNEL_1 + 1)
#define RTSP_THREAD_PRIORITY        (60)
#define RTSP_DEFAULT_FPS            (25)

enum PY_RTSP_STATE_T {
    RTSP_STATE_CLOSE,
    RTSP_STATE_READY,
    RTSP_STATE_WORK,
};

struct rtsp_video_thread_attr {
    ak_pthread_t thread_id;
    char *thread_name;
    int vi_channel_id;
    enum rtsp_chunnel_type rtsp_channel;
};

struct rtsp_ctrl {
    int fps;
    int camera_idx;
    int media_type;

    char *url;
    char *save_path;

    enum PY_RTSP_STATE_T chan_state[RTSP_CHAN_NUM_MAX];
    struct rtsp_video_thread_attr *attr;
};

/******************************************************
*                    Global Variables
******************************************************/
static struct  rtsp_ctrl g_rtsp_ctrl = {
    .fps = RTSP_DEFAULT_FPS,
    .camera_idx = -1,
    .media_type = VIDEO_MEDIA_TYPE_H264,
    .url = NULL,
    .save_path = NULL,
    .attr = NULL,
};

/*
 * @note: there are two channel concept.
 *        vi-channel refers to video data stream, one vi device has two related vi-channels.
 *        rtsp-channel refers to rtsp send data stream, rtsp module has two rtsp-channel,
 *        they are defined in <enum rtsp_chunnel_type> in "ak_rtsp.h"
 *        since there are two vi devices(sensor), we have 4 vi-channels, 2 rtsp-channel total.
 *
 * @rtsp_video_thread: get vi stream and send to rtsp
 * @param args[in]:  rtsp channel which thread will relate to
 * @return void *
 */

static void *rtsp_video_thread(void *arg)
{
    struct rtsp_video_thread_attr *self = (struct rtsp_video_thread_attr *)arg;

    int ret = -1;
    int venc_handle = -1;
    enum rtsp_frame_type frame_type;
    char ifram_force = 1;

    struct video_stream stream = {0};
    unsigned long long gop_size = 0;
    unsigned  max_size = 0;
    struct ak_timeval pre_tv;
    struct ak_timeval cur_tv;

    int width = CAMERA_SUB_CHN_DEFAULT_WIDTH;
    int height = CAMERA_SUB_CHN_DEFAULT_HEIGHT;

    FILE *save_fp = NULL;
    char save_file[96] = {0};

    ak_thread_set_name(self->thread_name);

    if (self->rtsp_channel == RTSP_CHUNNEL_0) {
        py_video_camera_config_get(&width, &height, NULL);
    }

    venc_handle = py_venc_init(width, height, g_rtsp_ctrl.fps, g_rtsp_ctrl.media_type);
    if (venc_handle < 0) {
        LOG_E("video encode open failed!\n");
        goto exit_thread;
    }

    APP_CHN_S Schn, Dchn;
    Schn.mid = MODULE_ID_VI;
    Schn.oid = self->vi_channel_id;
    Dchn.mid = MODULE_ID_VENC;
    Dchn.oid = self->rtsp_channel;
    APP_BIND_PARAM param;
    param.frame_rate = 0;
    param.frame_depth = 100;

    unsigned long kbps_r = 0;
    unsigned int count = 0;

    int bind_flag = 0;
    while (1) {
        /* iterate main and sub channel, get stream and set to rtsp */
        while (RTSP_STATE_WORK == g_rtsp_ctrl.chan_state[self->rtsp_channel]) {
            /* get stream until success */
            if (ak_rtsp_get_chunnel_state(self->rtsp_channel) == 0) {
                if (bind_flag == 0) {
                    bind_flag = 1;

                    ret = ak_app_video_bind_chn(&Schn, &Dchn, &param);
                    if (ret != AK_SUCCESS) {
                        LOG_E("ak_app_video_bind_chn failed [%d]\n",ret);
                        goto exit_venc;
                    }

                    ak_get_ostime(&pre_tv);
                    ak_get_ostime(&cur_tv);
                    ak_app_video_set_dst_chn_active(&Dchn, AK_TRUE);
                }

                if (ifram_force) {
                    ak_venc_request_idr(venc_handle);
                    ifram_force = 0;
                }
            } else {
                ifram_force = 1;
                if (bind_flag == 1) {
                    if (ak_app_video_venc_get_stream(&Dchn, &stream) == 0) {
                        ak_venc_release_stream(venc_handle, &stream);
                    } else {
                        ak_sleep_ms(10);
                    }

                    if (g_rtsp_ctrl.save_path) {
                        if (NULL != save_fp) {
                            fclose(save_fp);
                            save_fp = NULL;
                            count ++;
                        }
                    }
                } else {
                    ak_sleep_ms(200);
                }
                continue;
            }

            if (g_rtsp_ctrl.save_path) {
                if (NULL == save_fp) {
                    snprintf(save_file, sizeof(save_file), "%s/chn%d_%d.%s",
                            g_rtsp_ctrl.save_path, self->rtsp_channel, count,
                            g_rtsp_ctrl.media_type == VIDEO_MEDIA_TYPE_H264 ? "h264" : "hevc");
                    save_fp = fopen(save_file, "w+");
                    if (save_fp == NULL) {
                        LOG_E( "fopen %s failed\n", save_file);
                        return NULL;
                    }
                }
            }

            memset(&stream, 0, sizeof(struct video_stream));
            ret = ak_app_video_venc_get_stream(&Dchn, &stream);
            if (ret == AK_SUCCESS) {
                /* send stream */
                if (FRAME_TYPE_I == stream.frame_type) {
                    frame_type = RTSP_IFRAME;
                } else if(FRAME_TYPE_P == stream.frame_type) {
                    frame_type = RTSP_PFRAME;
                }

                ret = ak_rtsp_send_stream(self->rtsp_channel, frame_type, stream.data, stream.len, stream.ts);
                if (ret < 0) {
                    LOG_E("ak_rtsp_send_stream fail %d \n", ret);
                }

                if (g_rtsp_ctrl.save_path) {
                    if (stream.len > 0) {
                        if (fwrite(stream.data, stream.len, 1, save_fp) <= 0) {
                            LOG_E("fwrite %s failed!\n", save_file);
                        }
                    }
                }
                ak_venc_release_stream(venc_handle, &stream);
            } else {
                ak_sleep_ms(10);
            }
        }

        if (RTSP_STATE_CLOSE == g_rtsp_ctrl.chan_state[self->rtsp_channel]) {
            ak_app_video_unbind_chn(&Schn, &Dchn);
            goto exit_venc;
        }

        ak_sleep_ms(30);
    }

exit_venc:
    ak_venc_close(venc_handle);

exit_thread:
    ak_thread_exit();
    return NULL;
}

static int rtsp_create_monitor_thread()
{
    /* start rtsp_control, create two video threads for two RTSP channels */
    struct rtsp_video_thread_attr *attr;

    if (g_rtsp_ctrl.attr != NULL) return 0;

    attr = (struct rtsp_video_thread_attr *)
            malloc(sizeof(struct rtsp_video_thread_attr) * RTSP_CHAN_NUM_MAX);
    if (NULL == attr) {
        LOG_E("malloc video thread attr failed.\n");
        return -1;
    }

    // build main channel thread attribute
    attr[RTSP_CHUNNEL_0].thread_name = "rtsp_main_video";
    attr[RTSP_CHUNNEL_0].vi_channel_id = VIDEO_CHN_MAIN;
    attr[RTSP_CHUNNEL_0].rtsp_channel = RTSP_CHUNNEL_0;
    g_rtsp_ctrl.chan_state[RTSP_CHUNNEL_0] = RTSP_STATE_WORK;    //open audio rtsp main chan(defaut main chan)

    // build sub channel thread attribute
    attr[RTSP_CHUNNEL_1].thread_name = "rtsp_sub_video";
    attr[RTSP_CHUNNEL_1].vi_channel_id = VIDEO_CHN_SUB;
    attr[RTSP_CHUNNEL_1].rtsp_channel = RTSP_CHUNNEL_1;
    g_rtsp_ctrl.chan_state[RTSP_CHUNNEL_1] = RTSP_STATE_WORK;    //open video rtsp sub  chan for double chan use

    if (ak_thread_create(&attr[RTSP_CHUNNEL_0].thread_id, rtsp_video_thread,
            &attr[RTSP_CHUNNEL_0], RTSP_STREAM_TASK_STACK_SIZE, RTSP_THREAD_PRIORITY) != 0) {
        g_rtsp_ctrl.chan_state[RTSP_CHUNNEL_0] = RTSP_STATE_CLOSE;
        g_rtsp_ctrl.chan_state[RTSP_CHUNNEL_1] = RTSP_STATE_CLOSE;
        free(attr);
        LOG_E("create client_listen main_chan thread error.\r\n");
        return -1;
    }

    /* start sub video */
    if (ak_thread_create(&attr[RTSP_CHUNNEL_1].thread_id, rtsp_video_thread,
            &attr[RTSP_CHUNNEL_1], RTSP_STREAM_TASK_STACK_SIZE, RTSP_THREAD_PRIORITY) != 0) {
        g_rtsp_ctrl.chan_state[RTSP_CHUNNEL_0] = RTSP_STATE_CLOSE;
        g_rtsp_ctrl.chan_state[RTSP_CHUNNEL_1] = RTSP_STATE_CLOSE;
        ak_thread_join(attr[RTSP_CHUNNEL_0].thread_id);
        free(attr);
        LOG_E("create client_listen sub_chan thread error.\r\n");
        return -1;
    }

    g_rtsp_ctrl.attr = attr;

    return 0;
}

int py_rtsp_start(void)
{
    char path[32];

    if (RTSP_STATE_CLOSE == g_rtsp_ctrl.chan_state[0]) {
        /* init rtsp */
        if (ak_rtsp_init() != 0) {
            LOG_E("init rtsp failed\n");
            return -1;
        }

        /* init rtsp channel name,default name is camera-x,x: 1,2*/
        if (g_rtsp_ctrl.url == NULL) {
            strncpy(path, "main", sizeof(path));
        } else {
            snprintf(path, sizeof(path), "main/%s", g_rtsp_ctrl.url);
        }
        if (ak_rtsp_set_chunnel_name(RTSP_CHUNNEL_0, path) != 0) {
            LOG_E("rtsp set chan_main name error\n");
            ak_rtsp_deinit();
            return -1;
        }

        if (g_rtsp_ctrl.url == NULL) {
            strncpy(path, "sub", sizeof(path));
        } else {
            snprintf(path, sizeof(path), "sub/%s", g_rtsp_ctrl.url);
        }
        if (ak_rtsp_set_chunnel_name(RTSP_CHUNNEL_1, path) != 0) {
            LOG_E("rtsp set chan_sub name error\n");
            ak_rtsp_deinit();
            return -1;
        }

        /* set rtsp video type */
        enum rtsp_video_type rtsp_enc_type;
        if (g_rtsp_ctrl.media_type == VIDEO_MEDIA_TYPE_HEVC) {
            rtsp_enc_type = RTSP_VIDEO_H265;
        } else {
            rtsp_enc_type = RTSP_VIDEO_H264;
        }

        // for single steam, set both of channel rtsp type
        ak_rtsp_set_video_type(RTSP_CHUNNEL_0, rtsp_enc_type);
        ak_rtsp_set_video_type(RTSP_CHUNNEL_1, rtsp_enc_type);

        /* start rtsp service */
        ak_rtsp_start();

        if (rtsp_create_monitor_thread() < 0) {
            LOG_E("create rtsp main thread error\r\n");
            ak_rtsp_stop();
            ak_rtsp_deinit();
            return -1;
        }
    } else {
        LOG_W("rtsp state is not closed, can't be started!\n");
    }
    return 0;
}

int py_rtsp_open(int camera_idx, int media_type, int fps, const char *url)
{
    if (g_rtsp_ctrl.camera_idx >= 0) {
        py_rtsp_close();
    }

    if (g_rtsp_ctrl.camera_idx < 0) {
        if (url && strlen(url) > 0) {
            g_rtsp_ctrl.url = strdup(url);
            if (g_rtsp_ctrl.url == NULL) {
                LOG_E("rtsp alloc url fail\n");
                return -1;
            }
        }

        py_video_camera_config_set(0, 0, VIDEO_MEDIA_TYPE_H264);
        if (py_video_camera_open(camera_idx, fps) != 0) {
            LOG_E("vi dev[%d] init fail\n", camera_idx);
            if (g_rtsp_ctrl.url) {
                free(g_rtsp_ctrl.url);
                g_rtsp_ctrl.url = NULL;
            }
            return -1;
        }
        g_rtsp_ctrl.camera_idx = camera_idx;
        g_rtsp_ctrl.fps = fps;
        g_rtsp_ctrl.media_type = media_type;
    }
    return 0;
}

int py_rtsp_stop(void)
{
    if (RTSP_STATE_CLOSE != g_rtsp_ctrl.chan_state[0]) {
        g_rtsp_ctrl.chan_state[0] = RTSP_STATE_CLOSE;
        g_rtsp_ctrl.chan_state[1] = RTSP_STATE_CLOSE;
    }

    if (g_rtsp_ctrl.attr != NULL) {
        ak_thread_join(g_rtsp_ctrl.attr[RTSP_CHUNNEL_0].thread_id);
        ak_thread_join(g_rtsp_ctrl.attr[RTSP_CHUNNEL_1].thread_id);
        ak_rtsp_stop();
        free(g_rtsp_ctrl.attr);
        g_rtsp_ctrl.attr = NULL;
    }

    return 0;
}

int py_rtsp_close(void)
{
    py_rtsp_stop();

    py_video_camera_close(g_rtsp_ctrl.camera_idx);

    if (g_rtsp_ctrl.url) {
        free(g_rtsp_ctrl.url);
        g_rtsp_ctrl.url = NULL;
    }
    if (g_rtsp_ctrl.save_path) {
        free(g_rtsp_ctrl.save_path);
        g_rtsp_ctrl.save_path = NULL;
    }
}

int py_rtsp_resume(void)
{
    if (RTSP_STATE_READY == g_rtsp_ctrl.chan_state[0]) {
        g_rtsp_ctrl.chan_state[0] = RTSP_STATE_WORK;
        g_rtsp_ctrl.chan_state[1] = RTSP_STATE_WORK;
    } else {
        LOG_W("rtsp state is not ready, can't be resumed!\n");
    }
    return 0;
}

int py_rtsp_pause(void)
{
    if (RTSP_STATE_WORK == g_rtsp_ctrl.chan_state[0]) {
        g_rtsp_ctrl.chan_state[0] = RTSP_STATE_READY;
        g_rtsp_ctrl.chan_state[1] = RTSP_STATE_READY;
    } else {
        LOG_W("rtsp state is not work, can't be paused!\n");
    }
    return 0;
}