libcoap                         {#mainpage}
=======

A C implementation of the Constrained Application Protocol (RFC 7252)
=====================================================================

Copyright (C) 2010--2021 by Olaf Bergmann <bergmann@tzi.org> and others

About libcoap
=============

libcoap is a C implementation of a lightweight application-protocol
for devices that are constrained their resources such as computing
power, RF range, memory, bandwidth, or network packet sizes. This
protocol, CoAP, is standardized by the IETF as RFC 7252. For further
information related to CoAP, see <http://coap.technology>.

You might want to check out
[libcoap-minimal](https://github.com/obgm/libcoap-minimal) for usage
examples.

The following RFCs are supported

* RFC7252: The Constrained Application Protocol (CoAP)

* RFC7641: Observing Resources in the Constrained Application Protocol (CoAP)

* RFC7959: Block-Wise Transfers in the Constrained Application Protocol (CoAP)

* RFC7967: Constrained Application Protocol (CoAP) Option for No Server Response

* RFC8132: PATCH and FETCH Methods for the Constrained Application Protocol (CoAP)

* RFC8323: CoAP (Constrained Application Protocol) over TCP, TLS, and WebSockets
  [No WebSockets support]

* RFC8768: Constrained Application Protocol (CoAP) Hop-Limit Option

There is (D)TLS support for the following libraries

* OpenSSL (Minimum version 1.1.0) [PKI, PSK and PKCS11]

* GnuTLS (Minimum version 3.3.0) [PKI, PSK, RPK(3.6.6+) and PKCS11]

* Mbed TLS (Minimum version 2.7.10) [PKI and PSK] [Currently only DTLS]

* TinyDTLS [PSK and RPK] [DTLS Only]

Documentation
=============

This set of pages contains the current set of documention for the libcoap APIs.

License Information
===================

This library is published as open-source software without any warranty
of any kind. Use is permitted under the terms of the simplified BSD
license. It includes public domain software. libcoap binaries may also
include open-source software with their respective licensing terms.
Please refer to LICENSE for further details in the source.
