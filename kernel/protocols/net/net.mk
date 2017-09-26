NAME := net

$(NAME)_TYPE := kernel
include kernel/protocols/net/Filelists.mk
GLOBAL_INCLUDES += include port/include
ifneq ($(no_with_lwip),1)
GLOBAL_DEFINES += WITH_LWIP
with_lwip := 1
endif
GLOBAL_DEFINES += CONFIG_NET_LWIP

$(NAME)_INCLUDES += port/include

$(NAME)_SOURCES := $(COREFILES)
$(NAME)_SOURCES += $(CORE4FILES)
$(NAME)_SOURCES += $(CORE6FILES)
$(NAME)_SOURCES += $(APIFILES)
$(NAME)_SOURCES += $(NETIFFILES)
$(NAME)_SOURCES += $(TFTPFILES)

ifneq (,$(filter linuxhost,$(COMPONENTS)))
$(NAME)_SOURCES += port/sys_arch.c
endif

ifneq (,$(filter armhflinux,$(COMPONENTS)))
$(NAME)_SOURCES += port/sys_arch.c
endif

ifneq (,$(filter mk108 mk3060 b_l475e,$(COMPONENTS)))
ifneq ($(mico_lwip), 1)
$(NAME)_SOURCES += port/sys_arch.c
endif
endif
