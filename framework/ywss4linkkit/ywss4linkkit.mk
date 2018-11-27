NAME := ywss4linkkit

$(NAME)_TYPE := framework

$(NAME)_COMPONENTS += ywss4linkkit.libywss 

$(NAME)_COMPONENTS += digest_algorithm protocol.linkkit.sdk

GLOBAL_DEFINES += CONFIG_YWSS

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += ESP8266_CONFIG
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3165)
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
endif

$(NAME)_INCLUDES += ./libywss/os/  ./libywss/os/platform/ 
#$(NAME)_SOURCES +=  hal/HAL_AWSS_rhino.c


