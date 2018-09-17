INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/system)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/misc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/shadow/)

ADD_LIBRARY (iot_shadow STATIC
    ${PROJECT_SOURCE_DIR}/src/services/shadow/shadow_update.c
    ${PROJECT_SOURCE_DIR}/src/services/shadow/shadow_delta.c
    ${PROJECT_SOURCE_DIR}/src/services/shadow/shadow.c
    ${PROJECT_SOURCE_DIR}/src/services/shadow/shadow_common.c
    ${EXTRA_C_SOURCES}
)

