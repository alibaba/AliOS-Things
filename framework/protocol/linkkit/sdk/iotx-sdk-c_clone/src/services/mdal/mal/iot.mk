LIBA_TARGET     := libiot_mal.a

LIB_SRCS_PATTERN := mal.c

ifneq (,$(filter -DMAL_ICA_ENABLED, $(CFLAGS)))
    LIB_SRCS_PATTERN += ica/mdal_ica_at_client.c 
endif

HDR_REFS        += src/infra
