NAME := blemesh_cmds

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := blemesh_cmds
$(NAME)_SOURCES     := main.c

$(NAME)_INCLUDES    := ../../../network/bluetooth/include \
                       ../../../network/bluetooth/core/include

ble = 1

$(NAME)_COMPONENTS  += network.bluetooth.bt_mesh yloop cli

GLOBAL_DEFINES += AOS_NO_WIFI CONFIG_BT_SMP CONFIG_BT_MESH_PROV CONFIG_BT_MESH_PB_ADV CONFIG_BT_MESH_PB_GATT CONFIG_BT_TINYCRYPT_ECC CONFIG_BT_MESH_CFG_CLI CONFIG_BT_MESH_PROXY CONFIG_BT_MESH_GATT_PROXY CONFIG_BT_CONN CONFIG_BT_MESH_HEALTH_CLI CONFIG_BT_MESH_SHELL

GLOBAL_INCLUDES += ../
