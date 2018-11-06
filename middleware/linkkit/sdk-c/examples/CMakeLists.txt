INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/coap)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/http)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/http2)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/linkkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/linkkit/data)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/linkkit/deprecated)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/mqtt)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/ota)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/system)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/system/facility)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/misc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/os)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/os/product)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/http2_stream)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dev_reset)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/client)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/server)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/mal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/mal/ica)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/mal/ica/test)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal/include/internal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/ota)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/ota/impl)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/ota/prot)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/shadow)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/subdev)

ADD_EXECUTABLE (mqtt-example-rrpc
    app_entry.c
    mqtt/mqtt_example_rrpc.c
)
ADD_EXECUTABLE (mqtt-example
    app_entry.c
    mqtt/mqtt_example.c
)
ADD_EXECUTABLE (mqtt-example-multithread
    app_entry.c
    mqtt/mqtt_example_multithread.c
)
ADD_EXECUTABLE (ota-example-mqtt
    ota/ota_example_mqtt.c
)
ADD_EXECUTABLE (linkkit-example-cota
    app_entry.c
    linkkit/linkkit_example_cota.c
)
ADD_EXECUTABLE (linkkit-example-solo
    app_entry.c
    cJSON.c
    linkkit/linkkit_example_solo.c
)
ADD_EXECUTABLE (linkkit-example-countdown
    app_entry.c
    cJSON.c
    linkkit/linkkit_example_cntdown.c
)
ADD_EXECUTABLE (linkkit-example-sched
    app_entry.c
    cJSON.c
    linkkit/linkkit_example_sched.c
)

TARGET_LINK_LIBRARIES (mqtt-example-rrpc iot_sdk)
TARGET_LINK_LIBRARIES (mqtt-example-rrpc iot_hal)
TARGET_LINK_LIBRARIES (mqtt-example-rrpc iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example-rrpc pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example-rrpc rt)
ENDIF (NOT MSVC)

TARGET_LINK_LIBRARIES (mqtt-example iot_sdk)
TARGET_LINK_LIBRARIES (mqtt-example iot_hal)
TARGET_LINK_LIBRARIES (mqtt-example iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example rt)
ENDIF (NOT MSVC)

TARGET_LINK_LIBRARIES (mqtt-example-multithread iot_sdk)
TARGET_LINK_LIBRARIES (mqtt-example-multithread iot_hal)
TARGET_LINK_LIBRARIES (mqtt-example-multithread iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example-multithread pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example-multithread rt)
ENDIF (NOT MSVC)

TARGET_LINK_LIBRARIES (ota-example-mqtt iot_sdk)
TARGET_LINK_LIBRARIES (ota-example-mqtt iot_hal)
TARGET_LINK_LIBRARIES (ota-example-mqtt iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (ota-example-mqtt pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (ota-example-mqtt rt)
ENDIF (NOT MSVC)

TARGET_LINK_LIBRARIES (linkkit-example-cota iot_sdk)
TARGET_LINK_LIBRARIES (linkkit-example-cota iot_hal)
TARGET_LINK_LIBRARIES (linkkit-example-cota iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-cota pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-cota rt)
ENDIF (NOT MSVC)

TARGET_LINK_LIBRARIES (linkkit-example-solo iot_sdk)
TARGET_LINK_LIBRARIES (linkkit-example-solo iot_hal)
TARGET_LINK_LIBRARIES (linkkit-example-solo iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-solo pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-solo rt)
ENDIF (NOT MSVC)

TARGET_LINK_LIBRARIES (linkkit-example-countdown iot_sdk)
TARGET_LINK_LIBRARIES (linkkit-example-countdown iot_hal)
TARGET_LINK_LIBRARIES (linkkit-example-countdown iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-countdown pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-countdown rt)
ENDIF (NOT MSVC)

TARGET_LINK_LIBRARIES (linkkit-example-sched iot_sdk)
TARGET_LINK_LIBRARIES (linkkit-example-sched iot_hal)
TARGET_LINK_LIBRARIES (linkkit-example-sched iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-sched pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-sched rt)
ENDIF (NOT MSVC)

SET (EXECUTABLE_OUTPUT_PATH ../out)
