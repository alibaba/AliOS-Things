NAME := sound

# component information
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sound

# component source file
$(NAME)_SOURCES += ./sound_pcm.c
$(NAME)_SOURCES += ./sound_mixer.c

# component internal header file folder
$(NAME)_INCLUDES += ./../../drivers/audio/include/core

# component public API header file folder
GLOBAL_INCLUDES += ./

