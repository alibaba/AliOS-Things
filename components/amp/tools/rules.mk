include $(TOOLS_DIR)/config.mk

SUB_ALL_SRCS := $(MOD_SOURCES)

SUB_ALL_OBJS  := $(patsubst %.c,%.o,$(SUB_ALL_SRCS))

$(foreach inc, $(MOD_INCLUDES), $(eval SUB_ALL_INCS += -I$(inc)))

all: $(LIBS_DIR)/$(TARGET)

$(LIBS_DIR)/$(TARGET): $(SUB_ALL_OBJS)
	$(ECHO) "Archiving $@ ..."
	$(AR) $(ARFLAGS) $@ $^

$(SUB_ALL_OBJS): %.o: %.c
	$(ECHO) Compiling $< ...
	$(CC) -c -o $@ $< $(SUB_ALL_INCS) $(CCFLAGS)