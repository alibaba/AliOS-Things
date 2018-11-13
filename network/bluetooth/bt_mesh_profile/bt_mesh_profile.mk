NAME := bt_mesh_profile

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := bt_mesh_profile.c

$(NAME)_INCLUDES := ./ \
                    ../bt_mesh/inc \
                    ../bt_mesh_inc/api \
                    ../../bluetooth/bt/include \
                    ../../bluetooth/bt/common/tinycrypt/include \
                    ../../../platform/mcu/nrf52xxx/Drivers/drivers_nrf \
                    ../../../kernel/rhino/hal

GLOBAL_INCLUDES := ./
