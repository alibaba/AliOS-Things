config SYS_PORTS_PPS_INPUT_${INSTANCE}
    depends on USE_SYS_PORTS 
    bool "PPS Input ${INSTANCE}"
    default y

ifblock SYS_PORTS_PPS_INPUT_${INSTANCE}

config USE_PPS_INPUT_${INSTANCE}
    depends on USE_SYS_PORTS 
    bool "Enabled?"

config SYS_PORT_PPS_INPUT_FUNCTION_${INSTANCE}
    depends on USE_PPS_INPUT_${INSTANCE}
    string "Function"
    range PORTS_REMAP_INPUT_FUNCTION

config SYS_PORT_PPS_INPUT_PIN_${INSTANCE}
    depends on USE_PPS_INPUT_${INSTANCE}
    string "Pin"
    range PORTS_REMAP_INPUT_PIN

endif


