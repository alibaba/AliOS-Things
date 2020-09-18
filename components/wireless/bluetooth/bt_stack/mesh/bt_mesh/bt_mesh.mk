ifeq ($(AG_MODULE_BT_MESH), y)
BTMESH = ${ALI_GENIE_CORE_ROOT}/system/connectivity/core/mesh/bt_mesh

CFLAGS += -I${VPATH}/${BTMESH}/inc
CFLAGS += -I${VPATH}/${BTMESH}/inc/port
CFLAGS += -I${VPATH}/${BTMESH}/inc/api
#CFLAGS += -I${VPATH}/${BTMESH}/port/inc
#CFLAGS += -I${VPATH}/${BTMESH}/util/include
#CFLAGS += -I${VPATH}/${BTMESH}/util/port/include
CFLAGS += -I${VPATH}/${BTMESH}/ref_impl
CFLAGS += -I${VPATH}/../amlogic_sdk/lib/third_party/amlogic/drivers/mesh_os
CFLAGS += -I$(VPATH)/${ALI_GENIE_HAL_ROOT}/$(CONN_CHIP)/bluetooth/mesh

CFLAGS += -I${VPATH}/${BTMESH}/../bt_host/bt_common/port/include
CFLAGS += -I${VPATH}/${BTMESH}/../bt_host/bt_common/include
CFLAGS += -I${VPATH}/${BTMESH}/../bt_host/include
CFLAGS += -I${VPATH}/${BTMESH}/../bt_host/bt_common/tinycrypt/include

CPPFLAGS += -I${VPATH}/${BTMESH}/inc
CPPFLAGS += -I${VPATH}/${BTMESH}/inc/port
CPPFLAGS += -I${VPATH}/${BTMESH}/inc/api
#CPPFLAGS += -I${VPATH}/${BTMESH}/util/include
#CPPFLAGS += -I${VPATH}/${BTMESH}/util/port/include
CPPFLAGS += -I${VPATH}/${BTMESH}/../bt_host/bt_common/port/include
CPPFLAGS += -I${VPATH}/${BTMESH}/../bt_host/bt_common/include
CPPFLAGS += -I${VPATH}/../amlogic_sdk/lib/third_party/amlogic/drivers/mesh_os

aligenie-y += ${BTMESH}/../bt_host/bt_common/atomic_c.o
aligenie-y += ${BTMESH}/../bt_host/bt_common/buf.o
aligenie-y += ${BTMESH}/../bt_host/bt_common/log.o
aligenie-y += ${BTMESH}/../bt_host/bt_common/poll.o
aligenie-y += ${BTMESH}/../bt_host/bt_common/queue.o
#aligenie-y += ${BTMESH}/../bt_host/bt_common/util.o
aligenie-y += ${BTMESH}/../bt_host/bt_common/work.o
aligenie-y += ${BTMESH}/../bt_host/bt_common/event_scheduler.o

aligenie-y += ${BTMESH}/../bt_host/bt_common/tinycrypt/source/cmac_mode.o
aligenie-y += ${BTMESH}/../bt_host/bt_common/tinycrypt/source/crypto_utils.o
aligenie-y += ${BTMESH}/../bt_host/bt_common/tinycrypt/source/aes_encrypt.o

aligenie-y += ${BTMESH}/src/access.o
aligenie-y += ${BTMESH}/src/adv.o
aligenie-y += ${BTMESH}/src/beacon.o
aligenie-y += ${BTMESH}/src/crypto.o
aligenie-y += ${BTMESH}/src/cfg_srv.o
aligenie-y += ${BTMESH}/src/cfg_cli.o
#aligenie-y += ${BTMESH}/src/health_srv.o
aligenie-y += ${BTMESH}/src/health_cli.o
aligenie-y += ${BTMESH}/src/mesh_main.o
aligenie-y += ${BTMESH}/src/net.o
aligenie-y += ${BTMESH}/src/prov.o
#aligenie-y += ${BTMESH}/src/proxy.o
aligenie-y += ${BTMESH}/src/transport.o
aligenie-y += ${BTMESH}/src/friend.o
#aligenie-y += ${BTMESH}/src/lpn.o
#aligenie-y += ${BTMESH}/src/shell.o

