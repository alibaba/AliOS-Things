NAME := salapp

vcall ?= posix

$(NAME)_SOURCES := salapp.c

$(NAME)_COMPONENTS += netmgr sal atparser cli yloop

ifneq (,$(module))
$(NAME)_COMPONENTS += sal.$(module)
endif

sal := 1

GLOBAL_DEFINES += DEBUG
