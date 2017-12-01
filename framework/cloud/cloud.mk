NAME := cloud

$(NAME)_SOURCES := cloud.c

$(NAME)_TYPE := kernel
GLOBAL_DEFINES += CONFIG_CLOUD
