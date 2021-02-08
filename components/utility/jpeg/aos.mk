NAME := jpeg

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := jpeg decoder/encoder library

$(NAME)_SOURCES     += ./src/jcapimin.c ./src/jcapistd.c ./src/jccoefct.c ./src/jccolor.c ./src/jcdctmgr.c ./src/jchuff.c \
					./src/jcicc.c ./src/jcinit.c ./src/jcmainct.c ./src/jcmarker.c ./src/jcmaster.c ./src/jcomapi.c ./src/jcparam.c \
					./src/jcphuff.c ./src/jcprepct.c ./src/jcsample.c ./src/jctrans.c ./src/jdapimin.c ./src/jdapistd.c ./src/jdatadst.c \
					./src/jdatasrc.c ./src/jdatasrc-tj.c ./src/jdcoefct.c ./src/jdcolor.c ./src/jddctmgr.c ./src/jdhuff.c ./src/jdicc.c ./src/jdinput.c \
					./src/jdmainct.c ./src/jdmarker.c ./src/jdmaster.c ./src/jdmerge.c ./src/jdphuff.c ./src/jdpostct.c ./src/jdsample.c \
					./src/jdtrans.c ./src/jerror.c ./src/jfdctflt.c ./src/jfdctfst.c ./src/jfdctint.c ./src/jidctflt.c ./src/jidctfst.c \
					./src/jidctint.c ./src/jidctred.c ./src/jquant1.c ./src/jquant2.c ./src/jutils.c ./src/jmemmgr.c ./src/jmemnobs.c \
					./src/turbojpeg.c ./src/jsimd_none.c ./src/jdarith.c ./src/jaricom.c

__ARM_NEON = 0
ifeq ($(__ARM_NEON),1)
$(NAME)_SOURCES     += ./src/simd/arm/jcgray-neon.c ./src/simd/arm/jcphuff-neon.c ./src/simd/arm/jcsample-neon.c \
  ./src/simd/arm/jdmerge-neon.c ./src/simd/arm/jdsample-neon.c ./src/simd/arm/jfdctfst-neon.c \
  ./src/simd/arm/jidctred-neon.c ./src/simd/arm/jquanti-neon.c

$(NAME)_SOURCES     +=./src/simd/arm/aarch32/jsimd_neon.S
endif

$(NAME)_INCLUDES     += ./include/
GLOBAL_INCLUDES     += ./include/

GLOBAL_DEFINES += AOS_COMP_JPEG

