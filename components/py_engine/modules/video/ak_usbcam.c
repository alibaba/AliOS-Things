#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "usb_s_uvc_ioctl.h"
#include "dev_info.h"

#include "ak_thread.h"
#include "ak_common.h"
#include "ak_log.h"
#include "ak_vi.h"
#include "ak_common_video.h"
#include "ak_venc.h"
#include "minIni.h"
#include "ak_vpss.h"

#include "os_drv.h"
#include "list.h"
#include "HaasLog.h"
#include "videocommon.h"
#include "uvc.h"

enum uvc_stream_state_t {
    USBCAM_STATE_STOP = 0,
    USBCAM_STATE_STANDBY = 1,
    USBCAM_STATE_WORKING = 2,
};

typedef  struct uvc_dev {
    ak_pthread_t monitor_thread;
    ak_pthread_t stream_thread;
    ak_pthread_t ctrl_thread;
    int uvc_fd;
    int venc_fd;
    int camera_idx;
    int vi_chan;
    int width;
    int height;
    int sensor_fps;
    int out_fps;
    int ext_process;
    void (*ext_process_cb)(isp_frame_t *frame);
    void (*ext_event_cb)(int start);

    int stream_type;
    enum ak_vi_daynight_mode day_night;      // 0: day 1: night
    enum uvc_stream_state_t send_stream;
    char monitor;
    char ctrl;
} uvc_dev_t;

typedef struct _uvc_msg_queue{
    int ret;
    struct list_head cmd_list;
} uvc_msg_queue;

struct list_head uvc_cmd_list;
static ak_sem_t uvc_ctrl_sem;
static ak_mutex_t uvc_ctrl_mutex;
static uvc_dev_t m_uvc_dev = {
    .monitor_thread = NULL,
    .ctrl_thread = NULL,
    .stream_thread = NULL,
    .camera_idx = VIDEO_DEV0,
    .vi_chan = VIDEO_CHN0,
    .venc_fd = -1,
    .uvc_fd = -1,
    .stream_type = UVC_DEFAULT_FORMAT,
    .width = CAMERA_MAIN_CHN_DEFAULT_WIDTH,
    .height = CAMERA_MAIN_CHN_DEFAULT_HEIGHT,
    .day_night = VI_MODE_DAY_OUTDOOR,
    .out_fps = CAMERA_DEFAULT_FPS,
    .send_stream = USBCAM_STATE_STOP,
    .ctrl = 0,
    .monitor = 0,
    .ext_process = 0,
    .ext_process_cb = NULL,
    .ext_event_cb = NULL,
};

static int usbcam_video_get_format(int *width, int *height, enum uvc_stream_type *type)
{
   if (ioctl(m_uvc_dev.uvc_fd, IO_UVC_GET_FORMAT, type) == -1) {
       LOG_E("ioctl failed\n");
       return -1;
   }

   if (ioctl(m_uvc_dev.uvc_fd, IO_UVC_GET_HEIGHT, height) == -1) {
       LOG_E("ioctl failed\n");
       return -1;
   }

   if (ioctl(m_uvc_dev.uvc_fd, IO_UVC_GET_WIDTH, width) == -1) {
       LOG_E("ioctl failed\n");
       return -1;
   }
}

static int usbcam_video_open(void)
{
    RECTANGLE_S res;
    int fd = open(UVC_FILE, O_RDWR, 0);

    if (fd == -1) {
        LOG_E("uvc open failed,m_uvc_dev.uvc_fd:%d\n", fd);
        return -1;
    }
        // max sensor resolution
    if (ak_vi_get_sensor_resolution(m_uvc_dev.camera_idx, &res) != 0) {
        LOG_E("%s, get vi dev(%d) sensor resolution fail\n", __func__, m_uvc_dev.camera_idx);
        close(fd);
        return -1;
    }

    if (ioctl(fd, IO_UVC_SET_SENSOR_RES, &res) != 0) {
        LOG_E("%s, set sensor resolution fail\n", __func__);
        close(fd);
        return -1;
    }

    if (ioctl(fd, IO_UVC_START_CTL, NULL) == -1) {
        LOG_E("ioctl IO_UVC_START_CTL failed\n");
        close(fd);
        return -1;
    }

    return fd;
}

