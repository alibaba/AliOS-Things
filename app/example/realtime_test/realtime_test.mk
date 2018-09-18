NAME := realtime_test

$(NAME)_SOURCES := realtime_test.c

GLOBAL_DEFINES += AOS_NO_WIFI

GLOBAL_DEFINES += RHINO_CONFIG_INTRPT_STATS=1
GLOBAL_DEFINES += RHINO_CONFIG_REALTIME_TEST=1

$(NAME)_COMPONENTS := cli rhino/test/realtime

GLOBAL_INCLUDES += ./