##
 # Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #   http://www.apache.crg/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

ifeq ($(CONFIG_BT), y)

L_PATH := $(call cur-dir)

include $(DEFINE_LOCAL)

L_MODULE := libbt_mesh

L_CFLAGS := -Wall

#L_CFLAGS := -D__ORDER_LITTLE_ENDIAN__=1 -D__ORDER_BIG_ENDIAN__=2 -D__BYTE_ORDER__=__ORDER_LITTLE_ENDIAN__

include $(L_PATH)/../bt_defconfig

L_INCS += $(L_PATH)/inc $(L_PATH)/inc/api $(L_PATH)/inc/port $(L_PATH)/../bt_host/port/include $(L_PATH)/../bt_host/port/aos/include $(L_PATH)/../bt_host/host $(L_PATH)/../crypto  $(L_PATH)/../bt_host/
L_INCS += $(L_PATH)/../bt_crypto/include

ifeq ($(BT_HOST_CRYPTO), y)
L_INCS += $(L_PATH)/../crypto/tinycrypt/include
endif

ifeq ($(CONFIG_BT_MESH), y)
		L_SRCS +=	src/main.c \
			src/adv.c \
			src/beacon.c \
			src/net.c \
			src/transport.c \
			src/crypto.c \
			src/access.c \
			src/cfg_srv.c \
			src/health_srv.c

		ifeq ($(CONFIG_BT_SETTINGS), y)
			L_SRCS +=	src/settings.c
		endif

		ifeq ($(CONFIG_BT_MESH_LOW_POWER), y)
			L_SRCS +=	src/lpn.c
		endif

		ifeq ($(CONFIG_BT_MESH_FRIEND), y)
			L_SRCS +=	src/friend.c
		endif

		ifeq ($(CONFIG_BT_MESH_PROV), y)
			L_SRCS +=	src/prov.c
		endif
		ifeq ($(CONFIG_BT_MESH_PROXY), y)
			L_SRCS +=	src/proxy.c
		endif
		ifeq ($(CONFIG_BT_MESH_CFG_CLI), y)
			L_SRCS +=	src/cfg_cli.c
		endif
		ifeq ($(CONFIG_BT_MESH_HEALTH_CLI), y)
			L_SRCS +=	src/health_cli.c
		endif
		ifeq ($(CONFIG_BT_MESH_SELF_TEST), y)
			L_SRCS +=	src/test.c
		endif

		ifeq ($(CONFIG_BT_MESH_PROVISIONER), y)
		L_SRCS +=	src/provisioner_beacon.c \
				src/provisioner_main.c \
				src/provisioner_prov.c \
				src/provisioner_proxy.c \
				ref_impl/mesh_hal_ble.c
		endif

		ifeq ($(CONFIG_BT_MESH_EVENT_CALLBACK), y)
			L_SRCS +=	ref_impl/mesh_event_port.c
		endif
		L_SRCS +=	aos/mesh.c
endif

include $(BUILD_MODULE)

endif
