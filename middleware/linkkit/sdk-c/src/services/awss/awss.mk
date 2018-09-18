NAME := libawss

$(NAME)_DEFINES := USE_LPTHREAD

$(NAME)_COMPONENTS := digest_algorithm

$(NAME)_INCLUDES += . ../dev_bind/os/ ../dev_bind/os/product/ ../dev_bind/os/platform/ ../dev_bind/utility/
$(NAME)_SOURCES :=
$(NAME)_SOURCES += awss.c       registrar.c     zconfig_protocol.c
$(NAME)_SOURCES += awss_main.c  awss_wifimgr.c  zconfig_utils.c      zconfig_vendor_common.c
$(NAME)_SOURCES += enrollee.c   awss_crypt.c    zconfig_ieee80211.c

$(NAME)_DEFINES += DEBUG
GLOBAL_DEFINES += CONFIG_YWSS
#GLOBAL_DEFINES += DEV_BIND_TEST

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += ESP8266_CONFIG
endif

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
