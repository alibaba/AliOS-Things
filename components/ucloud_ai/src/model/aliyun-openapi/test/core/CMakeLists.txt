cmake_minimum_required(VERSION 2.8.2)

set(CMAKE_CXX_STANDARD 11)

include(CTest)

if (CMAKE_VERSION VERSION_LESS 3.2)
  set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
  set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
download_project(PROJ                googletest
                 PREFIX              CMAKE_SOURCE_DIR/test/googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include"
                      "${gmock_SOURCE_DIR}/include")
endif()

include_directories("../../core/include/")

add_executable(core_ut
  alibabacloud_ut.cc
  asynccallercontext_ut.cc
  coreclient_ut.cc
  clientconfiguration_ut.cc
  commonclient_ut.cc
  commonrequest_ut.cc
  commonresponse_ut.cc
  credentials_ut.cc
  curlhttpclient_ut.cc
  ecsmetadatafetcher_ut.cc
  endpointprovider_ut.cc
  error_ut.cc
  executor_ut.cc
  httprequest_ut.cc
  httpresponse_ut.cc
  httpmessage_ut.cc
  hmacsha1signer_ut.cc
  networkproxy_ut.cc
  roaserviceclient_ut.cc
  roaservicerequest_ut.cc
  rpcserviceclient_ut.cc
  rpcservicerequest_ut.cc
  runnable_ut.cc
  serviceresult_ut.cc
  servicerequest_ut.cc
  simplecredentialsprovider_ut.cc
  url_ut.cc
  utils_ut.cc

  instanceprofilecredentialsprovider_ut.cc

  locationclient_ut.cc
  location_model_describeendpoints_request_ut.cc
  location_model_describeendpoints_result_ut.cc

  stsclient_ut.cc
  stsassumerolecredentialsprovider_ut.cc
  sts_model_assumerole_request_ut.cc
  sts_model_assumerole_result_ut.cc
  sts_model_getcalleridentity_request_ut.cc
  sts_model_getcalleridentity_result_ut.cc
  timeout_ut.cc
)

set_target_properties(core_ut
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}core_ut
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(core_ut core gtest gmock_main)
add_test(NAME core_ut COMMAND core_ut)
