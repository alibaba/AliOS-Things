NAME := libawss

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

$(NAME)_DEFINES := USE_LPTHREAD

$(NAME)_COMPONENTS := digest_algorithm

$(NAME)_INCLUDES += . ../dev_bind/os/ ../dev_bind/os/product/ ../dev_bind/os/platform/
$(NAME)_SOURCES :=
$(NAME)_SOURCES += awss.c       awss_aha.c      awss_registrar.c     zconfig_protocol.c       awss_smartconfig.c
$(NAME)_SOURCES += awss_main.c  awss_wifimgr.c  zconfig_utils.c      zconfig_vendor_common.c  awss_aplist.c
$(NAME)_SOURCES += awss_crypt.c awss_enrollee.c zconfig_ieee80211.c  awss_adha.c              awss_wps.c
$(NAME)_SOURCES += awss_ht40.c  awss_info.c     awss_statis.c        awss_dev_ap.c

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += ESP8266_CONFIG
endif

$(NAME)_DEFINES += DEBUG
GLOBAL_DEFINES += CONFIG_YWSS
#GLOBAL_DEFINES += DEV_BIND_TEST

#
# If chipset or die set doesn't support device AP solution,
# or product doesn't need device AP solution, please remove
# the defination of AWSS_SUPPORT_DEV_AP
#
GLOBAL_DEFINES += AWSS_SUPPORT_DEV_AP

#
# If chipset or die set doesn't support smartconfig with broadcast,
# or product doesn't need smartconfig with broadcast, please remove
# the defination of AWSS_SUPPORT_SMARCONFIG
#
#Note:
#  AWSS_SUPPORT_SMARTCONFIG is depend on AWSS_SUPPORT_APLIST
#  AWSS_SUPPORT_SMARTCONFIG cann't work without AWSS_SUPPORT_APLIST
#
GLOBAL_DEFINES += AWSS_SUPPORT_SMARTCONFIG

#
# If chipset or die set doesn't support smartconfig with wps,
# or product doesn't need smartconfig with wps, please remove
# the defination of AWSS_SUPPORT_SMARCONFIG_WPS
#
#Note:
#  AWSS_SUPPORT_SMARTCONFIG_WPS is depend on AWSS_SUPPORT_SMARTCONFIG and AWSS_SUPPORT_APLIST
#  AWSS_SUPPORT_SMARTCONFIG_WPS can't work without AWSS_SUPPORT_SMARTCONFIG and AWSS_SUPPOrt_APLIST
#
GLOBAL_DEFINES += AWSS_SUPPORT_SMARTCONFIG_WPS

#
# If chipset or die set doesn't suppor AWSS with types of phone ap or router,
# or product doesn't need AWSS with types of phone ap or router, please remove
# the defination of AWSS_SUPPORT_AHA
#
#  AWSS_SUPPORT_AHA is depend on AWSS_SUPPORT_APLIST
#  AWSS_SUPPORT_AHA cann't work without AWSS_SUPPORT_APLIST
#
GLOBAL_DEFINES += AWSS_SUPPORT_AHA

#
# If chipset or die set doesn't suppor AWSS with type of router, or product
# doesn't need AWSS with type of router, please remove the defination of AWSS_SUPPORT_ADHA
#
#Note:
#  AWSS_SUPPORT_ADHA is depend on AWSS_SUPPORT_AHA and AWSS_SUPPORT_APLIST
#  AWSS_SUPPORT_ADHA can't work without AWSS_SUPPORT_AHA and AwSS_SUPPOrT_APLIST
#
GLOBAL_DEFINES += AWSS_SUPPORT_ADHA

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
# If chipset or die set doesn't support smartconfig with ht40, or product doesn't
# need smartconfig with ht40, please remove the defination of AWSS_SUPPORT_HT40
#
#Note:
#  Most of chipset or die set doesn't suport HT40, so we close HT40 defaultly.
#GLOBAL_DEFINES += AWSS_SUPPORT_HT40

#
# If chipset or die set doesn't support zconfig working as registrar,
# or product doesn't need to work as register, please define AWSS_DISABLE_REGISTRAR
# to disable registrar as follows:
#
# GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
#
# ESP8266 and MK3165 don't support registrar, so we define AWSS_DISABLE_REGISTRAR defaultly.
#
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3165)
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
endif

ifeq ($(COMPILER),)
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
endif

ifeq ($(auto_netmgr),0)
GLOBAL_DEFINES += AUTO_NETMGR_DISABLE
endif
