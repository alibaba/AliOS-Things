TARGET = libmain.a
MODULE = main

MOD_SOURCES += \
		amp_main.c \
		amp_engine.c \
		amp_task.c

MOD_INCLUDES := \
		../adapter/include \
		../adapter/include/peripheral \
		../adapter/platform/linux \
		../utils/mbedtls/include \
		../utils/mbedtls/platform/include \
		../utils/mbedtls/platform/amp/include \
		../utils/cJSON \
		../utils/list \
		../components/linkkit \
		../components/linkkit/infra \
		../components/ulog \
		../services/board_mgr \
		../services/recovery \
		../services/app_mgr
		
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
