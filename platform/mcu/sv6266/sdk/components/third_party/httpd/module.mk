LIB_SRC := src/auth_check.c
LIB_SRC += src/axhttpd.c
LIB_SRC += src/base64.c
LIB_SRC += src/cgi.c
LIB_SRC += src/cgi_api.c
LIB_SRC += src/httpd_api.c
LIB_SRC += src/os_port.c
LIB_SRC += src/proc.c
LIB_SRC += src/tdate_parse.c
LIB_SRC += src/web_proc.c
LIB_SRC += web_c/web__AP_html.c
LIB_SRC += web_c/webpage.c

LIB_ASRC :=
LIBRARY_NAME := httpd
LOCAL_CFLAGS += -Werror-implicit-function-declaration -Wno-address
LOCAL_AFLAGS +=
LOCAL_INC += -I$(TOPDIR)/components/third_party/httpd/inc
RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
