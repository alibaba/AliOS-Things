SUBDIRS := utils

SUBDIRS += libalilog
SUBDIRS += sim_ref_impl
SUBDIRS += modules
SUBDIRS_WITH_TEST = $(SUBDIRS)

PWD := $(shell pwd)

.PHONY: subdirs clean

all: rtos

at_debug:
	@for dir in $(SUBDIRS_WITH_TEST);do\
		make -C $$dir CFLAGS="-DDEBUG_LOG_ENABLED -DAT_LOG_ENABLED";\
	done

no_serial_ops:
	@for dir in $(SUBDIRS);do\
		make -C $$dir CFLAGS="-DNO_SERIAL_OPS" BUILD_WITH_NO_SERIAL_OPS="y";\
	done

rtos:
	@for dir in libalilog sim_ref_impl rtos_hal_impl modules ;do\
		make -C $$dir CFLAGS="-DNO_SERIAL_OPS" BUILD_WITH_NO_SERIAL_OPS="y" BUILD_RTOS="y";\
	done

sudbdirs:
	@for dir in $(SUBDIRS_WITH_TEST);do\
		make -C $$dir;\
	done

debug:
	@for dir in $(SUBDIRS_WITH_TEST);do\
		make -C $$dir CFLAGS="-DDEBUG_LOG_ENABLED";\
	done

clean:
	@rm -rf output
	@for dir in $(SUBDIRS_WITH_TEST) rtos_hal_impl;do\
		make -C $$dir clean;\
	done

install:
	@mkdir -p output
	@for dir in modules test; do\
		make -C $$dir install;\
	done

no_serial_ops_install:
	@mkdir -p output
	@for dir in modules ; do\
		make -C $$dir install BUILD_WITH_NO_SERIAL_OPS="y" ;\
	done

rtos_install:
	@mkdir -p output
	@for dir in modules ; do\
		make -C $$dir install BUILD_WITH_NO_SERIAL_OPS="y" BUILD_RTOS="y";\
	done
