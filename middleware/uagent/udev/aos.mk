NAME := udev

$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := developerment over the air
$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES     := udev.c udev_net.c udev_mqtt_client.c

$(NAME)_COMPONENTS := linkkit_sdk_c cjson ota

GLOBAL_INCLUDES += include
