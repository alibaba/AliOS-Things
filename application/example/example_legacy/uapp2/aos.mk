NAME := uapp2

$(NAME)_TYPE := app

$(NAME)_MBINS_TYPE := app

$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY :=

ifeq ($(ENABLE_USPACE),1)

ifeq ($(APP_FORMAT),bin)
$(NAME)_SOURCES := src/entry_bin.c
else ifeq ($(APP_FORMAT),elf)
$(NAME)_SOURCES := src/entry_elf.c
else
$(error "Unknown app format")
endif

$(NAME)_SOURCES += src/app.c

$(NAME)_INCLUDES += include

endif

