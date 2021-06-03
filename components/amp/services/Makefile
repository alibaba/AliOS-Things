TARGET = libservices.a
MODULE = services

MOD_SOURCES += \
		app_mgr/app_message.c \
		app_mgr/app_mgr.c \
		app_mgr/app_management_center.c \
		app_mgr/app_upgrade.c

MOD_SOURCES += \
		board_mgr/board_info.c \
		board_mgr/board_mgr.c

MOD_SOURCES += \
		recovery/recovery.c \
		recovery/ymodem.c

MOD_SOURCES += \
		amp_utils/amp_utils.c \

MOD_INCLUDES := \
		../main \
		../adapter/include \
		../adapter/include/peripheral \
		../adapter/platform/linux \
		../utils/mbedtls/include \
		../utils/mbedtls/platform/include \
		../utils/mbedtls/platform/amp/include \
		../utils/cJSON \
		../components/linksdk/components/bootstrap \
		../components/linksdk/components/das \
		../components/linksdk/components/data-model \
		../components/linksdk/components/devinfo \
		../components/linksdk/components/diag \
		../components/linksdk/components/dynreg \
		../components/linksdk/components/ntp \
		../components/linksdk/components/subdev \
		../components/linksdk/core \
		../components/linksdk/core/sysdep \
		../components/linksdk/core/utils \
		../components/ulog \
		../components/das/include \
		../components/ota/include \
		amp_utils \
		app_mgr \
		board_mgr \
		recovery

ifeq ($(ADDON), ui)
MOD_INCLUDES += \
		../ui/render/include \
		../ui/aui/ \
		../ui/aui/aui_core \
		../ui/aui/aui_draw \
		../ui/aui/aui_fonts \
		../ui/aui/aui_hal \
		../ui/aui/aui_misc \
		../ui/aui/aui_objx \
		../ui/aui/aui_themes \
		../ui/aui/libs \
		../ui/aui/libs/lvgl \
		../ui/aui/libs/lvgl/src\
		../ui/aui/libs/lvgl/src/lv_misc \
		../ui/aui/libs/lvgl/src/lv_font \
		../ui/aui/libs/lvgl/src/lv_core \
		../ui/aui/libs/lvgl/src/lv_draw \
		../ui/aui/libs/lvgl/src/lv_hal \
		../ui/aui/libs/lvgl/src/lv_objx \
		../ui/aui/libs/lvgl/src/lv_themes \
		../utils/ezxml \
		../utils/lexbor \
		../utils/lexbor/css \
		../adapter/include \
		../components/linkkit \
		../components/linkkit/infra \
		../main \
		../utils/mbedtls/include \
		../components/ulog \
		../engine/duktape_engine \
		../adapter/platform/linux

endif

include $(TOOLS_DIR)/rules.mk
