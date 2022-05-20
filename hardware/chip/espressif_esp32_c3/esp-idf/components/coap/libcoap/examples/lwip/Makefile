top_srcdir = ../..
include_dir = $(top_srcdir)/include

LIBCOAP_API_VERSION = $(shell sed -ne 's/^LIBCOAP_API_VERSION=\([0-9]\+\).*$$/\1/p' $(top_srcdir)/configure.ac)

coap_include_dir = $(include_dir)/coap$(LIBCOAP_API_VERSION)

WITH_LWIP_BRANCH=STABLE-2_0_3_RELEASE
WITH_LWIP_CONTRIB_BRANCH=STABLE-2_0_1_RELEASE

# Need to determine which library clock_gettime() resides in (as found by ./autogen.sh)
LDLIBS := $(shell if [ ../../config.log ] ; then grep ac_cv_search_clock_gettime=- ../../config.log|cut -d= -f2 ; fi)

all: server

lwip:
	git clone --depth 1 git://git.savannah.nongnu.org/lwip.git -b $(WITH_LWIP_BRANCH)
	(cd lwip ; git checkout $(WITH_LWIP_BRANCH))
	$(MAKE)

lwip-contrib:
	git clone --depth 1 git://git.savannah.nongnu.org/lwip/lwip-contrib.git -b $(WITH_LWIP_CONTRIB_BRANCH)
	(cd lwip-contrib ; git checkout $(WITH_LWIP_CONTRIB_BRANCH))
	$(MAKE)

# lwip and coap opts (include early to shadow the lwip-contrib/ports/unix/proj/minimal/ file and any ../../config.h)
CFLAGS += -DWITH_LWIP -iquote.

# lwip library

LWIPOBJS = def.o init.o tapif.o etharp.o netif.o timeouts.o stats.o mem.o memp.o udp.o tcp.o pbuf.o ip4_addr.o ip4.o inet_chksum.o tcp_in.o tcp_out.o icmp.o raw.o ip4_frag.o sys_arch.o ethernet.o ip.o
vpath %.c lwip/src/core/ lwip-contrib/ports/unix/proj/minimal/ lwip/src/netif/ lwip/src/core/ipv4/ lwip-contrib/ports/unix/port/ lwip-contrib/ports/unix/port/netif/
# CFLAGS += -DLWIP_UNIX_LINUX

# if ipv6 is used
vpath %.c lwip/src/core/ipv6/
LWIPOBJS += mld6.o ip6.o icmp6.o ethip6.o nd6.o ip6_addr.o ip6_frag.o

# coap library

CFLAGS += -std=gnu99

# set path to coap_include_dir for lwippools.h
CFLAGS += -I$(top_srcdir)/include -I$(coap_include_dir)

vpath %.c $(top_srcdir)/src

COAPOBJS = net.o coap_cache.o coap_debug.o option.o resource.o pdu.o encode.o subscribe.o coap_io_lwip.o block.o uri.o str.o coap_session.o coap_notls.o coap_hashkey.o address.o coap_tcp.o async.o

CFLAGS += -g3 -Wall -Wextra -pedantic -O0
# not sorted out yet
CFLAGS += -Wno-unused-parameter

CFLAGS += -Ilwip/src/include/ -Ilwip/src/include/ipv4/ -Ilwip-contrib/ports/unix/port/include/ -Ilwip-contrib/ports/unix/proj/minimal/

OBJS = server.o server-coap.o ${LWIPOBJS} ${COAPOBJS}

$(coap_include_dir)/coap.h:
	@echo "Error: $@ not present. Run the autotools chain (\`./autogen.sh && ./configure\`) in the project root directory to build the required coap.h file."
	@exit 1

${OBJS}: lwip lwip-contrib $(coap_include_dir)/coap.h

server: ${OBJS}

clean:
	rm -f ${OBJS}

.PHONY: clean
