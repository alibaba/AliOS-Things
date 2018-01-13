NAME := plat_gen

$(NAME)_CFLAGS      += -Wall -Werror -Os

GLOBAL_INCLUDES     += include

ifneq (,$(BINS))
ifeq ($(MBEDTLS_SHARE),1)
$(NAME)_TYPE := framework&kernel
else
$(NAME)_TYPE := kernel
endif
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libplat_gen.a

