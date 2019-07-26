DEPENDS             := tests/kconf/front/mconf/lxdialog
TARGET              := mconf
SRCS                := mconf.c zconf.tab.c mconf_util.c
LDFLAGS             := -llxdialog -lncurses

CFLAGS              := $(filter-out -ansi,$(CFLAGS))
