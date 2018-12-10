NAME := device_sal_n26a

GLOBAL_DEFINES += DEV_SAL_N26A

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += sal network.sal.atparser digest_algorithm

$(NAME)_SOURCES += n26a.c
endif
GLOBAL_INCLUDES += ./
