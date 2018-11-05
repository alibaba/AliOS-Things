config DRV_SPI_INSTANCES_NUMBER_GT_${INSTANCE+1}
    bool
    depends on DRV_SPI_USE_DRIVER
<#if INSTANCE != 0>
	default n if DRV_SPI_INSTANCES_NUMBER_GT_${INSTANCE} = n     
</#if>	
	default n if DRV_SPI_INSTANCES_NUMBER = ${INSTANCE+1}
	default y
	
config DRV_SPI_IDX${INSTANCE}
    depends on DRV_SPI_USE_DRIVER 
<#if INSTANCE != 0>
	             && DRV_SPI_INSTANCES_NUMBER_GT_${INSTANCE}
</#if>
    bool "SPI Driver Instance ${INSTANCE}"
    default y
    ---help---
    IDH_HTML_DRV_SPI_INSTANCES_NUMBER
    ---endhelp---
    
ifblock DRV_SPI_IDX${INSTANCE}

menu "RTOS Configuration (Instance ${INSTANCE})"
    depends on DRV_SPI_USE_DRIVER
    depends on USE_3RDPARTY_RTOS

config DRV_SPI_RTOS_IDX${INSTANCE}
    string "Run This Driver Instance As"
    range 3RDPARTY_RTOS_SYS_TASKS_OPTIONS
    default "Standalone"

config DRV_SPI_IDX${INSTANCE}_RTOS_TASK_SIZE
    int "Task Size"
    depends on DRV_SPI_RTOS_IDX${INSTANCE} = "Standalone"
    default 1024

config DRV_SPI_IDX${INSTANCE}_RTOS_TASK_PRIORITY
    int "Task Priority"
    depends on DRV_SPI_RTOS_IDX${INSTANCE} = "Standalone"
    default 1

config DRV_SPI_IDX${INSTANCE}_RTOS_USE_DELAY
    bool "Use Task Delay?"
    depends on DRV_SPI_RTOS_IDX${INSTANCE} = "Standalone"
    default y

config DRV_SPI_IDX${INSTANCE}_RTOS_DELAY
    int "Task Delay"
    depends on DRV_SPI_RTOS_IDX${INSTANCE} = "Standalone"
    depends on DRV_SPI_IDX${INSTANCE}_RTOS_USE_DELAY
    default 1000
endmenu

config DRV_SPI_SPI_ID_IDX${INSTANCE}
    depends on DRV_SPI_USE_DRIVER
    range SPI_MODULE_ID
    string "SPI Module ID"
    default "SPI_ID_0"
    set SYS_CLK_PMC_ID_SPI0 optionally to y if DRV_SPI_SPI_ID_IDX${INSTANCE}="SPI_ID_0"
    set SYS_CLK_PMC_ID_SPI1 optionally to y if DRV_SPI_SPI_ID_IDX${INSTANCE}="SPI_ID_1"
    ---help---
    IDH_HTML_SPI_MODULE_ID
    ---endhelp---
    
config DRV_SPI_TASK_MODE_POLLED_IDX${INSTANCE}
    bool
    depends on DRV_SPI_USE_DRIVER
	default n if DRV_SPI_INTERRUPT_MODE
    default y if !DRV_SPI_INTERRUPT_MODE
    
config DRV_SPI_TASK_MODE_ISR_IDX${INSTANCE}
    bool
    depends on DRV_SPI_USE_DRIVER
	default y if DRV_SPI_INTERRUPT_MODE
    default n if !DRV_SPI_INTERRUPT_MODE

config DRV_SPI_INTERRUPT_VECTOR_IDX${INSTANCE}
    string
    depends on DRV_SPI_USE_DRIVER
    depends on DRV_SPI_INTERRUPT_MODE
	default "SPI0_IRQn" if DRV_SPI_SPI_ID_IDX${INSTANCE} = "SPI_ID_0"
    default "SPI1_IRQn" if DRV_SPI_SPI_ID_IDX${INSTANCE} = "SPI_ID_1"
    ---help---
    IDH_HTML_INT_SOURCE
    ---endhelp---
    
config DRV_SPI_INTERRUPT_HANDLER_NAME_IDX${INSTANCE}
    string "Interrupt Handler Name"
    depends on DRV_SPI_USE_DRIVER
    depends on DRV_SPI_INTERRUPT_MODE
	default "SPI0_Handler" if DRV_SPI_SPI_ID_IDX${INSTANCE} = "SPI_ID_0"
    default "SPI1_Handler" if DRV_SPI_SPI_ID_IDX${INSTANCE} = "SPI_ID_1"
    ---help---
    IDH_HTML_INT_SOURCE
    ---endhelp---

config DRV_SPI_INT_PRIORITY_IDX${INSTANCE}
    string "Interrupt Priority"
    depends on DRV_SPI_USE_DRIVER
    depends on DRV_SPI_INTERRUPT_MODE
    range ATSAM_INT_PRIORITY_LEVEL
    default "INT_PRIORITY_LEVEL7"
    ---help---
    IDH_HTML_INT_PRIORITY_LEVEL
    ---endhelp---

