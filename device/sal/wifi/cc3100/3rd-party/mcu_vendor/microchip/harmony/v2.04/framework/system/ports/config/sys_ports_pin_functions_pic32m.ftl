menu "Type ${INSTANCE}"
config BSP_CUSTOM_TYPE${INSTANCE}
    string "Type Name"

config BSP_CUSTOM_MODE${INSTANCE}
    string "MODE"

config BSP_CUSTOM_DIR${INSTANCE}
    string "TRIS"

config BSP_CUSTOM_LAT${INSTANCE}
    string "LAT"

config BSP_CUSTOM_OD${INSTANCE}
    string "OD"

config BSP_CUSTOM_CN${INSTANCE}
    string "CN"

config BSP_CUSTOM_PU${INSTANCE}
    string "PU"

config BSP_CUSTOM_PD${INSTANCE}
    string "PD"
	
#Added new type that is supported by pic32c and but just declared here as it is new type 
config BSP_CUSTOM_PIO_INTERRUPT${INSTANCE}
	string "PIO_INTERRUPT"
	
endmenu
