cmake_minimum_required(VERSION 3.5)
project(ampsim C ASM CXX)
set(library amp)
set(EXE ampsim)

add_compile_options(-m32)

aux_source_directory(../engine/quickjs_engine/addons/hardware/gpio DIR_ADDON_GPIO_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/ir DIR_ADDON_IR_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/pwm DIR_ADDON_PWM_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/onewire DIR_ADDON_ONEWIRE_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/rtc DIR_ADDON_RTC_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/uart DIR_ADDON_UART_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/wdg DIR_ADDON_WDG_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/adc DIR_ADDON_ADC_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/network/http DIR_ADDON_HTTP_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/network/netmgr DIR_ADDON_NETMGR_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/timer DIR_ADDON_TIMER_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/dac DIR_ADDON_DAC_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/i2c DIR_ADDON_I2C_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/systimer DIR_ADDON_SYSTIMER_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/system DIR_ADDON_SYSTEM_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/fs DIR_ADDON_FS_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/kv DIR_ADDON_KV_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/advanced/ota DIR_ADDON_OTA_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/libc DIR_ADDON_LIBC_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/repl DIR_ADDON_REPL_SRCS)
aux_source_directory(../engine/quickjs_engine/addons DIR_ADDON_SRCS)
aux_source_directory(../engine/quickjs_engine/startup DIR_QJS_STARTUP_SRCS)
aux_source_directory(../main DIR_MAIN_SRCS)
aux_source_directory(../services/amp_boot DIR_AMP_BOOT)
aux_source_directory(../services/amp_utils DIR_AMP_UTILS)
aux_source_directory(../services/amp_mgr DIR_AMP_MGR)
aux_source_directory(../services/app_mgr DIR_AMP_APP_MGR)
aux_source_directory(../services/board_mgr DIR_BOARD_MGR)
aux_source_directory(../services/amp_memmgt DIR_AMP_MEMMGT)
aux_source_directory(../jslib/bytecode DIR_BOARD_MGR)
aux_source_directory(../../amp_adapter/platform/linux DIR_PLATFORM_LINUX)
aux_source_directory(../../amp_adapter/platform/linux/peripheral DIR_PLATFORM_LINUX_PERIPHERAL)

aux_source_directory(../../cjson/src DIR_CJSON)

set(ulog_sources
    ../../ulog/src/ulog_init.c
    ../../ulog/src/ulog_ring_fifo.c
    ../../ulog/src/ulog_utility.c
    ../../ulog/src/ulog.c
)

add_library(${library} STATIC
	../entry/amp_entry.c
	../engine/quickjs_engine/quickjs/quickjs.c
	../engine/quickjs_engine/quickjs/libregexp.c
	../engine/quickjs_engine/quickjs/libunicode.c
	../engine/quickjs_engine/quickjs/cutils.c
	../engine/quickjs_engine/quickjs/libbf.c
    ../engine/quickjs_engine/quickjs/linux_jquick_mutex.c
    ../engine/quickjs_engine/repl.c
	${DIR_ADDON_GPIO_SRCS}
	${DIR_ADDON_IR_SRCS}
	${DIR_ADDON_PWM_SRCS}
	${DIR_ADDON_ONEWIRE_SRCS}
	${DIR_ADDON_RTC_SRCS}
	${DIR_ADDON_UART_SRCS}
    ${DIR_ADDON_WDG_SRCS}
	${DIR_ADDON_ADC_SRCS}
	#${DIR_ADDON_HTTP_SRCS}
    ${DIR_ADDON_NETMGR_SRCS}
	${DIR_ADDON_TIMER_SRCS}
    ${DIR_ADDON_DAC_SRCS}
    ${DIR_ADDON_I2C_SRCS}
	${DIR_ADDON_SYSTIMER_SRCS}
    ${DIR_ADDON_SYSTEM_SRCS}
    ${DIR_ADDON_FS_SRCS}
    ${DIR_ADDON_KV_SRCS}
	${DIR_ADDON_OTA_SRCS}
	${DIR_ADDON_LIBC_SRCS}
    ${DIR_QJS_STARTUP_SRCS}
	${DIR_ADDON_SRCS}
	${DIR_MAIN_SRCS}
	${DIR_AMP_MGR}
    ${DIR_ADDON_REPL_SRCS}
    ${DIR_AMP_MEMMGT}
	${DIR_BOARD_MGR}
    ${DIR_PLATFORM_LINUX}
    ${DIR_PLATFORM_LINUX_PERIPHERAL}
    ${DIR_CJSON}
    ${ulog_sources}
	)

target_compile_definitions(${library} PRIVATE __AOS_AMP__)
target_compile_definitions(${library} PRIVATE CONFIG_VERSION=\"1.0\")

target_include_directories(${library} PRIVATE ../engine/quickjs_engine/aos_port)
target_include_directories(${library} PRIVATE ../engine/quickjs_engine/quickjs)
target_include_directories(${library} PRIVATE ../engine/quickjs_engine/addons)
target_include_directories(${library} PRIVATE ../engine/quickjs_engine/addons/common)
target_include_directories(${library} PRIVATE ../engine/quickjs_engine)
target_include_directories(${library} PRIVATE ../main/)
target_include_directories(${library} PRIVATE ../services/amp_utils/)
target_include_directories(${library} PRIVATE ../services/app_mgr/)
target_include_directories(${library} PRIVATE ../services/board_mgr/)
target_include_directories(${library} PRIVATE ../services/amp_boot/)
#target_include_directories(${library} PRIVATE ../utils/cJSON/)
target_include_directories(${library} PRIVATE ../utils/list/)
target_include_directories(${library} PRIVATE ../../amp_adapter/include)
target_include_directories(${library} PRIVATE ../../amp_adapter/include/peripheral)
target_include_directories(${library} PRIVATE ../../amp_adapter/platform/linux)
target_include_directories(${library} PRIVATE ../../amp_adapter/portfiles)

target_include_directories(${library} PRIVATE ../../mbedtls/include)
target_include_directories(${library} PRIVATE ../../mbedtls/platform/include)
target_include_directories(${library} PRIVATE ../../ulog/include)
target_include_directories(${library} PRIVATE ../../ulog/internal)
target_include_directories(${library} PRIVATE ../../osal_aos/include)
target_include_directories(${library} PRIVATE ../../cjson/include)
target_include_directories(${library} PRIVATE ../../ota/include)
target_include_directories(${library} PRIVATE ../../linksdk/core)
target_include_directories(${library} PRIVATE ../../netmgr/include)
target_include_directories(${library} PRIVATE ../../uservice/include)
target_include_directories(${library} PRIVATE ../../kv/include)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
add_executable(${EXE} ampsim.c amp_stub.c)
target_link_libraries(${EXE} ${library})
target_link_libraries(${EXE} "-lpthread")
target_link_libraries(${EXE} "-lm")
target_link_libraries(${EXE} "-lrt")






