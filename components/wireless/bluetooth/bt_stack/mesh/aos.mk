NAME := btmesh

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := bt mesh is mesh stack.

GLOBAL_INCLUDES     += aos/include/ include
GLOBAL_INCLUDES     += bt_mesh/inc
GLOBAL_INCLUDES     += bt_mesh/util/include
GLOBAL_INCLUDES     += bt_mesh/util/port/include
GLOBAL_INCLUDES     += bt_mesh/inc/port
GLOBAL_INCLUDES     += bt_mesh/inc/api
GLOBAL_INCLUDES     += bt_mesh/hal

ifeq ($(AG_MODULE_BT_MESH), y)


BTMESH = 

BUILD_FOR_GENIE := yes

$(NAME)_CXXFLAGS += -D CONFIG_MESH_STACK_FOR_GENIE
$(NAME)_CFLAGS += -D CONFIG_MESH_STACK_FOR_GENIE

$(NAME)_CFLAGS += -D CONFIG_MESH_STACK_ALONE
$(NAME)_CXXFLAGS += -D CONFIG_MESH_STACK_ALONE

$(NAME)_CFLAGS += -D CONFIG_BLUETOOTH
#$(NAME)_CFLAGS += -D CONFIG_BT_SMP
$(NAME)_CFLAGS += -D CONFIG_BT_TINYCRYPT_ECC
$(NAME)_CFLAGS += -D CONFIG_BT_CONN
$(NAME)_CFLAGS += -D CONFIG_BT_CENTRAL
#$(NAME)_CFLAGS += -D CONFIG_BT_PERIPHERAL
#$(NAME)_CFLAGS += -D CONFIG_BLE_50
$(NAME)_CFLAGS += -D CONFIG_BT_HCI_VS_EXT

# Mesh function select
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_CFG_CLI
$(NAME)_CFLAGS += -D CONFIG_BT_MESH
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_BEACON
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_PROV
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_PROVISIONER
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_PB_ADV
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_PB_GATT
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_PROXY
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_GATT_PROXY
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_RELAY
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_FRIEND
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_LOW_POWER
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_SHELL
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_BQB
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_IV_UPDATE_TEST

$(NAME)_CXXFLAGS += -D CONFIG_BT_MESH
$(NAME)_CXXFLAGS += -D CONFIG_BT_MESH_PROVISIONER

# Mesh foundation model select
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_CFG_CLI
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_HEALTH_CLI

#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_CFG_SRV
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_HEALTH_SRV

# Mesh debug message enable
#$(NAME)_CFLAGS += -D USE_BT_MESH_CUSTOM_LOG
$(NAME)_CFLAGS += -D GENIE_DEBUG_COLOR
$(NAME)_CFLAGS += -D MESH_DEBUG_PROV

$(NAME)_CFLAGS += -D CONFIG_BT_GATT_CLIENT

## MESH debug log control macro
$(NAME)_CFLAGS += -D CONFIG_BT_DEBUG_LOG
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_ADV
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_BEACON
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_PROXY
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_PROV
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_NET
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_CRYPTO
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_TRANS
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_FRIEND
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_LOW_POWER
$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_ACCESS
#$(NAME)_CFLAGS += -D CONFIG_BT_MESH_DEBUG_MODEL


$(NAME)_INCLUDES += ${BTMESH}/bt_mesh/inc
$(NAME)_INCLUDES += ${BTMESH}/bt_mesh/inc/port
$(NAME)_INCLUDES += ${BTMESH}/bt_mesh/inc/api
$(NAME)_INCLUDES += ${BTMESH}/bt_mesh/ref_impl
$(NAME)_INCLUDES += ${GENIE_CORE_PATH}/../$(CONN_CHIP)/bluetooth/mesh

$(NAME)_INCLUDES += ${BTMESH}/bt_mesh/util/port/include
$(NAME)_INCLUDES += ${BTMESH}/bt_mesh/util/include
$(NAME)_INCLUDES += ${BTMESH}/bt_mesh/util/include/net
$(NAME)_INCLUDES += ${BTMESH}/bt_mesh/util/tinycrypt/include

$(NAME)_INCLUDES += ${BTMESH}/aos_port/

$(warning NAME_INCLUDES = $(NAME)_INCLUDES)


$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/atomic_c.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/buf.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/log.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/poll.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/queue.c
#$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/util.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/work.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/event_scheduler.c

$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/access.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/adv.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/beacon.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/crypto.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/cfg_srv.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/cfg_cli.c
#$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/health_srv.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/health_cli.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/mesh_main.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/net.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/prov.c
#$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/proxy.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/transport.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/friend.c
#$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/lpn.c
#$(NAME)_SOURCES += ${BTMESH}/bt_mesh/src/shell.c

$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/tinycrypt/source/cmac_mode.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/tinycrypt/source/crypto_utils.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/tinycrypt/source/aes_encrypt.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/tinycrypt/source/ecc_dh.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/tinycrypt/source/ecc.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/tinycrypt/source/hmac_prng.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/tinycrypt/source/hmac.c
$(NAME)_SOURCES += ${BTMESH}/bt_mesh/util/tinycrypt/source/sha256.c

endif