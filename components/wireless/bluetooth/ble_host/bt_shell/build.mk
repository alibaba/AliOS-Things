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

L_MODULE := libbt_shell

L_CFLAGS := -Wall

include $(L_PATH)/../bt_defconfig

L_INCS +=  $(L_PATH)/../include $(L_PATH)/../bt_host/port/include $(L_PATH)/../bt_host/port/aos/include $(L_PATH)/../bt_host \
			$(L_PATH)/../bt_host/host

ifeq ($(CONFIG_BT_SHELL), y)
L_INCS +=   $(L_PATH)/bt_host
ifeq ($(CONFIG_BT_TEST), y)
L_INCS +=   $(L_PATH)/bt_host/test
L_SRCS +=	bt_host/test/bt.c
else
L_SRCS +=	bt_host/bt.c
endif

ifeq ($(CONFIG_BT_CONN), y)
ifeq ($(CONFIG_BT_TEST), y)
L_SRCS +=	bt_host/test/gatt.c
else
L_SRCS +=	bt_host/gatt.c
endif
endif
endif

ifeq ($(CONFIG_BT_MESH), y)
	ifeq ($(CONFIG_BT_MESH_SHELL), y)
	L_SRCS +=	bt_mesh/shell.c
	endif
endif

include $(BUILD_MODULE)

endif
