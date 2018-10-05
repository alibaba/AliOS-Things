DEPENDS             := src/tools/mconf/lxdialog
TARGET              := mconf
SRCS                := mconf.c zconf.tab.c mconf_util.c
LDFLAGS             := -llxdialog -lncurses
