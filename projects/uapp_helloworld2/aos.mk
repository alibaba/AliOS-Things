NAME := uapp_helloworld2

$(NAME)_TYPE := app

$(NAME)_MBINS_TYPE := app

$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY :=
$(NAME)_MBINS_APP_LD_FILE := app2

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
ifeq ($(BUILD_MINIAPP), 1)
$(NAME)_COMPONENTS += httpc
$(NAME)_COMPONENTS += miniapp
GLOBAL_DEFINES += RHINO_CONFIG_MINIAPP
endif

$(NAME)_SOURCES += src/app.c

$(NAME)_INCLUDES += include

GLOBAL_DEFINES += RHINO_CONFIG_UMM_DEBUG=0

endif

