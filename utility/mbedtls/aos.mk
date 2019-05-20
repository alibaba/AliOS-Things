NAME := mbedtls

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.16.0
$(NAME)_SUMMARY := mbedtls is an implemetation of TLS/SSL protocol and cryptographic algorithms and support code required.

GLOBAL_INCLUDES     += aos/include/ include


ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS      += -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"
else
GLOBAL_DEFINES      += MBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
$(NAME)_DEFINES     += XTENSE_MALLOC_IRAM
endif

# Build crypto
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_AES)        := library/aes.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/aesni.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_ARC4)       += library/arc4.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_ARIA)       += library/aria.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/asn1parse.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/asn1write.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/base64.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/bignum.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_BLOWFISH)   += library/blowfish.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_CAMELLIA)   += library/camellia.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_MODE_CCM)   += library/ccm.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_CHACHA20)   += library/chacha20.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_CHACHAPOLY) += library/chachapoly.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/cipher.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/cipher_wrap.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/cmac.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/ctr_drbg.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_DES)        += library/des.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/dhm.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/ecdh.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/ecdsa.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/ecjpake.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/ecp.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/ecp_curves.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/entropy.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/entropy_poll.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_ERROR)             += library/error.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_MODE_GCM)   += library/gcm.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/havege.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/hkdf.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/hmac_drbg.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/md.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/md2.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/md4.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_MD5)        += library/md5.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/md_wrap.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/memory_buffer_alloc.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/nist_kw.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/oid.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/padlock.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/pem.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/pk.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/pk_wrap.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/pkcs12.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/pkcs5.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/pkparse.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/pkwrite.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/platform.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/platform_util.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_POLY1305)   += library/poly1305.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_RIPEMD160)  += library/ripemd160.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/rsa_internal.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/rsa.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_SHA1)       += library/sha1.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_SHA256)     += library/sha256.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_SHA512)     += library/sha512.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/threading.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/timing.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/version.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += library/version_features.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO_XTEA)       += library/xtea.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += aos/library/threading_alt.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_CRYPTO)            += aos/library/platform.c

# Build X509
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/certs.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/pkcs11.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/x509.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/x509_create.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/x509_crl.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/x509_crt.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/x509_csr.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/x509write_crt.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_X509) += library/x509write_csr.c

# Build TLS
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS_DEBUG) += library/debug.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS)       += library/ssl_cache.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS)       += library/ssl_ciphersuites.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS)       += library/ssl_cli.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS)       += library/ssl_cookie.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS)       += library/ssl_srv.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS)       += library/ssl_ticket.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS)       += library/ssl_tls.c
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_TLS)       += aos/library/net_sockets.c

# Test program
$(NAME)_SOURCES-$(MBEDTLS_CONFIG_SELFTEST) += programs/test/selftest.c
