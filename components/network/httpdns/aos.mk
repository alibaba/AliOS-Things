NAME := httpdns

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := httpdns component

$(NAME)_SOURCES := ./src/httpdns.c         \
                   ./src/iplist.c          \
                   ./src/dict.c            \
                   ./src/http_ctx.c        \
                   ./src/sds.c             \
                   ./src/httpdns_aos.c     \
                   ./src/httpdns_opts.c    \

$(NAME)_INCLUDES += ./include/

