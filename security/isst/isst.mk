NAME := libsst

SST_DBG := N
SST_MIGRATION := N
DATA_MIGRATION := N
ID2_SUPPORT := Y

ifeq ($(SST_DBG), Y)
$(NAME)_DEFINES += CONFIG_SST_DBG
endif

ifeq ($(SST_MIGRATION), Y)
$(NAME)_DEFINES += CONFIG_SST_MIGRATION=1
endif

ifeq ($(DATA_MIGRATION), Y)
$(NAME)_DEFINES += CONFIG_DATA_MIGRATION=1
endif

ifeq ($(ID2_SUPPORT), Y)
$(NAME)_DEFINES += CONFIG_ID2_SUPPORT=1
endif

GLOBAL_INCLUDES     += include

$(NAME)_INCLUDES += src \
		  			src/wrapper/inc \
		  			src/osa/inc

$(NAME)_INCLUDES += ../alicrypto/libalicrypto/inc \
		  			../irot/include

$(NAME)_SOURCES += src/osa/aos/sst_osa.c \
				  src/wrapper/km/sst_wrapper.c \
				  src/sst.c

$(NAME)_COMPONENTS := irot alicrypto rhino.fs.kv

