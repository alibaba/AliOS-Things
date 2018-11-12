
$(NAME)_SOURCES += ./audio_hal/isd9160.c ./audio_hal/audio.c

$(NAME)_COMPONENTS += digest_algorithm 3rdparty.experimental.fs.fatfs

GLOBAL_INCLUDES += ./audio_hal/include

GLOBAL_DEFINES += DEVELOPERKIT_AUDIO CONFIG_AOS_FATFS_SUPPORT_MMC
