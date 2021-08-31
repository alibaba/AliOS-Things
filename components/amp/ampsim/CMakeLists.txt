cmake_minimum_required(VERSION 3.5)
project(ampsim C ASM CXX)
set(library amp)
set(EXE ampsim)

add_compile_options(-m32)
add_compile_options(-g)

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
aux_source_directory(../engine/quickjs_engine/addons/network/mqtt DIR_ADDON_MQTT_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/timer DIR_ADDON_TIMER_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/dac DIR_ADDON_DAC_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/hardware/i2c DIR_ADDON_I2C_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/systimer DIR_ADDON_SYSTIMER_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/system DIR_ADDON_SYSTEM_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/fs DIR_ADDON_FS_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/kv DIR_ADDON_KV_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/utils/builtin DIR_ADDON_BUILTIN_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/advanced/ota DIR_ADDON_OTA_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/advanced/audio DIR_ADDON_AUDIO_SRCS)
aux_source_directory(../engine/quickjs_engine/addons/advanced/aiot DIR_ADDON_AIOT_SRCS)
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
aux_source_directory(../jslib/bytecode DIR_JSLIB)
aux_source_directory(../../amp_adapter/platform/linux DIR_PLATFORM_LINUX)
aux_source_directory(../../amp_adapter/platform/linux/peripheral DIR_PLATFORM_LINUX_PERIPHERAL)
aux_source_directory(../../uvoice/media DIR_UVOICE_MEDIA)
aux_source_directory(../../uvoice/audio DIR_UVOICE_AUDIO)
aux_source_directory(../../uvoice/stream DIR_UVOICE_STREAM)

aux_source_directory(../../cjson/src DIR_CJSON)

set(ulog_sources
    ../../ulog/src/ulog_init.c
    ../../ulog/src/ulog_ring_fifo.c
    ../../ulog/src/ulog_utility.c
    ../../ulog/src/ulog.c
)

set(DIR_UVOICE_COMMON
    ../../uvoice/common/event.c
	../../uvoice/common/ringbuffer.c
)

aux_source_directory(../../linksdk/core/ DIR_LINKSDK_CORE)
aux_source_directory(../../linksdk/core/sysdep/ DIR_LINKSDK_SYSDEP)
aux_source_directory(../../linksdk/core/utils/ DIR_LINKSDK_UTILS)
aux_source_directory(../../linksdk/components/bootstrap/ DIR_LINKSDK_BOOTSTRAP)
aux_source_directory(../../linksdk/components/data-model/ DIR_LINKSDK_DATAMODEL)
aux_source_directory(../../linksdk/components/subdev/ DIR_LINKSDK_SUBDEV)
aux_source_directory(../../linksdk/external/ DIR_LINKSDK_EXTERNAL)
aux_source_directory(../../linksdk/components/devinfo/ DIR_LINKSDK_DEVINFO)
aux_source_directory(../../linksdk/components/dynreg/ DIR_LINKSDK_DYNREG)
aux_source_directory(../../linksdk/components/ntp/ DIR_LINKSDK_NRP)
aux_source_directory(../../http/src/ DIR_HTTP)

set(linksdk_source
	${DIR_LINKSDK_CORE}
	${DIR_LINKSDK_SYSDEP}
	${DIR_LINKSDK_UTILS}
	${DIR_LINKSDK_BOOTSTRAP}
	${DIR_LINKSDK_DATAMODEL}
	${DIR_LINKSDK_SUBDEV}
	${DIR_LINKSDK_EXTERNAL}
	${DIR_LINKSDK_DEVINFO}
	${DIR_LINKSDK_DYNREG}
	${DIR_LINKSDK_NRP})

