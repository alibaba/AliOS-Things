NAME := athost

$(NAME)_SOURCES := athost.c at_uart.c at_tcp_udp.c at_util.c at_wifi_mgmt.c at_uart.c at_wifi_ywss.c \
                   at_mqtt.c

$(NAME)_COMPONENTS := yloop atparser cli connectivity.mqtt

GLOBAL_DEFINES += AOS_ATCMD MQTT_TEST

GLOBAL_INCLUDES += ./include
