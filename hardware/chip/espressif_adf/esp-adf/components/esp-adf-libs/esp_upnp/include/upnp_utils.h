// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _UPNP_UTILS_H
#define _UPNP_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "audio_mem.h"

#define UPNP_MEM_FREE(a) if(a) { audio_free(a); a = NULL; }
#define UPNP_MEM_CHECK(addr, action) if (addr == NULL) { ESP_LOGE(TAG, "Memory exhausted at %d", __LINE__); action; }

/**
 * File format for UPnP
 */
typedef struct {
	const char *data;		/*!< Data pointer */
	const int size;			/*!< File size */
	const char *mime_type;	/*!< MIME Types */
	const char *path;		/*!< File path */
} upnp_file_info_t;

/**
 * @brief      Check the string ``str``  if it ends with ``suffix``
 *
 * @param[in]  str      The string to check
 * @param[in]  suffix   The suffix to look for
 *
 * @return     true or false
 */
bool utils_ends_with(const char *str, const char *suffix);

/**
 * @brief      Remove white space at begin and end of string
 *
 * @param      str   The string
 *
 * @return     the str pointer returned is difference pointer with the input string
 */
char *utils_trim(char *str);

#ifdef __cplusplus
}
#endif

#endif
