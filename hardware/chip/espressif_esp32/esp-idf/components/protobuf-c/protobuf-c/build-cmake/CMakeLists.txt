SET(PACKAGE protobuf-c)
SET(PACKAGE_NAME protobuf-c)
SET(PACKAGE_VERSION 1.3.0)


CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

PROJECT(protobuf-c)

#options
option(MSVC_STATIC_BUILD "MSVC_STATIC_BUILD" OFF)
option(BUILD_PROTO3 "BUILD_PROTO3" ON)

INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(WORDS_BIGENDIAN)

SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
ADD_DEFINITIONS(-DPACKAGE_VERSION="${PACKAGE_VERSION}")
ADD_DEFINITIONS(-DPACKAGE_STRING="${PACKAGE_STRING}")
ADD_DEFINITIONS(-DWORDS_BIGENDIAN=${WORDS_BIGENDIAN})

if(MSVC)
  # using Visual Studio C++
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4267 /wd4244")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267 /wd4244")
ENDIF()

get_filename_component(MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH)
SET(TEST_DIR ${MAIN_DIR}/t)

MESSAGE(${MAIN_DIR})

SET (PC_SOURCES
        ${MAIN_DIR}/protobuf-c/protobuf-c.c
        ${MAIN_DIR}/protobuf-c/protobuf-c.h)

ADD_LIBRARY(protobuf-c ${PC_SOURCES})

INCLUDE_DIRECTORIES(${MAIN_DIR})
INCLUDE_DIRECTORIES(${MAIN_DIR}/protobuf-c)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}) # for generated files

FIND_PACKAGE(Protobuf REQUIRED)
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})

if (BUILD_PROTO3)
	ADD_DEFINITIONS(-DHAVE_PROTO3)
endif()

