NAME := blemesh_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for ble mesh

GLOBAL_DEFINES += CONFIG_BT_CONN \
                  CONFIG_BT_TINYCRYPT_ECC

GLOBAL_DEFINES += CONFIG_BT_MESH_IV_UPDATE_TEST

$(NAME)_COMPONENTS += mesh_hal_ref_impl

$(NAME)_SOURCES += ble_mesh_test.c
