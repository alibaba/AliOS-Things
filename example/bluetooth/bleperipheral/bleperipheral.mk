NAME := bleperipheral

$(NAME)_SOURCES     := main.c

$(NAME)_INCLUDES    := ../../../kernel/protocols/bluetooth/include \
                       ../../../kernel/protocols/bluetooth/core/include

ble = 1

$(NAME)_COMPONENTS  += protocols.bluetooth yloop cli bluetooth.profile
GLOBAL_DEFINES      += AOS_NO_WIFI CONFIG_BT_SMP
