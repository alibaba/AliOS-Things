NAME := osal_cmsis

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := cmsis for alios things

GLOBAL_INCLUDES += .

$(NAME)_SOURCES := cmsis_os.c

GLOBAL_DEFINES  += AOS_COMP_OSAL_CMSIS
