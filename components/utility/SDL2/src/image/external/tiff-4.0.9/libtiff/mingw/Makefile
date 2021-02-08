CROSS	=
CC	= $(CROSS)gcc

DLLNAME = libtiff-5.dll
LIBNAME = libtiff.dll.a
INCLUDES= -I. -Iexternal/include
CPPFLAGS= -DDLL_EXPORT
CFLAGS	= -O2 -Wall
LDFLAGS = -Wl,--enable-auto-image-base -Wl,--no-undefined
LDLIBS	= -Lexternal/lib -ljpeg -lz

OBJ =	tif_aux.o \
	tif_close.o \
	tif_codec.o \
	tif_color.o \
	tif_compress.o \
	tif_dir.o \
	tif_dirinfo.o \
	tif_dirread.o \
	tif_dirwrite.o \
	tif_dumpmode.o \
	tif_error.o \
	tif_extension.o \
	tif_fax3.o \
	tif_fax3sm.o \
	tif_flush.o \
	tif_getimage.o \
	tif_jbig.o \
	tif_jpeg.o \
	tif_jpeg_12.o \
	tif_luv.o \
	tif_lzma.o \
	tif_lzw.o \
	tif_next.o \
	tif_ojpeg.o \
	tif_open.o \
	tif_packbits.o \
	tif_pixarlog.o \
	tif_predict.o \
	tif_print.o \
	tif_read.o \
	tif_strip.o \
	tif_swab.o \
	tif_thunder.o \
	tif_tile.o \
	tif_version.o \
	tif_warning.o \
	tif_write.o \
	tif_zip.o \
	tif_win32.o

all: $(DLLNAME)

.PHONY: clean distclean
.SUFFIXES:
.SUFFIXES: .o .c

%.o: ../%.c
	$(CC) -c $(CPPFLAGS) $(INCLUDES) $(CFLAGS) -o $@ $<

$(DLLNAME): $(OBJ)
	$(CC) -shared -o $(DLLNAME) ../libtiff.def $(OBJ) $(LDFLAGS) $(LDLIBS) \
		-Xlinker --out-implib -Xlinker $(LIBNAME)

distclean: clean
	$(RM) *.dll *.a
clean:
	$(RM) *.o
