/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _SDCARD_SCAN_H_
#define _SDCARD_SCAN_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*sdcard_scan_cb_t)(void *user_data, char *url);

/**
 * @brief Scan files in SD card and use callback function to save files that meet filtering conditions.
 *
 * @param cb              The callback function
 * @param path            The path to be scanned
 * @param depth           The depth of file scanning                             // .e.g. if you only want to save files in "/test" , depth = 0.
 *                                                                               //       if you want to save files in "/test/scan_test/", depth = 1
 * @param file_extension  File extension of files that are supposed to be saved  // .e.g. const char *[]{"mp3", "aac"}
 * @param filter_num      Number of filters
 * @param user_data       The data to be used by callback function
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 *
 * //example  sdcard_scan(callback, "/sdcard", 5, const char *[]{"mp3", "aac"}, 2, user_data);
 * Scan 5 levels folder in sdcard and save mp3 files and aac files.
 */
esp_err_t sdcard_scan(sdcard_scan_cb_t cb, const char *path, int depth, const char *file_extension[], int filter_num, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
