NAME := ywss

$(NAME)_TYPE := framework
GLOBAL_INCLUDES += .

$(NAME)_DEFINES += DEBUG

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_DEFINES += CONFIG_YWSS

ifeq ($(HOST_ARCH), linux)
LIB_PATH := linux
else ifeq ($(HOST_ARCH), ARM968E-S)
LIB_PATH := arm968es
else ifeq ($(HOST_ARCH), xtensa)
LIB_PATH := xtensa
else
$(error "not find correct platform!")
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(LIB_PATH)/libywss.a
