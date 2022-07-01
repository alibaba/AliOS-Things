#include "human_face_recognition.h"

#include "esp_log.h"
#include "esp_camera.h"

#include "dl_image.hpp"
#include "fb_gfx.h"

#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "face_recognition_tool.hpp"

// #if CONFIG_MFN_V1
// #if CONFIG_S8
#include "face_recognition_112_v1_s8.hpp"
// #elif CONFIG_S16
// #include "face_recognition_112_v1_s16.hpp"
// #endif
// #endif

#include "ai_utils.hpp"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
using namespace dl;
static face_info_t recognize_result;
static HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
static HumanFaceDetectMNP01 detector2(0.4F, 0.3F, 10);

// #if CONFIG_MFN_V1
// #if CONFIG_S8
FaceRecognition112V1S8 *recognizer = NULL;
// #elif CONFIG_S16
//     FaceRecognition112V1S16 *recognizer = NULL;
// #endif
// #endif

#define RGB565_MASK_RED 0xF800
#define RGB565_MASK_GREEN 0x07E0
#define RGB565_MASK_BLUE 0x001F
#define FRAME_DELAY_NUM 16

static void rgb_print(camera_fb_t *fb, uint32_t color, const char *str)
{
    fb_gfx_print(fb, (fb->width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(camera_fb_t *fb, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    rgb_print(fb, color, temp);
    if (len > 64)
    {
        free(temp);
    }
    return len;
}

int human_face_recognize_init()
{
// #if CONFIG_MFN_V1
// #if CONFIG_S8
    recognizer = new FaceRecognition112V1S8();
// #elif CONFIG_S16
//     recognizer = new FaceRecognition112V1S16();
// #endif
// #endif
    recognizer->set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
    int partition_result = recognizer->set_ids_from_flash();
    return partition_result;
}

bool human_face_detect(camera_fb_t *frame)
{
    std::list<dl::detect::result_t> &detect_candidates = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
    std::list<dl::detect::result_t> &detect_results = detector2.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

    if (detect_results.size())
    {
        draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
        return true;
    }
    else{
        return false;
    }
}

int human_face_enroll(camera_fb_t *frame)
{
    std::list<dl::detect::result_t> &detect_candidates = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
    std::list<dl::detect::result_t> &detect_results = detector2.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

    if (detect_results.size())
    {
        draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
    }

    if (detect_results.size() == 1){
        recognize_result = recognizer->recognize((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
        print_detection_result(detect_results);

        if (recognize_result.id > 0){
            ESP_LOGI("RECOGNIZE", "Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
        }
        else{
            ESP_LOGE("RECOGNIZE", "Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
        }

        recognizer->enroll_id((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint, "", true);
        ESP_LOGW("ENROLL", "ID %d is enrolled", recognizer->get_enrolled_ids().back().id);
        return recognizer->get_enrolled_ids().back().id;
    }
    else{
        ESP_LOGW("ENROLL", "Failed, can not detect human face");
        return -1;
    }
}

int human_face_recognize(camera_fb_t *frame)
{
    int64_t t1 = 0;
    int64_t t2 = 0;
    std::list<dl::detect::result_t> &detect_candidates = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
    std::list<dl::detect::result_t> &detect_results = detector2.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

    if (detect_results.size())
    {
        draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
    }

    if (detect_results.size() == 1){
        t1 = esp_timer_get_time();
        recognize_result = recognizer->recognize((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
        t2 = esp_timer_get_time();
        ESP_LOGI("RECOGNIZE", "Recognize time %llu ms", (t2 - t1) / 1000);
        print_detection_result(detect_results);

        if (recognize_result.id > 0){
            ESP_LOGI("RECOGNIZE", "Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
        }
        else{
            ESP_LOGE("RECOGNIZE", "Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
        }
    }
    else{
        ESP_LOGW("RECOGNIZE", "Failed, can not detect human face");
        recognize_result.id = -1;
    }
    return recognize_result.id;
}

int human_face_delete(camera_fb_t *frame)
{
    recognizer->delete_id(true);
    ESP_LOGE("DELETE", "%d IDs left", recognizer->get_enrolled_id_num());
    //rgb_printf(frame, RGB565_MASK_RED, "%d IDs left", recognizer->get_enrolled_id_num());
    return recognizer->get_enrolled_id_num();
}

void rgb_printf_result(camera_fb_t *frame, show_state_t frame_show_state)
{
    switch (frame_show_state)
    {
        case SHOW_STATE_DELETE:
            rgb_printf(frame, RGB565_MASK_RED, "%d IDs left", recognizer->get_enrolled_id_num());
            break;

        case SHOW_STATE_RECOGNIZE:
            if (recognize_result.id > 0)
                rgb_printf(frame, RGB565_MASK_GREEN, "ID %d", recognize_result.id);
            else
                rgb_print(frame, RGB565_MASK_RED, "who ?");
            break;

        case SHOW_STATE_ENROLL:
            rgb_printf(frame, RGB565_MASK_BLUE, "Enroll: ID %d", recognizer->get_enrolled_ids().back().id);
            break;

        default:
            break;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
