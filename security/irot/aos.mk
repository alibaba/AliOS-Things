NAME := irot

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := root of trust, based on mcu, se or tee

GLOBAL_INCLUDES += ../include/irot

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring mk3060, $(BUILD_STRING)), mk3060)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring mk3080, $(BUILD_STRING)), mk3080)
$(NAME)_COMPONENTS := irot.km
else ifeq ($(findstring cb2201, $(BUILD_STRING)), cb2201)
$(NAME)_COMPONENTS := irot.tee
endif

