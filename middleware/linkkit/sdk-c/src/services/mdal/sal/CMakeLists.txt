INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal/include/internal)

ADD_LIBRARY (iot_sal OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/mdal/sal/sal_arch.c
    ${PROJECT_SOURCE_DIR}/src/services/mdal/sal/sal_err.c
    ${PROJECT_SOURCE_DIR}/src/services/mdal/sal/sal_sockets.c
    ${PROJECT_SOURCE_DIR}/src/services/mdal/sal/ip4_addr.c
    ${PROJECT_SOURCE_DIR}/src/services/mdal/sal/sal_util.c
    ${EXTRA_C_SOURCES}
)
