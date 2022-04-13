NAME := ulwip_service

$(NAME)_TYPE := app

$(NAME)_MBINS_TYPE := app

$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY :=
$(NAME)_MBINS_APP_LD_FILE := app1

ifeq ($(ENABLE_USPACE),1)

ifeq ($(APP_FORMAT),bin)
$(NAME)_SOURCES := src/entry_bin.c
else ifeq ($(APP_FORMAT),elf)
$(NAME)_SOURCES := src/entry_elf.c
else
$(error "Unknown app format")
endif

$(NAME)_COMPONENTS += uspace_lib
$(NAME)_COMPONENTS += usyscall
$(NAME)_COMPONENTS += mbedtls
$(NAME)_COMPONENTS += link_sdk

$(NAME)_SOURCES += src/app.c

ifeq ($(AOS_FEATURE_NETWORK_USPACE_VEXPRESS), y)
$(NAME)_SOURCES += ../../vendor/vexpress/mcu/drivers/connectivity/wlan/drv_smc911x.c
$(NAME)_SOURCES += ../../vendor/vexpress/mcu/drivers/connectivity/wlan/ethernetif.c
$(NAME)_SOURCES += ../../vendor/vexpress/mcu/hal/wifi_port_userspace.c
$(NAME)_INCLUDES += ../../vendor/vexpress/mcu/drivers/connectivity/wlan/
$(NAME)_INCLUDES += ../../components/network/netmgr/network/netmgr/
$(NAME)_INCLUDES += ../../components/network/http/include/
endif #AOS_FEATURE_NETWORK_USPACE_VEXPRESS

$(NAME)_INCLUDES += ../../components/network/lwip/include/lwip/apps/

ifeq ($(AOS_BOARD_HAAS100_MK), y)
$(NAME)_PREBUILT_LIBRARY += ../../vendor/haas/mcu/drivers/libhaas1000_wifi.a
$(NAME)_PREBUILT_LIBRARY += ../../vendor/haas/mcu/drivers/libhaas1000_wpa.a
endif #AOS_BOARD_HAAS100_MK

$(NAME)_INCLUDES +=  include

GLOBAL_CFLAGS += -DUSER_SPACE_DRIVER=1

ifeq ($(AOS_BOARD_HAAS100_MK), y)
GLOBAL_CFLAGS += -DENABLE_IO_MMAP=1
endif

GLOBAL_CFLAGS += -DFEATURE_NETWORK_USPACE

$(NAME)_CFLAGS += -g -O2 -Wall -Werror

endif

