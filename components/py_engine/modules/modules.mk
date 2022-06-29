NAME ?= modules

MICROPY_PY_ALIYUNIOT ?= n

# modules
ifeq ($(MICROPY_PY_AIAGENT), y)
    CFLAGS_HAAS += -DMICROPY_PY_AIAGENT=1
    SRCDIRS_HAAS += $(wildcard $(MODULES_DIR)/aiagent/*.c)
endif

ifeq ($(MICROPY_PY_ALIYUNIOT), y)
    include $(EXTERNAL_DIR)/linksdk/component.mk

    CFLAGS_HAAS += -DMICROPY_PY_ALIYUNIOT=1 -Wno-discarded-qualifiers -Wno-unused-function -Wno-incompatible-pointer-types -Wno-pointer-sign -Wno-implicit-function-declaration
    SRCDIRS_HAAS += $(MODULES_DIR)/aliyunIoT
    INC_HAAS += $(MODULES_DIR)/aliyunIoT
endif

ifeq ($(MICROPY_PY_BLE), y)
    CFLAGS_HAAS += -DMICROPY_PY_BLE=1 -DCONFIG_BT=1
    SRCDIRS_HAAS += $(wildcard $(MODULES_DIR)/ble/*.c)
    INC_HAAS += \
        $(MODULES_DIR)/ble \
		$(AOS_DIR)/ble_host/bt_host/include \
    	$(AOS_DIR)/ble_host/include \
    	$(AOS_DIR)/ble_host/include
endif

ifeq ($(MICROPY_PY_HTTP), y)
    CFLAGS_HAAS += -DMICROPY_PY_HTTP=1
    SRCDIRS_HAAS += network/http/esp32/modhttp.c
    INC_HAAS += \
        $(MICROPY_MODULES_PORT_DIR)/network/http
endif

ifeq ($(MICROPY_PY_MODBUS), y)
    CFLAGS_HAAS += -DMICROPY_PY_MODBUS=1
    SRCDIRS_HAAS += modbus/modbus.c
    INC_HAAS += \
        $(MICROPY_MODULES_PORT_DIR)/modbus/include
endif

ifeq ($(MICROPY_PY_KV), y)
    CFLAGS_HAAS += -DMICROPY_PY_KV=1
    SRCDIRS_HAAS += kv/modkv.c
    INC_HAAS += \
        $(MICROPY_MODULES_PORT_DIR)/kv/include
endif

ifeq ($(MICROPY_PY_OTA), y)
    CFLAGS_HAAS += -DMICROPY_PY_OTA=1
    SRCDIRS_HAAS += oat/modota.c
    INC_HAAS += \
        $(MICROPY_MODULES_PORT_DIR)/ota/include
endif

ifeq ($(MICROPY_PY_OSS), y)
    CFLAGS_HAAS += -DMICROPY_PY_OSS=1
    SRCDIRS_HAAS += oss/modoss.c
    INC_HAAS += \
		$(MICROPY_MODULES_PORT_DIR)/oss/src \
		$(MICROPY_MODULES_PORT_DIR)/oss/include
else ifeq ($(MICROPY_PY_OSS_HTTP), y)
    CFLAGS_HAAS += -DMICROPY_PY_OSS_HTTP=1
    SRCDIRS_HAAS += $(patsubst oss/*.c)
    INC_HAAS += \
		oss/byhttp \
		$(MICROPY_MODULES_PORT_DIR)/http/include
endif

ifeq ($(MICROPY_PY_UCAMERA), y)
    CFLAGS_HAAS += -DMICROPY_PY_UCAMERA=1
    SRCDIRS_HAAS += ucamera/moducamera.c
    INC_HAAS += \
		$(AOS_DIR)/ucamera/include \
		$(AOS_DIR)/ucamera/include/device/wifi \
		$(AOS_DIR)/ucamera/include/device/uart
endif

ifeq ($(MICROPY_PY_DRIVER), y)
	CFLAGS_HAAS += -DMICROPY_PY_DRIVER=1
	SRCDIRS_HAAS += $(wildcard $(MODULES_DIR)/driver/*.c)
	INC_HAAS += \
		$(AOS_DIR)/cjson/include
endif

ifeq ($(MICROPY_PY_AUDIO), y)
	include audio/audio.mk
endif

ifeq ($(MICROPY_PY_SNTP), y)
	CFLAGS_HAAS += -DMICROPY_PY_SNTP=1
	ifeq ($(HAAS_PLATFORM), y)
		SRCDIRS_HAAS += sntp/aos/modsntp.c
		INC_HAAS += $(EXTERNAL_DIR)/sntp/include
	else ifeq ($(ESP_PLATFORM), y)
		SRCDIRS_HAAS += sntp/esp32/modsntp.c
	endif
endif

ifeq ($(MICROPY_PY_DISPLAY), y)
	CFLAGS_HAAS += -DMICROPY_PY_DISPLAY=1 -DLV_USE_SJPG=1
	SRCDIRS_HAAS += $(wildcard $(MODULES_DIR)/display/*.c)
	INC_HAAS += \
		display \
    	display/esp32 \
    	$(AOS_DIR)/little_ui \
    	$(AOS_DIR)/little_ui/3rdparty \
    	$(AOS_DIR)/little_ui/3rdparty/lvgl/src

	ifeq ($(MICROPY_PY_DISPLAY_LVGL), y)
		CFLAGS_HAAS += -DMICROPY_PY_DISPLAY_LVGL=1
		INC_HAAS += \
			$(AOS_DIR)/little_ui/driver/axp192 \
			$(AOS_DIR)/little_ui/driver/lvgl_esp32_drivers
    endif

	ifeq ($(MICROPY_PY_DISPLAY_TFT), y)
        CFLAGS_HAAS += -DMICROPY_PY_DISPLAY_TFT=1
    endif
endif

ifeq ($(MICROPY_PY_LVGL), y)
	CFLAGS_HAAS += -DMICROPY_PY_LVGL=1
endif

ifeq ($(MICROPY_PY_BLECFG), y)
	CFLAGS_HAAS += -DMICROPY_PY_BLECFG=1
	SRCDIRS_HAAS += netconfig/blecfg.c
endif

ifeq ($(MICROPY_PY_NETMGR), y)
	CFLAGS_HAAS += -DMICROPY_PY_NETMGR=1
	SRCDIRS_HAAS += netmgr/modnetmgr.c
	INC_HAAS += \
        $(AOS_DIR)/uservice/include \
        $(AOS_DIR)/netmgr/include
endif

ifeq ($(MICROPY_PY_ULOG), y)
	CFLAGS_HAAS += -DMICROPY_PY_ULOG=1
	SRCDIRS_HAAS += ulog/modulog.c
	INC_HAAS += \
		$(AOS_DIR)/ulog/include
endif


include $(MODULES_DIR)/utility/component.mk

include $(MODULES_DIR)/hmac/component.mk

include $(MODULES_DIR)/system/component.mk

INC += $(addprefix -I, ${INC_HAAS})
