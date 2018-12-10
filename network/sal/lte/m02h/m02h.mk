NAME := device_sal_m02h

GLOBAL_DEFINES += DEV_SAL_M02H

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += sal network.sal.atparser

$(NAME)_SOURCES += m02h.c
endif
GLOBAL_INCLUDES += ./
