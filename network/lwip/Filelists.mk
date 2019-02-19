#
# Copyright (c) 2001, 2002 Swedish Institute of Computer Science.
# All rights reserved. 
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission. 
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
# SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
# OF SUCH DAMAGE.
#
# This file is part of the lwIP TCP/IP stack.
# 
# Author: Adam Dunkels <adam@sics.se>
#

# COREFILES, CORE4FILES: The minimum set of files needed for lwIP.
COREFILES=core/init.c \
	core/def.c \
	core/dns.c \
	core/inet_chksum.c \
	core/ip.c \
	core/mem.c \
	core/memp.c \
	core/netif.c \
	core/pbuf.c \
	core/raw.c \
	core/stats.c \
	core/sys.c \
	core/tcp.c \
	core/tcp_in.c \
	core/tcp_out.c \
	core/timeouts.c \
	core/udp.c

CORE4FILES=core/ipv4/autoip.c \
	core/ipv4/dhcp.c \
	core/ipv4/etharp.c \
	core/ipv4/icmp.c \
	core/ipv4/igmp.c \
	core/ipv4/ip4_frag.c \
	core/ipv4/ip4.c \
	core/ipv4/ip4_addr.c

CORE6FILES=core/ipv6/dhcp6.c \
	core/ipv6/ethip6.c \
	core/ipv6/icmp6.c \
	core/ipv6/inet6.c \
	core/ipv6/ip6.c \
	core/ipv6/ip6_addr.c \
	core/ipv6/ip6_frag.c \
	core/ipv6/mld6.c \
	core/ipv6/nd6.c

# APIFILES: The files which implement the sequential and socket APIs.
APIFILES=api/api_lib.c \
	api/api_msg.c \
	api/err.c \
	api/netbuf.c \
	api/netdb.c \
	api/netifapi.c \
	api/sockets.c \
	api/tcpip.c

# NETIFFILES: Files implementing various generic network interface functions
NETIFFILES=netif/ethernet.c \
	netif/slipif.c

# SIXLOWPAN: 6LoWPAN
SIXLOWPAN=netif/lowpan6.c \

# PPPFILES: PPP
PPPFILES=netif/ppp/auth.c \
	netif/ppp/ccp.c \
	netif/ppp/chap-md5.c \
	netif/ppp/chap_ms.c \
	netif/ppp/chap-new.c \
	netif/ppp/demand.c \
	netif/ppp/eap.c \
	netif/ppp/ecp.c \
	netif/ppp/eui64.c \
	netif/ppp/fsm.c \
	netif/ppp/ipcp.c \
	netif/ppp/ipv6cp.c \
	netif/ppp/lcp.c \
	netif/ppp/magic.c \
	netif/ppp/mppe.c \
	netif/ppp/multilink.c \
	netif/ppp/ppp.c \
	netif/ppp/pppapi.c \
	netif/ppp/pppcrypt.c \
	netif/ppp/pppoe.c \
	netif/ppp/pppol2tp.c \
	netif/ppp/pppos.c \
	netif/ppp/upap.c \
	netif/ppp/utils.c \
	netif/ppp/vj.c \
	netif/ppp/polarssl/arc4.c \
	netif/ppp/polarssl/des.c \
	netif/ppp/polarssl/md4.c \
	netif/ppp/polarssl/md5.c \
	netif/ppp/polarssl/sha1.c

# LWIPNOAPPSFILES: All LWIP files without apps
LWIPNOAPPSFILES=$(COREFILES) \
	$(CORE4FILES) \
	$(CORE6FILES) \
	$(APIFILES) \
	$(NETIFFILES) \
	$(PPPFILES) \
	$(SIXLOWPAN)

# SNMPFILES: SNMPv2c agent
SNMPFILES=apps/snmp/snmp_asn1.c \
	apps/snmp/snmp_core.c \
	apps/snmp/snmp_mib2.c \
	apps/snmp/snmp_mib2_icmp.c \
	apps/snmp/snmp_mib2_interfaces.c \
	apps/snmp/snmp_mib2_ip.c \
	apps/snmp/snmp_mib2_snmp.c \
	apps/snmp/snmp_mib2_system.c \
	apps/snmp/snmp_mib2_tcp.c \
	apps/snmp/snmp_mib2_udp.c \
	apps/snmp/snmp_msg.c \
	apps/snmp/snmpv3.c \
	apps/snmp/snmp_netconn.c \
	apps/snmp/snmp_pbuf_stream.c \
	apps/snmp/snmp_raw.c \
	apps/snmp/snmp_scalar.c \
	apps/snmp/snmp_table.c \
	apps/snmp/snmp_threadsync.c \
	apps/snmp/snmp_traps.c \
	apps/snmp/snmpv3_mbedtls.c \
	apps/snmp/snmpv3_dummy.c

# HTTPDFILES: HTTP server
HTTPDFILES=apps/httpd/fs.c \
	apps/httpd/httpd.c

# LWIPERFFILES: IPERF server
LWIPERFFILES=apps/lwiperf/iperf_task.c \
             apps/lwiperf/iperf_cli.c

# SNTPFILES: SNTP client
SNTPFILES=apps/sntp/sntp.c

# MDNSFILES: MDNS responder
MDNSFILES=apps/mdns/mdns.c

# NETBIOSNSFILES: NetBIOS name server
NETBIOSNSFILES=apps/netbiosns/netbiosns.c

# TFTPFILES: TFTP server files
TFTPFILES=apps/tftp/tftp_server.c \
    apps/tftp/tftp_common.c \
    apps/tftp/tftp_client.c \
    apps/tftp/tftp_ota.c

# TELNETDFILES: telnet server files
TELNETDFILES=apps/telnetd/telnetserver.c \
             apps/telnetd/telnetserver_cli.c

# DHCPDFILES: DHCP Server files
DHCPDFILES=apps/dhcpd/dhcps.c

# LWIPAPPFILES: All LWIP APPs
LWIPAPPFILES=$(SNMPFILES) \
	$(HTTPDFILES) \
	$(LWIPERFFILES) \
	$(SNTPFILES) \
	$(MDNSFILES) \
	$(NETBIOSNSFILES) \
	$(TFTPFILES) \
	$(TELNETDFILES) \
	$(DHCPDFILES)
