NAME := net

$(NAME)_TYPE := kernel

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
$(NAME)_SOURCES += $(TFTPFILES)

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
