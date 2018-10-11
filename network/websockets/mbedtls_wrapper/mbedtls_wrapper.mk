NAME := mbedtls_wrapper


$(NAME)_COMPONENTS := mbedtls alicrypto 



$(NAME)_SOURCES := library/ssl_cert.c library/ssl_lib.c library/ssl_methods.c library/ssl_pkey.c library/ssl_stack.c library/ssl_x509.c platform/ssl_pm.c platform/ssl_port.c

GLOBAL_INCLUDES     += include/internal include/platform  

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_WEBSOCKET := linux
else 
PLATFORM_WEBSOCKET := rhino
endif


