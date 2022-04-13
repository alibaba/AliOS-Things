NAME = libamp_main

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := amp component main

$(NAME)_SOURCES += \
		amp_main.c \
		amp_engine.c \
		amp_task.c

$(NAME)_INCLUDES := \
		../adapter/include \
		../adapter/include/peripheral \
		../adapter/platform/aos \
		../utils/mbedtls/include \
		../utils/cJSON \
		../components/linkkit \
		../components/linkkit/infra \
		../components/ulog \
		../services/board_mgr \
		../services/recovery \
		../services/app_mgr

