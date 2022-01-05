# libcoap: A C implementation of the Constrained Application Protocol (RFC 7252)

[![Build Status](https://travis-ci.org/obgm/libcoap.svg?branch=master)](https://travis-ci.org/obgm/libcoap)
[![Static Analysis](https://scan.coverity.com/projects/10970/badge.svg?flat=1)](https://scan.coverity.com/projects/obgm-libcoap)

Copyright (C) 2010—2019 by Olaf Bergmann <bergmann@tzi.org> and others

ABOUT LIBCOAP
=============

libcoap is a C implementation of a lightweight application-protocol
for devices that are constrained their resources such as computing
power, RF range, memory, bandwidth, or network packet sizes. This
protocol, CoAP, is standardized by the IETF as RFC 7252. For further
information related to CoAP, see <http://coap.technology>.

You might want to check out
[libcoap-minimal](https://github.com/obgm/libcoap-minimal) for usage
examples.

PACKAGE CONTENTS
================

This directory contains a protocol parser and basic networking
functions for platform with support for malloc() and BSD-style
sockets. The examples directory contains a client and a server to
demonstrate the use of this library.

LICENSE INFORMATION
===================

This library is published as open-source software without any warranty
of any kind. Use is permitted under the terms of the simplified BSD
license. It includes public domain software. libcoap binaries may also
include open-source software with their respective licensing terms.
Please refer to LICENSE for further details.

