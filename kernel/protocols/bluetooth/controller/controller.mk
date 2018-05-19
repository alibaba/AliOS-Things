NAME := controller

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

GLOBAL_INCLUDES += .

$(NAME)_INCLUDES += ../common \
                    . \
                    hal/nrf5 \
                    hci \
                    include \
                    ll_sw \
                    ticker \
                    util

$(NAME)_SOURCES += hal/nrf5/cntr.c \
                   hal/nrf5/ecb.c \
                   hal/nrf5/radio.c \
                   hal/nrf5/rand.c \
                   hci/hci.c \
                   hci/hci_driver.c \
                   ll_sw/ctrl.c \
                   ll_sw/ll.c \
                   ll_sw/ll_adv.c \
                   ll_sw/ll_filter.c \
                   ll_sw/ll_master.c \
                   ll_sw/ll_scan.c \
                   ll_sw/crypto.c \
                   ticker/ticker.c \
                   util/mayfly.c \
                   util/mem.c \
                   util/memq.c \
                   util/util.c \
                   hal/device.c \
                   ../common/irq_manage.c

GLOBAL_DEFINES += CONFIG_BT_OBSERVER
GLOBAL_DEFINES += CONFIG_BT_BROADCASTER
GLOBAL_DEFINES += CONFIG_DEVICE_POWER_MANAGEMENT
GLOBAL_DEFINES += CONFIG_BT_CTLR_LE_ENC
#GLOBAL_DEFINES += CONFIG_BT_CTLR_PHY_2M
GLOBAL_DEFINES += CONFIG_BT_CTLR_MIN_USED_CHAN
GLOBAL_DEFINES += CONFIG_BT_CTLR_PHY
GLOBAL_DEFINES += CONFIG_BT_CTLR_CHAN_SEL_2
GLOBAL_DEFINES += CONFIG_BT_CTLR_CONN_PARAM_REQ