set(mbedtls_source
	../../mbedtls/library/aes.c
	../../mbedtls/library/aesni.c
	../../mbedtls/library/arc4.c
	../../mbedtls/library/asn1parse.c
	../../mbedtls/library/asn1write.c
	../../mbedtls/library/base64.c
	../../mbedtls/library/bignum.c
	../../mbedtls/library/blowfish.c
	../../mbedtls/library/camellia.c
	../../mbedtls/library/ccm.c
	../../mbedtls/library/cipher.c
	../../mbedtls/library/cipher_wrap.c
	../../mbedtls/library/cmac.c
	../../mbedtls/library/ctr_drbg.c
	../../mbedtls/library/des.c
	../../mbedtls/library/dhm.c
	../../mbedtls/library/ecdh.c
	../../mbedtls/library/ecdsa.c
	../../mbedtls/library/ecjpake.c
	../../mbedtls/library/ecp.c
	../../mbedtls/library/ecp_curves.c
	../../mbedtls/library/entropy.c
	../../mbedtls/library/entropy_poll.c
	../../mbedtls/library/error.c
	../../mbedtls/library/gcm.c
	../../mbedtls/library/havege.c
	../../mbedtls/library/hmac_drbg.c
	../../mbedtls/library/md.c
	../../mbedtls/library/md2.c
	../../mbedtls/library/md4.c
	../../mbedtls/library/md5.c
	../../mbedtls/library/md_wrap.c
	../../mbedtls/library/memory_buffer_alloc.c
	../../mbedtls/library/oid.c
	../../mbedtls/library/padlock.c
	../../mbedtls/library/pem.c
	../../mbedtls/library/pk.c
	../../mbedtls/library/pk_wrap.c
	../../mbedtls/library/pkcs12.c
	../../mbedtls/library/pkcs5.c
	../../mbedtls/library/pkparse.c
	../../mbedtls/library/pkwrite.c
	../../mbedtls/library/platform.c
	../../mbedtls/library/ripemd160.c
	../../mbedtls/library/rsa.c
	../../mbedtls/library/sha1.c
	../../mbedtls/library/sha256.c
	../../mbedtls/library/sha512.c
	../../mbedtls/library/threading.c
	../../mbedtls/library/timing.c
	../../mbedtls/library/version.c
	../../mbedtls/library/version_features.c
	../../mbedtls/library/xtea.c
	../../mbedtls/library/certs.c
	../../mbedtls/library/pkcs11.c
	../../mbedtls/library/x509.c
	../../mbedtls/library/x509_create.c
	../../mbedtls/library/x509_crl.c
	../../mbedtls/library/x509_crt.c
	../../mbedtls/library/x509_csr.c
	../../mbedtls/library/x509write_crt.c
	../../mbedtls/library/x509write_csr.c
	../../mbedtls/library/debug.c
	../../mbedtls/library/net_sockets.c
	../../mbedtls/library/ssl_cache.c
	../../mbedtls/library/ssl_ciphersuites.c
	../../mbedtls/library/ssl_cli.c
	../../mbedtls/library/ssl_cookie.c
	../../mbedtls/library/ssl_srv.c
	../../mbedtls/library/ssl_ticket.c
	../../mbedtls/library/ssl_tls.c
	../../mbedtls/library/platform_util.c
	../../mbedtls/library/rsa_internal.c
)

aux_source_directory(../../mbedtls/platform/aos/library/ DIR_MBEDTLS_LIB)

