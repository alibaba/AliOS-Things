NAME := arch_armv5

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := arch for armv5

GLOBAL_INCLUDES += include

$(NAME)_CFLAGS := -marm -mthumb-interwork

$(NAME)_SOURCES += gcc/port_c.c    \
                   gcc/port_s.S    \
                   common/panic_c.c \
                   common/backtrace.c \
                   common/panic_gcc.S

