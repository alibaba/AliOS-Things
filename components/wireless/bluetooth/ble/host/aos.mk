NAME := bt_host

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := BLE stack.

GLOBAL_DEFINES-y += AOS_BT

$(NAME)_INCLUDES-y += include \
                     include/drivers

$(NAME)_COMPONENTS-y += bt_common

$(NAME)_SOURCES-y := src/uuid.c \
                     src/hci_core.c \
                     src/conn.c \
                     src/l2cap.c \
                     src/att.c \
                     src/gatt.c \
                     src/crypto.c \
                     src/keys.c \
                     src/rpa.c

$(NAME)_SOURCES-y += src/hci_ecc.c

ifeq ($(hci_h4),1)
$(NAME)_SOURCES-y += hci_driver/h4.c
endif

ifeq ($(COMPILER),)
$(NAME)_CFLAGS-y      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS-y      += -Wall -Werror
endif

GLOBAL_DEFINES-y += CONFIG_BLE_LINK_PARAMETERS
GLOBAL_DEFINES-y += CONFIG_AOS_BLUETOOTH
GLOBAL_DEFINES-y += CONFIG_BLUETOOTH
GLOBAL_DEFINES-y += CONFIG_BT_PERIPHERAL
GLOBAL_DEFINES-y += CONFIG_BLE_50

EN_BT_SMP ?= 0
ifeq ($(EN_BT_SMP),1)
GLOBAL_DEFINES-y += CONFIG_BT_SMP
$(NAME)_SOURCES-y += src/smp.c
else
$(NAME)_SOURCES-y += src/smp_null.c
endif

## BLE debug log general control macro (Note: still to be affected by DEBUG)
## Enable below macros if BLE stack debug needed
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_LOG
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG

## BLE subsystem debug log control macro
## Enable below macros if component-specific debug needed
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_L2CAP
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_CONN
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_ATT
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_GATT
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_HCI_DRIVER
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_HCI_CORE
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_CORE
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_SMP
