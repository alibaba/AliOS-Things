NAME := blemesh_tmall

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := Sample code to demenstrate how to join AliGenie BLE Mesh network.

$(NAME)_SOURCES-y   := main.c

BLE = 1
EN_BT_SMP = 1
bt_mesh = 1

$(NAME)_COMPONENTS-y += tmall_model yloop cli
GLOBAL_DEFINES-y     += AOS_NO_WIFI CONFIG_BT_TINYCRYPT_ECC CONFIG_BT_CONN

$(NAME)_INCLUDES-y    += ../
