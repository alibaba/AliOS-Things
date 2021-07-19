#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "py/runtime0.h"
#include "py/nlr.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "py/mperrno.h"

#include "usb_s_uvc_ioctl.h"
#include "dev_info.h"

#include "ak_thread.h"
#include "ak_common.h"
#include "ak_log.h"
#include "ak_common_video.h"
#include "ak_venc.h"
#include "minIni.h"
#include "ak_vpss.h"
#include "ak_vi.h"
#include "camera_ioctl.h"
#include "ak_mem.h"

#include "os_drv.h"
#include "HaasLog.h"
#include "videocommon.h"

#define ISP_USE_DMA   (0)
#define ISP_CFG_FILE  "/etc/config/anyka_app.ini"

struct camera_ctx_t {
    int main_width;
    int main_height;
    int sub_width;
    int sub_height;
    int media_type;
    int codec;
};

static struct camera_ctx_t cctx = {
    .main_width = CAMERA_MAIN_CHN_DEFAULT_WIDTH,
    .main_height = CAMERA_MAIN_CHN_DEFAULT_HEIGHT,
    .sub_width = CAMERA_SUB_CHN_DEFAULT_WIDTH,
    .sub_height = CAMERA_SUB_CHN_DEFAULT_HEIGHT,
    .codec = -1,
    .media_type = VIDEO_MEDIA_TYPE_YUV,
};

#if ISP_USE_DMA
static int dma_pool_init(void)
{
    int vi_dma_size = 14336;    //18406, unit in KB
    int venc_dma_size = 12288;  //12000, unit in KB
    int osd_dma_size = 300 << 10;

    ak_mem_dma_pool_create(MODULE_ID_VI, vi_dma_size << 10);  /* 300KB left for OSD and audio*/
    ak_mem_dma_pool_create(MODULE_ID_VENC, venc_dma_size << 10);
    ak_mem_dma_pool_create(MODULE_ID_OSD, osd_dma_size);
    ak_mem_dma_pool_activate();

    return 0;
}
#endif

static int get_isp_cfg_path(int dev,char *buf, int size)
{
    if (dev) {
        ini_gets("isp_cfg", "isp_path_b", 0, buf, size, ISP_CFG_FILE);
    } else {
        ini_gets("isp_cfg", "isp_path_a", 0, buf, size, ISP_CFG_FILE);
    }

    return 0;
}

static int get_isp_ae_init_para(struct vpss_isp_ae_init_info *ae_init_info)
{
    ae_init_info->d_gain = ini_getl("isp_cfg", "d_gain", 0, ISP_CFG_FILE);
    ae_init_info->a_gain = ini_getl("isp_cfg", "a_gain", 0 , ISP_CFG_FILE);
    ae_init_info->exp_time =ini_getl("isp_cfg", "exp_time", 0, ISP_CFG_FILE);
    ae_init_info->isp_d_gain = ini_getl("isp_cfg", "isp_d_gain", 0, ISP_CFG_FILE);

    return 0;
}

static int camera_set_ircut(int fd, int status_level)
{
    int ret = 0;
    int mode = IRCUT_MODE_DAY;
    int feed_status = CAMERA_IRFEED_GPIO_STATUS_DAY;

    if (status_level != 0) {
        feed_status = CAMERA_IRFEED_GPIO_STATUS_NIGHT;
        mode = IRCUT_MODE_NIGHT;
    }

    ret |= ioctl(fd, IO_CAMERA_IRCUT_GPIO_SET, &mode);
    ret |= ioctl(fd, IO_CAMERA_IRFEED_GPIO_SET, &feed_status);

    return ret;
}

