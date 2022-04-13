NAME := devicevfs

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := Device VFS module for bus-disabled model


ifeq ($(HOST_ARCH),Cortex-M33)
ifneq ($(AOS_KERNEL_MODE),y)
$(NAME)_PREBUILT_LIBRARY += devicevfs_m_u.a
else
$(NAME)_PREBUILT_LIBRARY += devicevfs_m_k.a
endif
endif

ifeq ($(HOST_ARCH),Cortex-A9)
ifneq ($(AOS_KERNEL_MODE),y)
$(NAME)_PREBUILT_LIBRARY += devicevfs_a_u.a
else
$(NAME)_PREBUILT_LIBRARY += devicevfs_a_k.a
endif
endif

#$(NAME)_CFLAGS += -Werror

$(NAME)_DEFINES +=

ifneq ($(AOS_KERNEL_MODE),y)
GLOBAL_CFLAGS += -DUSER_SPACE_DRIVER=1
ifeq ($(AOS_BOARD_HAAS100_MK), y)
GLOBAL_CFLAGS += -DENABLE_IO_MMAP=1
endif
endif

GLOBAL_INCLUDES += ../include ../include/devicevfs

