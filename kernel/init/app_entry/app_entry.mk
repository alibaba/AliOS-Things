NAME := app_entry

$(NAME)_MBINS_TYPE := app

GLOBAL_LDFLAGS += -uapp_info

$(NAME)_SOURCES    := app_entry.c
$(NAME)_SOURCES    += app_mm.c

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += APP_MM

