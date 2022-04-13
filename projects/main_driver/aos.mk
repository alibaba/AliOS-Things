NAME := main_driver

$(NAME)_TYPE := app

$(NAME)_MBINS_TYPE := app

$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY :=
$(NAME)_MBINS_APP_LD_FILE := driver

ifeq ($(ENABLE_USPACE),1)

ifeq ($(APP_FORMAT),bin)
$(NAME)_SOURCES := src/entry_bin.c
else ifeq ($(APP_FORMAT),elf)
$(NAME)_SOURCES := src/entry_elf.c
else
$(error "Unknown app format")
endif

$(NAME)_COMPONENTS += uspace_lib ulog uspace_aos usyscall
$(NAME)_COMPONENTS += drivers

$(NAME)_SOURCES += src/app.c


$(NAME)_INCLUDES += include

$(NAME)_CFLAGS += -g -O2 -Wall #-Werror

endif

