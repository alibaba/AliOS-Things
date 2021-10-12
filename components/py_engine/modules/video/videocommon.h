#ifndef HAAS_VIDEO_COMMON_H
#define HAAS_VIDEO_COMMON_H

#include "ak_common.h"
#include "ak_vi.h"
#include "py/objlist.h"
#include "py/objstr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UVC_FILE                       "/dev/usbuvc"
#define UVC_DEFAULT_FORMAT             VIDEO_MEDIA_TYPE_YUV

#define CAMERA_MAIN_CHN_DEFAULT_WIDTH  (1920)
#define CAMERA_MAIN_CHN_DEFAULT_HEIGHT (1080)
#define CAMERA_SUB_CHN_DEFAULT_WIDTH   (640)
#define CAMERA_SUB_CHN_DEFAULT_HEIGHT  (320)
#define CAMERA_DEFAULT_FPS             (20)

#define VIDEO_CAMERA_OBJ_CHK(obj, err_type, err_str) \
    do {                                             \
        if (obj == NULL) {                           \
            LOG_E("%s, %s\n", __func__, err_str);    \
            mp_raise_OSError(err_type);              \
            return mp_const_none;                    \
        }                                            \
    } while (0)

#define VIDEO_CAMERA_RET_CHK(ret, dst_ret, err_str) \
    do {                                            \
        if (obj != dst_ret) {                       \
            LOG_E("%s, %s\n", __func__, err_str);   \
            mp_raise_OSError(MP_EIO);               \
            return mp_const_none;                   \
        }                                           \
    } while (0)

#define VIDEO_CAMERA_OBJ_INIT_CHK(obj, err_type, err_str) \
    do {                                                  \
        if (obj == NULL || obj->frame == NULL) {          \
            LOG_E("%s, %s\n", __func__, err_str);         \
            mp_raise_OSError(err_type);                   \
            return mp_const_none;                         \
        }                                                 \
    } while (0)

#define VIDEO_CAMERA_PARAM_CHK(n_arg, dst_cnt, err_str) \
    do {                                                \
        if (n_arg != dst_cnt) {                         \
            LOG_E("%s, %s, arg:%d\n", __func__, n_arg); \
            mp_raise_OSError(MP_EINVAL);                \
            return mp_const_none;                       \
        }                                               \
    } while (0)

enum {
    VIDEO_MEDIA_TYPE_YUV = 0,
    VIDEO_MEDIA_TYPE_JPEG,
    VIDEO_MEDIA_TYPE_H264,
    VIDEO_MEDIA_TYPE_HEVC,
};

typedef struct video_input_frame isp_frame_t;

typedef struct _camera_obj_t {
    mp_obj_base_t base;
    char *mname;
    int camera_idx;
    int frame_rate;
    int frame_release;
    isp_frame_t *frame;
} mp_video_camera_obj_t;

int py_video_camera_frame_get(isp_frame_t *frame);
int py_video_camera_frame_release(isp_frame_t *frame);
int py_video_camera_save(isp_frame_t *frame, int pic_type, const char *path);

int py_video_camera_open(int camera_idx, int frame_rate);
int py_video_camera_close(int camera_idx);
int py_video_camera_working();
int py_video_camera_config_set(int width, int height, int media_type);
int py_video_camera_config_get(int *width, int *height, int *media_type);
void py_usbcam_video_ext_process_config(void (*ext_process)(isp_frame_t *),
                                        void (*ext_evt_cb)(int));

int py_venc_init(int width, int height, int fps, int media_type);

int py_usbcam_video_init(int camera_idx, int chan_idx);
int py_usbcam_video_deinit();

int py_rtsp_stop(void);
int py_rtsp_start(void);
int py_rtsp_close(void);
int py_rtsp_pause(void);
int py_rtsp_resume(void);
int py_rtsp_open(int camera_idx, int media_type, int fps, const char *url);

#ifdef __cplusplus
}
#endif

#endif
