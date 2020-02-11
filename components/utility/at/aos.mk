NAME := at

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := component for sending and receiving AT command over uart

# source files and the folder of internal include files
$(NAME)_SOURCES := at.c        \
                   hal_at_os.c \
                   hal_at_dev.c

$(NAME)_INCLUDES += ./

# the folder of API files
GLOBAL_INCLUDES += ../../../include/utility/at
