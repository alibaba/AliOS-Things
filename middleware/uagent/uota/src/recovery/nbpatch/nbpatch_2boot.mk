NAME := nbpatch_2boot

$(NAME)_TYPE := uota
$(NAME)_SOURCES := nbpatch.c \
                   nbpatch_io.c \
                   xzdec.c \
                   flash_hal.c \
                   nbpatch_main.c
ifeq ($(strip $(CONFIG_SYSINFO_DEVICE_NAME)), $(filter $(CONFIG_SYSINFO_DEVICE_NAME), MK3060 MK3080))
$(NAME)_SOURCES += rec_md5.c \
                   rec_verify_fw.c
endif


$(NAME)_INCLUDES += ../xz-embedded/linux/include/linux \
                    ../xz-embedded/linux/lib/xz \
                    ../xz-embedded/userspace



