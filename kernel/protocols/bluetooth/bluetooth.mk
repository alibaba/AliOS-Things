NAME := bluetooth

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += include \
                   include/drivers \
                   core/include \
                   port/include

$(NAME)_INCLUDES += core/tinycrypt/include \
                    ../../rhino/core/include

$(NAME)_COMPONENTS += yloop

ifeq ($(bt_mesh), 1)
$(NAME)_COMPONENTS += protocols.bluetooth.host.bt_mesh
$(NAME)_INCLUDES += include/bluetooth/mesh
GLOBAL_DEFINES += CONFIG_BT_MESH
endif

$(NAME)_SOURCES := core/atomic_c.c \
                   core/buf.c \
                   core/log.c \
                   core/poll.c \
                   host/uuid.c \
                   host/hci_core.c \
                   host/conn.c \
                   host/l2cap.c \
                   host/att.c \
                   host/gatt.c \
                   host/crypto.c \
                   host/smp.c \
                   host/keys.c \
                   core/tinycrypt/source/cmac_mode.c \
                   core/tinycrypt/source/aes_encrypt.c \
                   core/rpa.c \
                   core/work.c \
                   port/aos_port.c

$(NAME)_SOURCES += host/hci_ecc.c \
                   core/tinycrypt/source/utils.c \
                   core/tinycrypt/source/sha256.c \
                   core/tinycrypt/source/hmac.c \
                   core/tinycrypt/source/hmac_prng.c \
                   core/tinycrypt/source/cmac_mode.c \
                   core/tinycrypt/source/aes_encrypt.c \
                   core/tinycrypt/source/ecc.c \
                   core/tinycrypt/source/ecc_dh.c

ifeq ($(BLUETOOTH_INC_CONTROL), 1)			   
$(NAME)_SOURCES += controller/hal/nrf5/cntr.c \
                   controller/hal/nrf5/ecb.c \
                   controller/hal/nrf5/radio.c \
				   controller/hal/nrf5/rand.c \
				   controller/hci/hci.c \
				   controller/hci/hci_driver.c \
				   controller/ll_sw/ctrl.c \
				   controller/ll_sw/ll.c \
				   controller/ll_sw/ll_adv.c \
				   controller/ll_sw/ll_filter.c \
				   controller/ll_sw/ll_master.c \
				   controller/ll_sw/ll_scan.c \
				   controller/ll_sw/crypto.c \
				   controller/ticker/ticker.c \
				   controller/util/mayfly.c \
				   controller/util/mem.c \
				   controller/util/memq.c \
				   controller/util/util.c \
				   common/dummy.c \
				   common/irq_manage.c \
				   controller/hal/device.c

$(NAME)_INCLUDES += common \
					controller \
                    controller/hal/nrf5 \
					controller/hci \
					controller/include \
					controller/ll_sw \
					controller/ticker \
					controller/util
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
