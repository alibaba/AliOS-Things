NAME := bt_smart

GLOBAL_INCLUDES += include

$(NAME)_INCLUDES += internal

$(NAME)_SOURCES := mico_bt_management.c \
                   mico_bt_peripheral.c \
                   mico_bt_smartbridge_gatt.c \
                   mico_bt_smartbridge.c \
                   mico_bt_smartbridge_cfg.c \
                   internal/bt_peripheral_stack_interface.c \
                   internal/bt_smart_attribute.c \
                   internal/bt_smartbridge_att_cache_manager.c \
                   internal/bt_smartbridge_socket_manager.c \
                   internal/bt_smartbridge_helper.c \
                   internal/bt_smartbridge_stack_interface.c \
                   LinkListUtils.c StringUtils.c
