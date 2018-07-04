本示例包括三个部分：
     示例如何使用subdev的API直接进行开发
    demo_gateway_function(msg_buf, msg_readbuf);

     示例使用subdev_example_api.h中对topic进行封装的API进行网关开发
    demo_thing_function(msg_buf, msg_readbuf);

     示例使用subdev_example_api.h中对topic进行封装的API进行单品设备开发
    demo_only_one_device(msg_buf, msg_readbuf);
    

目前在网关上添加子设备支持两种方式：
    静态注册：在云端控制台手动添加设备，获取设备三元组(productKey,deviceName和deviceSecret)，然后提供给网关，通过接口IOT_Thing_Register/IOT_Subdevice_Register进行静态注册(IOTX_Thing_REGISTER_TYPE_STATIC);
    动态注册：无需在云端控制添加设备，直接在网关通过接口接口IOT_Thing_Register/IOT_Subdevice_Register进行动态注册(IOTX_Thing_REGISTER_TYPE_DYNAMIC).

    动态注册具体请参考示例demo_gateway_function 中关于动态注册的范例。    


src/subdev中的代码实现主子设备功能，可以使用src/subdev代码直接进行网关开发，支持主子设备功能。
example/subdev_example_api.c/.h是对事物三要素property、event和service的topic的封装，使用者可以使用这些API直接进行物的操作，而无需关心具体topic。


主子设备功能需要首先定义在make.settings中定义    FEATURE_SUBDEVICE_ENABLED   = y
如果该设备是单品设备，请在make.settings中定义    FEATURE_SUBDEVICE_STATUS    = subdevice


网关设备在测试过程中出现MQTT_PUSH_TO_LIST_ERROR(-42)，原因是由于内存的关系，IoT套件目前预设支持最多的topic是30，如需调整，请修改src/mqtt/mqtt_client.h 中IOTX_MC_SUB_REQUEST_NUM_MAX和IOTX_MC_SUB_NUM_MAX的定义。

