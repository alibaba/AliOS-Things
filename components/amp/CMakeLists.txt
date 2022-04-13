# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

#configure_file(include/fibocom.h.in ${BINARY_TOP_DIR}/include/fibocom.h)
#execute_process(COMMAND bash -c "cd ${CMAKE_CURRENT_SOURCE_DIR} > /dev/null 2>&1;xxd -i #gtset/fibo_gtset.json ${BINARY_TOP_DIR}/include/gtset_json.h;cd - > /dev/null 2>&1")
set(target alios)
#set(libxgd_lib sweep_code/lib/libxgdzbar.a)

aux_source_directory(adapter/platform/FreeRTOS-l610/network DIR_ADAPTER_PLATFORM_NETWORK_SRCS)
aux_source_directory(adapter/platform/FreeRTOS-l610/peripheral DIR_ADAPTER_PLATFORM_PERIPHERAL_SRCS)
aux_source_directory(adapter/platform/FreeRTOS-l610/peripheral/hal DIR_ADAPTER_PLATFORM_PERIPHERAL_HAL_SRCS)
aux_source_directory(adapter/platform/FreeRTOS-l610 DIR_ADAPTER_PLATFORM_SRCS)
aux_source_directory(components/linkkit/dev_model DIR_LINKKIT_DEVMODEL_SRCS)
aux_source_directory(components/linkkit/infra DIR_LINKKIT_INFRA_SRCS)
aux_source_directory(components/linkkit/mqtt DIR_LINKKIT_MQTT_SRCS)
aux_source_directory(components/http/src DIR_HTTP_SRCS)
aux_source_directory(components/das/src/board DIR_DAS_BOARD_SRCS)
aux_source_directory(components/das/src/core DIR_DAS_CORE_SRCS)
aux_source_directory(components/das/src/core/proto DIR_DAS_CORE_PROTO_SRCS)
aux_source_directory(components/das/src/service DIR_DAS_SERVICE_SRCS)
aux_source_directory(components/das/src/service/netfilter DIR_DAS_SERVICE_NETFILTER_SRCS)
aux_source_directory(components/und/src DIR_UND_SRC)
aux_source_directory(engine/duktape_engine DIR_ENGINE_DUKTAPE_SRCS)
aux_source_directory(main DIR_MAIN_SRCS)
aux_source_directory(services/app_mgr DIR_SERVICES_APPMGR_SRCS)
aux_source_directory(services/board_mgr DIR_SERVICES_BOARD_SRCS)
aux_source_directory(services/recovery DIR_SERVICES_RECOVERY_SRCS)
aux_source_directory(components/kv DIR_KV_SRC)


add_library(${target} STATIC
	components/linkkit/certs/root_ca.c
	components/linkkit/dev_sign/dev_sign_mqtt.c
	components/linkkit/dynamic_register/dynreg.c
	components/ulog/ulog.c
	components/ulog/ulog_init.c
	components/ulog/ulog_utility.c
	components/das/src/core/proto/lsoc.pb.c
	components/das/src/core/proto/pb_common.c
	engine/duktape_engine/addons/hardware/adc/module_adc.c
	engine/duktape_engine/addons/hardware/dac/module_dac.c
	engine/duktape_engine/addons/hardware/gpio/module_gpio.c
	engine/duktape_engine/addons/hardware/i2c/module_i2c.c
	engine/duktape_engine/addons/hardware/ir/module_ir.c
	engine/duktape_engine/addons/hardware/lcd/module_lcd.c
	engine/duktape_engine/addons/hardware/pwm/module_pwm.c
	engine/duktape_engine/addons/hardware/rtc/module_rtc.c
	engine/duktape_engine/addons/hardware/spi/module_spi.c
	engine/duktape_engine/addons/hardware/timer/module_timer.c
	engine/duktape_engine/addons/hardware/uart/module_uart.c
	engine/duktape_engine/addons/hardware/wdg/module_wdg.c
	engine/duktape_engine/addons/network/iot/module_iot.c
	engine/duktape_engine/addons/network/http/module_http.c
	engine/duktape_engine/addons/network/mqtt/module_mqtt.c
	engine/duktape_engine/addons/network/tcp/module_tcp.c
	engine/duktape_engine/addons/network/udp/module_udp.c
	engine/duktape_engine/addons/network/wifi/module_wifi.c
	engine/duktape_engine/addons/network/cellular/module_cellular.c
	engine/duktape_engine/addons/utils/builtin/module_builtin.c
	engine/duktape_engine/addons/utils/crypto/module_crypto.c
	engine/duktape_engine/addons/utils/fs/module_fs.c
	engine/duktape_engine/addons/utils/system/module_system.c
	engine/duktape_engine/addons/utils/systimer/module_systimer.c
	engine/duktape_engine/addons/utils/pm/module_pm.c
	engine/duktape_engine/addons/utils/kv/module_kv.c
	engine/duktape_engine/addons/advanced/audio/module_audio.c
	engine/duktape_engine/addons/advanced/audio/module_tts.c
	engine/duktape_engine/addons/libjs.c
	engine/duktape_engine/duktape/duktape.c
	engine/duktape_engine/startup/app_entry.c
	entry/amp_entry.c
	services/amp_utils/amp_utils.c
	utils/cJSON/cJSON.c
	${DIR_ADAPTER_PLATFORM_NETWORK_SRCS}
	${DIR_ADAPTER_PLATFORM_PERIPHERAL_SRCS} 
	${DIR_ADAPTER_PLATFORM_SRCS} 
	${DIR_ADAPTER_PLATFORM_PERIPHERAL_HAL_SRCS}
	${DIR_LINKKIT_DEVMODEL_SRCS}
	${DIR_LINKKIT_INFRA_SRCS} 
	${DIR_LINKKIT_MQTT_SRCS} 
	${DIR_HTTP_SRCS}
	${DIR_DAS_BOARD_SRCS}
	${DIR_DAS_CORE_SRCS}
	${DIR_DAS_SERVICE_SRCS}
	${DIR_DAS_SERVICE_NETFILTER_SRCS}
	${DIR_UND_SRC}
	${DIR_ENGINE_DUKTAPE_SRCS} 
	${DIR_MAIN_SRCS}
	${DIR_SERVICES_APPMGR_SRCS}
	${DIR_SERVICES_BOARD_SRCS}
	${DIR_SERVICES_RECOVERY_SRCS}
	${DIR_KV_SRC}
	${DIR_ULOG_SRC}
	)

