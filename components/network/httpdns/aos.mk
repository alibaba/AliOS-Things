NAME := httpdns

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := httpdns component

# source files and the folder of internal include files
$(NAME)_SOURCES := ./src/httpdns.c         \
                   ./src/iplist.c          \
                   ./src/dict.c            \
                   ./src/http_ctx.c        \
                   ./src/sds.c             \
                   ./src/httpdns_aos.c     \
                   ./src/httpdns_opts.c

$(NAME)_INCLUDES += ./include/

# the folder of API files
GLOBAL_INCLUDES += ../../../include/network/httpdns

# mandatory dependencies
$(NAME)_COMPONENTS += http cjson
