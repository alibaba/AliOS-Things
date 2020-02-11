NAME := mal_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := MQTT Adapter Layer (MAL) app
$(NAME)_SOURCES := mal_app.c

$(NAME)_COMPONENTS += libiot_sign mal
