#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "usb_s_uvc_ioctl.h"
#include "dev_info.h"
#include "ak_common.h"
#include "ak_vi.h"
#include "ak_common_video.h"
#include "ak_venc.h"
#include "minIni.h"
#include "ak_vpss.h"
#include "videocommon.h"
#include "HaasLog.h"

#define UVC_ENC_CONFIG_FILE   "/etc/config/anyka_app.ini"

int py_venc_init(int width, int height, int fps, int media_type)
{
    int ret = -1, handle_id = -1;
    struct venc_param param;

    param.width  = width;
    param.height = height;
    param.fps = fps;
    param.goplen = fps * 2;
    param.target_kbps = 1024;
    param.max_kbps = 1024;
    param.br_mode = BR_MODE_CBR;
    param.minqp = 25;
    param.maxqp = 48;
    param.initqp = (param.minqp + param.maxqp) >> 1;
    param.chroma_mode = CHROMA_4_2_0;
    param.max_picture_size = 0;
    param.enc_level = 41;
    param.smart_mode = 0;
    param.smart_goplen = 100;
    param.smart_quality = 50;
    param.smart_static_value = 0;
    param.jpeg_qlevel = JPEG_QLEVEL_DEFAULT;
    param.smart_static_value = 550;

    if (VIDEO_MEDIA_TYPE_JPEG == media_type) {
        param.minqp= ini_getl("usbcam", "mjpeg_minqp", 50 , UVC_ENC_CONFIG_FILE);
        param.maxqp= ini_getl("usbcam", "mjpeg_maxqp", 50 , UVC_ENC_CONFIG_FILE);
        param.max_kbps = ini_getl("usbcam", "mjpeg_kbps", 1024, UVC_ENC_CONFIG_FILE);
        param.target_kbps = ini_getl("usbcam", "mjpeg_kbps", 1024, UVC_ENC_CONFIG_FILE);

        param.profile = PROFILE_JPEG;
        param.enc_out_type = MJPEG_ENC_TYPE;
    } else if (VIDEO_MEDIA_TYPE_H264 == media_type) {
        param.minqp= ini_getl("usbcam", "h264_minqp", 25 , UVC_ENC_CONFIG_FILE);
        param.maxqp= ini_getl("usbcam", "h264_maxqp", 48 , UVC_ENC_CONFIG_FILE);
        param.max_kbps = ini_getl("usbcam", "h264_kbps", 1024, UVC_ENC_CONFIG_FILE);
        param.target_kbps = ini_getl("usbcam", "h264_kbps", 1024, UVC_ENC_CONFIG_FILE);

        param.profile = PROFILE_MAIN;
        param.enc_out_type = H264_ENC_TYPE;
    } else { // HEVC
        param.minqp= ini_getl("usbcam", "hevc_minqp", 25 , UVC_ENC_CONFIG_FILE);
        param.maxqp= ini_getl("usbcam", "hevc_maxqp", 50 , UVC_ENC_CONFIG_FILE);
        param.max_kbps = ini_getl("usbcam", "hevc_kbps", 1024, UVC_ENC_CONFIG_FILE);
        param.target_kbps = ini_getl("usbcam", "hevc_kbps", 1024, UVC_ENC_CONFIG_FILE);

        param.profile = PROFILE_HEVC_MAIN;
        param.enc_out_type = HEVC_ENC_TYPE;
    }

    ret = ak_venc_open(&param, &handle_id);
    if (ret || -1 == handle_id) {
        LOG_E("%s, enc open fail(id:%d, ret:%d)\r\n", __func__, ret, handle_id);
        return ret;
    }

    return handle_id;
}
