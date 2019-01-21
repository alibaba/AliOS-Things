ifeq (0,$(MAKELEVEL))
TOPDIR := $(CURDIR)/../..
export TOPDIR
endif

include $(TOPDIR)/config.mk

INCLUDE         +=
SRC_DIRS        += cmds 
LOCAL_SRCS      += cli.c

##############################################################
# make rules
##############################################################
include $(TOPDIR)/rules.mk

