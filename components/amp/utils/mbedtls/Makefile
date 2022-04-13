TARGET = libmbedtls.a
MODULE = mbedtls

MOD_SOURCES := \
		library/aes.c \
		library/asn1parse.c \
		library/base64.c \
		library/bignum.c \
		library/cipher_wrap.c \
		library/cipher.c \
		library/ctr_drbg.c \
		library/debug.c \
		library/entropy_poll.c \
		library/entropy.c \
		library/error.c \
		library/md_wrap.c \
		library/md.c \
		library/md5.c \
		library/net_sockets.c \
		library/oid.c \
		library/pem.c \
		library/pk_wrap.c \
		library/pk.c \
		library/pkparse.c \
		library/platform.c \
		library/rsa.c \
		library/sha1.c \
		library/sha256.c \
		library/ssl_ciphersuites.c \
		library/ssl_cli.c \
		library/ssl_cookie.c \
		library/ssl_tls.c \
		library/timing.c \
		library/x509_crt.c \
		library/x509.c

MOD_INCLUDES := \
		../../adapter/platform/linux \
		include

include $(TOOLS_DIR)/rules.mk
