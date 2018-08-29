NAME := audio_hal

$(NAME)_SOURCES := isd9160.c audio.c

$(NAME)_COMPONENTS := digest_algorithm kernel.modules.fs.fatfs

GLOBAL_INCLUDES += include

GLOBAL_DEFINES += DEVELOPERKIT_AUDIO CONFIG_AOS_FATFS_SUPPORT_MMC