config DRV_SPI_INT_PRIO_NUM_IDX${INSTANCE}
    string
    depends on DRV_SPI_USE_DRIVER
    depends on DRV_SPI_INTERRUPT_MODE
    default "0" if DRV_SPI_INT_PRIORITY_IDX${INSTANCE} = "INT_DISABLE_INTERRUPT"
    default "1" if DRV_SPI_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL1"
    default "2" if DRV_SPI_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL2"
    default "3" if DRV_SPI_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL3"
    default "4" if DRV_SPI_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL4"
    default "5" if DRV_SPI_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL5"
    default "6" if DRV_SPI_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL6"
    default "7" if DRV_SPI_INT_PRIORITY_IDX${INSTANCE} = "INT_PRIORITY_LEVEL7"

config DRV_SPI_SPI_MODE_IDX${INSTANCE}
    depends on DRV_SPI_USE_DRIVER
    string "Mode"
    range DRV_SPI_MODE
    default "MASTER"
    ---help---
    IDH_HTML_DRV_SPI_MASTER
    ---endhelp---

comment "****Use SPIx_NPSC0/NSS as Slave Select****"
depends on DRV_SPI_SPI_MODE_IDX${INSTANCE} = "SLAVE"

config DRV_SPI_COMM_WIDTH_N_BIT_IDX${INSTANCE}
    depends on DRV_SPI_USE_DRIVER
    string "SPI Data Width"
    range DRV_SPI_COMMUNICATION_WIDTH
    default "8"
    persistent
    ---help---
    IDH_HTML_DRV_SPI_8BIT
    ---endhelp---

config DRV_SPI_CLOCK_SOURCE_IDX${INSTANCE}
    depends on DRV_SPI_USE_DRIVER
    string "Clock Source Frequency - Hz"
    default SYS_CLK_MASTERCLK_FREQ
    persistent
    ---help---
    IDH_HTML_SPI_BAUD_RATE_CLOCK
    ---endhelp---

comment "**** SPI Baud Rate can not be lesser than Clock Source Frequency/255 ****"    
config DRV_SPI_BAUD_RATE_IDX${INSTANCE}
    depends on DRV_SPI_USE_DRIVER
    int "SPI Baud Rate - Hz"
    default 1000000
    ---help---
    IDH_HTML_DRV_SPI_BAUD_RATE
    ---endhelp---

config DRV_SPI_CLOCK_MODE_IDX${INSTANCE}
    depends on DRV_SPI_USE_DRIVER
    string "Clock Mode"
    range DRV_SPI_CLOCK_MODE
    default "DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_RISE"
    ---help---
    IDH_HTML_DRV_SPI_CLOCK_MODE
    ---endhelp---

config DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX${INSTANCE}
    depends on DRV_SPI_USE_DRIVER
    string "Dummy Byte Value"
    default 0xFF if DRV_SPI_COMM_WIDTH_N_BIT_IDX${INSTANCE} = "8BITS"
    default 0xFFFF if DRV_SPI_COMM_WIDTH_N_BIT_IDX${INSTANCE} = "16BITS"
    default 0xFF

config DRV_SPI_QUEUE_SIZE_IDX${INSTANCE}
    int "Queue Size"
    depends on DRV_SPI_USE_DRIVER
    default 10
    ---help---
    IDH_HTML_DRV_SPI_INIT
    ---endhelp---
    
config DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX${INSTANCE}
    int "Number Of Transactions Before Yield"
    depends on DRV_SPI_USE_DRIVER
    depends on !DRV_SPI_INTERRUPT_MODE
    default 16
    ---help---
    IDH_HTML_DRV_SPI_INIT
    ---endhelp---

config DRV_SPI_USE_DMA_IDX${INSTANCE}
	bool "Use DMA?"
    depends on DRV_SPI_USE_DRIVER
    set DRV_SPI_USE_DMA to y if DRV_SPI_USE_DMA_IDX${INSTANCE}
    set DRV_SPI_INTERRUPT_MODE to y if DRV_SPI_USE_DMA_IDX${INSTANCE}
	default n
	   ---help---
    IDH_HTML_DRV_SPI_DMA
    ---endhelp---
    
ifblock DRV_SPI_USE_DMA_IDX${INSTANCE}

comment "**** DMA System Service should be configured to select DMA Channels ****"	
depends on DRV_SPI_USE_DMA_IDX${INSTANCE} && DRV_SPI_IDX${INSTANCE}

config DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX${INSTANCE}
    int "DMA Max Block Size"
    depends on DRV_SPI_USE_DRIVER
    default 256
    ---help---
    IDH_HTML_DRV_SPI_DMA_DUMMY_BUFFER_SIZE
    ---endhelp---
    
config DRV_SPI_TX_DMA_INST_IDX${INSTANCE}
	int "TX DMA Instance"	
    depends on DRV_SPI_USE_DRIVER
	default 1
	---help---
	IDH_HTML_DMA_CHANNEL
	---endhelp---
	
config DRV_SPI_RX_DMA_INST_IDX${INSTANCE}
	string "RX DMA Instance"	
    depends on DRV_SPI_USE_DRIVER
	default 0
	---help---
	IDH_HTML_DMA_CHANNEL
	---endhelp---
	
endif

endif
