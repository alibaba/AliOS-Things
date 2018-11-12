NAME := developerkitaudio

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
$(NAME)_SOURCES := developerkitaudio.c
endif

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli

ENABLE_AUDIO_HAL := 1

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./
