menu"Pin ${INSTANCE}"
config BSP_PIN_${INSTANCE}_FUNCTION_NAME
    string "Name"
    
config BSP_PIN_${INSTANCE}_FUNCTION_TYPE
    string "Type"
    
config BSP_PIN_${INSTANCE}_PORT_PIN
    string "Pin"
    
config BSP_PIN_${INSTANCE}_PORT_CHANNEL
    string "Channel"
    set SYS_PORT_A_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "A"
    set SYS_PORT_B_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "B"
    set SYS_PORT_C_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "C"
    set SYS_PORT_D_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "D"
    set SYS_PORT_E_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "E"
    set SYS_PORT_F_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "F"
    set SYS_PORT_G_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "G"
    set SYS_PORT_H_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "H"
    set SYS_PORT_J_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "J"
    set SYS_PORT_K_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "K"

config BSP_PIN_${INSTANCE}_MODE
    string "MODE"

config BSP_PIN_${INSTANCE}_DIR
    string "TRIS"

config BSP_PIN_${INSTANCE}_LAT
    string "LAT"

config BSP_PIN_${INSTANCE}_OD
    string "OD"

config BSP_PIN_${INSTANCE}_CN
    string "CN"
    set SYS_PORT_A_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "A" && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_B_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "B" && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_C_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "C" && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_D_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "D"  && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_E_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "E"  && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_F_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "F"  && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_G_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "G"  && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_H_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "H"  && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_J_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "J"  && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORT_K_CN_USED optionally to "Y" if BSP_PIN_${INSTANCE}_PORT_CHANNEL = "K"  && BSP_PIN_${INSTANCE}_CN = "True"
    set SYS_PORTS_CN optionally to y if BSP_PIN_${INSTANCE}_CN = "True" && !HAVE_PPS
    set USE_SYS_PORTS_CN_INTERRUPT optionally to y if BSP_PIN_${INSTANCE}_CN = "True"

config BSP_PIN_${INSTANCE}_PU
    string "PU"

config BSP_PIN_${INSTANCE}_PD
    string "PD"
endmenu
