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
 
CODEDIR=../../../../lwip
LWIPDIR=$(CODEDIR)/src

LDFLAGS:=$(LDFLAGS) -lcheck
CFLAGS:=$(CFLAGS) -I/usr/include/check -I$(LWIPDIR)/../test/unit \
	-I$(LWIPDIR)/include -I$(LWIPDIR)/include/ipv4 \
	-I$(LWIPDIR)/include/ipv6 -Iunix/include -I. -I../include

COREFILES=$(LWIPDIR)/core/mem.c $(LWIPDIR)/core/memp.c $(LWIPDIR)/core/netif.c \
	$(LWIPDIR)/core/pbuf.c $(LWIPDIR)/core/raw.c \
	$(LWIPDIR)/core/stats.c $(LWIPDIR)/core/sys.c \
        $(LWIPDIR)/core/tcp.c $(LWIPDIR)/core/tcp_in.c \
        $(LWIPDIR)/core/tcp_out.c $(LWIPDIR)/core/udp.c $(LWIPDIR)/core/dhcp.c \
	$(LWIPDIR)/core/init.c $(LWIPDIR)/core/timers.c $(LWIPDIR)/core/def.c \
	$(LWIPDIR)/core/ipv4/inet_chksum.c $(LWIPDIR)/netif/etharp.c
CORE4FILES=$(LWIPDIR)/core/ipv4/icmp.c $(LWIPDIR)/core/ipv4/ip.c \
	$(LWIPDIR)/core/ipv4/ip_addr.c $(LWIPDIR)/core/ipv4/ip_frag.c 

ARCHFILES=sys_arch.c

# LWIPFILES: All the above.
LWIPFILES=$(COREFILES) $(CORE4FILES) $(ARCHFILES)
LWIPOBJS=$(LWIPFILES:.c=.o)

TESTDIR=$(CODEDIR)/test/unit
TESTFILES=$(TESTDIR)/lwip_unittests.c $(TESTDIR)/udp/test_udp.c $(TESTDIR)/etharp/test_etharp.c $(TESTDIR)/tcp/tcp_helper.c \
	$(TESTDIR)/tcp/test_tcp_oos.c $(TESTDIR)/tcp/test_tcp.c $(TESTDIR)/core/test_mem.c
TESTOBJS=$(TESTFILES:.c=.o)

%.o: %.c Makefile $(TESTDIR)/lwipopts.h
	@$(CC) $(CFLAGS) -c $< -o $@

all: lwip_unittests
.PHONY: all clean check

clean:
	@rm -f lwip_unittests $(LWIPOBJS) $(TESTOBJS)

lwip_unittests: $(TESTOBJS) $(LWIPOBJS)
	@$(CC) $(TESTOBJS) $(LWIPOBJS) -o lwip_unittests $(LDFLAGS)

check: lwip_unittests
	@./lwip_unittests