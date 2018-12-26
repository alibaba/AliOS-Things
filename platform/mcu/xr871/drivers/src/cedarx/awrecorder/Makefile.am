
include $(top_srcdir)/Makefile.inc

lib_LTLIBRARIES = libawrecorder.la

libawrecorder_la_SOURCES =  AudioEncodeComponent.c \
			awencoder.c \
			EncDataComponent.c \
			VideoEncodeComponent.c

libawrecorder_la_CFLAGS = $(CFLAGS_CDXG)
libawrecorder_la_CPPFLAGS = $(CPPFLAGS_CDXG)
LOCAL_INCLUDE = -I$(top_srcdir) \
		-I$(top_srcdir)/libcore/include \
		-I$(top_srcdir)/libcore/base/include \
		-I$(top_srcdir)/libcore/muxer/include \
		-I$(top_srcdir)/external/include/aencoder \
		-I$(top_srcdir)/../libcedarc/include

libawrecorder_la_CFLAGS += $(LOCAL_INCLUDE)
libawrecorder_la_CPPFLAGS += $(LOCAL_INCLUDE)

libawrecorder_la_LDFLAGS = $(LDFLAGS_CDXG)

libawrecorder_la_LIBADD =  \
			$(top_srcdir)/libcore/base/libcdx_base.la \
			$(top_srcdir)/libcore/muxer/base/libcdx_muxer.la

LOCAL_LIB = -lpthread -dl -lcdc_vencoder -laencoder -lcdc_memory
libawrecorder_la_LDFLAGS += $(LOCAL_LIB)

