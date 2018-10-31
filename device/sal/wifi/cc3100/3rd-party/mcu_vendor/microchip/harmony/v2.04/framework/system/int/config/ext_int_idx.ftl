config EXT_INT_INSTANCES_NUMBER_GT_${INSTANCE+1}
    bool
    depends on USE_EXT_INT
<#if INSTANCE != 0>
	default n if EXT_INT_INSTANCES_NUMBER_GT_${INSTANCE} = n     
</#if>	
	default n if EXT_INT_INSTANCES_NUMBER = ${INSTANCE+1}
	default y

config EXT_INT_INST_IDX${INSTANCE}
    depends on USE_EXT_INT 
<#if INSTANCE != 0>
	             && EXT_INT_INSTANCES_NUMBER_GT_${INSTANCE}
</#if>
    bool "External Interrupt Instance ${INSTANCE}"
    default y

ifblock EXT_INT_INST_IDX${INSTANCE}

config EXT_INT_PERIPHERAL_ID_IDX${INSTANCE}
    string "External Interrupt Module ID"
    depends on USE_EXT_INT
    range INT_EXTERNAL_SOURCES
    default "INT_EXTERNAL_INT_SOURCE0"

config EXT_INT_INTERRUPT_SOURCE_IDX${INSTANCE}
    string
    depends on USE_EXT_INT
    range INT_SOURCE
    default "INT_SOURCE_EXTERNAL_0" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE0"
    default "INT_SOURCE_EXTERNAL_1" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE1"
    default "INT_SOURCE_EXTERNAL_2" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE2"
    default "INT_SOURCE_EXTERNAL_3" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE3"
    default "INT_SOURCE_EXTERNAL_4" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE4"
    ---help---
    IDH_HTML_INT_SOURCE
    ---endhelp---

config EXT_INT_GENERATE_CODE_IDX${INSTANCE}
    bool "Generate ISR Code?"
    depends on USE_EXT_INT
    default y

config EXT_INT_INTERRUPT_VECTOR_IDX${INSTANCE}
    string
    depends on USE_EXT_INT && EXT_INT_GENERATE_CODE_IDX${INSTANCE}
    range INT_VECTOR
    default "INT_VECTOR_INT0" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE0"
    default "INT_VECTOR_INT1" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE1"
    default "INT_VECTOR_INT2" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE2"
    default "INT_VECTOR_INT3" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE3"
    default "INT_VECTOR_INT4" if EXT_INT_PERIPHERAL_ID_IDX${INSTANCE} = "INT_EXTERNAL_INT_SOURCE4"
    ---help---
    IDH_HTML_INT_VECTOR
    ---endhelp---

config EXT_INT_PRIORITY_IDX${INSTANCE}
    string "Interrupt Priority"
    depends on USE_EXT_INT && EXT_INT_GENERATE_CODE_IDX${INSTANCE}
    range INT_PRIORITY_LEVEL
    default "INT_PRIORITY_LEVEL1"
    ---help---
    IDH_HTML_INT_PRIORITY_LEVEL
    ---endhelp---

config EXT_INT_SUB_PRIORITY_IDX${INSTANCE}
    string "Interrupt Sub-priority"
    depends on USE_EXT_INT && EXT_INT_GENERATE_CODE_IDX${INSTANCE}
    range INT_SUBPRIORITY_LEVEL
    default "INT_SUBPRIORITY_LEVEL0"
    ---help---
    IDH_HTML_INT_SUBPRIORITY_LEVEL
    ---endhelp---

config EXT_INT_PRIO_NUM_IDX${INSTANCE}
    string
    depends on USE_EXT_INT && EXT_INT_GENERATE_CODE_IDX${INSTANCE}
    default "0" if EXT_INT_PRIORITY_IDX${INSTANCE} = "INT_DISABLE_INTERRUPT"
    default "1" if EXT_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL1"
    default "2" if EXT_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL2"
    default "3" if EXT_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL3"
    default "4" if EXT_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL4"
    default "5" if EXT_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL5"
    default "6" if EXT_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL6"
    default "7" if EXT_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL7"

config EXT_INT_ISR_VECTOR_IDX${INSTANCE}
    string
    depends on USE_EXT_INT && EXT_INT_GENERATE_CODE_IDX${INSTANCE}
    default "_EXTERNAL_0_VECTOR" if EXT_INT_INTERRUPT_VECTOR_IDX${INSTANCE} = "INT_VECTOR_INT0"
    default "_EXTERNAL_1_VECTOR" if EXT_INT_INTERRUPT_VECTOR_IDX${INSTANCE} = "INT_VECTOR_INT1"
    default "_EXTERNAL_2_VECTOR" if EXT_INT_INTERRUPT_VECTOR_IDX${INSTANCE} = "INT_VECTOR_INT2"
    default "_EXTERNAL_3_VECTOR" if EXT_INT_INTERRUPT_VECTOR_IDX${INSTANCE} = "INT_VECTOR_INT3"
    default "_EXTERNAL_4_VECTOR" if EXT_INT_INTERRUPT_VECTOR_IDX${INSTANCE} = "INT_VECTOR_INT4"

config EXT_INT_POLARITY_IDX${INSTANCE}
    string "Polarity"
    depends on USE_EXT_INT && EXT_INT_GENERATE_CODE_IDX${INSTANCE}
    range INT_EXTERNAL_EDGE_TRIGGER
    default "INT_EDGE_TRIGGER_RISING"

config EXT_INT_ENABLE_IDX${INSTANCE}
    bool "Enabled by System Service?"
    depends on USE_EXT_INT
    default y
    ---help---
    Select this option if you want Interrupt System Service to enable this 
    Ext Interrupt during system initialization.
    If this option is not selected, Interrupt System Service will generate 
    ISR code, but will not enable this intrrupt. It will be utpo user application
    to enable this interrupt.
    ---endhelp---


config EXT_INT_INST_IDX${INSTANCE}_USED_BY_DRV_WIFI
    bool
    depends on USE_EXT_INT
    default y if ${INSTANCE} = DRV_WIFI_EXT_INT_INSTANCE_INDEX
    default n if ${INSTANCE} != DRV_WIFI_EXT_INT_INSTANCE_INDEX

endif
