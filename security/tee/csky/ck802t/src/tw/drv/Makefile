#
# Copyright (C) 2015 The YunOS Project. All rights reserved.
#
TOP  := ..
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

DIRS := asc crypto flash trm #test

SRCS := $(wildcard $(addsuffix /*.c, $(DIRS)))
OBJS := $(addsuffix /built-in.o, $(DIRS))
LIBS := drv.o

all: $(OBJS) $(LIBS)

$(LIBS): $(OBJS)
	$(NOECHO)$(LD) -r -o $(LIBS) $(OBJS)
	@echo "Finish building $@..."

$(OBJS): $(SRCS)
	@for dir in $(DIRS); do $(MAKE) -C $$dir; done

clean:
	@ echo "Cleaning $(LIBS)..."
	@ for dir in $(DIRS); do $(MAKE) -C $$dir clean; done
	@-rm -f $(LIBS)

