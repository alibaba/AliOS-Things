INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/security/base/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/security/base/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib/common/tfs/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/platform)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_system OBJECT
    ${PROJECT_SOURCE_DIR}/src/security/base/src/id2_guider.c
    ${PROJECT_SOURCE_DIR}/src/security/base/src/sdk-impl.c
    ${PROJECT_SOURCE_DIR}/src/security/base/src/guider.c
    ${PROJECT_SOURCE_DIR}/src/security/base/src/report.c
    ${PROJECT_SOURCE_DIR}/src/security/base/src/device.c
    ${PROJECT_SOURCE_DIR}/src/security/base/src/ca.c
    ${EXTRA_C_SOURCES}
)

