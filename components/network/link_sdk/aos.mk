NAME := link_sdk

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := link sdk

#add osal files
$(NAME)_SOURCES := portfiles/aiot_port/alios_things_port.c

#add core files
$(NAME)_SOURCES += core/*.c core/sysdep/*.c  core/utils/*.c
GLOBAL_INCLUDES += core/  core/sysdep/ core/utils/

#add bootstrap
$(NAME)_SOURCES-$(LINK_SDK_BOOTSTRAP)   += components/bootstrap/*.c
GLOBAL_INCLUDES-$(LINK_SDK_BOOTSTRAP)  += components/bootstrap/

#add data model
$(NAME)_SOURCES-$(LINK_SDK_DATA_MODEL)   += components/data-model/*.c
GLOBAL_INCLUDES-$(LINK_SDK_DATA_MODEL)   += components/data-model/

#add devinfo
$(NAME)_SOURCES-$(LINK_SDK_DEVINFO)   += components/devinfo/*.c
GLOBAL_INCLUDES-$(LINK_SDK_DEVINFO)  += components/devinfo/

#add diag
$(NAME)_SOURCES-$(LINK_SDK_DIAG)   += components/diag/*.c
GLOBAL_INCLUDES-$(LINK_SDK_DIAG)   += components/diag/

#add logpost
$(NAME)_SOURCES-$(LINK_SDK_LOGPOST)   += components/logpost/*.c
GLOBAL_INCLUDES-$(LINK_SDK_LOGPOST)   += components/logpost/

#add ntp
$(NAME)_SOURCES-$(LINK_SDK_NTP)   += components/ntp/*.c
GLOBAL_INCLUDES-$(LINK_SDK_NTP)   += components/ntp/

#add ota
$(NAME)_SOURCES-$(LINK_SDK_OTA)   += components/ota/*.c
GLOBAL_INCLUDES-$(LINK_SDK_OTA)   += components/ota/

#add shadow
$(NAME)_SOURCES-$(LINK_SDK_SHADOW)   += components/shadow/*.c
GLOBAL_INCLUDES-$(LINK_SDK_SHADOW)   += components/shadow/

#add subdev
$(NAME)_SOURCES-$(LINK_SDK_SUBDEV)   += components/subdev/*.c
GLOBAL_INCLUDES-$(LINK_SDK_SUBDEV)   += components/subdev/

#add dynreg with http
$(NAME)_SOURCES-$(LINK_SDK_DYNREG_HTTP)   += components/dynreg/*.c
GLOBAL_INCLUDES-$(LINK_SDK_DYNREG_HTTP)   += components/dynreg/

#add dynreg with mqtt
$(NAME)_SOURCES-$(LINK_SDK_DYNREG_MQTT)   += components/dynreg-mqtt/*.c
GLOBAL_INCLUDES-$(LINK_SDK_DYNREG_MQTT)   += components/dynreg-mqtt/

#add mbedtls
$(NAME)_SOURCES-$(LINK_SDK_TLS)   += external/*.c
$(NAME)_COMPONENTS-$(LINK_SDK_TLS) += mbedtls

$(NAME)_INCLUDES += core/sysdep/
