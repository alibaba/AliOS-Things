NAME := udev

$(NAME)_VERSION := 0.1.0
$(NAME)_SUMMARY := developerment over the air
$(NAME)_MBINS_TYPE := app

$(NAME)_SOURCES     := udev.c udev_net.c udev_mqtt_client.c

$(NAME)_COMPONENTS := feature.linkkit-mqtt cjson ota

GLOBAL_INCLUDES += include