static int vi_para_init(int camera_idx, int frame_rate)
{
    int ircut_level = 0;  // 0:day, 1:night
    enum ak_vi_daynight_mode mode = VI_MODE_DAY_OUTDOOR;
    int frame_depth = 2;
    int data_type = VI_DATA_TYPE_YUV420SP;

    if (cctx.media_type == VIDEO_MEDIA_TYPE_YUV) {
        data_type = VI_DATA_TYPE_YUV420P;
    }

    /*
     * step 1: open video input device
     */
    if (ak_vi_open(camera_idx) != 0) {
        LOG_E("%s, open vi dev(%d) fail\n", __func__, camera_idx);
        return -1;
    }

#if !QUICK_START
    /*
     * step 2: load isp config
     */
    char isp_cfg_path[128];
    if (get_isp_cfg_path(camera_idx, isp_cfg_path, sizeof(isp_cfg_path)) != 0) {
        LOG_E("%s, get dev(%d) isp cfg fail\n", __func__, camera_idx);
        ak_vi_close(camera_idx);
        return -1;
    }
    if (ak_vi_load_sensor_cfg(camera_idx, isp_cfg_path) != 0) {
        LOG_E("%s, load vi dev(%d) sensor cfg fail\n", __func__, camera_idx);
        ak_vi_close(camera_idx);
        return -1;
    }

    // get ae info from sensor
    struct vpss_isp_ae_init_info ae_init_info;
    ak_vpss_get_sensor_ae_info(camera_idx, &ae_init_info);
    if (ae_init_info.exp_time == 0) {
        //load init ae info
        get_isp_ae_init_para(&ae_init_info);
    }
    // set ae info to isp
    ak_vpss_set_ae_init_info(camera_idx, &ae_init_info);

    /*
     * step 3: set isp work mode
     */
    if (mode == VI_MODE_NIGHTTIME) {
        ircut_level = 1;
    }

    const char *ircut_dev[2] = {"/dev/ircut", "/dev/ircut1"};
    int fd = open(ircut_dev[camera_idx], O_RDWR);
    camera_set_ircut(fd, ircut_level);
    close(fd);
    ak_vi_switch_mode(camera_idx, mode);
#endif

    /*
     * step 4: get sensor support max resolution
     */
    RECTANGLE_S res;  // max sensor resolution
    if (ak_vi_get_sensor_resolution(camera_idx, &res) != 0) {
        LOG_E("%s, get vi dev(%d) sensor resolution fail\n", __func__, camera_idx);
        ak_vi_close(camera_idx);
        return -1;
    }

    /*
     * step 5: set vi working parameters
     * default parameters: 20fps, day mode, auto frame-control
     */
    VI_DEV_ATTR dev_attr;
    memset(&dev_attr, 0, sizeof(dev_attr));
    dev_attr.dev_id = camera_idx;
    dev_attr.crop.left = 0;
    dev_attr.crop.top = 0;
    dev_attr.crop.width = res.width;
    dev_attr.crop.height = res.height;
    dev_attr.max_width = cctx.main_width;
    dev_attr.max_height = cctx.main_height;
    dev_attr.data_type  = data_type;
    dev_attr.sub_max_width = cctx.sub_width;
    dev_attr.sub_max_height = cctx.sub_height;
    dev_attr.frame_rate = frame_rate;

    if (ak_vi_set_dev_attr(camera_idx, &dev_attr) != 0) {
        LOG_E("%s, set vi dev(%d) dev attr fail\n", __func__, camera_idx);
        ak_vi_close(camera_idx);
        return -1;
    }

    /*
     * step 5: set channel attribute
     */
    //ak_print_normal_ex(MODULE_ID_VI, "vi device set main channel\n");
    VI_CHN_ATTR chn_attr;
    memset(&chn_attr, 0, sizeof(chn_attr));
    chn_attr.chn_id = VIDEO_CHN0;
    chn_attr.res.width = cctx.main_width;
    chn_attr.res.height = cctx.main_height;
    chn_attr.frame_depth = frame_depth;
    chn_attr.frame_rate = frame_rate;
    if (ak_vi_set_chn_attr(VIDEO_CHN0, &chn_attr) != 0) {
        LOG_E("%s, set vi dev(%d) chn0 attr fail\n", __func__, camera_idx);
        ak_vi_close(camera_idx);
        return -1;
    }

    memset(&chn_attr, 0, sizeof(chn_attr));
    chn_attr.chn_id = VIDEO_CHN1;
    chn_attr.res.width = cctx.sub_width;
    chn_attr.res.height = cctx.sub_height;
    chn_attr.frame_depth = frame_depth;
    chn_attr.frame_rate = frame_rate;
    if (ak_vi_set_chn_attr(VIDEO_CHN1, &chn_attr) != 0) {
        LOG_E("%s, set vi dev(%d) chn1 attr fail\n", __func__, camera_idx);
        ak_vi_close(camera_idx);
        return -1;
    }

    /*
     * step 6: enable vi channel
     */
    if (ak_vi_enable_chn(VIDEO_CHN0) != 0) {
        LOG_E("%s, enable vi dev(%d) chn0 attr fail\n", __func__, camera_idx);
        ak_vi_close(camera_idx);
        return -1;
    }

    if (ak_vi_enable_chn(VIDEO_CHN1) != 0) {
        LOG_E("%s, enable vi dev(%d) chn1 attr fail\n", __func__, camera_idx);
        ak_vi_disable_chn(VIDEO_CHN0);
        ak_vi_close(camera_idx);
        return -1;
    }

    return 0;
}

