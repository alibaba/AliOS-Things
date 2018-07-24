NAME := id2

LIBID2CLIENT := .

$(NAME)_INCLUDES     += $(LIBID2CLIENT)/../irot/include
$(NAME)_INCLUDES     += $(LIBID2CLIENT)/include
$(NAME)_INCLUDES     += $(LIBID2CLIENT)/platform/include
$(NAME)_INCLUDES     += $(LIBID2CLIENT)/src
$(NAME)_INCLUDES     += $(LIBID2CLIENT)

GLOBAL_INCLUDES     += $(LIBID2CLIENT)/..
GLOBAL_INCLUDES     += $(LIBID2CLIENT)/platform/include
GLOBAL_INCLUDES     += $(LIBID2CLIENT)/include

$(NAME)_SOURCES     += 							\
    $(LIBID2CLIENT)/src/util/util.c \
    $(LIBID2CLIENT)/src/log/log.c \
    $(LIBID2CLIENT)/src/alg/alg.c \
    $(LIBID2CLIENT)/src/core/otp.c \
    $(LIBID2CLIENT)/src/core/km_impl.c \
    $(LIBID2CLIENT)/src/core/id2_client.c \
    $(LIBID2CLIENT)/platform/aos/irot_pal.c \

$(NAME)_COMPONENTS += base64
$(NAME)_COMPONENTS += alicrypto
$(NAME)_COMPONENTS += irot