static int usbcam_video_uvc_wait_start(void)
{
    return ioctl(m_uvc_dev.uvc_fd, IO_UVC_WAIT_START_CTL, NULL);
}

static int py_usbcam_video_stream_start(void)
{
    int stream_type;
    usbcam_video_get_format(&(m_uvc_dev.width), &(m_uvc_dev.height), &stream_type);

    switch (stream_type) {
        case UVC_STREAM_YUV:
            m_uvc_dev.stream_type = VIDEO_MEDIA_TYPE_YUV;
            break;
        case UVC_STREAM_MJPEG:
            m_uvc_dev.stream_type = VIDEO_MEDIA_TYPE_JPEG;
            break;
        case UVC_STREAM_H264:
            m_uvc_dev.stream_type = VIDEO_MEDIA_TYPE_H264;
            break;
        default:
            m_uvc_dev.stream_type = VIDEO_MEDIA_TYPE_YUV;
            break;
    }

    if (py_video_camera_config_set(m_uvc_dev.width, m_uvc_dev.height, m_uvc_dev.stream_type) != 0) {
        py_video_camera_close(m_uvc_dev.camera_idx);
        py_video_camera_open(m_uvc_dev.camera_idx, m_uvc_dev.out_fps);
    }

    if (m_uvc_dev.ext_process) {
        m_uvc_dev.out_fps = 2;
    }

    if (m_uvc_dev.stream_type != VIDEO_MEDIA_TYPE_YUV) {
        m_uvc_dev.venc_fd = py_venc_init(m_uvc_dev.width, m_uvc_dev.height,
                m_uvc_dev.out_fps, m_uvc_dev.stream_type);
        if (m_uvc_dev.venc_fd < 0) {
            LOG_E("%s, venc init fail:%d\r\n", __func__, m_uvc_dev.venc_fd);
            return -1;;
        }
    }
    m_uvc_dev.send_stream = USBCAM_STATE_WORKING;
    return 0;
}

static void py_usbcam_video_stream_stop(void)
{
    if (m_uvc_dev.send_stream) {
        m_uvc_dev.send_stream = USBCAM_STATE_STANDBY;
        if (m_uvc_dev.venc_fd >= 0) {
            ak_venc_close(m_uvc_dev.venc_fd);
            m_uvc_dev.venc_fd = -1;
        }
    }
}

static void usbcam_video_stream_send(void)
{
    int ret;
    int chan = m_uvc_dev.vi_chan;
    struct video_stream video_str;
    struct video_input_frame frame;

    if (ak_vi_get_frame(chan, &frame) == AK_FAILED) {
        LOG_E("get frame fail\r\n");
        return;
    }

    if (m_uvc_dev.ext_process && m_uvc_dev.ext_process_cb != NULL) {
        m_uvc_dev.ext_process_cb(&frame);
    }

    if (m_uvc_dev.stream_type != VIDEO_MEDIA_TYPE_YUV) {
        memset(&video_str, 0, sizeof(video_str));
        ret = ak_venc_encode_frame(m_uvc_dev.venc_fd, frame.vi_frame.data, frame.vi_frame.len,
                NULL, &video_str);
        ak_vi_release_frame(chan, &frame);
        if (ret) {
            LOG_E("encode fail\r\n");
            return;
        }

        write(m_uvc_dev.uvc_fd, video_str.data, video_str.len);
        ak_venc_release_stream(m_uvc_dev.venc_fd, &video_str);
    } else {
        write(m_uvc_dev.uvc_fd, frame.vi_frame.data, frame.vi_frame.len);
        ak_vi_release_frame(chan, &frame);
    }
}

