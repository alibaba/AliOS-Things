config DRV_USART_INSTANCES_NUMBER_GT_${INSTANCE+1}
    depends on USE_DRV_USART
    bool
<#if INSTANCE != 0>
    default n if DRV_USART_INSTANCES_NUMBER_GT_${INSTANCE} = n     
</#if>  
    default n if DRV_USART_INSTANCES_NUMBER = ${INSTANCE+1}
    default y
    
config DRV_USART_INST_IDX${INSTANCE}
    depends on USE_DRV_USART 
<#if INSTANCE != 0>
                 && DRV_USART_INSTANCES_NUMBER_GT_${INSTANCE}
</#if>
    bool "USART Driver Instance ${INSTANCE}"
    default y
    ---help---
    IDH_HTML_DRV_USART_INSTANCES_NUMBER
    ---endhelp---

ifblock DRV_USART_INST_IDX${INSTANCE}

config DRV_USART_PERIPHERAL_ID_IDX${INSTANCE}
    string "USART Hardware Instance"
    depends on USE_DRV_USART
    range USART_MODULE_ID
    default "USART_ID_0"
    set SYS_CLK_PMC_ID_USART0 optionally to y if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE}="USART_ID_0"
    set SYS_CLK_PMC_ID_USART1 optionally to y if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE}="USART_ID_1"
    set SYS_CLK_PMC_ID_USART2 optionally to y if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE}="USART_ID_2"
    ---help---
    IDH_HTML_USART_HARDWARE_INSTANCE
    ---endhelp---

menu "RTOS Configuration (Instance ${INSTANCE})"
    depends on USE_DRV_USART
    depends on USE_3RDPARTY_RTOS
    depends on DRV_USART_DRIVER_MODE = "DYNAMIC"

config DRV_USART_RTOS_IDX${INSTANCE}
    string "Run This Driver Instance As"
    depends on DRV_USART_DRIVER_MODE = "DYNAMIC"
    range 3RDPARTY_RTOS_SYS_TASKS_OPTIONS
    default "Combined with System Tasks"

config DRV_USART_IDX${INSTANCE}_RTOS_TASK_SIZE
    int "Task Size"
    depends on DRV_USART_RTOS_IDX${INSTANCE} = "Standalone"
    default 1024

config DRV_USART_IDX${INSTANCE}_RTOS_TASK_PRIORITY
    int "Task Priority"
    depends on DRV_USART_RTOS_IDX${INSTANCE} = "Standalone"
    default 1

config DRV_USART_IDX${INSTANCE}_RTOS_USE_DELAY
    bool "Use Task Delay?"
    depends on DRV_USART_RTOS_IDX${INSTANCE} = "Standalone"
    default y

config DRV_USART_IDX${INSTANCE}_RTOS_DELAY
    int "Task Delay"
    depends on DRV_USART_RTOS_IDX${INSTANCE} = "Standalone"
    depends on DRV_USART_IDX${INSTANCE}_RTOS_USE_DELAY
    default 1000
endmenu

config DRV_USART_INT_SRC_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    default "USART0_IRQn" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_0"
    default "USART1_IRQn" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_1"
    default "USART2_IRQn" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_2"

config DRV_USART_INT_ISR_HANDLER_IDX${INSTANCE}
    string "Interrupt handler name"
    depends on USE_DRV_USART
    depends on DRV_USART_INTERRUPT_MODE
    default "USART0_Handler" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_0"
    default "USART1_Handler" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_1"
    default "USART2_Handler" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_2"

config DRV_USART_INT_PRIORITY_IDX${INSTANCE}
    string "Interrupt Priority"
    depends on USE_DRV_USART
    depends on DRV_USART_INTERRUPT_MODE
    range INT_PRIORITY_LEVEL_PIC32C
    default "INT_PRIORITY_LEVEL7"
    ---help---
    IDH_HTML_INT_PRIORITY_LEVEL
    ---endhelp---

config DRV_USART_INT_PRIO_NUM_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    depends on DRV_USART_INTERRUPT_MODE
    default "0" if DRV_USART_INT_PRIORITY_IDX${INSTANCE} = "INT_DISABLE_INTERRUPT"
    default "1" if DRV_USART_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL1"
    default "2" if DRV_USART_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL2"
    default "3" if DRV_USART_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL3"
    default "4" if DRV_USART_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL4"
    default "5" if DRV_USART_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL5"
    default "6" if DRV_USART_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL6"
    default "7" if DRV_USART_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL7"

