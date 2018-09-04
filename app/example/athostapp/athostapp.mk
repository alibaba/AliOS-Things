NAME := athostapp

$(NAME)_SOURCES := athostapp.c
$(NAME)_COMPONENTS := network/sal/athost  \
                      network/sal/atparser \
                      network/netmgr \
                      tools/cli \
                      yloop \

GLOBAL_DEFINES += AOS_ATCMD DEBUG

$(NAME)_INCLUDES := ./
