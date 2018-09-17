NAME := blemesh_srv

$(NAME)_SOURCES     := main.c

$(NAME)_INCLUDES    := ../../../network/bluetooth/include \
                       ../../../network/bluetooth/core/include

ble = 1
bt_mesh = 1

$(NAME)_COMPONENTS  += network.bluetooth.bt yloop cli
GLOBAL_DEFINES      += AOS_NO_WIFI CONFIG_BT_SMP CONFIG_BT_MESH_PROV CONFIG_BT_MESH_PB_ADV CONFIG_BT_MESH_PB_GATT CONFIG_BT_TINYCRYPT_ECC CONFIG_BT_MESH_CFG_CLI CONFIG_BT_MESH_PROXY CONFIG_BT_MESH_GATT_PROXY CONFIG_BT_CONN  BLE_APP_RECONFIG_MESHSRV


GLOBAL_INCLUDES += ../
