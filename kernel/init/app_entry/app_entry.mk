NAME := app_entry

$(NAME)_MBINS_TYPE := app

GLOBAL_LDFLAGS += -uapp_info

$(NAME)_SOURCES    := app_entry.c

