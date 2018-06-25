BINARY = linux_spiffs_test

############
#
# Paths
#
############

sourcedir = src
builddir = build


#############
#
# Build tools
#
#############

CC ?= gcc
LD ?= ld
GDB ?= gdb
OBJCOPY ?= objcopy
OBJDUMP ?= objdump
MKDIR ?= mkdir -p

###############
#
# Files and libs
#
###############

NO_TEST ?= 0
CFLAGS = $(FLAGS)
ifeq (1, $(strip $(NO_TEST)))
CFILES_TEST = main.c
CFLAGS += -DNO_TEST -Werror
else
CFILES_TEST = main.c \
	test_spiffs.c \
	test_dev.c \
	test_check.c \
	test_hydrogen.c \
	test_bugreports.c \
	testsuites.c \
	testrunner.c
CFLAGS += -D_SPIFFS_TEST
endif
include files.mk
INCLUDE_DIRECTIVES = -I./${sourcedir} -I./${sourcedir}/default -I./${sourcedir}/test 
COMPILEROPTIONS = $(INCLUDE_DIRECTIVES)

COMPILEROPTIONS_APP = $(INCLUDE_DIRECTIVES) \
-Wall -Wno-format-y2k -W -Wstrict-prototypes -Wmissing-prototypes \
-Wpointer-arith -Wreturn-type -Wcast-qual -Wwrite-strings -Wswitch \
-Wshadow -Wcast-align -Wchar-subscripts -Winline -Wnested-externs\
-Wredundant-decls
		
############
#
# Tasks
#
############

vpath %.c ${sourcedir} ${sourcedir}/default ${sourcedir}/test

OBJFILES = $(CFILES:%.c=${builddir}/%.o)
OBJFILES_TEST = $(CFILES_TEST:%.c=${builddir}/%.o)

DEPFILES = $(CFILES:%.c=${builddir}/%.d) $(CFILES_TEST:%.c=${builddir}/%.d)

ALLOBJFILES += $(OBJFILES) $(OBJFILES_TEST)

DEPENDENCIES = $(DEPFILES) 

# link object files, create binary
$(BINARY): $(ALLOBJFILES)
	@echo "... linking"
	@${CC} $(LINKEROPTIONS) -o ${builddir}/$(BINARY) $(ALLOBJFILES) $(LIBS)
ifeq (1, $(strip $(NO_TEST)))
	@echo "size: `du -b ${builddir}/${BINARY} | sed 's/\([0-9]*\).*/\1/g '` bytes"
endif


-include $(DEPENDENCIES)	   	

# compile c files
$(OBJFILES) : ${builddir}/%.o:%.c
		@echo "... compile $@"
		@${CC} $(COMPILEROPTIONS_APP) $(CFLAGS) -g -c -o $@ $<

$(OBJFILES_TEST) : ${builddir}/%.o:%.c
		@echo "... compile $@"
		@${CC} ${COMPILEROPTIONS} $(CFLAGS) -g -c -o $@ $<

# make dependencies
#		@echo "... depend $@"; 
$(DEPFILES) : ${builddir}/%.d:%.c
		@rm -f $@; \
		${CC} $(COMPILEROPTIONS) -M $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*, ${builddir}/\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

all: mkdirs $(BINARY) 

mkdirs:
	-@${MKDIR} ${builddir}
	-@${MKDIR} test_data

FILTER ?=

test: $(BINARY)
ifdef $(FILTER)
		./build/$(BINARY)
else
		./build/$(BINARY) -f $(FILTER)
endif

test_failed: $(BINARY)
		./build/$(BINARY) _tests_fail
	
clean:
	@echo ... removing build files in ${builddir}
	@rm -f ${builddir}/*.o
	@rm -f ${builddir}/*.d
	@rm -f ${builddir}/*.elf
	
ONOFF = 1 0
OFFON = 0 1
build-all:
	@for rdonly in $(ONOFF); do \
		for singleton in $(ONOFF); do \
			for hal_cb_xtra in $(OFFON); do \
				for cache in $(OFFON); do \
					for magic in $(OFFON); do \
						for temporal_cache in $(OFFON); do \
  						for ix_map in $(OFFON); do \
  							echo; \
  							echo ============================================================; \
  							echo SPIFFS_READ_ONLY=$$rdonly; \
  							echo SPIFFS_SINGLETON=$$singleton; \
  							echo SPIFFS_HAL_CALLBACK_EXTRA=$$hal_cb_xtra; \
  							echo SPIFFS_CACHE, SPIFFS_CACHE_WR=$$cache; \
  							echo SPIFFS_USE_MAGIC, SPIFFS_USE_MAGIC_LENGTH=$$magic; \
  							echo SPIFFS_TEMPORAL_FD_CACHE=$$temporal_cache; \
  							echo SPIFFS_IX_MAP=$$ix_map; \
  							$(MAKE) clean && $(MAKE) FLAGS="\
  								-DSPIFFS_HAL_CALLBACK_EXTRA=$$hal_cb_xtra \
  								-DSPIFFS_SINGLETON=$$singleton \
  								-DSPIFFS_CACHE=$$cache \
  								-DSPIFFS_CACHE_WR=$$cache \
  								-DSPIFFS_READ_ONLY=$$rdonly \
  								-DSPIFFS_USE_MAGIC=$$magic \
  								-DSPIFFS_USE_MAGIC_LENGTH=$$magic \
  								-DSPIFFS_TEMPORAL_FD_CACHE=$$temporal_cache \
  								-DSPIFFS_IX_MAP=$$ix_map \
  								" NO_TEST=1; \
  						done || exit 1; \
						done \
					done \
				done \
			done \
		done \
	done 
