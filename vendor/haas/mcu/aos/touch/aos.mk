NAME         := input
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := TP driver

$(NAME)_COMPONENTS +=

GLOBAL_CFLAGS +=

GLOBAL_ASMFLAGS +=

GLOBAL_CXXFLAGS +=
GLOBAL_INCLUDES +=
GLOBAL_INCLUDES += .

GLOBAL_LDFLAGS +=

$(NAME)_SOURCES := \
	sunxi-input.c \
	tp/tlsc6x.c \
	tp/tlsc6x_comp.c
