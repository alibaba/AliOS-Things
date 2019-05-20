NAME := mbedtls

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.16.0
$(NAME)_SUMMARY := mbedtls is an implemetation of TLS/SSL protocol and cryptographic algorithms and support code required.

DEBUG := no

GLOBAL_INCLUDES     += aos/include/ include

ifeq ($(DEBUG), yes)
$(NAME)_DEFINES     += CONFIG_SSL_DEBUG
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
$(NAME)_DEFINES     += XTENSA_MALLOC_IRAM
endif

ifeq ($(HOST_ARCH),linux)
CONFIG_MBEDTLS_TLS := y
else
ifeq ($(with_lwip),1)
CONFIG_MBEDTLS_TLS := y
endif
endif

# Build crypto
$(NAME)_SOURCES := library/aes.c
$(NAME)_SOURCES += library/aesni.c
$(NAME)_SOURCES += library/arc4.c
$(NAME)_SOURCES += library/aria.c
$(NAME)_SOURCES += library/asn1parse.c
$(NAME)_SOURCES += library/asn1write.c
$(NAME)_SOURCES += library/base64.c
$(NAME)_SOURCES += library/bignum.c
$(NAME)_SOURCES += library/blowfish.c
$(NAME)_SOURCES += library/camellia.c
$(NAME)_SOURCES += library/ccm.c
$(NAME)_SOURCES += library/chacha20.c
$(NAME)_SOURCES += library/chachapoly.c
$(NAME)_SOURCES += library/cipher.c
$(NAME)_SOURCES += library/cipher_wrap.c
$(NAME)_SOURCES += library/cmac.c
$(NAME)_SOURCES += library/ctr_drbg.c
$(NAME)_SOURCES += library/des.c
$(NAME)_SOURCES += library/dhm.c
$(NAME)_SOURCES += library/ecdh.c
$(NAME)_SOURCES += library/ecdsa.c
$(NAME)_SOURCES += library/ecjpake.c
$(NAME)_SOURCES += library/ecp.c
$(NAME)_SOURCES += library/ecp_curves.c
$(NAME)_SOURCES += library/entropy.c
$(NAME)_SOURCES += library/entropy_poll.c
$(NAME)_SOURCES += library/error.c
$(NAME)_SOURCES += library/gcm.c
$(NAME)_SOURCES += library/havege.c
$(NAME)_SOURCES += library/hkdf.c
$(NAME)_SOURCES += library/hmac_drbg.c
$(NAME)_SOURCES += library/md.c
$(NAME)_SOURCES += library/md2.c
$(NAME)_SOURCES += library/md4.c
$(NAME)_SOURCES += library/md5.c
$(NAME)_SOURCES += library/md_wrap.c
$(NAME)_SOURCES += library/memory_buffer_alloc.c
$(NAME)_SOURCES += library/nist_kw.c
$(NAME)_SOURCES += library/oid.c
$(NAME)_SOURCES += library/padlock.c
$(NAME)_SOURCES += library/pem.c
$(NAME)_SOURCES += library/pk.c
$(NAME)_SOURCES += library/pk_wrap.c
$(NAME)_SOURCES += library/pkcs12.c
$(NAME)_SOURCES += library/pkcs5.c
$(NAME)_SOURCES += library/pkparse.c
$(NAME)_SOURCES += library/pkwrite.c
$(NAME)_SOURCES += library/platform.c
$(NAME)_SOURCES += library/platform_util.c
$(NAME)_SOURCES += library/poly1305.c
$(NAME)_SOURCES += library/ripemd160.c
$(NAME)_SOURCES += library/rsa_internal.c
$(NAME)_SOURCES += library/rsa.c
$(NAME)_SOURCES += library/sha1.c
$(NAME)_SOURCES += library/sha256.c
$(NAME)_SOURCES += library/sha512.c
$(NAME)_SOURCES += library/threading.c
$(NAME)_SOURCES += library/timing.c
$(NAME)_SOURCES += library/version.c
$(NAME)_SOURCES += library/version_features.c
$(NAME)_SOURCES += library/xtea.c
# Build X509
$(NAME)_SOURCES += library/certs.c
$(NAME)_SOURCES += library/pkcs11.c
$(NAME)_SOURCES += library/x509.c
$(NAME)_SOURCES += library/x509_create.c
$(NAME)_SOURCES += library/x509_crl.c
$(NAME)_SOURCES += library/x509_crt.c
$(NAME)_SOURCES += library/x509_csr.c
$(NAME)_SOURCES += library/x509write_crt.c
$(NAME)_SOURCES += library/x509write_csr.c
# Build TLS
ifeq ($(CONFIG_MBEDTLS_TLS),y)
$(NAME)_SOURCES += library/debug.c
$(NAME)_SOURCES += library/net_sockets.c
$(NAME)_SOURCES += library/ssl_cache.c
$(NAME)_SOURCES += library/ssl_ciphersuites.c
$(NAME)_SOURCES += library/ssl_cli.c
$(NAME)_SOURCES += library/ssl_cookie.c
$(NAME)_SOURCES += library/ssl_srv.c
$(NAME)_SOURCES += library/ssl_ticket.c
$(NAME)_SOURCES += library/ssl_tls.c
endif

# Adapt for AliOS Things.
$(NAME)_SOURCES += aos/library/threading_alt.c
$(NAME)_SOURCES += aos/library/platform.c
ifeq ($(CONFIG_MBEDTLS_TLS),y)
$(NAME)_SOURCES += aos/library/net_sockets.c
endif

# Test program
$(NAME)_SOURCES += programs/test/selftest.c
