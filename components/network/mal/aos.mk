NAME := mal

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := mqtt adapter layer

# source files and the folder of internal include files
$(NAME)_SOURCES-y := ./mal.c     \
                     ./mqtt_api.c

$(NAME)_SOURCES-$(MAL_AOS_HAL_IMPL) += ./mal_aos.c

$(NAME)_INCLUDES += ./

# the folder of API files
GLOBAL_INCLUDES += ../../../include/network/mal

# optional dependencies
