NAME := blemesh_tmall

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := Sample code to demenstrate how to join AliGenie BLE Mesh network.

$(NAME)_SOURCES-y   := ble_mesh_tmall_app.c

bt_mesh = 1

$(NAME)_COMPONENTS-y += tmall_model bt_host yloop cli
GLOBAL_DEFINES-y     += AOS_NO_WIFI CONFIG_BT_TINYCRYPT_ECC CONFIG_BT_CONN

$(NAME)_INCLUDES-y    += ../
