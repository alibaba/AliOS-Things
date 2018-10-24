NAME := audio_hal

$(NAME)_SOURCES := isd9160.c audio.c

$(NAME)_COMPONENTS := digest_algorithm 3rdparty.experimental.fs.fatfs

GLOBAL_INCLUDES += include

GLOBAL_DEFINES += DEVELOPERKIT_AUDIO CONFIG_AOS_FATFS_SUPPORT_MMC