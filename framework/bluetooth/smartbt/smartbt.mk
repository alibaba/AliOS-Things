NAME := smartbt

GLOBAL_INCLUDES += include

$(NAME)_INCLUDES += internal

$(NAME)_COMPONENTS := protocols.bluetooth

$(NAME)_SOURCES := bt_management.c \
                   bt_peripheral.c \
                   bt_smartbridge_gatt.c \
                   bt_smartbridge.c \
                   bt_smartbridge_cfg.c \
                   internal/bt_peripheral_stack_interface.c \
                   internal/bt_smart_attribute.c \
                   internal/bt_smartbridge_att_cache_manager.c \
                   internal/bt_smartbridge_socket_manager.c \
                   internal/bt_smartbridge_helper.c \
                   internal/bt_smartbridge_stack_interface.c \
                   LinkListUtils.c StringUtils.c \
                   internal/os_wrapper.c
