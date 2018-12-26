include $(top_srcdir)/Makefile.inc

lib_LTLIBRARIES = libcdx_muxer.la

## set the source files.
libcdx_muxer_la_SOURCES =  CdxFsCache.c \
			CdxFsSimpleCache.c \
			CdxFsWriteDirect.c \
			CdxFsWriter.c \
			CdxMuxer.c \
			CdxTsemaphore.c

libcdx_muxer_la_CFLAGS = $(CFLAGS_CDXG)
LOCAL_INCLUDE = -I../include \
		-I../../include \
		-I../../base/include \
		-I../base \
                -I$(top_srcdir)/external/include/aencoder \
                -I$(top_srcdir)/../libcedarc/include \
		-I$(top_srcdir)/

libcdx_muxer_la_CFLAGS += $(LOCAL_INCLUDE)


libcdx_muxer_la_LDFLAGS = $(LDFLAGS_CDXG)

libcdx_muxer_la_LIBADD = $(top_srcdir)/libcore/muxer/aac/libcdx_aac_muxer.la \
			$(top_srcdir)/libcore/muxer/mp4/libcdx_mp4_muxer.la \
			$(top_srcdir)/libcore/muxer/ts/libcdx_ts_muxer.la \
			$(top_srcdir)/libcore/base/libcdx_base.la

#			$(top_srcdir)/libcore/muxer/mp4ex/libcdx_mp4ex_muxer.la

LOCAL_LIB = -lpthread -laencoder -lcdc_vencoder
libcdx_muxer_la_LDFLAGS += $(LOCAL_LIB)



