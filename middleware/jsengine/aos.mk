NAME := jsengine

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := an engine for javascript application development
$(NAME)_COMPONENTS += ota linkkit_sdk_c cjson http mbedtls cli spiffs netmgr

JSE_ROOT := ${SOURCE_ROOT}middleware/jsengine/src

# JSEngine version, Naming rule: [mcu]-[os]-[MAJOR].[MINOR].[DATE].[JSE]
# e.g. : esp32devkitc-aos-1.0.071215.lite
MAJOR_VERSION=1
MINOR_VERSION=1
VERSION_DESCRIPTION=aos

NOW_DATE:=$(CURRENT_TIME)

ifeq ($(JSE_ENGINE_LITE),y)
JSE_ENGINE_TYPE := tiny
else ifeq ($(JSE_ENGINE_DUKTAPE),y)
JSE_ENGINE_TYPE := dkp
endif

ifeq ($(AOS_BOARD_ESP32DEVKITC),y)
GLOBAL_DEFINES += CONFIG_SPIFFS_PHYS_SZ=524288
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_BLOCK_SZ=65536
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_PAGE_SZ=256
TARGET_TYPE=esp32devkitc
else ifeq ($(AOS_BOARD_DEVELOPERKIT),y)
GLOBAL_DEFINES += CONFIG_SPIFFS_PHYS_SZ=262144
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_BLOCK_SZ=16384
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_PAGE_SZ=2048 LITTLEVGL_DEVELOPERKIT USE_FREETYPE=0
TARGET_TYPE=developerkit
$(NAME)_COMPONENTS += littlevGL
else ifeq ($(AOS_BOARD_STM32F429ZI_NUCLEO),y)
GLOBAL_DEFINES += CONFIG_SPIFFS_PHYS_SZ=262144
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_BLOCK_SZ=16384
GLOBAL_DEFINES += CONFIG_SPIFFS_LOG_PAGE_SZ=2048
TARGET_TYPE=stm32f429nucleo
else
endif

BONE_VERSION=$(TARGET_TYPE)-aos-$(MAJOR_VERSION).$(MINOR_VERSION).$(NOW_DATE).${JSE_ENGINE_TYPE}

GLOBAL_DEFINES += VERSION_DESCRIPTION=\"$(VERSION_DESCRIPTION)\"
GLOBAL_DEFINES += BONE_VERSION=\"$(BONE_VERSION)\"
GLOBAL_DEFINES += JSENGINE BE_OS_AOS
GLOBAL_DEFINES += SPIFFS_OBJ_NAME_LEN=96
GLOBAL_DEFINES += SPIFFS_CACHE
GLOBAL_DEFINES += SUPPORT_NODE_MODELES

# redefine CLI_CONFIG_INBUF_SIZE to 1024,
# or set CLI_CONFIG_INBUF_SIZE to 1024 in menuconfig: Kernel --> Commonad-Line Interface --> Input buffer size
ifneq (${CLI_CONFIG_INBUF_SIZE}, 1024)
cli_CFLAGS += -include middleware/jsengine/src/include/jse_patch.h
endif

$(info JSEngine Version: ${BONE_VERSION})

include ${JSE_ROOT}/engine/engine.mk
