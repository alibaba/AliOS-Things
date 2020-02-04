NAME := ulog

$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := ulog module
$(NAME)_MBINS_TYPE := kernel

$(NAME)_COMPONENTS += osal_aos

GLOBAL_DEFINES += AOS_COMP_ULOG
#sync mode is default selected using
$(NAME)_SOURCES     := ulog.c ulog_init.c ulog_utility.c

# armcc & iar without -Wall and -Werror
ifeq ($(COMPILER), )
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER), gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_CFLAGS      += -DMOD_VER=\"$($(NAME)_VERSION)\"

#ulog support fs record
ifeq ($(ULOG_CONFIG_POP_FS),y)
$(NAME)_SOURCES     += ulog_session_file.c
$(NAME)_SOURCES     += ulog_fs_cfg.c

ifeq ($(ULOG_CONFIG_FS_SPIFFS),y)
$(NAME)_COMPONENTS += spiffs
$(info >>> BE Reserved flash for spiffs in board/flashpartion under spiffs select)

else ifeq ($(ULOG_CONFIG_FS_FATFS_SD),y)
$(NAME)_COMPONENTS  += fatfs
GLOBAL_DEFINES      += CONFIG_AOS_FATFS_SUPPORT_MMC

else ifeq ($(ULOG_CONFIG_FS_FATFS_FLASH),y)
$(NAME)_COMPONENTS  += fatfs
GLOBAL_DEFINES      += CONFIG_AOS_FATFS_SUPPORT_RAW_FLASH

endif
endif

#ulog support syslog udp mode
ifeq ($(ULOG_CONFIG_POP_UDP), y)
$(NAME)_SOURCES     += ulog_session_udp.c
endif

#ulog support aync mode
ifeq ($(ULOG_CONFIG_ASYNC),y)
GLOBAL_DEFINES += ULOG_CONFIG_ASYNC
$(NAME)_SOURCES     += ulog_async.c
$(NAME)_SOURCES     += ulog_ring_fifo.c
endif

$(NAME)_INCLUDES += include

