NAME := libiot_awss

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := Alibaba Wireless Setup Service


$(NAME)_INCLUDES +=  ../dev_bind/impl/os/ ../dev_bind/impl/os/product/ ../dev_bind/impl/os/platform/ ../dev_bind/impl/ ../dev_bind/impl/awss_reset
$(NAME)_INCLUDES +=     frameworks/utils/ frameworks/aplist/
$(NAME)_SOURCES := frameworks/*.c  frameworks/ieee80211/*.c   frameworks/statics/*.c   frameworks/aplist/*.c frameworks/utils/*.c

$(NAME)_SOURCES-$(AWSS_SUPPORT_SMARTCONFIG) += smartconfig/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_SMARTCONFIG_MCAST) += mcast_smartconfig/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_ZEROCONFIG) += zero_config/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_SMARTCONFIG_WPS) += p2p/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_PHONEASAP) += phone_ap/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_DEV_AP) += dev_ap/*.c
$(NAME)_SOURCES-$(AWSS_SUPPORT_HT40) += ht40/*.c

ifeq (y,$(strip $(AWSS_SUPPORT_SMARTCONFIG)))
$(NAME)_INCLUDES += smartconfig
endif
ifeq (y,$(strip $(AWSS_SUPPORT_SMARTCONFIG_MCAST)))
$(NAME)_INCLUDES += mcast_smartconfig
endif
ifeq (y,$(strip $(AWSS_SUPPORT_ZEROCONFIG)))
GLOBAL_INCLUDES += zero_config
endif
ifeq (y,$(strip $(AWSS_SUPPORT_SMARTCONFIG_WPS)))
$(NAME)_INCLUDES += p2p
endif
ifeq (y,$(strip $(AWSS_SUPPORT_PHONEASAP)))
$(NAME)_INCLUDES += phone_ap
endif
ifeq (y,$(strip $(AWSS_SUPPORT_DEV_AP)))
$(NAME)_INCLUDES += dev_ap
endif
ifeq (y,$(strip $(AWSS_SUPPORT_HT40)))
$(NAME)_INCLUDES += ht40
endif

GLOBAL_INCLUDES += . frameworks frameworks/statics/   frameworks/ieee80211/  frameworks/utils/

$(NAME)_COMPONENTS := libiot_infra libiot_wrappers libiot_bind

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += ESP8266_CONFIG
endif

#
# If chipset or die set doesn't need to support statistic of AWSS,
# please remove the defination of AWSS_SUPPORT_STATIS
#
ifneq ($(CONFIG_SYSINFO_DEVICE_NAME), DH5021A)
GLOBAL_DEFINES += AWSS_SUPPORT_STATIS
endif

$(NAME)_DEFINES += DEBUG
GLOBAL_DEFINES += CONFIG_YWSS
#GLOBAL_DEFINES += DEV_BIND_TEST

#
# If chipset or die set doesn't support device AP solution,
# or product doesn't need device AP solution, please remove
# the defination of AWSS_SUPPORT_DEV_AP
#
ifeq ($(AWSS_SUPPORT_DEV_AP), y)
GLOBAL_DEFINES += AWSS_SUPPORT_DEV_AP
endif

#
# If chipset or die set doesn't support smartconfig with broadcast,
# or product doesn't need smartconfig with broadcast, please remove
# the defination of AWSS_SUPPORT_SMARCONFIG
#
#Note:
#  AWSS_SUPPORT_SMARTCONFIG is depend on AWSS_SUPPORT_APLIST
#  AWSS_SUPPORT_SMARTCONFIG cann't work without AWSS_SUPPORT_APLIST
#

ifeq ($(AWSS_SUPPORT_SMARTCONFIG), y)
GLOBAL_DEFINES += AWSS_SUPPORT_SMARTCONFIG
endif

#
# If chipset or die set doesn't support smartconfig with wps,
# or product doesn't need smartconfig with wps, please remove
# the defination of AWSS_SUPPORT_SMARCONFIG_WPS
#
#Note:
#  AWSS_SUPPORT_SMARTCONFIG_WPS is depend on AWSS_SUPPORT_SMARTCONFIG and AWSS_SUPPORT_APLIST
#  AWSS_SUPPORT_SMARTCONFIG_WPS can't work without AWSS_SUPPORT_SMARTCONFIG and AWSS_SUPPOrt_APLIST
#
ifeq ($(AWSS_SUPPORT_SMARTCONFIG), y)
GLOBAL_DEFINES += AWSS_SUPPORT_SMARTCONFIG_WPS
endif

#
# If chipset or die set doesn't support smartconfig with ht40, or product doesn't
# need smartconfig with ht40, please remove the defination of AWSS_SUPPORT_HT40
#
#Note:
#  Most of chipset or die set doesn't suport HT40, so we close HT40 defaultly.
#ifeq ($(AWSS_SUPPORT_SMARTCONFIG), y)
#GLOBAL_DEFINES += AWSS_SUPPORT_HT40
#endif

#
# If chipset or die set doesn't suppor AWSS with types of phone ap or router,
# or product doesn't need AWSS with types of phone ap or router, please remove
# the defination of AWSS_SUPPORT_AHA
#
#  AWSS_SUPPORT_AHA is depend on AWSS_SUPPORT_APLIST
#  AWSS_SUPPORT_AHA cann't work without AWSS_SUPPORT_APLIST
#
ifeq ($(AWSS_SUPPORT_PHONEASAP), y)
GLOBAL_DEFINES += AWSS_SUPPORT_AHA
endif

#
# If chipset or die set doesn't suppor AWSS with type of router, or product
# doesn't need AWSS with type of router, please remove the defination of AWSS_SUPPORT_ADHA
#
#Note:
#  AWSS_SUPPORT_ADHA is depend on AWSS_SUPPORT_AHA and AWSS_SUPPORT_APLIST
#  AWSS_SUPPORT_ADHA can't work without AWSS_SUPPORT_AHA and AwSS_SUPPOrT_APLIST
#

ifeq ($(AWSS_SUPPORT_ROUTER), y)
GLOBAL_DEFINES += AWSS_SUPPORT_ADHA
endif

#
# If chipset or die set doesn't suppor AWSS with aplist, or product doesn't need
# AWSS with aplist, please remove the defination of AWSS_SUPPORT_APLIST
#
#Note:
#  AWSS_SUPPORT_APLIST shouldn't to been closed, ortherwise, adha, aha and channel
#  amend would be disabled
#
GLOBAL_DEFINES += AWSS_SUPPORT_APLIST

#
# If chipset or die set doesn't support zconfig working as registrar,
# or product doesn't need to work as register, please define AWSS_DISABLE_REGISTRAR
# to disable registrar as follows:

ifneq ($(AWSS_SUPPORT_ZEROCONFIG), y)
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
else
#
# ESP8266 and MK3165 don't support registrar, so we define AWSS_DISABLE_REGISTRAR defaultly.
#
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3165)
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
endif
endif


ifeq ($(auto_netmgr),0)
GLOBAL_DEFINES += AUTO_NETMGR_DISABLE
endif
