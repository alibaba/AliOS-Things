
$(NAME)_SOURCES += ./audio_hal/isd9160.c ./audio_hal/audio.c ./audio_hal/crc16.c

$(NAME)_COMPONENTS += fatfs

GLOBAL_INCLUDES += ./audio_hal/include

GLOBAL_DEFINES += DEVELOPERKIT_AUDIO CONFIG_AOS_FATFS_SUPPORT_MMC
