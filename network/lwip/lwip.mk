NAME := lwip

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

ifneq ($(no_with_lwip),1)
GLOBAL_DEFINES += WITH_LWIP
with_lwip := 1
endif

ifneq ($(use_private_lwip),1)
include network/lwip/Filelists.mk

GLOBAL_INCLUDES += include port/include

GLOBAL_DEFINES += CONFIG_NET_LWIP

$(NAME)_INCLUDES += port/include

$(NAME)_SOURCES := $(COREFILES)
$(NAME)_SOURCES += $(CORE4FILES)
$(NAME)_SOURCES += $(CORE6FILES)
$(NAME)_SOURCES += $(APIFILES)
$(NAME)_SOURCES += $(NETIFFILES)

use_lwip_tftp := 0
ifeq ($(use_lwip_tftp), 1)
GLOBAL_DEFINES += WITH_LWIP_TFPT
$(NAME)_SOURCES += $(TFTPFILES)
endif

use_private_telnetd ?= 1
ifneq ($(use_private_telnetd), 1)
GLOBAL_DEFINES += WITH_LWIP_TELNETD
$(NAME)_SOURCES += $(TELNETDFILES)
endif

#default use the private dhcpd
use_private_dhcpd := 1
ifneq ($(use_private_dhcpd), 1)
$(NAME)_SOURCES += $(DHCPDFILES)
endif
$(NAME)_SOURCES += port/sys_arch.c

endif
