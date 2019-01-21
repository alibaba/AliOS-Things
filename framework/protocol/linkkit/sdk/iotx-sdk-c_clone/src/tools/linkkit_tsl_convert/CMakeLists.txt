INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/tools/linkkit_tsl_convert/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/tools/linkkit_tsl_convert/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/tools/linkkit_tsl_convert/src)

IF (NOT WIN32)
ADD_EXECUTABLE (linkkit_tsl_convert
    ${PROJECT_SOURCE_DIR}/src/tools/linkkit_tsl_convert/linkkit_tsl_convert.c
    ${PROJECT_SOURCE_DIR}/src/tools/linkkit_tsl_convert/src/tsl_opt_impl.c
    ${PROJECT_SOURCE_DIR}/src/tools/linkkit_tsl_convert/src/tsl_file.c
    ${PROJECT_SOURCE_DIR}/src/tools/linkkit_tsl_convert/src/cJSON.c
    ${PROJECT_SOURCE_DIR}/src/tools/linkkit_tsl_convert/src/tsl_format_export.c
)

IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit_tsl_convert pthread)
ENDIF (NOT MSVC)
IF (NOT MSVC)
TARGET_LINK_LIBRARIES (linkkit_tsl_convert rt)
ENDIF (NOT MSVC)
ENDIF (NOT WIN32)

