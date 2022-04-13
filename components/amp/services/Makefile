TARGET = libservices.a
MODULE = services

MOD_SOURCES += \
		app_mgr/app_mgr.c \
		app_mgr/app_ntp.c \
		app_mgr/app_ota_service.c \
		app_mgr/ota_socket.c

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
		../utils/cJSON \
		../components/linkkit \
		../components/linkkit/infra \
		../components/linkkit/dev_model \
		../components/linkkit/dev_sign \
		../components/linkkit/mqtt \
		../components/ulog \
		../components/das/include \
		amp_utils \
		app_mgr \
		board_mgr \
		recovery

include $(TOOLS_DIR)/rules.mk
