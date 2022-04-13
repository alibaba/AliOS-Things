##
 # Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #   http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

L_PATH := $(call cur-dir)

include $(DEFINE_LOCAL)

L_MODULE := libbt_crypto

L_CFLAGS := -Wall

L_INCS += $(L_PATH)/../bt_host/port/include $(L_PATH)/../bt_host/port/aos/include \
			$(L_PATH)/../bt_host/host $(L_PATH)/../crypto  $(L_PATH)/../bt_host/

L_INCS += $(L_PATH)/include $(L_PATH)/tinycrypt/include

L_SRCS += bt_crypto.c

include $(BUILD_MODULE)

$(call include-all-subdir-makefiles,$(L_PATH))

