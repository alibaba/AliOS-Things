#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_camera.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    IDLE = 0,
    DETECT,
    ENROLL,
    RECOGNIZE,
    DELETE,
} recognizer_state_t;

typedef enum
{
    SHOW_STATE_IDLE,
    SHOW_STATE_DELETE,
    SHOW_STATE_RECOGNIZE,
    SHOW_STATE_ENROLL,
} show_state_t;

bool human_face_detect(camera_fb_t *frame);
int human_face_recognize_init();
int human_face_enroll(camera_fb_t *frame);
int human_face_recognize(camera_fb_t *frame);
int human_face_delete(camera_fb_t *frame);
void rgb_printf_result(camera_fb_t *frame, show_state_t frame_show_state);

#ifdef __cplusplus
} // extern "C"
#endif
