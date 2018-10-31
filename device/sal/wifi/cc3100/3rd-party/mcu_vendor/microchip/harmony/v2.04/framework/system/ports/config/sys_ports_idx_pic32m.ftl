<#assign port = ["A", "B", "C", "D", "E", "F", "G", "H", "J", "K"]>
menu "Port ${port[INSTANCE]} Registers"
depends on USE_SYS_PORTS

config SYS_PORTS_INST_IDX${INSTANCE}
    depends on USE_SYS_PORTS 
    bool "Use Port?"
    default y

ifblock SYS_PORTS_INST_IDX${INSTANCE}

config SYS_PORT_${port[INSTANCE]}_USED
    string " Use Port ${port[INSTANCE]}?"

config SYS_PORT_${port[INSTANCE]}_CN_USED
    string " Use Port ${port[INSTANCE]} CN?"

endif
endmenu
