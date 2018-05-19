sinclude project.mk
sinclude $(CONFIG_TPL)

SUBDIRS += \
    external/cut \
    src \
    example \
#    testsuites \

#CFLAGS += -DUTILS_SELF_TEST     -Os -Wall -Werror  -g3 -rdynamic --coverage -m32 
CFLAGS += -DUTILS_SELF_TEST -Os -Wall -Werror -g3  -rdynamic --coverage #-m32 

#UTEST_PROG  := coap-tests

include $(RULE_DIR)/rules.mk
