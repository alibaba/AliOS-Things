/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#if defined(NEW_NV_RECORD_ENALBED)
#if defined(TGENIE_MESH_DATA_SECTION)

#ifndef __NVRECORD_MESH_DATA_H__
#define __NVRECORD_MESH_DATA_H__

#include "nvrecord_extension.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    GENIE_FLASH_SUCCESS,
    GENIE_FLASH_FAIL,
}GENIE_STATUS;

GENIE_STATUS nvrecord_mesh_data_init(void);
GENIE_STATUS nvrecord_reset_mesh_configuration(void);
GENIE_STATUS nvrecord_get_onoff_times(uint8_t* times);
GENIE_STATUS nvrecord_set_onoff_times(uint8_t times);
GENIE_STATUS nvrecord_rebuild_mesh_configuration(NV_MESH_DATA_T* pMeshData);
GENIE_STATUS nvrecord_get_onoff_status(uint8_t* onoff);
GENIE_STATUS nvrecord_set_onoff_status(uint8_t onoff);
GENIE_STATUS nvrecord_get_lightness(uint16_t* lightness);
GENIE_STATUS nvrecord_set_lightness(uint16_t lightness);
GENIE_STATUS nvrecord_get_addr(uint16_t* addr);
GENIE_STATUS nvrecord_set_addr(uint16_t addr);
GENIE_STATUS nvrecord_get_seq(uint32_t* seq);
GENIE_STATUS nvrecord_set_seq(uint32_t seq);

GENIE_STATUS nvrecord_get_light_powerup(MESH_POWERUP* powerup);
GENIE_STATUS nvrecord_set_light_powerup(MESH_POWERUP* powerup);

GENIE_STATUS nvrecord_get_mesh_param(uint8_t* mesh_data,uint32_t len);
GENIE_STATUS nvrecord_set_mesh_param(uint8_t* mesh_data,uint32_t len);

GENIE_STATUS nvrecord_get_time_scene(MESH_TIME_SCENE* time_scene,uint32_t len);
GENIE_STATUS nvrecord_set_time_scene(MESH_TIME_SCENE* time_scene,uint32_t len);


#ifdef __cplusplus
}
#endif
#endif
#endif  // #if defined(TGENIE_MESH_DATA_SECTION)
#endif  // #if defined(NEW_NV_RECORD_ENALBED)

