NAME := ble_host_comp

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := BLE stack.

GLOBAL_DEFINES-y += AOS_BT

GLOBAL_INCLUDES-y += include \
                     include/ble_types \
                     bt_host/port/include \
                     bt_host/include \
					 bt_host/host

$(NAME)_INCLUDES += include/misc \
                      include/net \
                      bt_host \
                      bt_host/include/drivers \
                      bt_host/host \
                      bt_host/port/core/settings/include \
                      bt_host/bt_crypto/include \
                      bt_host/port/core/settings/src \
		      bt_crypto/include \
		      bt_crypto/tinycrypt/include

#$(NAME)_COMPONENTS += bt_common

$(NAME)_SOURCES := bt_host/host/uuid.c \
                     bt_host/host/hci_core.c \
                     bt_host/host/conn.c \
                     bt_host/host/l2cap.c \
                     bt_host/host/att.c \
                     bt_host/host/gatt.c \
                     bt_host/host/crypto.c \
                     bt_host/host/keys.c \
                     bt_host/host/hci_ecc.c \
                     bt_host/host/hci_api.c \
                     bt_host/host/smp.c \
		     bt_host/host/settings.c

$(NAME)_SOURCES += bt_host/common/log.c \
                   bt_host/common/rpa.c \
                   bt_host/common/dec.c \
                   bt_host/common/hex.c

$(NAME)_SOURCES += bt_host/port/core/buf.c \
                   bt_host/port/core/atomic_c.c \
                   bt_host/port/aos/event_scheduler.c

$(NAME)_SOURCES += bt_host/port/core/settings/src/settings_store.c \
                   bt_host/port/core/settings/src/settings.c \
                   bt_host/port/core/settings/src/settings_init.c \
                   bt_host/port/core/settings/src/settings_line.c \
                   bt_host/port/core/settings/src/settings_kv.c

$(NAME)_SOURCES += bt_host/aos/ble.c \
                   bt_host/port/aos/aos_port.c \
                   bt_host/port/aos/work.c  \
                   bt_host/port/aos/poll.c
		   
$(NAME)_SOURCES += bt_crypto/tinycrypt/source/cbc_mode.c \
                   bt_crypto/tinycrypt/source/cmac_mode.c \
                   bt_crypto/tinycrypt/source/aes_encrypt.c \
                   bt_crypto/tinycrypt/source/aes_decrypt.c \
                   bt_crypto/tinycrypt/source/hmac_prng.c \
                   bt_crypto/tinycrypt/source/hmac.c \
                   bt_crypto/tinycrypt/source/sha256.c \
                   bt_crypto/tinycrypt/source/utils.c \
                   bt_crypto/bt_crypto.c

#ifeq ($(hci_h4),1)
$(NAME)_SOURCES += bt_host/hci_driver/h4_driver.c
#endif

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
GLOBAL_DEFINES-y += CONFIG_BT_SMP
GLOBAL_DEFINES-y += CONFIG_BT_HCI_TX_PRIO=30
GLOBAL_DEFINES-y += CONFIG_BT_RX_PRIO=31
GLOBAL_DEFINES-y += CONFIG_BT_HCI_TX_STACK_SIZE=4096

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
GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_HCI_CORE
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_CORE
#GLOBAL_DEFINES-y += CONFIG_BT_DEBUG_SMP

# DO NOT DELETE, for RPM package
RPM_INCLUDE_DIR := wireless/bluetooth/bluetooth
