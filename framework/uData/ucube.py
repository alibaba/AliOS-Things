src =Split(''' 
    uData_main.c
    uData_interface.c
    service_mgr/service_mgr.c
    cali_data/calibrated_example_algo.c
    abs_data_model/abs_data_model.c
    service_algo/udata_baro_service.c
    service_algo/udata_gps_service.c
''')
component =aos_component('uData', src)


global_includes =Split(''' 
    .
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    AOS_UDATA
    AOS_UDATA_SERVICE_BARO
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    ./include
    ../../include/aos
    ../../include/hal
''')
for i in includes:
    component.add_includes(i)


