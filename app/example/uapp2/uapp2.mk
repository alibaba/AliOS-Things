NAME := uapp2

$(NAME)_TYPE := app
$(NAME)_MBINS_TYPE := app

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := src/entry.c \
                   src/app.c
else
$(NAME)_SOURCES := src/app.c
endif

