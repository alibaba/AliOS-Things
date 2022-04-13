NAME := nand_kv

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := lightweight key-value style storage module for NAND Flash

$(NAME)_SOURCES := kv.c kv_util.c
$(NAME)_SOURCES-$(AOS_COMP_CLI) += kv_cli.c
GLOBAL_DEFINES  += AOS_COMP_KV
GLOBAL_INCLUDES += .
