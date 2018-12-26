AudioEncBASE = /home/$(USER)/SVNAudioLib/dev/audiodec/codec/CdxEncPlugin
AudioLibBASE    = /home/$(USER)/SVNAudioLib/dev/audiodec/codec/AudioLib

include $(AudioEncBASE)/config.mk

################################################################################
## set flags for golobal compile and link setting.
################################################################################

CONFIG_FOR_COMPILE =
CONFIG_FOR_LINK =

################################################################################
## set target.
################################################################################

TargetName = libaencoder.so
Target = $(LIBPATH)/$(TargetName)

################################################################################
## build configures print.
################################################################################

$(warning "#################################################")
$(warning "#   Allwinner audio codec compile information    ")
$(warning "#                                                ")
$(warning "# OS_PLATFORM is $(OS_PLATFORM)")
$(warning "# CONFIG_CC is $(CONFIG_CC)")
$(warning "# CONFIG_LOG_LEVEL is $(CONFIG_LOG_LEVEL)")
$(warning "# OS_DEBUG_CONFIG is $(OS_DEBUG_CONFIG)")
$(warning "# OS_CHECK_MEMORY_CONFIG is $(OS_CHECK_MEMORY_CONFIG)")
$(warning "# TargetName is $(TargetName)")
$(warning "# Target generated at $(LIBPATH)")
$(warning "#                                                ")
$(warning "#################################################")

################################################################################
## set build path and build target.
################################################################################

ifneq ($(LIBPATH),wildcard($(LIBPATH)))
a := $(shell mkdir -p $(LIBPATH))
endif
ifneq ($(BUILDPATH),wildcard($(BUILDPATH)))
a := $(shell mkdir -p $(BUILDPATH))
endif

################################################################################
## set the source files, object files and dependency files
################################################################################

## set the source files.
SourceFiles = aencoder.c pcm_enc.c
$(warning "SourceFiles : $(SourceFiles)")

################################################################################
## set obj files, mkdir respect build dir
################################################################################

SourcePath = $(foreach src_dir, $(SourceFiles), $(dir $(src_dir)))
$(foreach src_dir, $(SourcePath), $(shell mkdir -p $(BUILDPATH)/$(src_dir)))

ObjectFiles = $(addprefix $(BUILDPATH)/, $(addsuffix .o ,$(basename $(SourceFiles))))

## set the include path for compile flags.
SourceIncludePath = $(foreach dir,$(SourcePath),-I$(dir)) \
                    -I../../../ \
                    -I./include \

## set compile flags
CompileFlags = $(CONFIG_FOR_COMPILE) $(SourceIncludePath) -O1 -fPIC -ldl
#-mfpu=neon

ifeq ($(OS_PLATFORM), OS_LINUX)
CompileFlags += -DOS_LINUX $(EXTRA_FLAG)
endif

ifeq ($(OS_DEBUG_CONFIG), true)
CompileFlags += -D__AD_CEDAR_DBG_WRITEOUT_BITSTREAM
CompileFlags += -D__AD_CEDAR_DBG_WRITEOUT_PCM_BUFFER
endif

ifeq ($(CONFIG_LOG_LEVEL), OPTION_LOG_LEVEL_CLOSE)
CompileFlags += -DOPTION_LOG_LEVEL_CLOSE
endif

ifeq ($(CONFIG_LOG_LEVEL), OPTION_LOG_LEVEL_ERROR)
CompileFlags += -DOPTION_LOG_LEVEL_ERROR
endif

ifeq ($(CONFIG_LOG_LEVEL), OPTION_LOG_LEVEL_WARNING)
CompileFlags += -DOPTION_LOG_LEVEL_WARNING
endif

ifeq ($(CONFIG_LOG_LEVEL), OPTION_LOG_LEVEL_DEFAULT)
CompileFlags += -DOPTION_LOG_LEVEL_DEFAULT
endif

ifeq ($(CONFIG_LOG_LEVEL), OPTION_LOG_LEVEL_DETAIL)
CompileFlags += -DOPTION_LOG_LEVEL_DETAIL
endif

## set link flags
LoadFlags = $(CONFIG_FOR_LINK) -ldl -shared

LoadFlags += -lpthread -laacenc -lmp3enc -lpcmenc -lm
#-lstdc++ -Wl,--whole-archive -Wl,--no-whole-archive

ifeq ($(CONFIG_CC), OPTION_CC_GNUEABIHF)
LoadFlags += -Llib/build_by_arm_linux_gnueabihf_static
endif

ifeq ($(CONFIG_CC),OPTION_CC_GNUEABI)
LoadFlags += -Llib/linuxgnueabi/
endif

ifeq ($(CONFIG_CC),OPTION_CC_UCGNUEABI)
LoadFlags += -Llib/uclgnueabi/
endif

ifeq ($(CONFIG_CC),OPTION_CC_LINUX_UCGNUEABI)
LoadFlags += -Llib/linuxgnueabi/
endif

ifeq ($(CONFIG_CC),OPTION_CC_LINUX_MUSLGNUEABI)
LoadFlags +=-Llib/arm926_musl
#LoadFlags +=-L./lib/muslgnueabi
endif

ifeq ($(CONFIG_CC),OPTION_CC_LINUX_MUSLGNUEABI64)
LoadFlags +=-Llib/muslgnueabi64
endif

ifeq ($(CONFIG_CC),OPTION_CC_AARCH64_OPENWRT_MUSL)
LoadFlags +=-Llib/aarch64_openwrt_linux_musl_gcc
endif

################################################################################
## make commands, all/clean/cleanall
################################################################################

## define commands for make, sush as all, clean
.PHONY: all clean cleantarget cleanall
all:$(Target)
	@echo Make $(TargetName) over...
	@echo Target path : $(LIBPATH)

clean:
	-rm -rf $(ObjectFiles)
	-rm -rf $(Target)

cleanall: clean
	-rm -rf $(BUILDPATH)


################################################################################
## define target , ld target.
################################################################################

## compile source files to object files.
$(BUILDPATH)/%.o:%.c
	$(CC) $(CompileFlags) -o $@ -c $<

## link object files to the target share library.
$(Target):$(ObjectFiles)
	$(LD) -o $@ $^ $(LoadFlags)


## include the .d files to set dependency rules.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleantarget)
ifneq ($(MAKECMDGOALS),cleanall)
-include $(DependFiles)
endif
endif
endif
