NAME := bluetooth

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += include \
                   include/drivers \
                   port/include

$(NAME)_INCLUDES += core/tinycrypt/include \
                    core/include \
                    profile \
                    ../../rhino/core/include

$(NAME)_COMPONENTS += yloop

$(NAME)_SOURCES := core/atomic_c.c \
                   core/buf.c \
                   host/uuid.c \
                   host/hci_core.c \
                   host/log.c \
                   core/tinycrypt/source/utils.c \
                   core/tinycrypt/source/sha256.c \
                   core/tinycrypt/source/hmac.c \
                   core/tinycrypt/source/hmac_prng.c \
                   host/conn.c \
                   host/l2cap.c \
                   host/att.c \
                   host/gatt.c \
                   host/smp_null.c \
                   core/work.c \
                   port/rhino_port.c

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall
endif

GLOBAL_DEFINES += CONFIG_AOS_BLUETOOTH
GLOBAL_DEFINES += CONFIG_BLUETOOTH
GLOBAL_DEFINES += CONFIG_AOS_RHINO
GLOBAL_DEFINES += CONFIG_BLUETOOTH_PERIPHERAL

## BLE debug log general control macro (Note: still to be affected by DEBUG)
## Enable below macros if BLE stack debug needed
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG_LOG
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG

## BLE subsystem debug log control macro
## Enable below macros if component-specific debug needed
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG_L2CAP
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG_CONN
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG_ATT
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG_GATT
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG_HCI_DRIVER
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG_HCI_CORE
#GLOBAL_DEFINES += CONFIG_BLUETOOTH_DEBUG_CORE