else

NAME := bt_mesh

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := BLE Mesh stack.

ifeq ($(bt_mesh_standalone_deploy),1)
GLOBAL_DEFINES += CONFIG_MESH_STACK_ALONE
$(NAME)_COMPONENTS += bluetooth.bt_mesh.util
else
$(NAME)_COMPONENTS += bluetooth.bt_host.bt_common 
endif

$(NAME)_COMPONENTS += bluetooth.bt_mesh.ref_impl

$(NAME)_INCLUDES += ./inc/ \
                    ./inc/api/ \
                    ./inc/api/mesh/ \
                    ../../../genie_app/ \
                    mesh_model/ \
                    mesh_model/inc/ \
                    vendor_model/

$(NAME)_SOURCES  :=  ./src/access.c \
                     ./src/adv.c \
                     ./src/beacon.c \
                     ./src/crypto.c \
                     ./src/cfg_srv.c \
                     ./src/cfg_cli.c \
                     ./src/health_srv.c \
                     ./src/health_cli.c \
                     ./src/mesh_main.c \
                     ./src/net.c \
                     ./src/prov.c \
                     ./src/proxy.c \
                     ./src/transport.c \
                     ./src/friend.c \
                     ./src/lpn.c \
                     ./src/shell.c \
                     ./src/provisioner_main.c \
					 ./src/provisioner_prov.c \
					 ./src/provisioner_beacon.c \
					 ./src/provisioner_proxy.c   \


GLOBAL_INCLUDES += ./inc/ \
                   ./inc/api

ifeq ($(MESH_MODEL_CTL_SRV),1)
MESH_MODEL_LIGHTNESS_SRV = 1
GLOBAL_DEFINES += CONFIG_MESH_MODEL_CTL_SRV
GLOBAL_DEFINES += CONFIG_MESH_MODEL_CTL_SETUP_SRV
#GLOBAL_DEFINES += CONFIG_MESH_MODEL_CTL_TEMPERATURE_SRV
$(NAME)_SOURCES += mesh_model/src/light_ctl_srv.c
endif

ifeq ($(MESH_MODEL_LIGHTNESS_SRV),1)
MESH_MODEL_GEN_ONOFF_SRV = 1
MESH_MODEL_GEN_LEVEL_SRV = 1
GLOBAL_DEFINES += CONFIG_MESH_MODEL_LIGHTNESS_SRV
GLOBAL_DEFINES += CONFIG_MESH_MODEL_LIGHT_CMD
$(NAME)_SOURCES += mesh_model/src/lightness_srv.c
endif

ifeq ($(MESH_MODEL_GEN_LEVEL_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_GEN_LEVEL_SRV
$(NAME)_SOURCES += mesh_model/src/gen_level_srv.c
endif

ifeq ($(MESH_MODEL_GEN_ONOFF_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_GEN_ONOFF_SRV
$(NAME)_SOURCES += mesh_model/src/gen_onoff_srv.c
endif

$(NAME)_SOURCES += mesh_model/src/model_bind_ops.c

ifeq ($(MESH_MODEL_VENDOR_SRV),1)
GLOBAL_DEFINES += CONFIG_MESH_MODEL_VENDOR_SRV
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_VENDOR_MODEL
$(NAME)_SOURCES += mesh_model/src/vendor_model_srv.c
endif

GLOBAL_DEFINES += CRC16_ENABLED

## MESH debug log control macro
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_ADV
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_BEACON
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROXY
GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_PROV
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_NET
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_CRYPTO
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_TRANS
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_FRIEND
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_LOW_POWER
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_ACCESS
#GLOBAL_DEFINES += CONFIG_BT_MESH_DEBUG_MODEL
GLOBAL_DEFINES += CONFIG_GENIE_DEBUG_CMD_FLASH
endif

