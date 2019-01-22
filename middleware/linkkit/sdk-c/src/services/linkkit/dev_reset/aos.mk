NAME := libdev_reset

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := reset device to factory settings.

$(NAME)_DEFINES := USE_LPTHREAD

$(NAME)_INCLUDES += .
$(NAME)_SOURCES :=
$(NAME)_SOURCES += awss_reset.c awss_reset_statis.c

$(NAME)_DEFINES += DEBUG

ifneq ($(CONFIG_SYSINFO_DEVICE_NAME), DH5021A)
GLOBAL_DEFINES += AWSS_SUPPORT_DEV_RESET_STATIS
endif

ifeq ($(COMPILER),)
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
endif
