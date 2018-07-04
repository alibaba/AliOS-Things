INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/security/base/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/security/base/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib/common/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib/common/itls)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib/common/tfs)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/platform)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_tfs STATIC
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/tfs_demo.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/id2_crypto.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/tfs_uttest.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/platform/pal_platform_base64.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/platform/pal_platform_random.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/platform/pal_platform_log.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/platform/pal_platform_memory.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/platform/pal_platform_md5.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tfs/platform/pal_platform_sha256.c
    ${EXTRA_C_SOURCES}
)

