NAME := ota_2ndboot_nbpatch

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := nbpatch.c \
                   nbpatch_io.c \
                   xzdec.c \
                   flash_hal.c \
                   nbpatch_main.c \
                   rec_md5.c \
                   rec_verify_fw.c
 
$(NAME)_INCLUDES += ../xz/linux/include/linux \
                    ../xz/linux/lib/xz \
                    ../xz/userspace