int py_video_camera_open(int camera_idx, int frame_rate)
{
    do {
        cctx.codec = -1;
        sdk_run_config config;
        memset(&config, 0, sizeof(config));
        config.mem_trace_flag = SDK_RUN_NORMAL;

        if (ak_sdk_init(&config) != 0) break;

        /* LOG_LEVEL_NORMAL for debug */
        //ak_print_set_level(MODULE_ID_ALL, LOG_LEVEL_ERROR);
        //ak_print_set_syslog_level(MODULE_ID_ALL, LOG_LEVEL_ERROR);
        ak_print_set_level(MODULE_ID_ALL, LOG_LEVEL_NORMAL);
        ak_print_set_syslog_level(MODULE_ID_ALL, LOG_LEVEL_NORMAL);

#if ISP_USE_DMA
        dma_pool_init();
#endif

        if (vi_para_init(camera_idx, frame_rate) != 0) break;

        if (ak_vi_enable_dev(camera_idx) != 0) {
            ak_vi_disable_chn(VIDEO_CHN0);
            ak_vi_disable_chn(VIDEO_CHN1);
            ak_vi_close(camera_idx);
            LOG_E("%s, enable vi dev(%d) fail\n", __func__, camera_idx);
            break;
        }

        return 0;

    } while (0);

    /* process failure of sdk */
#if ISP_USE_DMA
    ak_mem_dma_pool_exit();
#endif
    ak_sdk_exit();

    return -1;
}

int py_video_camera_close(int camera_idx)
{
    ak_vi_disable_chn(VIDEO_CHN0);
    ak_vi_disable_chn(VIDEO_CHN1);
    ak_vi_disable_dev(camera_idx);
    ak_vi_close(camera_idx);
#if ISP_USE_DMA
    ak_mem_dma_pool_exit();
#endif
    if (cctx.codec >= 0) {
        ak_venc_close(cctx.codec);
        cctx.codec = -1;
    }
    ak_sdk_exit();

    return 0;
}

int py_video_camera_frame_get(isp_frame_t *frame)
{
    if (ak_vi_get_frame(VIDEO_CHN0, frame) != 0) {
        LOG_E("%s, get frame fail\n", __func__);
        return -1;
    }

    return 0;
}

int py_video_camera_frame_release(isp_frame_t *frame)
{
    ak_vi_release_frame(VIDEO_CHN0, frame);
    return 0;
}

static int save_data(const char *path, const char *extension, char *buf, unsigned int len)
{
    FILE *fd = NULL;
    struct ak_date date;
    char time_str[32] = {0};
    char file_path[64] = {0};

    /* construct file name */
    ak_get_localdate(&date);
    ak_date_to_string(&date, time_str);
    snprintf(file_path, sizeof(file_path), "/mnt/sdcard/%s_%s.%s", path, time_str, extension);

    /*
     * open appointed file to save YUV data
     * save main channel yuv here
     */
    fd = fopen(file_path, "wb+");
    if (fd == NULL) {
        LOG_E("%s, save file fail, fd:%d\n", __func__, fd);
        return -1;
    }

    do {
        len -= fwrite(buf, 1, len, fd);
    } while (len != 0);

    fclose(fd);

    return 0;
}

int py_video_camera_config_set(int width, int height, int media_type)
{
    int ret = 0;

    if (cctx.media_type != media_type) {
        cctx.media_type = media_type;
        ret = 1;
    }

    if (height != 0 && cctx.main_height != height) {
        cctx.main_height = height;
        ret = 1;
    }

    if (width != 0 && cctx.main_width != width) {
        cctx.main_width = width;
        ret = 1;
    }

    return ret;
}

int py_video_camera_config_get(int *width, int *height, int *media_type)
{
    if (width) *width = cctx.main_width;
    if (height) *height = cctx.main_height;
    if (media_type) *media_type = cctx.media_type;

    return 0;
}

int py_video_camera_save(isp_frame_t *frame, int pic_type, const char *path)
{
    int ret;

    if (pic_type != VIDEO_MEDIA_TYPE_YUV) {
        struct video_stream video_str;
        if (cctx.codec < 0) {
            cctx.codec = py_venc_init(cctx.main_width, cctx.main_height, 1, pic_type);
            if (cctx.codec < 0) {
                LOG_E("%s, open venc fail, fd:%d\n", __func__, cctx.codec);
                return -1;;
            }
        }

        memset(&video_str, 0, sizeof(video_str));
        ret = ak_venc_encode_frame(cctx.codec, frame->vi_frame.data, frame->vi_frame.len, NULL, &video_str);
        if (ret) {
            ak_venc_close(cctx.codec);
            cctx.codec = -1;
            LOG_E("%s, encode fail, ret:%d\n", __func__, ret);
            return -1;
        }
        ret = save_data(path, "jpg", video_str.data, video_str.len);
        ak_venc_release_stream(cctx.codec, &video_str);
    } else {
        ret = save_data(path, "yuv", frame->vi_frame.data, frame->vi_frame.len);
    }

    return ret;
}