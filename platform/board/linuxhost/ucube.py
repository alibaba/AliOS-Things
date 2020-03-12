src     = []

component = aos_board_component('board_linuxhost', 'linux', src)

# Define the default component testcase set for this board
testcases = Split('''
    test/testcase/basic_test
    test/testcase/framework/alink_test
    test/testcase/middleware/uagent/uota_test
    test/testcase/framework/mqtt_test
    test/testcase/network/netmgr_test
    test/testcase/framework/wifi_hal_test
    test/testcase/kernel/modules/fatfs_test
    test/testcase/kernel/modules/kv_test
    test/testcase/kernel/rhino_test
    test/testcase/osal/aos/aos_test
    test/testcase/kernel/vfs_test
    test/testcase/network/protocols/umesh_test
    test/testcase/security/alicrypto_test
    test/testcase/utility/cjson_test
''')

component.set_global_testcases(testcases)
aos_global_config.set('MESHLOWPOWER',1)

build_types=""

linux_only_targets="coap_demo debug_demo hal_demo helloworld_demo http2_demo http_demo kernel_demo.aos_api_demo kernel_demo.krhino_api_demo linkkit_demo linkkit_gateway_demo lwm2m_demo modbus_demo mqtt_demo ota_demo ulocation.base_demo ulog_demo vfs_demo yts_demo"
