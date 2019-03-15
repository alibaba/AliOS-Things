src =Split(''' 
    udata_main.c
    udata_interface.c
    service_mgr/service_mgr.c
    cali_data/calibrated_app/example_algo.c
    abs_data_model/abs_data_model.c
    service/service_process.c
    service/service_data_to_cloud.c
    udata_queue.c
    udata_service_task.c
''')
component =aos_component('udata', src)


global_includes =Split(''' 
    .
    include
    ../../drivers/sensor/include
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    AOS_UDATA
    AOS_UDATA_SERVICE_ACC
    AOS_UDATA_SERVICE_MAG
    AOS_UDATA_SERVICE_GYRO
    AOS_UDATA_SERVICE_ALS
    AOS_UDATA_SERVICE_PS
    AOS_UDATA_SERVICE_BARO
    AOS_UDATA_SERVICE_TEMP
    AOS_UDATA_SERVICE_UV
    AOS_UDATA_SERVICE_HUMI
    AOS_UDATA_SERVICE_NOISE
    AOS_UDATA_SERVICE_PM25
    AOS_UDATA_SERVICE_CO2
    AOS_UDATA_SERVICE_HCHO
    AOS_UDATA_SERVICE_TVOC
    AOS_UDATA_SERVICE_HALL
    AOS_UDATA_SERVICE_HR
    AOS_UDATA_SERVICE_GPS	
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    ./include
    ../../include/aos
    ../../kernel/hal/include
''')
for i in includes:
    component.add_includes(i)


