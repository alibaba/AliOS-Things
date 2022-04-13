
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)intersyshci.cpp))

ifeq ($(ENHANCED_STACK),1)
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)trans_adapt_v2.cpp))
else
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)trans_adapt_v1.cpp))
endif

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

INTERSYSHCI_LIB_NAME := libintersyshci

ifeq ($(INTERSYS_NO_THREAD),1)
INTERSYSHCI_LIB_NAME := $(INTERSYSHCI_LIB_NAME)_nothread
endif

ifeq ($(BYPASS_STACK_AUDIO_STREAM),1)
INTERSYSHCI_LIB_NAME := $(INTERSYSHCI_LIB_NAME)_bypasssas
endif

ifeq ($(ENHANCED_STACK),1)
INTERSYSHCI_LIB_NAME := $(INTERSYSHCI_LIB_NAME)_enhanced_stack
endif

$(INTERSYSHCI_LIB_NAME)-y := $(src_obj)
obj-y += $(INTERSYSHCI_LIB_NAME).a
ccflags-y := \
    $(BT_IF_INCLUDES) \
	$(BT_PROFILES_INCLUDES) \
	-Iservices/bt_app/ \
	-Iapps/common/ \
	-Iutils/cqueue \
	-Iplatform/drivers/bt

ifeq ($(INTERSYS_NO_THREAD),1)
CFLAGS_intersyshci.o += -D_INTERSYS_NO_THREAD_
endif

