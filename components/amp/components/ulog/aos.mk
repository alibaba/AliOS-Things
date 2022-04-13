NAME = libamp_ulog

$(NAME)_SOURCES += \
		ulog.c \
		ulog_init.c \
		ulog_utility.c

$(NAME)_INCLUDES := \
		../../adapter/include \
		../linkkit \
		../linkkit/infra \
		. \
		include \
		../../main