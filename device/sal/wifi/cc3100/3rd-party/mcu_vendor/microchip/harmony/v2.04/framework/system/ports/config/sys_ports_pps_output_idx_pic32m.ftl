config SYS_PORTS_PPS_OUTPUT_${INSTANCE}
    depends on USE_SYS_PORTS 
    bool "PPS Output ${INSTANCE}"
    default y

ifblock SYS_PORTS_PPS_OUTPUT_${INSTANCE}


config USE_PPS_OUTPUT_${INSTANCE}
    depends on USE_SYS_PORTS
    bool "Enabled?"

config SYS_PORT_PPS_OUTPUT_FUNCTION_${INSTANCE}
    depends on USE_PPS_OUTPUT_${INSTANCE}
    string "Function"
    range PORTS_REMAP_OUTPUT_FUNCTION

config SYS_PORT_PPS_OUTPUT_PIN_${INSTANCE}
    depends on USE_PPS_OUTPUT_${INSTANCE}
    string "Pin"
    range PORTS_REMAP_OUTPUT_PIN

endif


	
