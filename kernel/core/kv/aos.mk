NAME := kv

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := lightweight key-value style storage module

GLOBAL_DEFINES  += AOS_COMP_KV

$(NAME)_COMPONENTS-$(AOS_COMP_NOR_KV) += nor_kv
$(NAME)_COMPONENTS-$(AOS_COMP_NAND_KV) += nand_kv
