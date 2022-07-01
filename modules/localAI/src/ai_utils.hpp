#pragma once

#include <list>
#include "dl_detect_define.hpp"
#include "esp_camera.h"

/**
 * @brief Draw detection result on RGB565 image.
 *
 * @param image_ptr     image
 * @param image_height  height of image
 * @param image_width   width of image
 * @param results       detection results
 */
void draw_detection_result(uint16_t *image_ptr, int image_height, int image_width, std::list<dl::detect::result_t> &results);

/**
 * @brief Draw detection result on RGB888 image.
 *
 * @param image_ptr     image
 * @param image_height  height of image
 * @param image_width   width of image
 * @param results       detection results
 */

void draw_detection_result(uint8_t *image_ptr, int image_height, int image_width, std::list<dl::detect::result_t> &results);

/**
 * @brief Print detection result in terminal
 *
 * @param results detection results
 */
void print_detection_result(std::list<dl::detect::result_t> &results);

/**
 * @brief Decode fb ,
 *        - if fb->format == PIXFORMAT_RGB565, then return fb->buf
 *        - else, then return a new memory with RGB888, don't forget to free it
 *
 * @param fb
 */
void *app_camera_decode(camera_fb_t *fb);
