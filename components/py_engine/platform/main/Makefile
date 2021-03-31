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
		../utils/cJSON \
		../components/linkkit \
		../components/linkkit/infra \
		../components/ulog \
		../services/board_mgr \
		../services/recovery \
		../services/app_mgr

include $(TOOLS_DIR)/rules.mk
