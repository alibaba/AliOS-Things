NAME := a2sa

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Advanced AliOS Things Sounch Architecture component

# component dependancy
$(NAME)_COMPONENTS += osal_posix

# component public API header file folder
GLOBAL_INCLUDES += ./driver/include/core
GLOBAL_INCLUDES += ./driver/include/platform/rtos
GLOBAL_INCLUDES += ./framework/sound
GLOBAL_INCLUDES += ../../platform/mcu/haas1000/drivers/services/wifi_app/alsa_adapter/
GLOBAL_INCLUDES += ../../platform/mcu/haas1000/drivers/services/a7_dsp/common/

# component source file
$(NAME)_SOURCES := ./driver/src/core/*.c \
                   ./driver/src/platform/rtos/audio_rtos.c \
				   ./driver/src/platform/rtos/cap_task.c \
				   ./driver/src/platform/rtos/pb_task.c \
				   ./framework/sound/*.c \
				   ./demo/sound_playback.c \
                   ./demo/sound_loopback.c
                   
