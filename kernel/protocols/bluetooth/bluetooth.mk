NAME := bluetooth

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

GLOBAL_INCLUDES += include \
                   include/drivers \
                   common/include \
                   port/include

$(NAME)_INCLUDES += common/tinycrypt/include \
                    ../../rhino/core/include

$(NAME)_COMPONENTS += yloop

ifeq ($(bt_mesh), 1)
$(NAME)_COMPONENTS += protocols.bluetooth.host.bt_mesh
$(NAME)_INCLUDES += include/bluetooth/mesh
GLOBAL_DEFINES += CONFIG_BT_MESH
endif

$(NAME)_SOURCES := common/atomic_c.c \
                   common/buf.c \
                   common/log.c \
                   common/poll.c \
                   host/uuid.c \
                   host/hci_core.c \
                   host/conn.c \
                   host/l2cap.c \
                   host/att.c \
                   host/gatt.c \
                   host/crypto.c \
                   host/smp.c \
                   host/keys.c \
                   common/tinycrypt/source/cmac_mode.c \
                   common/tinycrypt/source/aes_encrypt.c \
                   common/rpa.c \
                   common/work.c \
                   port/aos_port.c

$(NAME)_SOURCES += host/hci_ecc.c \
                   common/tinycrypt/source/utils.c \
                   common/tinycrypt/source/sha256.c \
                   common/tinycrypt/source/hmac.c \
                   common/tinycrypt/source/hmac_prng.c \
                   common/tinycrypt/source/cmac_mode.c \
                   common/tinycrypt/source/aes_encrypt.c \
                   common/tinycrypt/source/ecc.c \
                   common/tinycrypt/source/ecc_dh.c

bt_controller?=0
ifeq ($(bt_controller), 1)
$(NAME)_COMPONENTS += protocols.bluetooth.controller
GLOBAL_DEFINES += CONFIG_BT_CTLR
endif

ifeq ($(hci_h4),1)
$(NAME)_SOURCES += hci_drivers/h4.c
endif

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_DEFINES += CONFIG_AOS_BLUETOOTH
GLOBAL_DEFINES += CONFIG_BLUETOOTH
GLOBAL_DEFINES += CONFIG_BT_PERIPHERAL
GLOBAL_DEFINES += CONFIG_BT_SMP
GLOBAL_DEFINES += CONFIG_BT_CONN

GLOBAL_DEFINES += CONFIG_BLE_50

## BLE debug log general control macro (Note: still to be affected by DEBUG)
## Enable below macros if BLE stack debug needed
GLOBAL_DEFINES += CONFIG_BT_DEBUG_LOG
GLOBAL_DEFINES += CONFIG_BT_DEBUG

## BLE subsystem debug log control macro
## Enable below macros if component-specific debug needed
#GLOBAL_DEFINES += CONFIG_BT_DEBUG_L2CAP
#GLOBAL_DEFINES += CONFIG_BT_DEBUG_CONN
#GLOBAL_DEFINES += CONFIG_BT_DEBUG_ATT
#GLOBAL_DEFINES += CONFIG_BT_DEBUG_GATT
#GLOBAL_DEFINES += CONFIG_BT_DEBUG_HCI_DRIVER
#GLOBAL_DEFINES += CONFIG_BT_DEBUG_HCI_CORE
#GLOBAL_DEFINES += CONFIG_BT_DEBUG_CORE
