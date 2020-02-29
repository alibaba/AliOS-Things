NAME := libiot_awss

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.2
$(NAME)_SUMMARY := Alibaba Wireless Setup Service

$(NAME)_INCLUDES +=   ./                    \
                      dev_bind/             \
                      dev_bind/awss_reset   \
                      dev_bind/os

$(NAME)_INCLUDES += ../iot_coap             \
                    ../iot_coap/server

$(NAME)_INCLUDES +=   frameworks             \
                      frameworks/statics/    \
                      frameworks/ieee80211/  \
                      frameworks/utils/      \
                      frameworks/aplist/

$(NAME)_SOURCES := frameworks/*.c           \
                   frameworks/ieee80211/*.c \
                   frameworks/statics/*.c   \
                   frameworks/aplist/*.c    \
                   frameworks/utils/*.c

$(NAME)_SOURCES += dev_bind/*.c           \
                   dev_bind/awss_reset/*.c \
                   dev_bind/os/*.c 

$(NAME)_COMPONENTS := libiot_infra    \
                      libiot_wrappers \
                      libiot_coap

$(NAME)_COMPONENTS-$(AWSS_SUPPORT_BLE_PROVISION) += breeze

$(NAME)_SOURCES-$(AWSS_SUPPORT_SMARTCONFIG) += smartconfig/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_SMARTCONFIG_MCAST) += mcast_smartconfig/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_ZEROCONFIG) += zero_config/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_SMARTCONFIG_WPS) += p2p/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_AHA) += phone_ap/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_DEV_AP) += dev_ap/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_HT40) += ht40/*.c

ifeq (y,$(strip $(AWSS_SUPPORT_SMARTCONFIG)))
$(NAME)_INCLUDES += smartconfig
endif
ifeq (y,$(strip $(AWSS_SUPPORT_SMARTCONFIG_MCAST)))
$(NAME)_INCLUDES += mcast_smartconfig
endif
ifeq (y,$(strip $(AWSS_SUPPORT_ZEROCONFIG)))
$(NAME)_INCLUDES += zero_config
endif
ifeq (y,$(strip $(AWSS_SUPPORT_SMARTCONFIG_WPS)))
$(NAME)_INCLUDES += p2p
endif
ifeq (y,$(strip $(AWSS_SUPPORT_AHA)))
$(NAME)_INCLUDES += phone_ap
endif
ifeq (y,$(strip $(AWSS_SUPPORT_DEV_AP)))
$(NAME)_INCLUDES += dev_ap
endif
ifeq (y,$(strip $(AWSS_SUPPORT_HT40)))
$(NAME)_INCLUDES += ht40
endif

$(NAME)_DEFINES += DEBUG
