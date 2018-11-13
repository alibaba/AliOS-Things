NAME := uapp2

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := src/entry.c \
                   src/app.c
else
$(NAME)_SOURCES := src/app.c
endif

