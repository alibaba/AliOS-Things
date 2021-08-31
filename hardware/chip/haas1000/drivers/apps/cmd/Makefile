cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += \
					-Iservices/audio_process \
					-Iservices/multimedia/audio/process/filters/include

ifeq ($(ALSA_AUDIO_PROCESS_MODE_CHECK),1)
subdir-ccflags-y +=	-Iservices/wifi_app/alsa_adapter/
endif