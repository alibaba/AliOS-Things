
##############################################################
# Global variables
##############################################################
LIBSUFFIX = .a
ELFSUFFIX = .elf

ifeq ("$(VERBOSE)","1")
Q :=
else
Q := @
endif


################################################################
# Include project makefile
################################################################
#include $(TOPDIR)/projects/$(PROJECT)/mk/project.mk


################################################################
# Define the make variables
################################################################
AS              = $(CROSS_COMPILER)as
LD              = $(CROSS_COMPILER)ld
CC              = $(CROSS_COMPILER)gcc
SIZE            = $(CROSS_COMPILER)size
AR              = $(CROSS_COMPILER)ar
STRIP           = $(CROSS_COMPILER)strip
OBJDUMP         = $(CROSS_COMPILER)objdump
OBJCOPY         = $(CROSS_COMPILER)objcopy
RANLIB          = $(CROSS_COMPILER)RANLIB
RM              = rm
CP              = cp -rf
MKDIR           = mkdir -p
CHMOD			= chmod +x
LOCK 			= $(TOPDIR)/utils/locker.sh
UNLOCK 			= $(TOPDIR)/utils/unlocker.sh
LOG				= $(TOPDIR)/utils/icomm_log_parser

################################################################
export AS LD CC SIZE AR STRIP OBJDUMP OBJCOPY RANLIB RM CP MKDIR
export LDFLAGS CFLAGS AFLAGS
export CC_VERBOSE
################################################################

