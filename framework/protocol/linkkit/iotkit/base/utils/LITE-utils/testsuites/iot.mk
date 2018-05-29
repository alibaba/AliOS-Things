TARGET      := utils-tests

DEPENDS     += external/cut external/log
HDR_REFS    := src external/log

LIBA_TARGET := liblite-utils-test.a

LDFLAGS     += -llite-utils
LDFLAGS     += -llite-log
LDFLAGS     += -llite-cut

LIB_SRCS_PATTERN    := ut_*.c
SRCS_PATTERN        := utils-tests.c
