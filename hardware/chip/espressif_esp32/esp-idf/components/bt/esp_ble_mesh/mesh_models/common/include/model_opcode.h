// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _MODEL_OPCODE_H_
#define _MODEL_OPCODE_H_

#include "mesh_access.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generic OnOff Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_ONOFF_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x01)
#define BLE_MESH_MODEL_OP_GEN_ONOFF_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x02)
#define BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x03)
#define BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x04)

/* Generic Level Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_LEVEL_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x05)
#define BLE_MESH_MODEL_OP_GEN_LEVEL_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x06)
#define BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x07)
#define BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x08)
#define BLE_MESH_MODEL_OP_GEN_DELTA_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x09)
#define BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x0A)
#define BLE_MESH_MODEL_OP_GEN_MOVE_SET                           BLE_MESH_MODEL_OP_2(0x82, 0x0B)
#define BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK                     BLE_MESH_MODEL_OP_2(0x82, 0x0C)

/* Generic Default Transition Time Message Opcode*/
#define BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET                 BLE_MESH_MODEL_OP_2(0x82, 0x0D)
#define BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET                 BLE_MESH_MODEL_OP_2(0x82, 0x0E)
#define BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK           BLE_MESH_MODEL_OP_2(0x82, 0x0F)
#define BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS              BLE_MESH_MODEL_OP_2(0x82, 0x10)

/* Generic Power OnOff Message Opcode*/
#define BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x11)
#define BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x12)

/* Generic Power OnOff Setup Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x13)
#define BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK                BLE_MESH_MODEL_OP_2(0x82, 0x14)

/* Generic Power Level Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x15)
#define BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x16)
#define BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x17)
#define BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x18)
#define BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x19)
#define BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS                  BLE_MESH_MODEL_OP_2(0x82, 0x1A)
#define BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x1B)
#define BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS               BLE_MESH_MODEL_OP_2(0x82, 0x1C)
#define BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x1D)
#define BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x1E)

/* Generic Power Level Setup Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET                  BLE_MESH_MODEL_OP_2(0x82, 0x1F)
#define BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK            BLE_MESH_MODEL_OP_2(0x82, 0x20)
#define BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x21)
#define BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x22)

/* Generic Battery Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_BATTERY_GET                        BLE_MESH_MODEL_OP_2(0x82, 0x23)
#define BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS                     BLE_MESH_MODEL_OP_2(0x82, 0x24)

/* Generic Location Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x25)
#define BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS                  BLE_MESH_MODEL_OP_1(0x40)
#define BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x26)
#define BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x27)

/* Generic Location Setup Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET                     BLE_MESH_MODEL_OP_1(0x41)
#define BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK               BLE_MESH_MODEL_OP_1(0x42)
#define BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x28)
#define BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK                BLE_MESH_MODEL_OP_2(0x82, 0x29)

/* Generic Manufacturer Property Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET                BLE_MESH_MODEL_OP_2(0x82, 0x2A)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS             BLE_MESH_MODEL_OP_1(0x43)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x2B)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET                  BLE_MESH_MODEL_OP_1(0x44)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK            BLE_MESH_MODEL_OP_1(0x45)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS               BLE_MESH_MODEL_OP_1(0x46)

/* Generic Admin Property Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET               BLE_MESH_MODEL_OP_2(0x82, 0x2C)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS            BLE_MESH_MODEL_OP_1(0x47)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET                 BLE_MESH_MODEL_OP_2(0x82, 0x2D)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET                 BLE_MESH_MODEL_OP_1(0x48)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK           BLE_MESH_MODEL_OP_1(0x49)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS              BLE_MESH_MODEL_OP_1(0x4A)

/* Generic User Property Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET                BLE_MESH_MODEL_OP_2(0x82, 0x2E)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS             BLE_MESH_MODEL_OP_1(0x4B)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x2F)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET                  BLE_MESH_MODEL_OP_1(0x4C)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK            BLE_MESH_MODEL_OP_1(0x4D)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS               BLE_MESH_MODEL_OP_1(0x4E)

/* Generic Client Property Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET              BLE_MESH_MODEL_OP_1(0x4F)
#define BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS           BLE_MESH_MODEL_OP_1(0x50)

/* Sensor Message Opcode */
#define BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x30)
#define BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS               BLE_MESH_MODEL_OP_1(0x51)
#define BLE_MESH_MODEL_OP_SENSOR_GET                             BLE_MESH_MODEL_OP_2(0x82, 0x31)
#define BLE_MESH_MODEL_OP_SENSOR_STATUS                          BLE_MESH_MODEL_OP_1(0x52)
#define BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x32)
#define BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS                   BLE_MESH_MODEL_OP_1(0x53)
#define BLE_MESH_MODEL_OP_SENSOR_SERIES_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x33)
#define BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS                   BLE_MESH_MODEL_OP_1(0x54)

