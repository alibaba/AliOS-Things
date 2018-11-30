NAME := blemesh_tmall

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := Sample code to demenstrate how to join AliGenie BLE Mesh network.

$(NAME)_SOURCES-y   := main.c

$(NAME)_INCLUDES    := ../../../network/bluetooth/bt_mesh_profile \
                       ../../../../kernel/rhino/hal

ble = 1
bt_mesh = 1

$(NAME)_COMPONENTS-y += network.bluetooth.bt bluetooth.bt_mesh_profile yloop cli
GLOBAL_DEFINES-y     += AOS_NO_WIFI CONFIG_BT_SMP CONFIG_BT_MESH_PROV CONFIG_BT_MESH_PB_ADV CONFIG_BT_MESH_PB_GATT CONFIG_BT_TINYCRYPT_ECC CONFIG_BT_MESH_CFG_CLI CONFIG_BT_MESH_PROXY CONFIG_BT_MESH_GATT_PROXY CONFIG_BT_CONN CONFIG_BT_MESH_RELAY CONFIG_BT_MESH_HEALTH_CLI

GLOBAL_INCLUDES-y    += ../
