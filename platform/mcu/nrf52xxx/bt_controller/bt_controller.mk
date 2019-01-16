GLOBAL_INCLUDES += bt_controller/.

$(NAME)_INCLUDES += bt_controller/hal/nrf5 \
                    bt_controller/hci \
                    bt_controller/include \
                    bt_controller/ll_sw \
                    bt_controller/ticker \
                    bt_controller/util \
		    bt_controller/irq \

$(NAME)_SOURCES += bt_controller/hal/nrf5/cntr.c \
                   bt_controller/hal/nrf5/ecb.c \
                   bt_controller/hal/nrf5/radio.c \
                   bt_controller/hal/nrf5/rand.c \
                   bt_controller/hci/hci.c \
                   bt_controller/hci/hci_driver.c \
                   bt_controller/ll_sw/ctrl.c \
                   bt_controller/ll_sw/ll.c \
                   bt_controller/ll_sw/ll_adv.c \
                   bt_controller/ll_sw/ll_filter.c \
                   bt_controller/ll_sw/ll_master.c \
                   bt_controller/ll_sw/ll_scan.c \
                   bt_controller/ll_sw/crypto.c \
                   bt_controller/ticker/ticker.c \
                   bt_controller/util/mayfly.c \
                   bt_controller/util/mem.c \
                   bt_controller/util/memq.c \
                   bt_controller/util/util.c \
                   bt_controller/hal/device.c \
	           bt_controller/irq/irq_manage.c \

GLOBAL_DEFINES-y += CONFIG_BT_CTLR
GLOBAL_DEFINES-y += CONFIG_BT_OBSERVER
GLOBAL_DEFINES-y += CONFIG_BT_BROADCASTER
GLOBAL_DEFINES-y += CONFIG_DEVICE_POWER_MANAGEMENT
GLOBAL_DEFINES-y += CONFIG_BT_CTLR_LE_ENC
GLOBAL_DEFINES-y += CONFIG_BT_CTLR_MIN_USED_CHAN
GLOBAL_DEFINES-y += CONFIG_BT_CTLR_CHAN_SEL_2
GLOBAL_DEFINES-y += CONFIG_BT_CTLR_TIFS_HW
GLOBAL_DEFINES-y += CONFIG_CONTROLLER_IN_ONE_TASK
