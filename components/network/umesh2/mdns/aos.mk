NAME := mdns

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := mdns implement.

$(NAME)_COMPONENTS-$(MDNS_USE_YLOOP) := yloop

$(NAME)_INCLUDES += include/ 
$(NAME)_SOURCES := src/*.c

