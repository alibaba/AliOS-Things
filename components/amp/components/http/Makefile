TARGET = libhttp.a
MODULE = http

MOD_SOURCES := \
		src/http_client.c \
		src/http_parser.c \
		src/http_string.c \
		src/http_wrapper.c

MOD_INCLUDES := \
		../../adapter/include \
		../../adapter/platform/linux \
		../../main \
		../../utils/mbedtls/include \
		../linkkit \
		../linkkit/infra \
		../ulog \
		include

include $(TOOLS_DIR)/rules.mk
