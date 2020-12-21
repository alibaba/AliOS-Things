NAME := romfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := romfs filesystem

mkfile_path := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))

# generate rootfs
$(info making rootfs from dir '$(mkfile_path)/rootfs' ....)
$(shell python $(mkfile_path)/mkromfs.py $(mkfile_path)/rootfs $(mkfile_path)/romfs.c)

ifneq (,$(wildcard $(mkfile_path)/romfs.c))
$(info using romfs.c)
$(NAME)_SOURCES += romfs.c
else
$(info using romfs_dummy.c)
$(NAME)_SOURCES += romfs_dummy.c
endif

GLOBAL_INCLUDES += .

ifeq ($(AOS_COMP_VFS), y)
$(NAME)_SOURCES += romfs_vfs.c
endif

GLOBAL_DEFINES  += AOS_COMP_ROMFS VFS_CONFIG_ROOTFS
