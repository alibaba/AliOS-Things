NAME := BLE
$(info $(NAME))
$(NAME)_TYPE := framework

$(NAME)_SOURCES += \
    src/ota_breeze.c \
    src/ota_breeze_service.c \
    src/ota_breeze_transport.c \
    hal/ota_hal_os.c

GLOBAL_INCLUDES += \
                inc \
                hal \
