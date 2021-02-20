NAME := audio

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := audio driver component

# component public API header file folder
GLOBAL_INCLUDES += ./include/
GLOBAL_INCLUDES += ./include/core/
GLOBAL_INCLUDES += ./include/platform/rtos/

# component source file
$(NAME)_SOURCES := ./src/core/audio.c \
                   ./src/core/audio_card.c \
                   ./src/core/control.c \
                   ./src/core/control_dev.c \
                   ./src/core/pcm_dev.c \
                   ./src/platform/rtos/audio_rtos.c \
                   ./src/platform/rtos/pb_task.c \
                   ./src/platform/rtos/cap_task.c

#GLOBAL_INCLUDES += ./include/platform/linux/
#$(NAME)_SOURCES += ./src/platform/linux/soc_card.c \
                   ./src/platform/linux/soc_codec.c \
                   ./src/platform/linux/soc_component.c \
                   ./src/platform/linux/soc_pcm.c \
                   ./src/platform/linux/soc_platform.c \
                   ./src/platform/linux/control_linux.c