add_library(${library} STATIC
	../entry/amp_entry.c
	../engine/quickjs_engine/quickjs/quickjs.c
	../engine/quickjs_engine/quickjs/libregexp.c
	../engine/quickjs_engine/quickjs/libunicode.c
	../engine/quickjs_engine/quickjs/cutils.c
	../engine/quickjs_engine/quickjs/libbf.c
    ../engine/quickjs_engine/quickjs/linux_jquick_mutex.c
	../../linksdk/portfiles/aiot_port/aos_port.c
	${DIR_JSLIB}
	${DIR_ADDON_GPIO_SRCS}
	${DIR_ADDON_IR_SRCS}
	${DIR_ADDON_PWM_SRCS}
	${DIR_ADDON_ONEWIRE_SRCS}
	${DIR_ADDON_RTC_SRCS}
	${DIR_ADDON_UART_SRCS}
    ${DIR_ADDON_WDG_SRCS}
	${DIR_ADDON_ADC_SRCS}
	${DIR_ADDON_HTTP_SRCS}
	${DIR_ADDON_AUDIO_SRCS}
    ${DIR_ADDON_NETMGR_SRCS}
	${DIR_ADDON_TIMER_SRCS}
    ${DIR_ADDON_DAC_SRCS}
    ${DIR_ADDON_I2C_SRCS}
	${DIR_ADDON_SYSTIMER_SRCS}
    ${DIR_ADDON_SYSTEM_SRCS}
    ${DIR_ADDON_FS_SRCS}
    ${DIR_ADDON_KV_SRCS}
	${DIR_ADDON_BUILTIN_SRCS}
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
	${DIR_ADDON_AIOT_SRCS}
    ${DIR_CJSON}
    ${ulog_sources}
	${DIR_UVOICE_COMMON}
	../../uvoice/application/comb/comb.c
	../../uvoice/uvoice.c
	../../uvoice/audio/hal/linux/uvoice_pcm.c
	${DIR_UVOICE_MEDIA}
	${DIR_UVOICE_AUDIO}
	${DIR_UVOICE_STREAM}
	${linksdk_source}
	${mbedtls_source}
	${DIR_MBEDTLS_LIB}
	${DIR_HTTP}
	${DIR_ADDON_MQTT_SRCS}
	)

	add_definitions(
		-D UVOICE_PROJECT
		-D UVOICE_PLAYER_ENABLE
		-D UVOICE_EVENT_ENABLE
		-D UVOICE_FILE_ENABLE
		-D UVOICE_HTTP_ENABLE
		-D UVOICE_MLIST_ENABLE
		-D AUDIO_CACHE_ENABLE
		-D ALICLOUD_TTS_SUPPORT
		-D UVOICE_TTS_ENABLE
		-D UVOICE_DOWNLOAD_ENABLE
		-D UVOICE_RELEASE_VERSION_MAIN=1
		-D UVOICE_RELEASE_VERSION_SUB=0
		-D DEBUG
		-D __os_linux__
		-D CONFIG_HTTP_SECURE=1
		-D UVOICE_PCM_NO_ALSA
		-D MBEDTLS_CONFIG_TLS_MAX_CONTENT_LEN=16384)

target_compile_definitions(${library} PRIVATE __AOS_AMP__)
target_compile_definitions(${library} PRIVATE CONFIG_VERSION=\"1.0\")
target_compile_definitions(${library} PRIVATE CONFIG_KERNEL_LINUX)
target_compile_definitions(${library} PRIVATE IPADDR_STRLEN_MAX=46)
target_compile_definitions(${library} PRIVATE __os_linux__)

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
target_include_directories(${library} PRIVATE ../services/amp_memmgt/)
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
target_include_directories(${library} PRIVATE ../../linksdk/core/utils)
target_include_directories(${library} PRIVATE ../../linksdk/core/sysdep)
target_include_directories(${library} PRIVATE ../../linksdk/components/data-model)
target_include_directories(${library} PRIVATE ../../linksdk/components/subdev)
target_include_directories(${library} PRIVATE ../../linksdk/components/ntp)
target_include_directories(${library} PRIVATE ../../linksdk/components/devinfo)
target_include_directories(${library} PRIVATE ../../linksdk/components/dynreg)
target_include_directories(${library} PRIVATE ../../netmgr/include)
target_include_directories(${library} PRIVATE ../../uservice/include)
target_include_directories(${library} PRIVATE ../../kv/include)
target_include_directories(${library} PRIVATE ../../uvoice/include)
target_include_directories(${library} PRIVATE ../../uvoice/internal)
target_include_directories(${library} PRIVATE ../../uvoice/application/comb/include)
target_include_directories(${library} PRIVATE ../../uvoice/audio)
target_include_directories(${library} PRIVATE ../../http/include)
target_include_directories(${library} PRIVATE ../../http/internal)
target_include_directories(${library} PRIVATE ../../mbedtls/platform/aos/include)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
add_executable(${EXE} ampsim.c amp_stub.c)
target_link_libraries(${EXE} ${library})
target_link_libraries(${EXE} "-lpthread")
target_link_libraries(${EXE} "-lm")
target_link_libraries(${EXE} "-lrt")







