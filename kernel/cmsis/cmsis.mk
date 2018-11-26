NAME := cmsis

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := cmsis for alios things

GLOBAL_INCLUDES += .

$(NAME)_SOURCES := cmsis_os.c
