NAME := libiot_tls

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := ./library/asn1parse.c \
./library/aes.c \
./library/cipher.c \
./library/oid.c \
./library/md_wrap.c \
./library/debug.c \
./library/platform.c \
./library/ssl_cookie.c \
./library/pkparse.c \
./library/ctr_drbg.c \
./library/x509_crt.c \
./library/rsa.c \
./library/sha1.c \
./library/ssl_ciphersuites.c \
./library/timing.c \
./library/pem.c \
./library/md5.c \
./library/sha256.c \
./library/entropy.c \
./library/pk.c \
./library/md.c \
./library/entropy_poll.c \
./library/base64.c \
./library/pk_wrap.c \
./library/error.c \
./library/ssl_tls.c \
./library/x509.c \
./library/net_sockets.c \
./library/cipher_wrap.c \
./library/uOTA_required/ssl.c \
./library/uOTA_required/hash.c \
./library/ssl_cli.c \
./library/bignum.c \
 
$(NAME)_COMPONENTS := 

$(NAME)_INCLUDES := ../../../src/infra \
../../../src/infra/log \
../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/system \
 
