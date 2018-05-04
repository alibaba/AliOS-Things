NAME := audio_hal

$(NAME)_SOURCES := nau8810.c audio.c

$(NAME)_COMPONENTS += modules.fs.kv yloop hal

GLOBAL_INCLUDES += include

GLOBAL_DEFINES += STARTERKIT_AUDIO