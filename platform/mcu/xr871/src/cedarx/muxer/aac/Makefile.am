
include $(top_srcdir)/Makefile.inc

noinst_LTLIBRARIES = libcdx_aac_muxer.la

## set the source files.
libcdx_aac_muxer_la_SOURCES =  CdxAacMuxer.c

libcdx_aac_muxer_la_CFLAGS = $(CFLAGS_CDXG)
LOCAL_INCLUDE = -I../include \
		-I../../include \
		-I../../base/include \
		-I../base \
		-I$(top_srcdir)/external/include/aencoder \
		-I$(top_srcdir)/../libcedarc/include \
		-I$(top_srcdir)/

libcdx_aac_muxer_la_CFLAGS += $(LOCAL_INCLUDE)


libcdx_aac_muxer_la_LDFLAGS = $(LDFLAGS_CDXG)

libcdx_aac_muxer_la_LIBADD = $(top_srcdir)/libcore/base/libcdx_base.la

#LOCAL_LIB = -lpthread
libcdx_aac_muxer_la_LDFLAGS += $(LOCAL_LIB)

