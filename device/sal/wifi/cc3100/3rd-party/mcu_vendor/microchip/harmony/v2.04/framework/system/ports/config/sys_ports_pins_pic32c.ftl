menu"Pin ${INSTANCE}"
config BSP_PIN_${INSTANCE}_FUNCTION_NAME
    string "Name"
    
config BSP_PIN_${INSTANCE}_FUNCTION_TYPE
    string "Type"
    
config BSP_PIN_${INSTANCE}_PERIPHERAL_FUNCTION	
    string "Peripheral Selection"	    
	
config BSP_PIN_${INSTANCE}_PORT_PIN
    string "Pin"
    
config BSP_PIN_${INSTANCE}_PORT_CHANNEL
    string "Channel"
    set SYS_PORTS_INST_IDX0 optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "A"
    set SYS_PORTS_INST_IDX1 optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "B"
    set SYS_PORTS_INST_IDX2 optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "C"
    set SYS_PORTS_INST_IDX3 optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "D"
    set SYS_PORTS_INST_IDX4 optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "E"

config BSP_PIN_${INSTANCE}_MODE
    string "MODE"

config BSP_PIN_${INSTANCE}_DIR
    string "TRIS"

config BSP_PIN_${INSTANCE}_LAT
    string "LAT"

config BSP_PIN_${INSTANCE}_OD
    string "OD"

config BSP_PIN_${INSTANCE}_PU
    string "PU"

config BSP_PIN_${INSTANCE}_PD
    string "PD"
    
config BSP_PIN_${INSTANCE}_PIO_INTERRUPT
    string "PIO Interrupt"
    set SYS_PORT_A_INTERRUPT_USED optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "A" && BSP_PIN_${INSTANCE}_PIO_INTERRUPT != ""
    set SYS_PORT_B_INTERRUPT_USED optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "B" && BSP_PIN_${INSTANCE}_PIO_INTERRUPT != ""
    set SYS_PORT_C_INTERRUPT_USED optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "C" && BSP_PIN_${INSTANCE}_PIO_INTERRUPT != ""
    set SYS_PORT_D_INTERRUPT_USED optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "D"  && BSP_PIN_${INSTANCE}_PIO_INTERRUPT != ""
    set SYS_PORT_E_INTERRUPT_USED optionally to y if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "E"  && BSP_PIN_${INSTANCE}_PIO_INTERRUPT != ""		
endmenu
