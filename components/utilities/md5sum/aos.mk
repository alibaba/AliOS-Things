NAME := md5sum

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := MD5 message digest utility.

$(NAME)_INCLUDES += ./md5/
$(NAME)_SOURCES += ./md5/md5.c ./md5sum.c
