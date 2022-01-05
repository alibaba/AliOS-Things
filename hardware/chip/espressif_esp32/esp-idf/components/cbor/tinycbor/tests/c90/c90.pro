CONFIG += testcase parallel_test console
CONFIG -= qt app_bundle
gcc: QMAKE_CFLAGS += -std=c90 -pedantic-errors -Wall -Wextra -Werror
darwin: QMAKE_CFLAGS += -Wno-long-long

SOURCES += tst_c90.c
INCLUDEPATH += ../../src
