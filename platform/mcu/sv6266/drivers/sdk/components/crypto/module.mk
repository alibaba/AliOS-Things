
LIB_SRC := md5.c
LIB_SRC += rc2.c
LIB_SRC += sha1ssl.c
LIB_SRC += sha224.c
LIB_SRC += sha256.c
LIB_SRC += sha384.c
LIB_SRC += sha512.c
LIB_SRC += osdep.c
LIB_SRC += hash_alg.c
LIB_SRC += hmac.c
LIB_SRC += arc4.c
LIB_SRC += aes.c
LIB_ASRC :=
LIBRARY_NAME := crypto
ifeq ($(strip $(OS)), freertos)
LOCAL_CFLAGS := -DFREERTOS
endif
ifeq ($(strip $(OS)), rhino)
LOCAL_CFLAGS := -DRHINO
endif
LOCAL_AFLAGS :=
LOCAL_INC += -I$(TOPDIR)/components/inc
LOCAL_INC += -I$(TOPDIR)/components/inc/crypto
LOCAL_INC += -I$(TOPDIR)/components/inc/bsp/$(CHIP_NAME)
LOCAL_INC += -I$(TOPDIR)/components/inc/bsp
LOCAL_INC += -I$(TOPDIR)/components/drv/$(CHIP_NAME)

RELEASE_SRC := 1
$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
