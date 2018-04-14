NAME := ywss4linkkit

$(NAME)_TYPE := framework

$(NAME)_COMPONENTS += ywss4linkkit.libywss 

$(NAME)_COMPONENTS += digest_algorithm protocol.alink-ilop connectivity.link-coap connectivity.mqtt

GLOBAL_DEFINES += CONFIG_YWSS

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += ESP8266_CONFIG
endif


$(NAME)_INCLUDES += ./libywss/os/  ./libywss/os/platform/ 
#$(NAME)_SOURCES +=  hal/HAL_AWSS_rhino.c


