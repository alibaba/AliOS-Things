NAME := sal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.2
$(NAME)_SUMMARY := socket adapter layer

GLOBAL_DEFINES += WITH_SAL                   \
                  SAL_USE_AOS_HAL

$(NAME)_COMPONENTS += device_sal

$(NAME)_SOURCES := ./src/sal_sockets.c \
                   ./src/sal_err.c     \
                   ./src/ip4_addr.c    \
                   ./src/sal_arch_aos.c

$(NAME)_INCLUDES += ./                  \
                    ./include
