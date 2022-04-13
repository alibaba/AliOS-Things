cur_makefile := $(lastword $(MAKEFILE_LIST))

# ==========================================================================
# Cleaning up
# ==========================================================================

src := $(obj)

PHONY := __clean
__clean:

$(cur_makefile): ;

# Shorthand for $(Q)$(MAKE) scripts/Makefile.clean obj=dir
# Usage:
# $(Q)$(MAKE) $(clean)=dir
clean := -f $(srctree)/scripts/clean.mk obj

kbuild-dir := $(if $(filter /%,$(src)),$(src),$(srctree)/$(src))
include $(kbuild-dir)/Makefile

# Figure out what we need to build from the various variables
# ==========================================================================

__subdir-y	:= $(patsubst %/,%,$(filter %/, $(obj-y)))
subdir-y	+= $(__subdir-y)
__subdir-m      := $(patsubst %/,%,$(filter %/, $(obj-m)))
subdir-m        += $(__subdir-m)
__subdir-       := $(patsubst %/,%,$(filter %/, $(obj-)))
subdir-         += $(__subdir-)

# Subdirectories we need to descend into

subdir-ym	:= $(sort $(subdir-y) $(subdir-m))
subdir-ymn      := $(sort $(subdir-ym) $(subdir-))

obj-ymn		:= $(obj-y) $(obj-m) $(obj-)
lib-ymn		:= $(lib-y) $(lib-m) $(lib-)

# if $(foo-objs) exists, foo.o is a composite object
multi-used-ymn	:= $(sort $(foreach m,$(obj-ymn), $(if $(strip $($(m:.o=-objs)) $($(m:.o=-y))), $(m))))
multi-objs-ymn	:= $(foreach m, $(multi-used-ymn), $($(m:.o=-objs)) $($(m:.o=-y)))

obj-ymn		:= $(patsubst %/, %/built-in.o %/built-in.a, $(obj-y) $(obj-m) $(obj-))

extra-ymn	:= $(extra-y) $(extra-m) $(extra-)

# build a list of files to remove, usually relative to the current
# directory

__clean-files	:= $(obj-ymn) $(lib-ymn) $(multi-used-ymn) \
		   $(multi-objs-ymn) $(extra-ymn) \
		   built-in.o built-in.a lib.a \
		   $(always) $(targets) $(clean-files)

__clean-files   := $(filter-out $(no-clean-files), $(__clean-files))

# clean-files is given relative to the current directory, unless it
# starts with $(objtree)/ (which means "./", so do not add "./" unless
# you want to delete a file from the toplevel object directory).

__clean-files   := $(addprefix $(obj)/, $(filter-out $(objtree)/%, $(__clean-files))) \
		   $(filter $(objtree)/%, $(__clean-files))

# same as clean-files

__clean-dirs    := $(addprefix $(obj)/, $(filter-out $(objtree)/%, $(clean-dirs)))    \
		   $(filter $(objtree)/%, $(clean-dirs))

# Add subdir path
subdir-ymn	:= $(addprefix $(obj)/,$(subdir-ymn))
obj-ymn		:= $(addprefix $(obj)/,$(obj-ymn))
lib-ymn		:= $(addprefix $(obj)/,$(lib-ymn))
multi-used-ymn	:= $(addprefix $(obj)/,$(multi-used-ymn))
multi-objs-ymn	:= $(addprefix $(obj)/,$(multi-objs-ymn))

SUBMODS		:= $(filter $(obj)/%,$(SUBMODS))

ifneq ($(SUBMODS),)
include scripts/submods.mk

buildsubdir-ymn	:= $(call get_subdirs,$(subdir-ymn),$(SUBMODS))
__full-clean-files	:= $(__clean-files)
__clean-files	:= $(filter $(SUBMODS),$(__full-clean-files))
__clean-files	+= $(obj)/built-in.o $(obj)/built-in.a $(obj)/lib.a
__clean-files	+= $(foreach m,$(multi-used-ymn),$(if $(filter $(SUBMODS),$(call get_multi_objs,$m))),$m,)

# Check for invalid targets
errobj-ymn := $(SUBMODS)
errobj-ymn := $(filter-out $(buildsubdir-ymn) $(addsuffix /%,$(buildsubdir-ymn)),$(errobj-ymn))
errobj-ymn := $(filter-out $(__clean-files),$(errobj-ymn))
errobj-ymn := $(filter-out $(foreach m,$(multi-used-ymn),$(call get_multi_objs,$m)),$(errobj-ymn))
ifneq ($(errobj-ymn),)
$(warning Unknown targets in [$(obj)]: $(errobj-ymn))
endif

else # SUBMODS

ifeq ($(ALLCLEAN),1)
RM_ALL := 1
ifneq ($(KBUILD_OUTPUT),)
RM_DIR := 1
endif
endif

ifeq ($(RM_DIR),1)
buildsubdir-ymn	:=
else
buildsubdir-ymn	:= $(subdir-ymn)
endif

endif # SUBMODS

# Include intermediate files
include $(srctree)/scripts/include.mk
__clean-files	+= $(foreach f,$(__clean-files),$(call get_depfile_name,$f))

#$(info RM_DIR=$(RM_DIR), RM_ALL=$(RM_ALL))
#$(info buildsubdir-ymn=$(buildsubdir-ymn))
#$(info __clean-files=$(__clean-files))

__clean-files	:= $(wildcard $(__clean-files))
__clean-dirs	:= $(wildcard $(__clean-dirs))

# ==========================================================================

ifeq ($(RM_DIR),1)

quiet_cmd_clean    = RMDIR   $(obj)
      cmd_clean    = $(call CMDRMDIR,$(obj))

else # RM_DIR

quiet_cmd_clean    = CLEAN   $(obj)
ifeq ($(RM_ALL),1)
      cmd_clean    = $(call CMDRMFILER,$(obj),*.o *.a *.s *.d)
else
      cmd_clean    = $(call CMDRMFILE,$(__clean-files))
endif
quiet_cmd_cleandir = CLEAN   $(__clean-dirs)
      cmd_cleandir = $(call CMDRMDIR,$(__clean-dirs))

endif # RM_DIR


__clean: $(buildsubdir-ymn)
	@:
ifeq ($(RM_DIR),1)
	+$(call cmd,clean)
else # RM_DIR
ifeq ($(RM_ALL),1)
	+$(call cmd,clean)
else
ifneq ($(strip $(__clean-files)),)
	+$(call cmd,clean)
endif
endif
ifneq ($(strip $(__clean-dirs)),)
	+$(call cmd,cleandir)
endif
endif # RM_DIR


# ===========================================================================
# Generic stuff
# ===========================================================================

# Descending
# ---------------------------------------------------------------------------

PHONY += $(subdir-ymn)
$(subdir-ymn):
	$(Q)$(MAKE) $(clean)=$@


# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable se we can use it in if_changed and friends.

.PHONY: $(PHONY)
