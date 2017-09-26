.PHONY: all lib library

ifeq (cc,$(CC))
CC          := gcc
endif

UT_PROG     := lite-utils_prog
SRCS        := $(wildcard *.c)
SRCS        := $(filter-out $(UT_PROG).c,$(SRCS))
OBJS        := $(subst .c,.o,$(SRCS))

LIB_FMT     ?= static
CC          ?= gcc
AR          ?= ar
LD          ?= ld
CFLAGS      += -Werror
LDFLAGS     += -L. -llite-utils

INS_LIBDIR  ?= ./fakeroot/usr/lib
INS_INCDIR  ?= ./fakeroot/usr/include
INS_HDRS    := lite-utils.h lite-list.h
INS_LIBS    ?= libutils.a

VARS    := \
    UT_PROG \
    SRCS \
    OBJS \
    LIB_FMT \
    CC \
    AR \
    LD \
    ALL_TARGET \
    LIB_TARGET \
    CFLAGS \
    LDFLAGS \
    INS_HDRS \
    INS_LIBDIR \
    INS_INCDIR \

ifeq (static,$(LIB_FMT))
ALL_TARGET := $(INS_LIBS)
else
$(error LIB_FMT must be static!)
endif

LIB_TARGET := $(ALL_TARGET)
ALL_TARGET += $(UT_PROG)

all: $(ALL_TARGET)

lib library: $(LIB_TARGET)

$(UT_PROG): $(UT_PROG).c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

install: $(LIB_TARGET)
	@mkdir -p $(INS_LIBDIR) $(INS_INCDIR)
	@cp -f $(INS_HDRS) $(INS_INCDIR)
	@cp -f $(LIB_TARGET) $(INS_LIBDIR)

clean:
	@rm -f *.o *.d *.a *.so $(UT_PROG) $(foreach H,$(INS_HDRS),$(INS_INCDIR)/$(H)) $(INS_LIBDIR)/$(LIB_TARGET)

%.a: $(OBJS)
	@$(AR) -rcs $@ $(OBJS)
	@echo "  AR    $@"
	@rm -f *.o *.d

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo "  CC    $<"

# %.d: %.c
# 	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
# 	    sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' < $@.$$$$ > $@; \
# 	    rm -f $@.$$$$
#
# sinclude $(OBJS:.o=.d)

info:
	@rm -f *.o *.d *.a *.so

ifeq (info,$(MAKECMDGOALS))
$(foreach V,$(sort $(VARS)),$(info $(V) = $($(V))))
endif
