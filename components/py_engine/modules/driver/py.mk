#board json parse
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/board_mgr.c")

#driver
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/keypad.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/location.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/und.c")

list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/crypto.c")

list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/adc.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/can.c")
#list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/dac.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/gpio.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/i2c.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/ir.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/pwm.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/rtc.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/spi.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/timer.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/uart.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/driver/wdg.c")

