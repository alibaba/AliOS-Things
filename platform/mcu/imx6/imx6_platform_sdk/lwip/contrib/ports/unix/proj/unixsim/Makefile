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

CCDEP=gcc
CC=gcc

#To compile for linux: make ARCH=linux
#To compile for cygwin: make ARCH=cygwin
ARCH=unix
CFLAGS=-g -Wall -D$(ARCH) -DIPv4 -DLWIP_DEBUG -pedantic -Werror \
	-Wparentheses -Wsequence-point -Wswitch-default \
	-Wextra -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast \
	-Wc++-compat -Wwrite-strings -Wold-style-definition \
	-Wmissing-prototypes -Wredundant-decls -Wnested-externs
# not used for now but interesting:
# -Wpacked
# -Wunreachable-code
# -ansi
# -std=c89
LDFLAGS=-lpthread -lutil
CONTRIBDIR=../../../..
LWIPARCH=$(CONTRIBDIR)/ports/unix
ARFLAGS=rs

#Set this to where you have the lwip core module checked out from CVS
#default assumes it's a dir named lwip at the same level as the contrib module
LWIPDIR=$(CONTRIBDIR)/../lwip/src

CFLAGS:=$(CFLAGS) \
	-I. -I$(CONTRIBDIR)/apps/httpserver_raw -I$(CONTRIBDIR)/apps/shell \
	-I$(CONTRIBDIR)/apps/tcpecho -I$(CONTRIBDIR)/apps/udpecho \
	-I$(LWIPDIR)/include -I$(LWIPARCH)/include -I$(LWIPDIR)/include/ipv4 \
	-I$(LWIPDIR)

# COREFILES, CORE4FILES: The minimum set of files needed for lwIP.
COREFILES=$(LWIPDIR)/core/mem.c $(LWIPDIR)/core/memp.c $(LWIPDIR)/core/netif.c \
	$(LWIPDIR)/core/pbuf.c $(LWIPDIR)/core/raw.c $(LWIPDIR)/core/stats.c \
	$(LWIPDIR)/core/sys.c $(LWIPDIR)/core/tcp.c $(LWIPDIR)/core/tcp_in.c \
	$(LWIPDIR)/core/tcp_out.c $(LWIPDIR)/core/udp.c $(LWIPDIR)/core/dhcp.c \
	$(LWIPDIR)/core/init.c $(LWIPDIR)/core/timers.c $(LWIPDIR)/core/def.c
CORE4FILES=$(wildcard $(LWIPDIR)/core/ipv4/*.c) $(LWIPDIR)/core/ipv4/inet.c \
	$(LWIPDIR)/core/ipv4/inet_chksum.c

# SNMPFILES: Extra SNMPv1 agent
SNMPFILES=$(LWIPDIR)/core/snmp/asn1_dec.c $(LWIPDIR)/core/snmp/asn1_enc.c \
	$(LWIPDIR)/core/snmp/mib2.c $(LWIPDIR)/core/snmp/mib_structs.c \
	$(LWIPDIR)/core/snmp/msg_in.c $(LWIPDIR)/core/snmp/msg_out.c

# APIFILES: The files which implement the sequential and socket APIs.
APIFILES=$(LWIPDIR)/api/api_lib.c $(LWIPDIR)/api/api_msg.c $(LWIPDIR)/api/tcpip.c \
	$(LWIPDIR)/api/err.c $(LWIPDIR)/api/sockets.c $(LWIPDIR)/api/netbuf.c $(LWIPDIR)/api/netdb.c

# NETIFFILES: Files implementing various generic network interface functions.'
NETIFFILES=$(LWIPDIR)/netif/etharp.c $(LWIPDIR)/netif/slipif.c

# NETIFFILES: Add PPP netif
NETIFFILES+=$(LWIPDIR)/netif/ppp/auth.c $(LWIPDIR)/netif/ppp/chap.c \
	$(LWIPDIR)/netif/ppp/chpms.c $(LWIPDIR)/netif/ppp/fsm.c \
	$(LWIPDIR)/netif/ppp/ipcp.c $(LWIPDIR)/netif/ppp/lcp.c \
	$(LWIPDIR)/netif/ppp/magic.c $(LWIPDIR)/netif/ppp/md5.c \
	$(LWIPDIR)/netif/ppp/pap.c $(LWIPDIR)/netif/ppp/ppp.c \
	$(LWIPDIR)/netif/ppp/randm.c $(LWIPDIR)/netif/ppp/vj.c \
	$(LWIPARCH)/netif/sio.c

# ARCHFILES: Architecture specific files.
ARCHFILES=$(wildcard $(LWIPARCH)/*.c $(LWIPARCH)/netif/tapif.c $(LWIPARCH)/netif/tunif.c $(LWIPARCH)/netif/unixif.c $(LWIPARCH)/netif/list.c $(LWIPARCH)/netif/tcpdump.c)

# APPFILES: Applications.
APPFILES=$(CONTRIBDIR)/apps/httpserver_raw/fs.c $(CONTRIBDIR)/apps/httpserver_raw/httpd.c \
	$(CONTRIBDIR)/apps/udpecho/udpecho.c $(CONTRIBDIR)/apps/tcpecho/tcpecho.c \
	$(CONTRIBDIR)/apps/shell/shell.c

# LWIPFILES: All the above.
LWIPFILES=$(COREFILES) $(CORE4FILES) $(SNMPFILES) $(APIFILES) $(NETIFFILES) $(ARCHFILES)
LWIPFILESW=$(wildcard $(LWIPFILES))
LWIPOBJS=$(notdir $(LWIPFILESW:.c=.o))

LWIPLIB=liblwip4.a
APPLIB=liblwipapps.a
APPOBJS=$(notdir $(APPFILES:.c=.o))

%.o:
	$(CC) $(CFLAGS) -c $(<:.o=.c)

all ipv4 compile: simhost 
.PHONY: all

clean:
	rm -f *.o $(LWIPLIB) $(APPLIB) simhost simnode simrouter *.s .depend* *.core core

depend dep: .depend

include .depend

$(APPLIB): $(APPOBJS)
	$(AR) $(ARFLAGS) $(APPLIB) $?

$(LWIPLIB): $(LWIPOBJS)
	$(AR) $(ARFLAGS) $(LWIPLIB) $?

.depend: simhost.c simnode.c simrouter.c $(LWIPFILES) $(APPFILES)
	$(CCDEP) $(CFLAGS) -MM $^ > .depend || rm -f .depend

simhost: .depend $(LWIPLIB) $(APPLIB) simhost.o $(APPFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o simhost simhost.o $(APPLIB) $(LWIPLIB)

simrouter: .depend $(LWIPLIB) $(APPLIB) simrouter.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o simrouter simrouter.o $(APPLIB) $(LWIPLIB) 

simnode: .depend $(LWIPLIB) $(APPLIB) simnode.o 
	$(CC) $(CFLAGS) $(LDFLAGS) -o simnode simnode.o $(APPLIB) $(LWIPLIB)

