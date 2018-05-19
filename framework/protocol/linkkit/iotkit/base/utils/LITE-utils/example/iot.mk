TARGET      := utils-example
DEPENDS     += testsuites
HDR_REFS    := src external

LDFLAGS     += -llite-utils -llite-log -llite-cut
