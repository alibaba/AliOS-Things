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

L_MODULE := libbt_host

L_CFLAGS := -Wall

include $(L_PATH)/../bt_defconfig

#L_CFLAGS := -D__ORDER_LITTLE_ENDIAN__=1 -D__ORDER_BIG_ENDIAN__=2 -D__BYTE_ORDER__=__ORDER_LITTLE_ENDIAN__

L_INCS += $(L_PATH)/../include $(L_PATH)/include $(L_PATH)/port/include $(L_PATH)/../crypto $(L_PATH) $(L_PATH)/include/drivers \
			$(L_PATH)/host
L_INCS += $(L_PATH)/../bt_crypto/include

L_SRCS +=	port/core/buf.c \
			port/core/atomic_c.c \
			hci_driver/h4_driver.c

ifeq ($(CONFIG_OS_CSI), y)
L_SRCS +=	port/csi/csi_port.c \
			port/csi/work.c
L_INCS +=   $(L_PATH)/port/csi
else
L_SRCS +=	port/aos/aos_port.c \
			port/aos/work.c  \
			port/aos/poll.c
L_INCS +=   $(L_PATH)/port/aos
L_INCS +=   $(L_PATH)/port/aos/include $(L_PATH)/../../../../csi/csi_kernel/rhino/core/include  $(L_PATH)/../../../../csi/csi_kernel/rhino/arch/include
endif

#L_SRCS += common/dummy.c

L_SRCS +=	common/log.c

	L_SRCS +=	common/rpa.c

ifeq ($(CONFIG_BT_HCI_RAW), y)
	L_SRCS +=	host/hci_raw.c
endif

ifeq ($(CONFIG_BT_DEBUG_MONITOR), y)
	L_SRCS +=	host/monitor.c
endif

ifeq ($(CONFIG_BT_TINYCRYPT_ECC), y)
	L_INCS +=	$(L_PATH)/../bt_crypto/tinycrypt/include
	L_SRCS +=	host/hci_ecc.c
endif

ifeq ($(CONFIG_BT_A2DP), y)
		L_SRCS +=	host/a2dp.c
endif

ifeq ($(CONFIG_BT_AVDTP), y)
		L_SRCS +=	host/avdtp.c
endif

ifeq ($(CONFIG_BT_RFCOMM), y)
		L_SRCS +=	host/rfcomm.c
endif

ifeq ($(CONFIG_BT_TESTING), y)
		L_SRCS +=	host/testing.c
endif

ifeq ($(CONFIG_BT_SETTINGS), y)
		L_SRCS +=	host/settings.c
endif

ifeq ($(CONFIG_BT_BREDR), y)
	L_SRCS +=	host/keys_br.c \
				host/l2cap_br.c \
				host/sdp.c
endif

ifeq ($(CONFIG_BT_HFP_HF), y)
	L_SRCS +=	host/hfp_hf.c \
				host/at.c
endif

ifeq ($(CONFIG_BT_HCI_HOST), y)
	L_SRCS +=	host/uuid.c \
				host/hci_api.c \
				host/hci_core.c

	L_SRCS +=	host/crypto.c

	ifeq ($(CONFIG_BT_CONN), y)
		L_SRCS +=	host/conn.c \
					host/l2cap.c \
					host/att.c \
					host/gatt.c


		ifeq ($(CONFIG_BT_SMP), y)
			L_SRCS +=	host/smp.c \
						host/keys.c
		else
			L_SRCS +=	host/smp_null.c
		endif
	endif
endif

ifeq ($(CONFIG_SETTINGS), y)
L_INCS +=   $(L_PATH)/port/core/settings/src \
			$(L_PATH)/port/core/settings/include

L_SRCS +=	port/core/settings/src/settings_store.c \
			port/core/settings/src/settings.c \
			port/core/settings/src/settings_init.c \
			port/core/settings/src/settings_line.c \
			port/core/settings/src/settings_kv.c
endif

L_SRCS +=	aos/ble.c

include $(BUILD_MODULE)

gen_btconfig:
	@make -C kernel/protocols/bluetooth/

endif

L_PATH := $(call cur-dir)

L_POSIX_PATH := $(L_PATH)

$(call include-all-subdir-makefiles,$(L_PATH))
