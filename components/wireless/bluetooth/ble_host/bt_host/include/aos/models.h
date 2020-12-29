/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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

#ifndef __MODELS_H__
#define __MODELS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* SIG Foundation Models, SIG Mesh Profile SPEC v1.0, 4.5.5 */

/* Configuration Server  */
#define MESH_MODEL_ID_CFG_SRV                   0x0000
/* Configuration Client */
#define MESH_MODEL_ID_CFG_CLI                   0x0001
/* Health Server */
#define MESH_MODEL_ID_HEALTH_SRV                0x0002
/* Health Client */
#define MESH_MODEL_ID_HEALTH_CLI                0x0003

/* SIG Mesh Models,  SIG Mesh Model SPEC v1.0, 7.3 */
#define MESH_MODEL_ID_GEN_ONOFF_SRV             0x1000
#define MESH_MODEL_ID_GEN_ONOFF_CLI             0x1001
#define MESH_MODEL_ID_GEN_LEVEL_SRV             0x1002
#define MESH_MODEL_ID_GEN_LEVEL_CLI             0x1003
#define MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV    0x1004
#define MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI    0x1005
#define MESH_MODEL_ID_GEN_POWER_ONOFF_SRV       0x1006
#define MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV 0x1007
#define MESH_MODEL_ID_GEN_POWER_ONOFF_CLI       0x1008
#define MESH_MODEL_ID_GEN_POWER_LEVEL_SRV       0x1009
#define MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV 0x100a
#define MESH_MODEL_ID_GEN_POWER_LEVEL_CLI       0x100b
#define MESH_MODEL_ID_GEN_BATTERY_SRV           0x100c
#define MESH_MODEL_ID_GEN_BATTERY_CLI           0x100d
#define MESH_MODEL_ID_GEN_LOCATION_SRV          0x100e
#define MESH_MODEL_ID_GEN_LOCATION_SETUPSRV     0x100f
#define MESH_MODEL_ID_GEN_LOCATION_CLI          0x1010
#define MESH_MODEL_ID_GEN_ADMIN_PROP_SRV        0x1011
#define MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV 0x1012
#define MESH_MODEL_ID_GEN_USER_PROP_SRV         0x1013
#define MESH_MODEL_ID_GEN_CLIENT_PROP_SRV       0x1014
#define MESH_MODEL_ID_GEN_PROP_CLI              0x1015
#define MESH_MODEL_ID_SENSOR_SRV                0x1100
#define MESH_MODEL_ID_SENSOR_SETUP_SRV          0x1101
#define MESH_MODEL_ID_SENSOR_CLI                0x1102
#define MESH_MODEL_ID_TIME_SRV                  0x1200
#define MESH_MODEL_ID_TIME_SETUP_SRV            0x1201
#define MESH_MODEL_ID_TIME_CLI                  0x1202
#define MESH_MODEL_ID_SCENE_SRV                 0x1203
#define MESH_MODEL_ID_SCENE_SETUP_SRV           0x1204
#define MESH_MODEL_ID_SCENE_CLI                 0x1205
#define MESH_MODEL_ID_SCHEDULER_SRV             0x1206
#define MESH_MODEL_ID_SCHEDULER_SETUP_SRV       0x1207
#define MESH_MODEL_ID_SCHEDULER_CLI             0x1208
#define MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV       0x1300
#define MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV 0x1301
#define MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI       0x1302
#define MESH_MODEL_ID_LIGHT_CTL_SRV             0x1303
#define MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV       0x1304
#define MESH_MODEL_ID_LIGHT_CTL_CLI             0x1305
#define MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV        0x1306
#define MESH_MODEL_ID_LIGHT_HSL_SRV             0x1307
#define MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV       0x1308
#define MESH_MODEL_ID_LIGHT_HSL_CLI             0x1309
#define MESH_MODEL_ID_LIGHT_HSL_HUE_SRV         0x130a
#define MESH_MODEL_ID_LIGHT_HSL_SAT_SRV         0x130b
#define MESH_MODEL_ID_LIGHT_XYL_SRV             0x130c
#define MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV       0x130d
#define MESH_MODEL_ID_LIGHT_XYL_CLI             0x130e
#define MESH_MODEL_ID_LIGHT_LC_SRV              0x130f
#define MESH_MODEL_ID_LIGHT_LC_SETUPSRV         0x1310
#define MESH_MODEL_ID_LIGHT_LC_CLI              0x1311

#ifndef BIT
#define BIT(s) (1 << (s))
#endif

/** @def MESH_MODEL_OP1
 *  @brief 1-octet opcodes
 *
 *  used for Bluetooth SIG defined application opcodes
 *
 *  @param b0 1-octet opcode, 0x7F is reserved
 */
#define MESH_MODEL_OP1(b0) (b0)

/** @def MESH_MODEL_OP2
 *  @brief 2-octet opcodes
 *
 *  used for Bluetooth SIG defined application opcodes
 *
 *  @param b0 MSB octet of the 2-octets
 *  @param b1 LSB octet of the 2-octets
 */
