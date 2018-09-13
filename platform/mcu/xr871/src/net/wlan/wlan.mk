NAME := wlan

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := airkiss/airkiss_ack.c \
                   airkiss/airkiss_discover.c \
                   airkiss/airkiss_main.c \
                   smartconfig/smart_config_ack.c \
                   smartconfig/smart_config_crc.c \
                   smartconfig/smart_config_decode.c \
                   smartconfig/smart_config_main.c \
                   ethernetif.c \
                   wlan.c \
                   wlan_ctrl.c