config DRV_USART_USE_RX_PIN_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    default DRV_USART_PERIPHERAL_ID_IDX${INSTANCE}

config DRV_USART_USE_TX_PIN_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    default DRV_USART_PERIPHERAL_ID_IDX${INSTANCE}

config DRV_USART_USE_CTS_PIN_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    default DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} if DRV_USART_OPER_MODE_IDX${INSTANCE} = "DRV_USART_OPERATION_MODE_HANDSHAKE"

config DRV_USART_USE_RTS_PIN_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    default DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} if DRV_USART_OPER_MODE_IDX${INSTANCE} = "DRV_USART_OPERATION_MODE_IRDA"
    default DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} if DRV_USART_OPER_MODE_IDX${INSTANCE} = "DRV_USART_OPERATION_MODE_HANDSHAKE"

config DRV_USART_OPER_MODE_IDX${INSTANCE}
    string "Operation Mode"
    depends on USE_DRV_USART
    range DRV_USART_OPERATION_MODE
#First default value is for the old static driver compatibility.
    default DRV_USART_OPERATION_MAPPING_IDX${INSTANCE} if (DRV_USART_DRIVER_MODE = "STATIC" && DRV_USART_OPERATION_MAPPING_IDX${INSTANCE} != "DRV_USART_OPERATION_MODE_NORMAL")
    default "DRV_USART_OPERATION_MODE_HANDSHAKE" if (DRV_USART_HANDSHAKE_MODE_IDX${INSTANCE} != "DRV_USART_HANDSHAKE_NONE")
    default "DRV_USART_OPERATION_MODE_NORMAL"
    ---help---
    IDH_HTML_DRV_USART_OPERATION_MODE
    ---endhelp---

ifblock DRV_USART_OPER_MODE_IDX${INSTANCE}="DRV_USART_OPERATION_MODE_ADDRESSED"
config DRV_USART_OPER_MODE_DATA_IDX${INSTANCE}
    hex "Operation Mode Data (hexadecimal)"
    depends on USE_DRV_USART
    default 0x00
    ---help---
    IDH_HTML_DRV_USART_OPERATION_MODE_DATA
    ---endhelp---
endif

config DRV_USART_BRG_CLOCK_IDX${INSTANCE}
    string "Baud Rate Generator Clock"
    depends on USE_DRV_USART
#For now using MCLK as default, clock selection option will be added later
    default SYS_CLK_MASTERCLK_FREQ
    persistent
    ---help---
    IDH_HTML_DRV_USART_INIT
    ---endhelp---
    
config DRV_USART_BAUD_RATE_IDX${INSTANCE}
    int "Baud Rate"
    depends on USE_DRV_USART
    default 9600
    ---help---
    IDH_HTML_DRV_USART_BAUD_RATE_IDXn
    ---endhelp---

config DRV_USART_LINE_CNTRL_IDX${INSTANCE}
    string "Line Control"
    depends on USE_DRV_USART
    range DRV_USART_LINE_CONTROL
#First default value is for the old static driver compatibility.
    default DRV_USART_LINE_CONTROL_MAPPING_IDX${INSTANCE} if (DRV_USART_DRIVER_MODE = "STATIC" && DRV_USART_LINE_CONTROL_MAPPING_IDX${INSTANCE} != "DRV_USART_LINE_CONTROL_8NONE1")
    default "DRV_USART_LINE_CONTROL_8NONE1" if DRV_USART_OPER_MODE_IDX${INSTANCE}!="DRV_USART_OPERATION_MODE_ADDRESSED"
    default "DRV_USART_LINE_CONTROL_9NONE1" if DRV_USART_OPER_MODE_IDX${INSTANCE}="DRV_USART_OPERATION_MODE_ADDRESSED"
    ---help---
    IDH_HTML_DRV_USART_LINE_CONTROL
    ---endhelp---

#For pic32 to pic32c switch only
config DRV_USART_HANDSHAKE_MODE_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    range DRV_USART_HANDSHAKE
#First default value is for the old static driver compatibility.
    default DRV_USART_HANDSHAKEMODE_MAPPING_IDX${INSTANCE} if (DRV_USART_DRIVER_MODE = "STATIC" && DRV_USART_HANDSHAKEMODE_MAPPING_IDX${INSTANCE} != "DRV_USART_HANDSHAKE_NONE")
    default "DRV_USART_HANDSHAKE_NONE"
