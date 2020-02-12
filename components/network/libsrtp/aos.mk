NAME := srtp

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := srtp component

GLOBAL_INCLUDES += ./include \
                   ./crypto/include

$(NAME)_SOURCES := ./srtp/ekt.c \
                   ./srtp/srtp.c \
                   ./crypto/math/datatypes.c \
                   ./crypto/math/stat.c \
                   ./crypto/kernel/err.c \
                   ./crypto/kernel/alloc.c \
                   ./crypto/kernel/crypto_kernel.c \
                   ./crypto/kernel/key.c \
                   ./crypto/cipher/cipher.c \
                   ./crypto/cipher/null_cipher.c \
                   ./crypto/cipher/aes_icm.c \
                   ./crypto/cipher/aes.c \
                   ./crypto/cipher/aes_gcm_mbedtls.c \
                   ./crypto/replay/rdbx.c \
                   ./crypto/replay/rdb.c \
                   ./crypto/hash/auth.c \
                   ./crypto/hash/null_auth.c \
                   ./crypto/hash/hmac.c \
                   ./crypto/hash/sha1.c

