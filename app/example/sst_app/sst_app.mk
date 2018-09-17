NAME := sst_test

$(NAME)_DEFINES += CONFIG_AOS_SUPPORT=1

$(NAME)_COMPONENTS  += isst alicrypto

$(NAME)_SOURCES     += sst_test.c sst_test_basic.c sst_test_performance.c

$(NAME)_CFLAGS      += -Wall -Werror -Wno-pointer-sign

GLOBAL_INCLUDES += ./