if (MSVC AND MSVC_STATIC_BUILD)
	# In case we are building static libraries, link also the runtime library statically
	# so that MSVCR*.DLL is not required at runtime.
	# https://msdn.microsoft.com/en-us/library/2kzt1wy3.aspx
	# This is achieved by replacing msvc option /MD with /MT and /MDd with /MTd
	# http://www.cmake.org/Wiki/CMake_FAQ#How_can_I_build_my_MSVC_application_with_a_static_runtime.3F
	foreach(flag_var
		CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
	  if(${flag_var} MATCHES "/MD")
		string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
	  endif(${flag_var} MATCHES "/MD")
	endforeach(flag_var)
endif (MSVC AND MSVC_STATIC_BUILD)
FILE(GLOB PROTOC_GEN_C_SRC ${MAIN_DIR}/protoc-c/*.h ${MAIN_DIR}/protoc-c/*.cc )
ADD_EXECUTABLE(protoc-gen-c ${PROTOC_GEN_C_SRC})

TARGET_LINK_LIBRARIES(protoc-gen-c ${PROTOBUF_PROTOC_LIBRARY} ${PROTOBUF_LIBRARY})

IF(CMAKE_HOST_UNIX)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ln -sf protoc-gen-c protoc-c
                   DEPENDS protoc-gen-c)
ENDIF()

FUNCTION(GENERATE_TEST_SOURCES PROTO_FILE SRC HDR)
	ADD_CUSTOM_COMMAND(OUTPUT ${SRC} ${HDR}
                   COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
                   ARGS --plugin=$<TARGET_FILE:protoc-gen-c> -I${MAIN_DIR} ${PROTO_FILE} --c_out=${CMAKE_BINARY_DIR}
                   DEPENDS protoc-gen-c)
ENDFUNCTION()


IF(CMAKE_BUILD_TYPE MATCHES Debug)
ENABLE_TESTING()

GENERATE_TEST_SOURCES(${TEST_DIR}/test.proto t/test.pb-c.c t/test.pb-c.h)

ADD_EXECUTABLE(test-generated-code ${TEST_DIR}/generated-code/test-generated-code.c t/test.pb-c.c t/test.pb-c.h )
TARGET_LINK_LIBRARIES(test-generated-code protobuf-c)


ADD_CUSTOM_COMMAND(OUTPUT t/test-full.pb.cc t/test-full.pb.h
                   COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
                   ARGS --cpp_out ${CMAKE_BINARY_DIR} -I${MAIN_DIR} ${TEST_DIR}/test-full.proto)

GENERATE_TEST_SOURCES(${TEST_DIR}/test-full.proto t/test-full.pb-c.c t/test-full.pb-c.h)

ADD_EXECUTABLE(cxx-generate-packed-data ${TEST_DIR}/generated-code2/cxx-generate-packed-data.cc t/test-full.pb.h t/test-full.pb.cc)
TARGET_LINK_LIBRARIES(cxx-generate-packed-data ${PROTOBUF_LIBRARY})

FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/t/generated-code2)
ADD_CUSTOM_COMMAND(OUTPUT t/generated-code2/test-full-cxx-output.inc
                   COMMAND ${CMAKE_BINARY_DIR}/cxx-generate-packed-data ">t/generated-code2/test-full-cxx-output.inc"
                   DEPENDS cxx-generate-packed-data
                   )

GENERATE_TEST_SOURCES(${TEST_DIR}/test-optimized.proto t/test-optimized.pb-c.c t/test-optimized.pb-c.h)

ADD_EXECUTABLE(test-generated-code2 ${TEST_DIR}/generated-code2/test-generated-code2.c t/generated-code2/test-full-cxx-output.inc t/test-full.pb-c.h t/test-full.pb-c.c t/test-optimized.pb-c.h t/test-optimized.pb-c.c)
TARGET_LINK_LIBRARIES(test-generated-code2 protobuf-c)



GENERATE_TEST_SOURCES(${TEST_DIR}/issue220/issue220.proto t/issue220/issue220.pb-c.c t/issue220/issue220.pb-c.h)
ADD_EXECUTABLE(test-issue220 ${TEST_DIR}/issue220/issue220.c t/issue220/issue220.pb-c.c t/issue220/issue220.pb-c.h)
TARGET_LINK_LIBRARIES(test-issue220 protobuf-c)

GENERATE_TEST_SOURCES(${TEST_DIR}/issue251/issue251.proto t/issue251/issue251.pb-c.c t/issue251/issue251.pb-c.h)
ADD_EXECUTABLE(test-issue251 ${TEST_DIR}/issue251/issue251.c t/issue251/issue251.pb-c.c t/issue251/issue251.pb-c.h)
TARGET_LINK_LIBRARIES(test-issue251 protobuf-c)

ADD_EXECUTABLE(test-version ${TEST_DIR}/version/version.c)
TARGET_LINK_LIBRARIES(test-version protobuf-c)

GENERATE_TEST_SOURCES(${TEST_DIR}/test-proto3.proto t/test-proto3.pb-c.c t/test-proto3.pb-c.h)
ADD_EXECUTABLE(test-generated-code3 ${TEST_DIR}/generated-code/test-generated-code.c t/test-proto3.pb-c.c t/test-proto3.pb-c.h)
TARGET_COMPILE_DEFINITIONS(test-generated-code3 PUBLIC -DPROTO3)
TARGET_LINK_LIBRARIES(test-generated-code3 protobuf-c)

ENDIF()

INSTALL(TARGETS protoc-gen-c protobuf-c RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(FILES ${MAIN_DIR}/protobuf-c/protobuf-c.h DESTINATION include/protobuf-c)
INSTALL(FILES ${MAIN_DIR}/protobuf-c/protobuf-c.h DESTINATION include)

IF(CMAKE_HOST_UNIX)
INSTALL(CODE "EXECUTE_PROCESS (COMMAND ln -sf protoc-gen-c protoc-c WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)")
ENDIF()

INCLUDE(Dart)

SET(DART_TESTING_TIMEOUT 5)
ADD_TEST(test-generated-code test-generated-code)
ADD_TEST(test-generated-code2 test-generated-code2)
ADD_TEST(test-generated-code3 test-generated-code3)
ADD_TEST(test-issue220 test-issue220)
ADD_TEST(test-issue251 test-issue251)
ADD_TEST(test-version test-version)


INCLUDE(CPack)
