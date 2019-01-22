LIBA_TARGET     := libiot_awss.a

HDR_REFS        := src/infra
HDR_REFS        += src/protocol/mqtt
HDR_REFS        += src/protocol/coap/server
HDR_REFS        += src/services/dev_bind
HDR_REFS        += src/sdk-impl

ifneq (,$(filter -DWIFI_PROVISION_ENABLED,$(CFLAGS)))
    CFLAGS      += -DAWSS_SUPPORT_APLIST
    CFLAGS      += -DAWSS_SUPPORT_STATIS

    ifneq (,$(filter -DAWSS_SUPPORT_DEV_AP,$(CFLAGS)))
        CFLAGS  += -DAWSS_SUPPORT_DEV_AP
    endif

    ifneq (,$(filter -DAWSS_SUPPORT_SMARTCONFIG,$(CFLAGS)))
        CFLAGS  += -DAWSS_SUPPORT_SMARTCONFIG \
                   -DAWSS_SUPPORT_SMARTCONFIG_WPS
    endif

    ifneq (,$(filter -DAWSS_SUPPORT_PHONEASAP,$(CFLAGS)))
        CFLAGS  := $(filter-out -DAWSS_SUPPORT_AHA,$(CFLAGS))
        CFLAGS  += -DAWSS_SUPPORT_AHA
    endif

    ifneq (,$(filter -DAWSS_SUPPORT_ROUTER,$(CFLAGS)))
        CFLAGS  := $(filter-out -DAWSS_SUPPORT_AHA,$(CFLAGS))
        CFLAGS  += -DAWSS_SUPPORT_AHA \
                   -DAWSS_SUPPORT_ADHA
    endif

    ifeq (,$(filter -DAWSS_SUPPORT_ZEROCONFIG,$(CFLAGS)))
        CFLAGS  += -DAWSS_DISABLE_ENROLLEE \
                   -DAWSS_DISABLE_REGISTRAR
    endif
endif