set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_FIBO)

target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} PRIVATE net lwip kernel cfw fs)

target_include_directories(${target} PRIVATE adapter/include/)
target_include_directories(${target} PRIVATE adapter/include/peripheral/)
target_include_directories(${target} PRIVATE adapter/platform/FreeRTOS-l610/)
target_include_directories(${target} PRIVATE components/http/include/)
target_include_directories(${target} PRIVATE components/linkkit/dev_model/)
target_include_directories(${target} PRIVATE components/linkkit/dev_sign/)
target_include_directories(${target} PRIVATE components/linkkit/dynamic_register/)
target_include_directories(${target} PRIVATE components/linkkit/fibo_at/)
target_include_directories(${target} PRIVATE components/linkkit/infra/)
target_include_directories(${target} PRIVATE components/linkkit/mqtt/)
target_include_directories(${target} PRIVATE components/linkkit/)
target_include_directories(${target} PRIVATE engine/duktape_engine/addons/hardware/lcd)
target_include_directories(${target} PRIVATE engine/duktape_engine/addons)
target_include_directories(${target} PRIVATE engine/duktape_engine/duktape)
target_include_directories(${target} PRIVATE engine/duktape_engine)
target_include_directories(${target} PRIVATE main/)
target_include_directories(${target} PRIVATE services/amp_utils/)
target_include_directories(${target} PRIVATE services/app_mgr/)
target_include_directories(${target} PRIVATE services/board_mgr/)
target_include_directories(${target} PRIVATE services/recovery/)
target_include_directories(${target} PRIVATE utils/cJSON/)
target_include_directories(${target} PRIVATE ../../net/mbedtls/include)
target_include_directories(${target} PRIVATE ../../net/include/)
target_include_directories(${target} PRIVATE ../../net/lwip/include/)
target_include_directories(${target} PRIVATE ../../net/lwip/src/include/)
target_include_directories(${target} PRIVATE ../../net/lwip/src/include/lwip/)
target_include_directories(${target} PRIVATE ../../net/lwip/src/include/posix/net/)
target_include_directories(${target} PRIVATE ../../kernel/include/)
target_include_directories(${target} PRIVATE ../../cfw/include/)
target_include_directories(${target} PRIVATE ../../atr/include/)
target_include_directories(${target} PRIVATE ../../fs/include/)
target_include_directories(${target} PRIVATE ../../driver/include/)
target_include_directories(${target} PRIVATE ../../nvm/include/)
target_include_directories(${target} PRIVATE ../../fibocom/include/)
target_include_directories(${target} PRIVATE ../../fibocom/auto_apn/)
target_include_directories(${target} PRIVATE ../../tts/aisound/include)
target_include_directories(${target} PRIVATE components/kv/)
target_include_directories(${target} PRIVATE components/kv/include)
target_include_directories(${target} PRIVATE components/ulog)
target_include_directories(${target} PRIVATE components/ulog/include)
target_include_directories(${target} PRIVATE adapter/platform/FreeRTOS-l610)
target_include_directories(${target} PRIVATE components/das/include/)
target_include_directories(${target} PRIVATE components/das/include/das/)
target_include_directories(${target} PRIVATE components/das/src/board/)
target_include_directories(${target} PRIVATE components/und/include/)

relative_glob(srcs */*.h */*.c)
beautify_c_code(${target} ${srcs})