static void *usbcam_video_monitor_thread(void *arg)
{
    int ret;
    fd_set readset;
    struct timeval timeout;
    uvc_msg_queue *pmsg;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    while (m_uvc_dev.monitor) {
        FD_ZERO(&readset);
        FD_SET(m_uvc_dev.uvc_fd, &readset);
        /* Wait for read or write */
        ret = select(m_uvc_dev.uvc_fd + 1, &readset, NULL, NULL, &timeout);
        if (ret == 0) {
            //LOG_E("select timeout\r\n");
            continue;
        }
        ret = usbcam_video_uvc_wait_start();

        pmsg = (uvc_msg_queue *)malloc(sizeof(uvc_msg_queue));
        if (pmsg) {
            pmsg->ret = ret;
            list_add_tail(&pmsg->cmd_list, &uvc_cmd_list);
            ak_thread_sem_post(&uvc_ctrl_sem);
        }
    }
    ak_thread_exit();
}

static void *usbcam_video_ctrl_thread(void *arg)
{
    struct list_head *pos;
    struct list_head *q;
    uvc_msg_queue *pmsg;

    while (m_uvc_dev.ctrl) {
        if (ak_thread_sem_wait(&uvc_ctrl_sem) == 0) {
            list_for_each_safe(pos, q, &uvc_cmd_list) {
                pmsg = list_entry(pos, uvc_msg_queue, cmd_list);
                if (pmsg) {
                    ak_thread_mutex_lock(&uvc_ctrl_mutex);
                    if (pmsg->ret) {
                        if (m_uvc_dev.ext_process && m_uvc_dev.ext_event_cb != NULL) {
                            m_uvc_dev.ext_event_cb(1);
                        }
                        py_usbcam_video_stream_stop();
                        py_usbcam_video_stream_start();
                    } else {
                        py_usbcam_video_stream_stop();
                        if (m_uvc_dev.ext_process && m_uvc_dev.ext_event_cb != NULL) {
                            m_uvc_dev.ext_event_cb(0);
                        }
                    }
                    ak_thread_mutex_unlock(&uvc_ctrl_mutex);
                }

                list_del(pos);
                if (pmsg) free(pmsg);
            }
        }
    }
    ak_thread_exit();
}

static void *usbcam_video_stream_thread(void *arg)
{
    while (m_uvc_dev.send_stream) {
        if (m_uvc_dev.send_stream == USBCAM_STATE_STANDBY) {
            ak_sleep_ms(200);
            continue;
        }
        ak_thread_mutex_lock(&uvc_ctrl_mutex);
        usbcam_video_stream_send();
        ak_thread_mutex_unlock(&uvc_ctrl_mutex);
        if (m_uvc_dev.ext_process) {
            ak_sleep_ms(1000 / m_uvc_dev.out_fps);
        } else {
            ak_sleep_ms(5);
        }
    }
    ak_thread_exit();
}

void py_usbcam_video_ext_process_config(void (*ext_process)(isp_frame_t *),
                                       void (*ext_evt_cb)(int))
{
    m_uvc_dev.ext_process = 1;
    m_uvc_dev.ext_process_cb = ext_process;
    m_uvc_dev.ext_event_cb = ext_evt_cb;
}

int py_video_camera_working()
{
    if (m_uvc_dev.send_stream == USBCAM_STATE_WORKING) {
        return 1;
    }
    return 0;
}

