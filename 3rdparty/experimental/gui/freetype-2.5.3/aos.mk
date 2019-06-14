NAME := freetype253

$(NAME)_MBINS_TYPE := framework
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := freetype

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif

GLOBAL_DEFINES += FT2_BUILD_LIBRARY

GLOBAL_INCLUDES     += .
GLOBAL_DEFINES      += AOS_COMP_FREETYPE

GLOBAL_INCLUDES   += /include/
GLOBAL_INCLUDES   += /include/freetype/
GLOBAL_INCLUDES   += /include/freetype/config
GLOBAL_INCLUDES   += /include/freetype/internal
GLOBAL_INCLUDES   += /include/freetype/internal/services

$(NAME)_SOURCES     +=./src/autofit/autofit.c

$(NAME)_SOURCES     +=./src/base/ftbase.c
$(NAME)_SOURCES     +=./src/base/ftsystem.c
$(NAME)_SOURCES     +=./src/base/ftdebug.c
$(NAME)_SOURCES     +=./src/base/ftinit.c
$(NAME)_SOURCES     +=./src/base/ftlcdfil.c
#$(NAME)_SOURCES     +=./src/base/ftfntfmt.c
$(NAME)_SOURCES     +=./src/base/ftbitmap.c

$(NAME)_SOURCES     +=./src/bdf/bdf.c
$(NAME)_SOURCES     +=./src/bzip2/ftbzip2.c
$(NAME)_SOURCES     +=./src/cache/ftcache.c
$(NAME)_SOURCES     +=./src/cff/cff.c
$(NAME)_SOURCES     +=./src/cid/type1cid.c
$(NAME)_SOURCES     +=./src/gxvalid/gxvalid.c
$(NAME)_SOURCES     +=./src/gzip/ftgzip.c
$(NAME)_SOURCES     +=./src/lzw/ftlzw.c
$(NAME)_SOURCES     +=./src/otvalid/otvalid.c
$(NAME)_SOURCES     +=./src/pcf/pcf.c
$(NAME)_SOURCES     +=./src/pfr/pfr.c
$(NAME)_SOURCES     +=./src/psaux/psaux.c
$(NAME)_SOURCES     +=./src/pshinter/pshinter.c
$(NAME)_SOURCES     +=./src/psnames/psnames.c
$(NAME)_SOURCES     +=./src/raster/raster.c
$(NAME)_SOURCES     +=./src/sfnt/sfnt.c
$(NAME)_SOURCES     +=./src/smooth/smooth.c
$(NAME)_SOURCES     +=./src/truetype/truetype.c
$(NAME)_SOURCES     +=./src/type1/type1.c
$(NAME)_SOURCES     +=./src/type42/type42.c
$(NAME)_SOURCES     +=./src/winfonts/winfnt.c



