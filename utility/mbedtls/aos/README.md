## Introduction
This Mbed TLS layer is ported from the open source Mbed TLS, and we only modified the needed parts
to adapt to AliOS Things, so this layer provides the same APIs and features as the open source
Mbed TLS. For details, please refer to https://tls.mbed.org/ .

The major modification for porting Mbed TLS includes:
- provides our network socket APIs in aos/library/net_sockets.c
- provides our threading mutex APIs in aos/library/threading_alt.c
- provides our memory management APIs in aos/library/platform.c
- provides our configuration in include/mbedtls/config.h

## Dependencies
N/A

