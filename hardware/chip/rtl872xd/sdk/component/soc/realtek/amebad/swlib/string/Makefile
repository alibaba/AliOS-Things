
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

MODULE_IFLAGS = -I./include


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
#OBJS = diag.o memset.o strproc.o rand.o memcpy.o memcmp.o rtl_utility.o

OBJS = 

INFRA_ROM_OBJS = diag.o memset.o strproc.o rand.o memcpy.o memcmp.o

OBJS = $(INFRA_ROM_OBJS)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS RENAME_ROM_OBJS COPY_ROM_OBJS
	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)



#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
