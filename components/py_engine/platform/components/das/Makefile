TARGET = libdas.a
MODULE = das

MOD_SOURCES += \
		src/core/das_attest.c \
		src/core/das_core.c \
		src/core/sha1.c \
		src/core/utils.c \
		src/core/proto/lsoc.pb.c \
		src/core/proto/pb_common.c \
		src/core/proto/pb_decode.c \
		src/core/proto/pb_encode.c

MOD_SOURCES += \
		src/service/service_fscan.c \
		src/service/service_lwip_nfi.c \
		src/service/service_rom.c \
		src/service/service_sys.c \
		src/service/service.c \
		src/service/netfilter/domain_parser.c \
		src/service/netfilter/icmp_ignore.c \
		src/service/netfilter/service_netfilter.c

MOD_SOURCES += \
		src/board/das_board.c

MOD_INCLUDES := \
		include \
		include/das \
		src/board

include $(TOOLS_DIR)/rules.mk
