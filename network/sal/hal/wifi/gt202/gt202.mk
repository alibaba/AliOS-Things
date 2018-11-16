NAME := sal_modulue_gt202

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
#$(NAME)_COMPONENTS += network.sal.atparser

$(NAME)_SOURCES += gt202_sal.c

GLOBAL_DEFINES += DEV_SAL_GT202
