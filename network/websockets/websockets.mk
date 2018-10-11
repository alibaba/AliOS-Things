NAME := websockets


GLOBAL_CFLAGS       += -Wall 


$(NAME)_COMPONENTS := mbedtls alicrypto connectivity.websockets.mbedtls_wrapper

$(NAME)_SOURCES := handshake.c libwebsockets.c service.c pollfd.c output.c context.c alloc.c header.c ssl.c 

$(NAME)_SOURCES += misc/base64-decode.c misc/lws-ring.c misc/lws-genhash.c misc/sha-1.c

#$(NAME)_SOURCES += server/ranges.c server/server.c server/parsers.c server/ssl-server.c server/server-handshake.c
$(NAME)_SOURCES +=    server/parsers.c

$(NAME)_SOURCES += client/client.c client/client-handshake.c client/client-parser.c client/ssl-client.c

$(NAME)_SOURCES += plat/lws-plat-aos.c

#$(NAME)_SOURCES += http2/hpack.c http2/http2.c http2/ssl-http2.c

GLOBAL_INCLUDES     += ./ ./mbedtls_wrapper/include


ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_WEBSOCKET := linux
else 
PLATFORM_WEBSOCKET := rhino
endif