int py_usbcam_video_init(int camera_idx, int chan_idx)
{
    int ret;

    m_uvc_dev.camera_idx = camera_idx;
    m_uvc_dev.uvc_fd = usbcam_video_open();
    if (m_uvc_dev.uvc_fd < 0) {
        LOG_E("usbcam_video_init  sem create error\n");
        m_uvc_dev.camera_idx = VIDEO_DEV0;
        return -1;
    }
    m_uvc_dev.vi_chan = chan_idx;

    INIT_LIST_HEAD(&uvc_cmd_list);

    ret = ak_thread_sem_init(&uvc_ctrl_sem, 1);
    if (ret < 0) {
        close(m_uvc_dev.uvc_fd);
        m_uvc_dev.uvc_fd = -1;
        LOG_E("usbcam_video_init  sem create error\n");
        return -1;
    }

    ret = ak_thread_mutex_init(&uvc_ctrl_mutex, NULL);
    if (ret < 0) {
        close(m_uvc_dev.uvc_fd);
        m_uvc_dev.uvc_fd = -1;
        LOG_E("usbcam_video_init  mutex create error\n");
        return -1;
    }

    m_uvc_dev.monitor = 1;
    if (ak_thread_create(&m_uvc_dev.monitor_thread, usbcam_video_monitor_thread, NULL,
            ANYKA_THREAD_MIN_STACK_SIZE, 20) != 0) {
        close(m_uvc_dev.uvc_fd);
        m_uvc_dev.uvc_fd = -1;
        m_uvc_dev.monitor = 0;
        return -1;
    }

    m_uvc_dev.ctrl = 1;
    if (ak_thread_create(&m_uvc_dev.ctrl_thread, usbcam_video_ctrl_thread, NULL,
            ANYKA_THREAD_MIN_STACK_SIZE, 20) != 0) {
        m_uvc_dev.monitor = 0;
        ak_thread_join(m_uvc_dev.monitor_thread);
        m_uvc_dev.monitor_thread = NULL;
        close(m_uvc_dev.uvc_fd);
        m_uvc_dev.uvc_fd = -1;
        m_uvc_dev.ctrl = 0;
        return -1;
    }

    m_uvc_dev.send_stream = USBCAM_STATE_STANDBY;
    if (ak_thread_create(&m_uvc_dev.stream_thread, usbcam_video_stream_thread, NULL,
            ANYKA_THREAD_MIN_STACK_SIZE, 20) != 0) {
        m_uvc_dev.ctrl = 0;
        ak_thread_sem_post(&uvc_ctrl_sem);
        ak_thread_join(m_uvc_dev.ctrl_thread);
        m_uvc_dev.ctrl_thread = NULL;
        m_uvc_dev.monitor = 0;
        ak_thread_join(m_uvc_dev.monitor_thread);
        m_uvc_dev.monitor_thread = NULL;
        close(m_uvc_dev.uvc_fd);
        m_uvc_dev.uvc_fd = -1;
        m_uvc_dev.send_stream = USBCAM_STATE_STOP;
        return -1;
    }

    return 0;
}

int py_usbcam_video_deinit()
{
    m_uvc_dev.send_stream = USBCAM_STATE_STOP;
    if (m_uvc_dev.stream_thread != NULL) {
        ak_thread_join(m_uvc_dev.stream_thread);
        m_uvc_dev.stream_thread = NULL;
    }

    m_uvc_dev.send_stream = USBCAM_STATE_STANDBY;
    py_usbcam_video_stream_stop();
    m_uvc_dev.send_stream = USBCAM_STATE_STOP;

    m_uvc_dev.ctrl = 0;
    if (m_uvc_dev.ctrl_thread != NULL) {
        ak_thread_sem_post(&uvc_ctrl_sem);
        ak_thread_join(m_uvc_dev.ctrl_thread);
        m_uvc_dev.ctrl_thread = NULL;
    }

    m_uvc_dev.monitor = 0;
    if (m_uvc_dev.monitor_thread != NULL) {
        ak_thread_join(m_uvc_dev.monitor_thread);
        m_uvc_dev.monitor_thread = NULL;
    }

    if (m_uvc_dev.uvc_fd > 0) {
        close(m_uvc_dev.uvc_fd);
        m_uvc_dev.uvc_fd = -1;
    }
    ak_thread_mutex_destroy(&uvc_ctrl_mutex);
    ak_thread_sem_destroy(&uvc_ctrl_sem);

    m_uvc_dev.ext_process = 0;
    m_uvc_dev.ext_event_cb = NULL;
    m_uvc_dev.ext_process_cb = NULL;

    return 0;
}
