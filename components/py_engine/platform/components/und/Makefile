TARGET = libund.a
MODULE = und

MOD_SOURCES := \
		src/und_adapter.c \
		src/und_manage.c \
		src/und_packet.c \
		src/und_report.c \
		src/und_sched.c \
		src/und.c

MOD_INCLUDES := \
		../../adapter/include \
		../../adapter/platform/linux \
		../../main \
		../../utils/mbedtls/include \
		../linkkit \
		../linkkit/infra \
		../ulog \
		include

include $(TOOLS_DIR)/rules.mk
