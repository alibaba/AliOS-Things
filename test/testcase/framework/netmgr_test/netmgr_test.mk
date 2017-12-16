NAME := netmgr_test

$(NAME)_COMPONENTS  +=  

$(NAME)_SOURCES     += netmgr_test.c

$(NAME)_CFLAGS      += -Wall -Werror

ifeq (,$(findstring linux, $(BUILD_STRING)))
GLOBAL_DEFINES += CONFIG_AOS_NETMGRYTS_NOSMARTCONFIG
endif
