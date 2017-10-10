NAME := ywss

GLOBAL_INCLUDES += .

$(NAME)_DEFINES += DEBUG

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_DEFINES += CONFIG_YWSS

PLATFORM := linuxhost
ifeq ($(HOST_ARCH), linux)
PLATFORM := linuxhost
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libywss.a
else ifeq ($(HOST_ARCH), ARM968E-S)
PLATFORM := mk3060
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libywss.a
else
$(error "not find correct platform!")
endif
