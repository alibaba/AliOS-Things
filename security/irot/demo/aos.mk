NAME := libkm_demo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.1
$(NAME)_SUMMARY := key management for demo

GLOBAL_INCLUDES    += ../../include/irot

$(NAME)_SOURCES    := km_demo.c

$(NAME)_COMPONENTS := ls_osa
$(NAME)_COMPONENTS += alicrypto

