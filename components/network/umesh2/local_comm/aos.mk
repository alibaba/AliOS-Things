NAME := local_comm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := The local communication implement.

#$(NAME)_DEFINES += 

$(NAME)_INCLUDES += include/ 

$(NAME)_SOURCES := src/osal.c src/umesh_api.c 

$(NAME)_COMPONENTS-$(UMESH_WITH_DEFAULT_MDNS) += mdns