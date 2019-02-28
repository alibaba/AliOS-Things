
NAME := alicrypto

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.1
$(NAME)_SUMMARY := Crypto algorithms and APIs for AliOS Things.
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR    := ../include/icrypto

GLOBAL_INCLUDES     += ../include/icrypto

$(NAME)_INCLUDES    := ./core/inc
$(NAME)_INCLUDES    += ./sal/inc
$(NAME)_INCLUDES    += ./api/inc
$(NAME)_INCLUDES    += ./hal/inc
$(NAME)_INCLUDES    += ./config

$(NAME)_DEFINES     += CONFIG_PLAT_AOS
$(NAME)_DEFINES     += CONFIG_CRYPT_ALI_ALGO=1

$(NAME)_DEFINES     += CONFIG_SAL_AES_ECB=1
$(NAME)_DEFINES     += CONFIG_HAL_AES_ECB=0
$(NAME)_DEFINES     += CONFIG_SAL_AES_CBC=1
$(NAME)_DEFINES     += CONFIG_HAL_AES_CBC=0
$(NAME)_DEFINES     += CONFIG_SAL_AES_CTR=1
$(NAME)_DEFINES     += CONFIG_HAL_AES_CTR=0
$(NAME)_DEFINES     += CONFIG_SAL_AES_CFB=1
$(NAME)_DEFINES     += CONFIG_HAL_AES_CFB=0
$(NAME)_DEFINES     += CONFIG_SAL_SHA1=1
$(NAME)_DEFINES     += CONFIG_HAL_SHA1=0
$(NAME)_DEFINES     += CONFIG_SAL_SHA256=1
$(NAME)_DEFINES     += CONFIG_HAL_SHA256=0
$(NAME)_DEFINES     += CONFIG_SAL_MD5=1
$(NAME)_DEFINES     += CONFIG_HAL_MD5=0

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
$(NAME)_DEFINES     += ALI_ALGO_AES_ROM_TABLES
endif
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), CKHOBBIT)
$(NAME)_DEFINES     += ALI_ALGO_AES_ROM_TABLES
endif
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), DH5021A)
$(NAME)_DEFINES     += ALI_ALGO_AES_ROM_TABLES
endif

$(NAME)_SOURCES     := ./core/src/aes.c
$(NAME)_SOURCES     += ./core/src/sm4.c
$(NAME)_SOURCES     += ./core/src/md5.c
$(NAME)_SOURCES     += ./core/src/sha1.c
$(NAME)_SOURCES     += ./core/src/sha256.c
$(NAME)_SOURCES     += ./core/src/sm3.c
$(NAME)_SOURCES     += ./core/src/hash_wrap.c
$(NAME)_SOURCES     += ./core/src/bignum.c
$(NAME)_SOURCES     += ./core/src/oid.c
$(NAME)_SOURCES     += ./core/src/ecp.c
$(NAME)_SOURCES     += ./core/src/ecp_curves.c
$(NAME)_SOURCES     += ./core/src/rsa.c
$(NAME)_SOURCES     += ./core/src/sm2.c
$(NAME)_SOURCES     += ./core/src/asn1parse.c
$(NAME)_SOURCES     += ./core/src/hmac.c
$(NAME)_SOURCES     += ./core/src/threading.c
$(NAME)_SOURCES     += ./sal/src/sal_aes.c
$(NAME)_SOURCES     += ./sal/src/sal_sm4.c
$(NAME)_SOURCES     += ./sal/src/sal_hash.c
$(NAME)_SOURCES     += ./sal/src/sal_rsa.c
$(NAME)_SOURCES     += ./sal/src/sal_sm2.c
$(NAME)_SOURCES     += ./sal/src/sal_hmac.c
$(NAME)_SOURCES     += ./sal/src/sal_rand.c
$(NAME)_SOURCES     += ./api/src/api_init.c
$(NAME)_SOURCES     += ./api/src/api_aes.c
$(NAME)_SOURCES     += ./api/src/api_sm4.c
$(NAME)_SOURCES     += ./api/src/api_hash.c
$(NAME)_SOURCES     += ./api/src/api_hmac.c
$(NAME)_SOURCES     += ./api/src/api_rsa.c
$(NAME)_SOURCES     += ./api/src/api_sm2.c
$(NAME)_SOURCES     += ./api/src/api_rand.c
$(NAME)_SOURCES     += ./plat/oem/hal_init.c
$(NAME)_SOURCES     += ./plat/oem/hal_aes.c
$(NAME)_SOURCES     += ./plat/oem/hal_sm4.c
$(NAME)_SOURCES     += ./plat/oem/hal_hash.c
$(NAME)_SOURCES     += ./plat/oem/hal_hmac.c
$(NAME)_SOURCES     += ./plat/oem/hal_rsa.c
$(NAME)_SOURCES     += ./plat/oem/hal_sm2.c
$(NAME)_SOURCES     += ./plat/oem/hal_rand.c

$(NAME)_COMPONENTS  := ls_osa

