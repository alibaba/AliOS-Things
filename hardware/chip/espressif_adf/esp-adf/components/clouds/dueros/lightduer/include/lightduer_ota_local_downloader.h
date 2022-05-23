/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * File: lightduer_ota_local_downloader.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: OTA local Downloader Head File
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_LOCAL_DOWNLOADER_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_LOCAL_DOWNLOADER_H

#include "lightduer_ota_downloader.h"

/*
 * Initialise local Downloader
 *
 * @param void:
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_init_local_downloader(void);

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_HTTP_DOWNLOADER_H
