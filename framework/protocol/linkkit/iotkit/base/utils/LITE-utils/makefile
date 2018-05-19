sinclude project.mk
sinclude $(CONFIG_TPL)

SUBDIRS += \
    external/cut \
    src \
    testsuites \
    example \

CFLAGS += -g -D_PLATFORM_IS_LINUX_

UTEST_PROG  := utils-tests

include $(RULE_DIR)/rules.mk
