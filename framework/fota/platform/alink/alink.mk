NAME := fota_alink_transport

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := ota_transport.c
$(NAME)_INCLUDES := ../../../protocol/alink/system/ \
                    ../../../protocol/alink/os/ \
                    ../../ \
                    ../
