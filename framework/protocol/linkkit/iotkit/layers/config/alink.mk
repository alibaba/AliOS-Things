LIBA_TARGET         := libilop-config.a
LIB_SRCS_PATTERN    := src/*.c

HDR_REFS            += base sdk-encap

CFLAGS              := $(filter-out -Werror,$(CFLAGS))