/* Sensor Setup Message Opcode */
#define BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x34)
#define BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET                     BLE_MESH_MODEL_OP_1(0x55)
#define BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK               BLE_MESH_MODEL_OP_1(0x56)
#define BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS                  BLE_MESH_MODEL_OP_1(0x57)
#define BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x35)
#define BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS                 BLE_MESH_MODEL_OP_1(0x58)
#define BLE_MESH_MODEL_OP_SENSOR_SETTING_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x36)
#define BLE_MESH_MODEL_OP_SENSOR_SETTING_SET                     BLE_MESH_MODEL_OP_1(0x59)
#define BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK               BLE_MESH_MODEL_OP_1(0x5A)
#define BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS                  BLE_MESH_MODEL_OP_1(0x5B)

/* Time Message Opcode */
#define BLE_MESH_MODEL_OP_TIME_GET                               BLE_MESH_MODEL_OP_2(0x82, 0x37)
#define BLE_MESH_MODEL_OP_TIME_SET                               BLE_MESH_MODEL_OP_1(0x5C)
#define BLE_MESH_MODEL_OP_TIME_STATUS                            BLE_MESH_MODEL_OP_1(0x5D)
#define BLE_MESH_MODEL_OP_TIME_ROLE_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x38)
#define BLE_MESH_MODEL_OP_TIME_ROLE_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x39)
#define BLE_MESH_MODEL_OP_TIME_ROLE_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x3A)
#define BLE_MESH_MODEL_OP_TIME_ZONE_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x3B)
#define BLE_MESH_MODEL_OP_TIME_ZONE_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x3C)
#define BLE_MESH_MODEL_OP_TIME_ZONE_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x3D)
#define BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x3E)
#define BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x3F)
#define BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x40)

/* Scene Message Opcode */
#define BLE_MESH_MODEL_OP_SCENE_GET                              BLE_MESH_MODEL_OP_2(0x82, 0x41)
#define BLE_MESH_MODEL_OP_SCENE_RECALL                           BLE_MESH_MODEL_OP_2(0x82, 0x42)
#define BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK                     BLE_MESH_MODEL_OP_2(0x82, 0x43)
#define BLE_MESH_MODEL_OP_SCENE_STATUS                           BLE_MESH_MODEL_OP_1(0x5E)
#define BLE_MESH_MODEL_OP_SCENE_REGISTER_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x44)
#define BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS                  BLE_MESH_MODEL_OP_2(0x82, 0x45)

/* Scene Setup Message Opcode */
#define BLE_MESH_MODEL_OP_SCENE_STORE                            BLE_MESH_MODEL_OP_2(0x82, 0x46)
#define BLE_MESH_MODEL_OP_SCENE_STORE_UNACK                      BLE_MESH_MODEL_OP_2(0x82, 0x47)
#define BLE_MESH_MODEL_OP_SCENE_DELETE                           BLE_MESH_MODEL_OP_2(0x82, 0x9E)
#define BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK                     BLE_MESH_MODEL_OP_2(0x82, 0x9F)

/* Scheduler Message Opcode */
#define BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x48)
#define BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS                   BLE_MESH_MODEL_OP_1(0x5F)
#define BLE_MESH_MODEL_OP_SCHEDULER_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x49)
#define BLE_MESH_MODEL_OP_SCHEDULER_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x4A)

/* Scheduler Setup Message Opcode */
#define BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET                      BLE_MESH_MODEL_OP_1(0x60)
#define BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK                BLE_MESH_MODEL_OP_1(0x61)

