NAME = libamp_http


$(NAME)_SOURCES += \
		src/http_client.c \
		src/http_parser.c \
		src/http_string.c \
		src/http_wrapper.c

$(NAME)_INCLUDES := \
		../../adapter/include \
		include \
		../../adapter/include \
		../../adapter/platform/aos \
		../linkkit \
		../linkkit/infra \
		../../main \
		../ulog

