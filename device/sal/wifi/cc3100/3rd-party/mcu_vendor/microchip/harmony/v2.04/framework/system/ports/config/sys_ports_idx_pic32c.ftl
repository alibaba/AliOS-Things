<#assign port = ["A", "B", "C", "D", "E"]>
menu "Port ${port[INSTANCE]} Registers"
depends on USE_SYS_PORTS
    
config SYS_PORTS_INST_IDX${INSTANCE}
    depends on USE_SYS_PORTS
    bool "Use Port?"
    default n
    set SYS_CLK_PMC_ID_PORT${port[INSTANCE]} optionally to y if SYS_PORTS_INST_IDX${INSTANCE}
        
config SYS_PORT_${port[INSTANCE]}_INTERRUPT_USED
    bool " Use Interrupt?"
    set USE_SYS_PORTS_INTERRUPT optionally to y if SYS_PORT_${port[INSTANCE]}_INTERRUPT_USED
    set USE_SYS_PORTS_INTERRUPT_${port[INSTANCE]} optionally to y if SYS_PORT_${port[INSTANCE]}_INTERRUPT_USED
    depends on SYS_PORTS_INST_IDX${INSTANCE}
    default n
    
endmenu
