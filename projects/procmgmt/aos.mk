NAME := procmgmt

$(NAME)_TYPE := app

$(NAME)_MBINS_TYPE := app

$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY :=
$(NAME)_MBINS_APP_LD_FILE := procmgmt

ifeq ($(ENABLE_USPACE),1)

ifeq ($(APP_FORMAT),bin)
$(NAME)_SOURCES := src/entry_bin.c
else ifeq ($(APP_FORMAT),elf)
$(NAME)_SOURCES := src/entry_elf.c
else
$(error "Unknown app format")
endif

$(NAME)_COMPONENTS += uspace_lib ulog
$(NAME)_COMPONENTS += usyscall
$(NAME)_COMPONENTS += uspace_pm
ifeq ($(BUILD_MINIAPP), 1)
endif

$(NAME)_SOURCES += src/app.c
$(NAME)_SOURCES += src/procm_load/procm_load.c
$(NAME)_SOURCES += src/procm_load/procm_ash.c
$(NAME)_SOURCES += src/procserv/process_service.c
$(NAME)_SOURCES += src/procserv/process_map.c
$(NAME)_SOURCES += src/procserv/process_cmd.c

$(NAME)_SOURCES += src/pathserv/path_manager.c

$(NAME)_INCLUDES += include
$(NAME)_INCLUDES += src/procserv
$(NAME)_INCLUDES += src/pathserv

GLOBAL_DEFINES += AOS_PROC_LOAD_FROM_USAPCE

$(NAME)_CFLAGS += -g -O2 -Wall -Werror

endif

