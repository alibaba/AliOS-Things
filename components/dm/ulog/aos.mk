# component name
NAME := ulog
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := ulog module
# source files and the folder of internal include files
$(NAME)_INCLUDES += include
$(NAME)_SOURCES := ulog.c ulog_init.c ulog_utility.c

# armcc & iar without -Wall and -Werror
#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS  += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif
$(NAME)_CFLAGS += -DMOD_VER=\"$($(NAME)_VERSION)\"

# the folder of API files
GLOBAL_INCLUDES += ../../../include/dm/ulog

#async mode actived
ifeq ($(ULOG_CONFIG_ASYNC),y)
$(NAME)_SOURCES += ulog_async.c ulog_ring_fifo.c
endif

#ulog support fs record
ifeq ($(ULOG_CONFIG_POP_FS),y)
$(NAME)_SOURCES += ulog_session_file.c ulog_fs_cfg.c
endif #ULOG_CONFIG_POP_FS

#ulog support syslog udp mode
ifeq ($(ULOG_CONFIG_POP_UDP), y)
$(NAME)_SOURCES += ulog_session_udp.c
endif

# optional dependencies
$(NAME)_COMPONENTS-$(ULOG_CONFIG_FS_SPIFFS) += spiffs
$(NAME)_COMPONENTS-$(ULOG_CONFIG_FS_FATFS_FLASH) += fatfs
$(NAME)_COMPONENTS-$(ULOG_CONFIG_FS_FATFS_SD) += fatfs
$(NAME)_COMPONENTS-$(ULOG_CONFIG_POP_FS) += cjson vfs
$(NAME)_COMPONENTS-$(ULOG_CONFIG_UPLOAD) += uagent
$(NAME)_COMPONENTS-$(ULOG_CONFIG_POP_CLOUD) += uagent
