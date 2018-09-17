NAME := mbmaster

$(NAME)_TYPE 	    := kernel
$(NAME)_MBINS_TYPE  := kernel


ifeq ($(HOST_ARCH),linux)
$(NAME)_DEFINES     += IO_NEED_TRAP
endif

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_MBMASTER

ifeq ($(COMPILER),armcc)
$(error no armcc lib provided!)
else ifeq ($(COMPILER),iar)
$(error no iar lib provided!)
else ifeq ($(ENABLE_VFP),1)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/mbmaster.a
else
$(error no none-vfp gcc lib provided!)
endif

