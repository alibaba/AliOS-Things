NAME := bt_mesh_profile

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := Tmall BLE Mesh profile implementation.

$(NAME)_COMPONENTS-y := network.bluetooth.bt_mesh

$(NAME)_SOURCES-y := bt_mesh_profile.c

$(NAME)_INCLUDES-y := ./ \
                      ../bt_mesh/inc \
                      ../bt_mesh_inc/api \
                      ../../bluetooth/bt/include \
                      ../../bluetooth/bt/common/tinycrypt/include \
                      ../../../platform/mcu/nrf52xxx/Drivers/drivers_nrf \
                      ../../../kernel/rhino/hal

GLOBAL_INCLUDES-y := ./
