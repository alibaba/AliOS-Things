NAME := alios_kernel

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := alios kernel
$(NAME)_SOURCES := alios_kernel.c \
                   pm_load.c

$(NAME)_COMPONENTS += osal_aos irq
#$(NAME)_COMPONENTS += lwip
$(NAME)_COMPONENTS += kv
$(NAME)_COMPONENTS-$(AOS_COMP_AUTOTEST)= testcase

$(NAME)_INCLUDES += ./

# ifeq ($(fs_test),1)
# $(NAME)_COMPONENTS += littlefs
# GLOBAL_DEFINES += ENABALE_FS_STRESS_TEST
# ifeq ($(sqlite_test),1)
# $(NAME)_COMPONENTS += sqlite_test
# $(NAME)_SOURCES += sqlitedemo.c
# GLOBAL_DEFINES += ENBALE_SQLITE_TEST
# endif
# endif

ALIOS_DEBUG ?= 1
ifeq ($(ALIOS_DEBUG), 1)
GLOBAL_DEFINES += CONFIG_ALIOS_DEBUG
GLOBAL_DEFINES += DEBUG_CONFIG_PANIC_CLI=1
endif
# sqlite_test ?= 0
# ifeq ($(sqlite_test),1)
# $(NAME)_COMPONENTS += sqlite
# endif
