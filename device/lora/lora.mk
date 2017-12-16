NAME := lora 
GLOBAL_DEFINES += WITH_LORA # for sal general use
$(NAME)_TYPE := kernel
$(NAME)_SOURCES := sx1276/sx1276.c   \
                   eml3047_lrwan/eml3047.c   \	


GLOBAL_INCLUDES += .        \
                   sx1276   \
                   eml3047_lrwan

