NAME := rtp

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := rtp component

$(NAME)_INCLUDES += ./include \
	                ../../../include \
$GLOBAL_INCLUDES += ../../../include/network/rtp

$(NAME)_SOURCES := ./src/fb.c \
                   ./src/member.c \
                   ./src/ntp.c \
                   ./src/pkt.c \
                   ./src/rr.c \
                   ./src/rtcp.c \
                   ./src/rtp.c \
                   ./src/sdes.c \
                   ./src/sess.c \
                   ./src/source.c

$(NAME)_SOURCES += ./src/list/list.c

$(NAME)_SOURCES += ./src/mbuf/mbuf.c

$(NAME)_SOURCES += ./src/sys/rand.c

$(NAME)_SOURCES += ./src/fmt/ch.c \
                   ./src/fmt/hexdump.c \
                   ./src/fmt/pl.c \
                   ./src/fmt/print.c \
                   ./src/fmt/str.c \
                   ./src/fmt/str_error.c

$(NAME)_SOURCES += ./src/lock/lock.c

$(NAME)_SOURCES += ./src/mem/mem.c

$(NAME)_SOURCES += ./src/net/sockopt.c

$(NAME)_SOURCES += ./src/tmr/tmr.c

$(NAME)_SOURCES += ./src/dbg/dbg.c

$(NAME)_SOURCES += ./src/hash/hash.c

$(NAME)_SOURCES += ./src/main/main.c \
                   ./src/main/method.c

$(NAME)_SOURCES += ./src/sa/ntop.c \
                   ./src/sa/printaddr.c \
                   ./src/sa/pton.c \
                   ./src/sa/sa.c

$(NAME)_SOURCES += ./src/udp/udp.c
