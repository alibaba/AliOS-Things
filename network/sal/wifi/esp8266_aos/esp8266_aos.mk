NAME := device_sal_esp8266_aos

GLOBAL_DEFINES += DEV_SAL_ESP8266

$(NAME)_COMPONENTS += yloop sal

$(NAME)_SOURCES += esp8266.c
$(NAME)_SOURCES += esp8266_at.c
$(NAME)_SOURCES += ringbuffer.c
$(NAME)_SOURCES += wifi_port.c
GLOBAL_INCLUDES += ./
