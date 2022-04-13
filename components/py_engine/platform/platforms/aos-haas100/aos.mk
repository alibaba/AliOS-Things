
#add source file
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/amp_fs.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/amp_kv.c")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/amp_ota.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/amp_pm.c")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/amp_socket.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/amp_system.c")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/amp_tls_mbedtls.c")

#network
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/network/amp_cellular.c")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/network/amp_dns.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/network/amp_httpc.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/network/amp_tcp.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/network/amp_udp.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/network/amp_wifi.c")

#peripheral
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_adc.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_can.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_dac.c")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_flash.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_gpio.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_i2c.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_pwm.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_rtc.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_spi.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_timer.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_uart.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/peripheral/amp_hal_wdg.c")

#jquick
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/jquick/jquick_condition.c")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/jquick/jquick_mutex.c")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/jquick/jquick_network.cpp")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/jquick/jquick_thread.c")
#list(APPEND SOURCEFILE "${DRIVERDIR}/platforms/aos-haas100/jquick/jquick_time.c")
