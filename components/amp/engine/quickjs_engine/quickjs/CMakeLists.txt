set(CMAKE_INSTALL_PREFIX ${PRODUCT_DEPLOY_DIR})

add_definitions(-D_GNU_SOURCE)
add_definitions(-DCONFIG_VERSION="2020-04-12")
#add_definitions(-DCONFIG_BIGNUM)
add_definitions(-DCONFIG_LTO)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -pipe -Wno-narrowing -funwind-tables -feliminate-unused-debug-types -fexceptions -fPIC" CACHE INTERNAL "" FORCE)

file(GLOB LIBSOURCE "quickjs.c"
                    "libregexp.c"
                    "libunicode.c"
                    "cutils.c"
                    "libbf.c")

if(BUILD_JS_EXTENSION_SHARED)
    add_library(quickjs SHARED ${LIBSOURCE})
    target_compile_options(quickjs PRIVATE -fvisibility=default)
else()
    add_library(quickjs STATIC ${LIBSOURCE})
endif()


# add_library(quickjs SHARED ${LIBSOURCE})

#add_executable(qjs ${APPSOURCE})
#target_link_libraries(qjs m dl)

install(TARGETS
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" )
