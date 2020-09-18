NAME := littlefs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := littlefs filesystem

# select littlefs version
lfs_v220 ?= 1
# use littlefs v2.2.0
ifeq ($(lfs_v220),1)
$(NAME)_SOURCES += littlefs-v220/lfs.c \
                   littlefs-v220/lfs_util.c
GLOBAL_INCLUDES += littlefs-v220/include
else
# use littlefs v2.1.4
$(NAME)_SOURCES += littlefs/lfs.c \
                   littlefs/lfs_util.c
GLOBAL_INCLUDES += littlefs/include
endif

# select header file for block device settings
ifeq ($(AOS_MCU_HAAS1000),y)
multi_fs := 0
$(NAME)_INCLUDES += ./platform_conf/haas1000/
else ifeq ($(AOS_MCU_VEXPRESSA9),y)
multi_fs := 1
$(NAME)_INCLUDES += ./platform_conf/vexpressa9/
else
$(error No platform setting provided for littlefs!)
endif

ifeq ($(AOS_COMP_VFS), y)
$(NAME)_SOURCES += littlefs_vfs.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

GLOBAL_DEFINES  += AOS_COMP_LITTLEFS

GLOBAL_DEFINES += LFS_STATIC_OBJECT LITTLEFS_FORMAT

multi_fs ?= 0
ifeq ($(multi_fs),1)
GLOBAL_DEFINES += CONFIG_MULTI_FS
endif

ifeq ($(fs_cli),1)
GLOBAL_DEFINES += CONFIG_LFS_CLI_TOOLS
endif

ifeq ($(flash_trace),1)
GLOBAL_DEFINES += CONFIG_FLASH_TRACE
endif

ifeq ($(vfs_trace),1)
GLOBAL_DEFINES += CONFIG_VFS_TRACE
endif

ifeq ($(lfs_trace),1)
GLOBAL_DEFINES += LFS_YES_TRACE
endif

GLOBAL_DEFINES += FS_TIMESTAMP_WORKAROUND

nftl ?= 0
ifeq ($(nftl),1)
$(NAME)_COMPONENTS += nftl
endif

sys_readonly ?= 0
ifeq ($(sys_readonly),1)
GLOBAL_DEFINES += CONFIG_LFS_SYSTEM_READONLY
endif
