NAME ?= external

AOS_LINKSDK ?= 0
AOS_CJSON ?= 0
AOS_OSAL ?= 0
AOS_ULOG ?= 0
AOS_KV ?= 0
AOS_AMP_TASK ?= 0

# common include for osal_aos and ulog
INC_HAAS += $(EXTERNAL_DIR)/common

# Enable activation forcibly
include $(EXTERNAL_DIR)/activation/component.mk

ifeq ($(AOS_LINKSDK), 1)
    include $(EXTERNAL_DIR)/linksdk/component.mk
endif

ifeq ($(AOS_CJSON), 1)
    include $(EXTERNAL_DIR)/cjson/component.mk
endif

ifeq ($(AOS_OSAL), 1)
    include $(EXTERNAL_DIR)/osal/component.mk
endif

ifeq ($(AOS_ULOG), 1)
    include $(EXTERNAL_DIR)/ulog/component.mk
endif

ifeq ($(AOS_KV), 1)
    include $(EXTERNAL_DIR)/kv/component.mk
endif

ifeq ($(AOS_OSAL), 1)
    include $(EXTERNAL_DIR)/osal/osal.mk
endif

ifeq ($(AOS_AMP_TASK), 1)
    include $(EXTERNAL_DIR)/amp_task/amp_task.mk
endif

INC += $(addprefix -I, ${INC_HAAS})