#End of compatibility block

ifblock DRV_USART_BUFFER_QUEUE_SUPPORT 

config DRV_USART_XMIT_QUEUE_SIZE_IDX${INSTANCE}
    int "Transmit Queue Size"
    depends on USE_DRV_USART
    default 10
    ---help---
    IDH_HTML_DRV_USART_XMIT_QUEUE_SIZE_IDXn
    ---endhelp---

config DRV_USART_RCV_QUEUE_SIZE_IDX${INSTANCE}
    int "Receive Queue Size"
    depends on USE_DRV_USART
    default 10
    ---help---
    IDH_HTML_DRV_USART_RCV_QUEUE_SIZE_IDXn
    ---endhelp---

config DRV_USART_SUPPORT_TRANSMIT_DMA_IDX${INSTANCE}
    bool "Transmit DMA Support"
    depends on USE_DRV_USART
    select USE_SYS_DMA_NEEDED
    default y if DRV_USART_SUPPORT_TRANSMIT_DMA
    default n
#Persistent on !DRV_USART_SUPPORT_TRANSMIT_DMA_IDXn is for the backward compatibility, to allow the user control on DMA option when all the 3 options: BQ model, R/W model and DMA are selected.
    persistent if DRV_USART_READ_WRITE_MODEL_SUPPORT && !DRV_USART_SUPPORT_TRANSMIT_DMA_IDX${INSTANCE}
    set USE_DRV_USART_DMA_SUPPORT to y if DRV_USART_SUPPORT_TRANSMIT_DMA_IDX${INSTANCE}
    ---help---
    IDH_HTML_DRV_USART_TRANSMIT_DMA
    ---endhelp---

config DRV_USART_XMIT_DMA_CH_IDX${INSTANCE}
    int "Transmit DMA Channel Instance"
    depends on USE_DRV_USART
    depends on DRV_USART_SUPPORT_TRANSMIT_DMA_IDX${INSTANCE}
    range 0 23
    default 0
    ---help---
    IDH_HTML_DMA_CHANNEL
    ---endhelp---
    
config DRV_USART_XMIT_DMA_CH_TRIG_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    depends on DRV_USART_SUPPORT_TRANSMIT_DMA_IDX${INSTANCE}
    default "DMA_TRIGGER_USART0_TX" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_0"
    default "DMA_TRIGGER_USART1_TX" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_1"
    default "DMA_TRIGGER_USART2_TX" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_2"

config DRV_USART_SUPPORT_RECEIVE_DMA_IDX${INSTANCE}
    bool "Receive DMA Support"
    depends on USE_DRV_USART
    select USE_SYS_DMA_NEEDED
    default y if DRV_USART_SUPPORT_RECEIVE_DMA
    default n
#Persistent on !DRV_USART_SUPPORT_RECEIVE_DMA_IDXn is for the backward compatibility, to allow the user control on DMA option when all the 3 options: BQ model, R/W model and DMA are selected.
    persistent if DRV_USART_READ_WRITE_MODEL_SUPPORT && !DRV_USART_SUPPORT_RECEIVE_DMA_IDX${INSTANCE}
    set USE_DRV_USART_DMA_SUPPORT to y if DRV_USART_SUPPORT_RECEIVE_DMA_IDX${INSTANCE}
    ---help---
    IDH_HTML_DRV_USART_RECEIVE_DMA
    ---endhelp---

config DRV_USART_RCV_DMA_CH_IDX${INSTANCE}
    int "Receive DMA Channel Instance"
    depends on USE_DRV_USART
    depends on DRV_USART_SUPPORT_RECEIVE_DMA_IDX${INSTANCE}
    set DRV_DMA_INSTANCES_NUMBER to 2
    range 0 23
    default 1
    ---help---
    IDH_HTML_DMA_CHANNEL
    ---endhelp---
    
config DRV_USART_RCV_DMA_CH_TRIG_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    depends on DRV_USART_SUPPORT_RECEIVE_DMA_IDX${INSTANCE}
    default "DMA_TRIGGER_USART0_RX" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_0"
    default "DMA_TRIGGER_USART1_RX" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_1"
    default "DMA_TRIGGER_USART2_RX" if DRV_USART_PERIPHERAL_ID_IDX${INSTANCE} = "USART_ID_2"

