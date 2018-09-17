INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/coap)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/linkkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/linkkit/data)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/linkkit/newapi)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/linkkit_gateway)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/linkkit_gateway/newapi)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/examples/mqtt)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/system)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/misc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/os)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/os/product)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/utility)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/file_upload)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/example)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/example)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/shadow)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/subdev)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/app)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/hal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/inc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/download)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/otalib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery/nbpatch)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery/xz-embedded)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery/xz-embedded/linux)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery/xz-embedded/linux/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery/xz-embedded/linux/include/linux)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery/xz-embedded/linux/include/linux/decompress)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery/xz-embedded/linux/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/recovery/xz-embedded/linux/lib/xz)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/service)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/transport)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/utility)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/utility/base64)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/utility/cjson)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/utility/crc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/utility/hexstr2hexbuf)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uota/src/verify)

ADD_EXECUTABLE (mqtt-example
    app_entry.c
    mqtt/mqtt_example.c
)
ADD_EXECUTABLE (mqtt-example-rrpc
    app_entry.c
    mqtt/mqtt_example_rrpc.c
)
ADD_EXECUTABLE (mqtt-example-multithread
    app_entry.c
    mqtt/mqtt_example_multithread.c
)
ADD_EXECUTABLE (linkkit-example-solo
    app_entry.c
    linkkit/linkkit_example_solo.c
)
ADD_EXECUTABLE (linkkit-example-countdown
    app_entry.c
    linkkit/linkkit_example_cntdown.c
)
ADD_EXECUTABLE (linkkit-example-sched
    app_entry.c
    linkkit/linkkit_example_sched.c
)

TARGET_LINK_LIBRARIES (mqtt-example iot_sdk)
TARGET_LINK_LIBRARIES (mqtt-example iot_hal)
TARGET_LINK_LIBRARIES (mqtt-example iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example pthread)
ENDIF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example rt)

TARGET_LINK_LIBRARIES (mqtt-example-rrpc iot_sdk)
TARGET_LINK_LIBRARIES (mqtt-example-rrpc iot_hal)
TARGET_LINK_LIBRARIES (mqtt-example-rrpc iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example-rrpc pthread)
ENDIF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example-rrpc rt)

TARGET_LINK_LIBRARIES (mqtt-example-multithread iot_sdk)
TARGET_LINK_LIBRARIES (mqtt-example-multithread iot_hal)
TARGET_LINK_LIBRARIES (mqtt-example-multithread iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example-multithread pthread)
ENDIF (NOT MSVC)
TARGET_LINK_LIBRARIES (mqtt-example-multithread rt)

TARGET_LINK_LIBRARIES (linkkit-example-solo iot_sdk)
TARGET_LINK_LIBRARIES (linkkit-example-solo iot_hal)
TARGET_LINK_LIBRARIES (linkkit-example-solo iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-solo pthread)
ENDIF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-solo rt)

TARGET_LINK_LIBRARIES (linkkit-example-countdown iot_sdk)
TARGET_LINK_LIBRARIES (linkkit-example-countdown iot_hal)
TARGET_LINK_LIBRARIES (linkkit-example-countdown iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-countdown pthread)
ENDIF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-countdown rt)

TARGET_LINK_LIBRARIES (linkkit-example-sched iot_sdk)
TARGET_LINK_LIBRARIES (linkkit-example-sched iot_hal)
TARGET_LINK_LIBRARIES (linkkit-example-sched iot_tls)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-sched pthread)
ENDIF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit-example-sched rt)

