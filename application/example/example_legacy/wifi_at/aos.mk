NAME := wifi_at_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := WIFI AT
$(NAME)_SOURCES := wifi_at.c                 \
                   atcmd_util.c

$(NAME)_INCLUDES := ./include

$(NAME)_COMPONENTS := at
