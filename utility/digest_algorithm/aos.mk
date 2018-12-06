NAME := digest_algorithm

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := digest_algorithm provides many checksum and digest algorithm with the friendly APIs.

$(NAME)_SOURCES := digest_algorithm.c CheckSumUtils.c crc.c md5.c
GLOBAL_INCLUDES += .
