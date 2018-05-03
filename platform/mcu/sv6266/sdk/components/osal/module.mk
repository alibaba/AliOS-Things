ifeq ($(strip $(OS)), freertos)
LIB_SRC := osal_freertos.c
LIB_SRC += heap_5.c
LIB_SRC += heap_psram.c
LIB_SRC += heap_debug.c
endif
ifeq ($(strip $(OS)), rhino)
LIB_SRC := osal_rhino.c
LIB_SRC += heap_psram.c
LIB_SRC += heap_debug.c
endif

ifeq ($(strip $(SUPPORT_SRM_TASK_LOG)), 1)
LIB_SRC += tasklog.c
endif

LIB_ASRC :=
LIBRARY_NAME := srm_osal
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC := -I$(TOPDIR)/components/drv

RELEASE_SRC := 1
$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
