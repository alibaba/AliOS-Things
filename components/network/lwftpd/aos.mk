NAME := lwftpd

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ftp server component

GLOBAL_DEFINES += CONFIG_AOS_LWFTPD

$(NAME)_SOURCES := server.c handles.c
