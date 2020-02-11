NAME := sal

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.2
$(NAME)_SUMMARY := socket adapter layer

# source files and the folder of internal include files
$(NAME)_SOURCES := ./src/sal_sockets.c \
                   ./src/sal_err.c     \
                   ./src/ip4_addr.c    \
                   ./src/sal_arch_aos.c

$(NAME)_INCLUDES += ./                  \
                    ./include

# the folder of API files
GLOBAL_INCLUDES += ../../../include/network/sal

# mandatory dependencies
$(NAME)_COMPONENTS += device_sal

# optional dependencies
