NAME := imbedtls

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.1
$(NAME)_SUMMARY := Mbed Transport Layer Security on Embedded Devices for AliOS Things
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/mbedtls

DEBUG := no

ifneq (,$(BINS))
GLOBAL_DEFINES      += MBEDTLS_IN_KERNEL
endif

GLOBAL_INCLUDES     += ../include

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif
$(NAME)_DEFINES     += CONFIG_PLAT_AOS

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

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
$(NAME)_DEFINES     += XTENSA_MALLOC_IRAM
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP32)
$(NAME)_DEFINES     += CONFIG_PLAT_AOS_ESP32
endif

$(NAME)_SOURCES     := src/aes.c
$(NAME)_SOURCES     += src/aesni.c
$(NAME)_SOURCES     += src/arc4.c
$(NAME)_SOURCES     += src/asn1write.c
$(NAME)_SOURCES     += src/asn1parse.c
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
$(NAME)_SOURCES     += src/md5.c
$(NAME)_SOURCES     += src/sha1.c
$(NAME)_SOURCES     += src/sha256.c
$(NAME)_SOURCES     += src/sha512.c
$(NAME)_SOURCES     += src/ssl_cache.c
$(NAME)_SOURCES     += src/ssl_ciphersuites.c
$(NAME)_SOURCES     += src/ssl_cli.c
$(NAME)_SOURCES     += src/ssl_cookie.c
$(NAME)_SOURCES     += src/ssl_srv.c
$(NAME)_SOURCES     += src/ssl_ticket.c
$(NAME)_SOURCES     += src/ssl_tls.c
$(NAME)_SOURCES     += src/timing.c
$(NAME)_SOURCES     += src/threading.c
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
$(NAME)_SOURCES     += src/mbedtls_net.c
$(NAME)_SOURCES     += src/mbedtls_ssl.c

$(NAME)_COMPONENTS  := alicrypto

