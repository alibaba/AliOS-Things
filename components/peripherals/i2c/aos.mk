# component name
NAME := i2c
# component information
$(NAME)_MBINS_TYPE  := kernel
$(NAME)_VERSION     := 1.0.2
$(NAME)_SUMMARY     := i2c device
# source files and the folder of internal include files
GLOBAL_INCLUDES     += wrap adapter include
# armcc & iar without -Wall and -Werror
#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif
ifeq ($(HOST_ARCH),linux)
$(NAME)_DEFINES     += IO_NEED_TRAP
endif

GLOBAL_INCLUDES += ../../../include/aos/hal/

$(NAME)_SOURCES     := pca9544.c 

# DO NOT DELETE, for RPM package
RPM_INCLUDE_DIR := peripheral/i2c
