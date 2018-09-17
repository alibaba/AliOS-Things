#-------------------------------------------------------------------------------
# Copyright (C) 2013 Freescale Semiconductor, Inc. All Rights Reserved.
#
# THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
# SHALL FREESCALE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
# OF SUCH DAMAGE.
#-------------------------------------------------------------------------------

include $(SDK_ROOT)/mk/common.mk

# Export the target library name to all child make invocations.
TARGET_LIB_NAME = lwip

# Note: PPP sources are not included in the build.
define SOURCES
lwip/src/api/api_lib.c
lwip/src/api/api_msg.c
lwip/src/api/err.c
lwip/src/api/netbuf.c
lwip/src/api/netdb.c
lwip/src/api/netifapi.c
lwip/src/api/sockets.c
lwip/src/api/tcpip.c
lwip/src/core/def.c
lwip/src/core/dhcp.c
lwip/src/core/dns.c
lwip/src/core/inet_chksum.c
lwip/src/core/init.c
lwip/src/core/mem.c
lwip/src/core/memp.c
lwip/src/core/netif.c
lwip/src/core/pbuf.c
lwip/src/core/raw.c
lwip/src/core/stats.c
lwip/src/core/sys.c
lwip/src/core/tcp_in.c
lwip/src/core/tcp_out.c
lwip/src/core/tcp.c
lwip/src/core/timers.c
lwip/src/core/udp.c
lwip/src/core/ipv4/autoip.c
lwip/src/core/ipv4/icmp.c
lwip/src/core/ipv4/igmp.c
lwip/src/core/ipv4/ip_frag.c
lwip/src/core/ipv4/ip4_addr.c
lwip/src/core/ipv4/ip4.c
lwip/src/core/ipv6/dhcp6.c
lwip/src/core/ipv6/ethip6.c
lwip/src/core/ipv6/icmp6.c
lwip/src/core/ipv6/inet6.c
lwip/src/core/ipv6/ip6_addr.c
lwip/src/core/ipv6/ip6_frag.c
lwip/src/core/ipv6/ip6.c
lwip/src/core/ipv6/mld6.c
lwip/src/core/ipv6/nd6.c
lwip/src/core/snmp/asn1_dec.c
lwip/src/core/snmp/asn1_enc.c
lwip/src/core/snmp/mib_structs.c
lwip/src/core/snmp/mib2.c
lwip/src/core/snmp/msg_in.c
lwip/src/core/snmp/msg_out.c
lwip/src/netif/etharp.c
lwip/src/netif/slipif.c
mx6/sys_arch.c
mx6/mx6_ethernetif.c
mx6/mx6_lwip.c
endef

# Turn off this warning to get the lwIP sources to compile unmodified.
CFLAGS += -Wno-address

include $(SDK_ROOT)/mk/targets.mk
