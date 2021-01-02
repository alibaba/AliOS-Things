NAME := sal

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.2
$(NAME)_SUMMARY := socket adapter layer

# source files and the folder of internal include files
$(NAME)_SOURCES := ./src/sal_sockets.c \
                   ./src/sal_arch_aos.c \
                   ./src/ip4_addr_inner.c

ifneq (y,$(AOS_COMP_LWIP))
$(NAME)_SOURCES  += ./src/sal_err.c \
                    ./src/ip4_addr.c
endif

$(NAME)_INCLUDES += ./                  \
                    ./include

# the folder of API files
GLOBAL_INCLUDES += ../../../include/network/sal

# optional dependencies

# DO NOT DELETE, for RPM package
RPM_INCLUDE_DIR := network/sal