endif

#This menu is for the old static driver compatibility and is not visible.
menu "USART_OLD_STATIC_IDX"
visible if 0
ifblock DRV_USART_DRIVER_MODE = "STATIC"
config DRV_USART_STATIC_RX_MODES_IDX${INSTANCE}
    string "Deprecated Option 1"
    depends on USE_DRV_USART
    range USART_HANDSHAKE_MODE
    default "USART_HANDSHAKE_MODE_FLOW_CONTROL"
config DRV_USART_HANDSHAKEMODE_MAPPING_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    range DRV_USART_HANDSHAKE
    default "DRV_USART_HANDSHAKE_FLOWCONTROL" if (DRV_USART_STATIC_RX_MODES_IDX${INSTANCE} = "USART_HANDSHAKE_MODE_FLOW_CONTROL" && DRV_USART_STATIC_OP_MODES_IDX${INSTANCE} = "USART_ENABLE_TX_RX_CTS_RTS_USED")
    default "DRV_USART_HANDSHAKE_SIMPLEX" if (DRV_USART_STATIC_RX_MODES_IDX${INSTANCE} = "USART_HANDSHAKE_MODE_SIMPLEX" && DRV_USART_STATIC_OP_MODES_IDX${INSTANCE} = "USART_ENABLE_TX_RX_CTS_RTS_USED")
    default "DRV_USART_HANDSHAKE_NONE"

config DRV_USART_STATIC_OP_MODES_IDX${INSTANCE}
    string "Deprecated Option 2"
    depends on USE_DRV_USART
    range USART_OPERATION_MODE
    default "USART_ENABLE_TX_RX_USED"
config DRV_USART_OPERATION_MAPPING_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    range DRV_USART_OPERATION_MODE
    default "DRV_USART_OPERATION_MODE_IRDA" if DRV_USART_STATIC_OP_MODES_IDX${INSTANCE} = "USART_ENABLE_TX_RX_BCLK_USED"
    default "DRV_USART_OPERATION_MODE_NORMAL"

config DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE}
    string "Deprecated Option 3"
    depends on USE_DRV_USART
    range USART_LINECONTROL_MODE
    default "USART_8N1"
config DRV_USART_LINE_CONTROL_MAPPING_IDX${INSTANCE}
    string
    depends on USE_DRV_USART
    range DRV_USART_LINE_CONTROL
    default "DRV_USART_LINE_CONTROL_8NONE1" if DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE} = "USART_8N1"
    default "DRV_USART_LINE_CONTROL_9NONE1" if DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE} = "USART_9N1"
    default "DRV_USART_LINE_CONTROL_8EVEN1" if DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE} = "USART_8E1"
    default "DRV_USART_LINE_CONTROL_8EVEN2" if DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE} = "USART_8E2"
    default "DRV_USART_LINE_CONTROL_8ODD1" if DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE} = "USART_8O1"
    default "DRV_USART_LINE_CONTROL_8ODD2" if DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE} = "USART_8O2"
    default "DRV_USART_LINE_CONTROL_8NONE2" if DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE} = "USART_8N2"
    default "DRV_USART_LINE_CONTROL_9NONE2" if DRV_USART_STATIC_LINECONTROL_MODES_IDX${INSTANCE} = "USART_9N2"

config DRV_USART_STATIC_TX_ENABLE_IDX${INSTANCE}
    bool "Enable TX"
    depends on USE_DRV_USART
    default y

config DRV_USART_STATIC_RX_ENABLE_IDX${INSTANCE}
    bool "Enable RX"
    depends on USE_DRV_USART
    default y

config DRV_USART_STATIC_TX_INTR_MODES_IDX${INSTANCE}
    string "Deprecated Option 4"
    depends on USE_DRV_USART
    depends on DRV_USART_STATIC_TX_ENABLE_IDX${INSTANCE}
    range USART_TRANSMIT_INTR_MODE
    default "USART_TRANSMIT_FIFO_NOT_FULL"

config DRV_USART_STATIC_RX_INTR_MODES_IDX${INSTANCE}
    string "Deprecated Option 5"
    depends on USE_DRV_USART
    depends on DRV_USART_STATIC_RX_ENABLE_IDX${INSTANCE}
    range USART_RECEIVE_INTR_MODE
    default "USART_RECEIVE_FIFO_ONE_CHAR"
endif
endmenu

endif

