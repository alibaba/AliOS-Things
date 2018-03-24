NAME := mbedtls

DEBUG := no

ifneq (,$(BINS))
MBEDTLS_SHARE := 0
ifeq ($(MBEDTLS_SHARE),1)
$(NAME)_TYPE := framework&kernel
else
$(NAME)_TYPE := kernel
GLOBAL_DEFINES     += MBEDTLS_IN_KERNEL
endif
endif

GLOBAL_INCLUDES     += include

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
$(NAME)_CFLAGS      += -Wall -Werror -Os
endif


ifeq ($(DEBUG), yes)
$(NAME)_DEFINES     += CONFIG_SSL_DEBUG
endif

ifeq ($(HOST_ARCH), linux)
ifeq ($(LWIP), 1)
$(NAME)_DEFINES     += LWIP_ENABLED
endif
else
$(NAME)_DEFINES     += LWIP_ENABLED
endif

ifeq ($(COMPILER),armcc)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/KEIL/libmbedtls.lib
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
$(NAME)_DEFINES     += XTENSE_MALLOC_IRAM
endif

$(NAME)_COMPONENTS  := alicrypto

$(NAME)_SOURCES     := src/mbedtls_net.c
$(NAME)_SOURCES     += src/mbedtls_ssl.c

$(NAME)_SOURCES     += src/aes.c
$(NAME)_SOURCES     += src/aesni.c
$(NAME)_SOURCES     += src/arc4.c
$(NAME)_SOURCES     += src/asn1write.c
$(NAME)_SOURCES     += src/base64.c
$(NAME)_SOURCES     += src/bignum.c
$(NAME)_SOURCES     += src/blowfish.c
$(NAME)_SOURCES     += src/camellia.c
$(NAME)_SOURCES     += src/ccm.c
$(NAME)_SOURCES     += src/cipher.c
$(NAME)_SOURCES     += src/cipher_wrap.c
$(NAME)_SOURCES     += src/cmac.c
$(NAME)_SOURCES     += src/ctr_drbg.c
$(NAME)_SOURCES     += src/debug.c
$(NAME)_SOURCES     += src/des.c
$(NAME)_SOURCES     += src/dhm.c
$(NAME)_SOURCES     += src/ecdh.c
$(NAME)_SOURCES     += src/ecdsa.c
$(NAME)_SOURCES     += src/ecjpake.c
$(NAME)_SOURCES     += src/ecp.c
$(NAME)_SOURCES     += src/ecp_curves.c
$(NAME)_SOURCES     += src/entropy.c
$(NAME)_SOURCES     += src/entropy_poll.c
$(NAME)_SOURCES     += src/error.c
$(NAME)_SOURCES     += src/gcm.c
$(NAME)_SOURCES     += src/havege.c
$(NAME)_SOURCES     += src/hmac_drbg.c
$(NAME)_SOURCES     += src/md2.c
$(NAME)_SOURCES     += src/md4.c
$(NAME)_SOURCES     += src/md.c
$(NAME)_SOURCES     += src/md_wrap.c
$(NAME)_SOURCES     += src/memory_buffer_alloc.c
$(NAME)_SOURCES     += src/net_sockets.c
$(NAME)_SOURCES     += src/oid.c
$(NAME)_SOURCES     += src/padlock.c
$(NAME)_SOURCES     += src/pem.c
$(NAME)_SOURCES     += src/pk.c
$(NAME)_SOURCES     += src/pkcs11.c
$(NAME)_SOURCES     += src/pkcs12.c
$(NAME)_SOURCES     += src/pkcs5.c
$(NAME)_SOURCES     += src/pkparse.c
$(NAME)_SOURCES     += src/pk_wrap.c
$(NAME)_SOURCES     += src/pkwrite.c
$(NAME)_SOURCES     += src/platform.c
$(NAME)_SOURCES     += src/ripemd160.c
$(NAME)_SOURCES     += src/rsa.c
$(NAME)_SOURCES     += src/sha512.c
$(NAME)_SOURCES     += src/ssl_cache.c
$(NAME)_SOURCES     += src/ssl_ciphersuites.c
$(NAME)_SOURCES     += src/ssl_cli.c
$(NAME)_SOURCES     += src/ssl_cookie.c
$(NAME)_SOURCES     += src/ssl_srv.c
$(NAME)_SOURCES     += src/ssl_ticket.c
$(NAME)_SOURCES     += src/ssl_tls.c
$(NAME)_SOURCES     += src/timing.c
$(NAME)_SOURCES     += src/version.c
$(NAME)_SOURCES     += src/version_features.c
$(NAME)_SOURCES     += src/x509.c
$(NAME)_SOURCES     += src/x509_create.c
$(NAME)_SOURCES     += src/x509_crl.c
$(NAME)_SOURCES     += src/x509_crt.c
$(NAME)_SOURCES     += src/x509_csr.c
$(NAME)_SOURCES     += src/x509write_crt.c
$(NAME)_SOURCES     += src/x509write_csr.c
$(NAME)_SOURCES     += src/xtea.c
$(NAME)_SOURCES     += src/mbedtls_alt.c

