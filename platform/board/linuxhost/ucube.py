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

linux_only_targets="coapapp das_app helloworld http2app httpapp id2_app itls_app linkkit_gateway linkkitapp lwm2mapp meshapp mqttapp otaapp prov_app  udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udataapp ulocation.baseapp yts"
