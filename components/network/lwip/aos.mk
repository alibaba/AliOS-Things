NAME := lwip

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := lightweight tcp/ip stack

ifeq ($(CONFIG_AOS_LWIP), y)
include components/network/lwip/Filelists.mk

GLOBAL_INCLUDES += include port/include

$(NAME)_SOURCES := $(COREFILES)
$(NAME)_SOURCES += $(CORE4FILES)
$(NAME)_SOURCES += $(CORE6FILES)
$(NAME)_SOURCES += $(APIFILES)
$(NAME)_SOURCES += $(NETIFFILES)

ifeq (y,$(AF_PACKET_ENABLED))
#define LWIP_PACKET  1
$(NAME)_SOURCES += core/af_packet.c
endif

ifeq (y,$(SNMP_ENABLED))
$(NAME)_SOURCES += $(SNMPFILES)
endif

ifeq (y,$(HTTPD_ENABLED))
$(NAME)_SOURCES += $(HTTPDFILES)
endif

ifeq (y,$(IPERF_ENABLED))
$(NAME)_SOURCES += $(IPERFFILES)
endif

ifeq (y,$(SNTP_ENABLED))
$(NAME)_SOURCES += $(SNTPFILES)
endif

ifeq (y,$(MDNS_ENABLED))
$(NAME)_SOURCES += $(MDNSFILES)
endif

ifeq (y,$(NETBIOSNS_ENABLED))
$(NAME)_SOURCES += $(NETBIOSNSFILES)
endif

ifeq (y,$(TFTP_ENABLED))
$(NAME)_SOURCES += $(TFTPFILES)
endif

ifeq (y,$(TELNETD_ENABLED))
$(NAME)_SOURCES += $(TELNETDFILES)
endif

ifeq (y,$(DHCPD_ENABLED))
$(NAME)_SOURCES += $(DHCPDFILES)
endif

ifeq (y,$(PING_ENABLED))
$(NAME)_SOURCES += $(PINGFILES)
endif

ifeq (y,$(SENDFILE_ENABLED))
$(NAME)_SOURCES += $(SENDFILEFILES)
endif

ifeq (y,$(DNSCLI_ENABLED))
$(NAME)_SOURCES += $(DNSCLIFILES)
endif

ifeq (y,$(ARPCLI_ENABLED))
$(NAME)_SOURCES += $(ARPCLIFILES)
endif

ifeq (y,$(LSFDCLI_ENABLED))
$(NAME)_SOURCES += $(LSFDCLIFILES)
endif

$(NAME)_SOURCES += port/sys_arch.c

endif