/* Light Lightness Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x4B)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x4C)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x4D)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x4E)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET             BLE_MESH_MODEL_OP_2(0x82, 0x4F)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET             BLE_MESH_MODEL_OP_2(0x82, 0x50)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK       BLE_MESH_MODEL_OP_2(0x82, 0x51)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS          BLE_MESH_MODEL_OP_2(0x82, 0x52)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET               BLE_MESH_MODEL_OP_2(0x82, 0x53)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS            BLE_MESH_MODEL_OP_2(0x82, 0x54)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET            BLE_MESH_MODEL_OP_2(0x82, 0x55)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS         BLE_MESH_MODEL_OP_2(0x82, 0x56)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET              BLE_MESH_MODEL_OP_2(0x82, 0x57)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS           BLE_MESH_MODEL_OP_2(0x82, 0x58)

/* Light Lightness Setup Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET            BLE_MESH_MODEL_OP_2(0x82, 0x59)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK      BLE_MESH_MODEL_OP_2(0x82, 0x5A)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET              BLE_MESH_MODEL_OP_2(0x82, 0x5B)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK        BLE_MESH_MODEL_OP_2(0x82, 0x5C)

/* Light CTL Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_CTL_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x5D)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x5E)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x5F)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x60)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET              BLE_MESH_MODEL_OP_2(0x82, 0x61)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET        BLE_MESH_MODEL_OP_2(0x82, 0x62)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS     BLE_MESH_MODEL_OP_2(0x82, 0x63)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET              BLE_MESH_MODEL_OP_2(0x82, 0x64)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK        BLE_MESH_MODEL_OP_2(0x82, 0x65)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS           BLE_MESH_MODEL_OP_2(0x82, 0x66)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x67)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS               BLE_MESH_MODEL_OP_2(0x82, 0x68)

/* Light CTL Setup Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET                  BLE_MESH_MODEL_OP_2(0x82, 0x69)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK            BLE_MESH_MODEL_OP_2(0x82, 0x6A)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET        BLE_MESH_MODEL_OP_2(0x82, 0x6B)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK  BLE_MESH_MODEL_OP_2(0x82, 0x6C)

/* Light HSL Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_HSL_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x6D)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x6E)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x6F)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK                BLE_MESH_MODEL_OP_2(0x82, 0x70)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x71)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET               BLE_MESH_MODEL_OP_2(0x82, 0x72)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET               BLE_MESH_MODEL_OP_2(0x82, 0x73)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK         BLE_MESH_MODEL_OP_2(0x82, 0x74)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS            BLE_MESH_MODEL_OP_2(0x82, 0x75)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x76)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x77)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x78)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET                   BLE_MESH_MODEL_OP_2(0x82, 0x79)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS                BLE_MESH_MODEL_OP_2(0x82, 0x7A)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x7B)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS               BLE_MESH_MODEL_OP_2(0x82, 0x7C)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x7D)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x7E)

/* Light HSL Setup Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET                  BLE_MESH_MODEL_OP_2(0x82, 0x7F)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK            BLE_MESH_MODEL_OP_2(0x82, 0x80)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x81)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x82)

/* Light xyL Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_XYL_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x83)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x84)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x85)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x86)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET                   BLE_MESH_MODEL_OP_2(0x82, 0x87)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS                BLE_MESH_MODEL_OP_2(0x82, 0x88)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x89)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS               BLE_MESH_MODEL_OP_2(0x82, 0x8A)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x8B)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x8C)

/* Light xyL Setup Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET                  BLE_MESH_MODEL_OP_2(0x82, 0x8D)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK            BLE_MESH_MODEL_OP_2(0x82, 0x8E)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x8F)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x90)

/* Light Control Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x91)
#define BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x92)
#define BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK                BLE_MESH_MODEL_OP_2(0x82, 0x93)
#define BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x94)
#define BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET                        BLE_MESH_MODEL_OP_2(0x82, 0x95)
#define BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET                        BLE_MESH_MODEL_OP_2(0x82, 0x96)
#define BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK                  BLE_MESH_MODEL_OP_2(0x82, 0x97)
#define BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS                     BLE_MESH_MODEL_OP_2(0x82, 0x98)
#define BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET               BLE_MESH_MODEL_OP_2(0x82, 0x99)
#define BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET               BLE_MESH_MODEL_OP_2(0x82, 0x9A)
#define BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK         BLE_MESH_MODEL_OP_2(0x82, 0x9B)
#define BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS            BLE_MESH_MODEL_OP_2(0x82, 0x9C)
#define BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x9D)
#define BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET                  BLE_MESH_MODEL_OP_1(0x62)
#define BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK            BLE_MESH_MODEL_OP_1(0x63)
#define BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS               BLE_MESH_MODEL_OP_1(0x64)

#ifdef __cplusplus
}
#endif

#endif /* _MODEL_OPCODE_H_ */
