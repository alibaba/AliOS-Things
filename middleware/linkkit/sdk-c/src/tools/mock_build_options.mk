
ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
    CFLAGS := $(filter-out -DOTA_ENABLED,$(CFLAGS))
endif

ifeq (Darwin,$(shell uname))
    CFLAGS := $(filter-out -DOTA_ENABLED,$(CFLAGS))
    CFLAGS := $(filter-out -DWIFI_AWSS_ENABLED,$(CFLAGS))
endif

