NAME := digest_algorithm

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := digest_algorithm.c CheckSumUtils.c crc.c crc16.c md5.c
GLOBAL_INCLUDES += .
