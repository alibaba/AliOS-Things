NAME := mbedcrypto

$(NAME)_CFLAGS      += -Wall -Werror -Os -W -Wdeclaration-after-statement -D_FILE_OFFSET_BITS=64
GLOBAL_LDFLAGS      +=

GLOBAL_INCLUDES 	+= ./include
GLOBAL_INCLUDES 	+= ../../../include

$(NAME)_SOURCES     += 							\
           ./library/aes.c 						\

$(NAME)_SOURCES     += 							\
           ./library/md5.c 						\
           ./library/sha1.c 					\
           ./library/sha256.c 					\

$(NAME)_SOURCES     += 							\
           ./library/hash_wrap.c 				\
           ./library/bignum.c 					\
           ./library/oid.c 						\
           ./library/rsa.c 						\
           ./library/threading.c 				\
           ./library/mbedtls_alt.c 				\
           ./library/asn1parse.c 				\

$(NAME)_SOURCES     += 							\
           ./library/hmac.c 					\