#define MESH_MODEL_OP2(b0, b1) (((b0) << 8) | (b1))

/** @def MESH_MODEL_OP3
 *  @brief 3-octet opcodes
 *
 *  used for manufacturer-specific opcodes
 *
 *  @param b0 opcode, 0~63 is vaild
 *  @param cid company identifiers, 16-bit values defined by the Bluetooth SIG
 */
#define MESH_MODEL_OP3(b0, cid) ((((b0) << 16) | 0xc00000) | (cid))

/* SIG Mesh opcodes, SIG Mesh Profile SPEC v1.0, 4.3.4 */
#define OP_APP_KEY_ADD                     MESH_MODEL_OP1(0x00)
#define OP_APP_KEY_UPDATE                  MESH_MODEL_OP1(0x01)
#define OP_DEV_COMP_DATA_STATUS            MESH_MODEL_OP1(0x02)
#define OP_MOD_PUB_SET                     MESH_MODEL_OP1(0x03)
#define OP_HEALTH_CURRENT_STATUS           MESH_MODEL_OP1(0x04)
#define OP_HEALTH_FAULT_STATUS             MESH_MODEL_OP1(0x05)
#define OP_HEARTBEAT_PUB_STATUS            MESH_MODEL_OP1(0x06)
#define OP_APP_KEY_DEL                     MESH_MODEL_OP2(0x80, 0x00)
#define OP_APP_KEY_GET                     MESH_MODEL_OP2(0x80, 0x01)
#define OP_APP_KEY_LIST                    MESH_MODEL_OP2(0x80, 0x02)
#define OP_APP_KEY_STATUS                  MESH_MODEL_OP2(0x80, 0x03)
#define OP_ATTENTION_GET                   MESH_MODEL_OP2(0x80, 0x04)
#define OP_ATTENTION_SET                   MESH_MODEL_OP2(0x80, 0x05)
#define OP_ATTENTION_SET_UNREL             MESH_MODEL_OP2(0x80, 0x06)
#define OP_ATTENTION_STATUS                MESH_MODEL_OP2(0x80, 0x07)
#define OP_DEV_COMP_DATA_GET               MESH_MODEL_OP2(0x80, 0x08)
#define OP_BEACON_GET                      MESH_MODEL_OP2(0x80, 0x09)
#define OP_BEACON_SET                      MESH_MODEL_OP2(0x80, 0x0a)
#define OP_BEACON_STATUS                   MESH_MODEL_OP2(0x80, 0x0b)
#define OP_DEFAULT_TTL_GET                 MESH_MODEL_OP2(0x80, 0x0c)
#define OP_DEFAULT_TTL_SET                 MESH_MODEL_OP2(0x80, 0x0d)
#define OP_DEFAULT_TTL_STATUS              MESH_MODEL_OP2(0x80, 0x0e)
#define OP_FRIEND_GET                      MESH_MODEL_OP2(0x80, 0x0f)
#define OP_FRIEND_SET                      MESH_MODEL_OP2(0x80, 0x10)
#define OP_FRIEND_STATUS                   MESH_MODEL_OP2(0x80, 0x11)
#define OP_GATT_PROXY_GET                  MESH_MODEL_OP2(0x80, 0x12)
#define OP_GATT_PROXY_SET                  MESH_MODEL_OP2(0x80, 0x13)
#define OP_GATT_PROXY_STATUS               MESH_MODEL_OP2(0x80, 0x14)
#define OP_KRP_GET                         MESH_MODEL_OP2(0x80, 0x15)
#define OP_KRP_SET                         MESH_MODEL_OP2(0x80, 0x16)
#define OP_KRP_STATUS                      MESH_MODEL_OP2(0x80, 0x17)
#define OP_MOD_PUB_GET                     MESH_MODEL_OP2(0x80, 0x18)
#define OP_MOD_PUB_STATUS                  MESH_MODEL_OP2(0x80, 0x19)
#define OP_MOD_PUB_VA_SET                  MESH_MODEL_OP2(0x80, 0x1a)
#define OP_MOD_SUB_ADD                     MESH_MODEL_OP2(0x80, 0x1b)
#define OP_MOD_SUB_DEL                     MESH_MODEL_OP2(0x80, 0x1c)
#define OP_MOD_SUB_DEL_ALL                 MESH_MODEL_OP2(0x80, 0x1d)
#define OP_MOD_SUB_OVERWRITE               MESH_MODEL_OP2(0x80, 0x1e)
#define OP_MOD_SUB_STATUS                  MESH_MODEL_OP2(0x80, 0x1f)
#define OP_MOD_SUB_VA_ADD                  MESH_MODEL_OP2(0x80, 0x20)
#define OP_MOD_SUB_VA_DEL                  MESH_MODEL_OP2(0x80, 0x21)
#define OP_MOD_SUB_VA_OVERWRITE            MESH_MODEL_OP2(0x80, 0x22)
#define OP_NET_TRANSMIT_GET                MESH_MODEL_OP2(0x80, 0x23)
#define OP_NET_TRANSMIT_SET                MESH_MODEL_OP2(0x80, 0x24)
#define OP_NET_TRANSMIT_STATUS             MESH_MODEL_OP2(0x80, 0x25)
#define OP_RELAY_GET                       MESH_MODEL_OP2(0x80, 0x26)
#define OP_RELAY_SET                       MESH_MODEL_OP2(0x80, 0x27)
#define OP_RELAY_STATUS                    MESH_MODEL_OP2(0x80, 0x28)
#define OP_MOD_SUB_GET                     MESH_MODEL_OP2(0x80, 0x29)
#define OP_MOD_SUB_LIST                    MESH_MODEL_OP2(0x80, 0x2a)
#define OP_MOD_SUB_GET_VND                 MESH_MODEL_OP2(0x80, 0x2b)
#define OP_MOD_SUB_LIST_VND                MESH_MODEL_OP2(0x80, 0x2c)
#define OP_LPN_TIMEOUT_GET                 MESH_MODEL_OP2(0x80, 0x2d)
#define OP_LPN_TIMEOUT_STATUS              MESH_MODEL_OP2(0x80, 0x2e)
#define OP_HEALTH_FAULT_CLEAR              MESH_MODEL_OP2(0x80, 0x2f)
#define OP_HEALTH_FAULT_CLEAR_UNREL        MESH_MODEL_OP2(0x80, 0x30)
#define OP_HEALTH_FAULT_GET                MESH_MODEL_OP2(0x80, 0x31)
#define OP_HEALTH_FAULT_TEST               MESH_MODEL_OP2(0x80, 0x32)
#define OP_HEALTH_FAULT_TEST_UNREL         MESH_MODEL_OP2(0x80, 0x33)
#define OP_HEALTH_PERIOD_GET               MESH_MODEL_OP2(0x80, 0x34)
#define OP_HEALTH_PERIOD_SET               MESH_MODEL_OP2(0x80, 0x35)
#define OP_HEALTH_PERIOD_SET_UNREL         MESH_MODEL_OP2(0x80, 0x36)
#define OP_HEALTH_PERIOD_STATUS            MESH_MODEL_OP2(0x80, 0x37)
#define OP_HEARTBEAT_PUB_GET               MESH_MODEL_OP2(0x80, 0x38)
#define OP_HEARTBEAT_PUB_SET               MESH_MODEL_OP2(0x80, 0x39)
#define OP_HEARTBEAT_SUB_GET               MESH_MODEL_OP2(0x80, 0x3a)
#define OP_HEARTBEAT_SUB_SET               MESH_MODEL_OP2(0x80, 0x3b)
#define OP_HEARTBEAT_SUB_STATUS            MESH_MODEL_OP2(0x80, 0x3c)
#define OP_MOD_APP_BIND                    MESH_MODEL_OP2(0x80, 0x3d)
#define OP_MOD_APP_STATUS                  MESH_MODEL_OP2(0x80, 0x3e)
#define OP_MOD_APP_UNBIND                  MESH_MODEL_OP2(0x80, 0x3f)
#define OP_NET_KEY_ADD                     MESH_MODEL_OP2(0x80, 0x40)
#define OP_NET_KEY_DEL                     MESH_MODEL_OP2(0x80, 0x41)
#define OP_NET_KEY_GET                     MESH_MODEL_OP2(0x80, 0x42)
#define OP_NET_KEY_LIST                    MESH_MODEL_OP2(0x80, 0x43)
#define OP_NET_KEY_STATUS                  MESH_MODEL_OP2(0x80, 0x44)
#define OP_NET_KEY_UPDATE                  MESH_MODEL_OP2(0x80, 0x45)
#define OP_NODE_IDENTITY_GET               MESH_MODEL_OP2(0x80, 0x46)
#define OP_NODE_IDENTITY_SET               MESH_MODEL_OP2(0x80, 0x47)
#define OP_NODE_IDENTITY_STATUS            MESH_MODEL_OP2(0x80, 0x48)
#define OP_NODE_RESET                      MESH_MODEL_OP2(0x80, 0x49)
#define OP_NODE_RESET_STATUS               MESH_MODEL_OP2(0x80, 0x4a)
#define OP_SIG_MOD_APP_GET                 MESH_MODEL_OP2(0x80, 0x4b)
#define OP_SIG_MOD_APP_LIST                MESH_MODEL_OP2(0x80, 0x4c)
#define OP_VND_MOD_APP_GET                 MESH_MODEL_OP2(0x80, 0x4d)
#define OP_VND_MOD_APP_LIST                MESH_MODEL_OP2(0x80, 0x4e)

#ifdef __cplusplus
}
#endif

#endif   /* __MODELS_H__ */

