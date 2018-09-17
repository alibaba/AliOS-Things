
ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
    CFLAGS := $(filter-out -DOTA_ENABLED,$(CFLAGS))
endif

