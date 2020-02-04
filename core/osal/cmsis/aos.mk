NAME := osal_cmsis

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := cmsis for alios things
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR    := cmsis

$(NAME)_SOURCES := cmsis_os.c

GLOBAL_DEFINES  += AOS_COMP_OSAL_CMSIS
