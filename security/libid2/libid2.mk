NAME := libid2
ID2_TEST := no
ID2_TEST_DEBUG := no

$(NAME)_CFLAGS      += -Wall -Werror -Os

GLOBAL_INCLUDES     += include
$(NAME)_COMPONENTS := plat_gen libkm

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libid2.a

ifeq ($(ID2_TEST), yes)
ifeq ($(HOST_ARCH), linux)
$(NAME)_DEFINES     += CONFIG_GENERIC_LINUX=1
else
$(NAME)_DEFINES     += CONFIG_AOS_SUPPORT=1
endif

ifeq ($(ID2_TEST_DEBUG), yes)
$(NAME)_DEFINES     += ID2_TEST_DEBUG=1
endif

$(NAME)_SOURCES += sample/id2_test.c

endif

