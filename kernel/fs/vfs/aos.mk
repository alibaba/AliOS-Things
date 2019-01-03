NAME := vfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := Virtual File System

$(NAME)_SOURCES := vfs.c
$(NAME)_SOURCES += vfs_file.c
$(NAME)_SOURCES += vfs_inode.c
$(NAME)_SOURCES += vfs_adapt.c

$(NAME)_SOURCES += vfs_device_adc.c  \
                   vfs_device_gpio.c \
                   vfs_device_i2c.c  \
                   vfs_device_pwm.c  \
                   vfs_device_rtc.c  \
                   vfs_device_spi.c  \
                   vfs_device_uart.c \
                   vfs_device_wdg.c

ifeq ($(HOST_ARCH),linux)
$(NAME)_DEFINES += IO_NEED_TRAP
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES += __BSD_VISIBLE
else ifeq ($(COMPILER),rvct)
GLOBAL_DEFINES += __BSD_VISIBLE
endif

GLOBAL_INCLUDES += . include
GLOBAL_DEFINES  += AOS_COMP_VFS

AOS_COMP_VFS ?= y
