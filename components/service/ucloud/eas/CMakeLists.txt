#
# Copyright 2009-2017 Alibaba Cloud All rights reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(public_header_dir ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(eas_public_header 
	include/alibabacloud/eas/EasClient.h
	include/alibabacloud/eas/EasExport.h )

set(eas_public_header_model 
	include/alibabacloud/eas/model/CheckServiceExistsRequest.h
	include/alibabacloud/eas/model/CheckServiceExistsResult.h
	include/alibabacloud/eas/model/CreateNetworkInterfaceRequest.h
	include/alibabacloud/eas/model/CreateNetworkInterfaceResult.h
	include/alibabacloud/eas/model/CreateSLSRequest.h
	include/alibabacloud/eas/model/CreateSLSResult.h
	include/alibabacloud/eas/model/CreateServiceRequest.h
	include/alibabacloud/eas/model/CreateServiceResult.h
	include/alibabacloud/eas/model/CreateTaskRequest.h
	include/alibabacloud/eas/model/CreateTaskResult.h
	include/alibabacloud/eas/model/DeleteResourceRequest.h
	include/alibabacloud/eas/model/DeleteResourceResult.h
	include/alibabacloud/eas/model/DeleteResourceInstanceRequest.h
	include/alibabacloud/eas/model/DeleteResourceInstanceResult.h
	include/alibabacloud/eas/model/DeleteServiceRequest.h
	include/alibabacloud/eas/model/DeleteServiceResult.h
	include/alibabacloud/eas/model/DeleteTaskRequest.h
	include/alibabacloud/eas/model/DeleteTaskResult.h
	include/alibabacloud/eas/model/GetAvailableRegionsRequest.h
	include/alibabacloud/eas/model/GetAvailableRegionsResult.h
	include/alibabacloud/eas/model/GetNetworkInterfaceRequest.h
	include/alibabacloud/eas/model/GetNetworkInterfaceResult.h
	include/alibabacloud/eas/model/GetOrCreateResourceRequest.h
	include/alibabacloud/eas/model/GetOrCreateResourceResult.h
	include/alibabacloud/eas/model/GetOssAuthorizeRequest.h
	include/alibabacloud/eas/model/GetOssAuthorizeResult.h
	include/alibabacloud/eas/model/GetResourceInstancesRequest.h
	include/alibabacloud/eas/model/GetResourceInstancesResult.h
	include/alibabacloud/eas/model/GetResourcePodsRequest.h
	include/alibabacloud/eas/model/GetResourcePodsResult.h
	include/alibabacloud/eas/model/GetResourceServicesRequest.h
	include/alibabacloud/eas/model/GetResourceServicesResult.h
	include/alibabacloud/eas/model/GetResourceStatusRequest.h
	include/alibabacloud/eas/model/GetResourceStatusResult.h
	include/alibabacloud/eas/model/GetServiceRequest.h
	include/alibabacloud/eas/model/GetServiceResult.h
	include/alibabacloud/eas/model/GetServiceRegionRequest.h
	include/alibabacloud/eas/model/GetServiceRegionResult.h
	include/alibabacloud/eas/model/GetServiceStatisticsRequest.h
	include/alibabacloud/eas/model/GetServiceStatisticsResult.h
	include/alibabacloud/eas/model/GetServiceTokenRequest.h
	include/alibabacloud/eas/model/GetServiceTokenResult.h
	include/alibabacloud/eas/model/GetServiceVersionRequest.h
	include/alibabacloud/eas/model/GetServiceVersionResult.h
	include/alibabacloud/eas/model/GetServiceWorkerRequest.h
	include/alibabacloud/eas/model/GetServiceWorkerResult.h
	include/alibabacloud/eas/model/GetTaskRequest.h
	include/alibabacloud/eas/model/GetTaskResult.h
	include/alibabacloud/eas/model/ListServicesRequest.h
	include/alibabacloud/eas/model/ListServicesResult.h
	include/alibabacloud/eas/model/ListTasksRequest.h
	include/alibabacloud/eas/model/ListTasksResult.h
	include/alibabacloud/eas/model/ListVIndexRequest.h
	include/alibabacloud/eas/model/ListVIndexResult.h
	include/alibabacloud/eas/model/ModifyServiceRequest.h
	include/alibabacloud/eas/model/ModifyServiceResult.h
	include/alibabacloud/eas/model/ModifyServiceVersionRequest.h
	include/alibabacloud/eas/model/ModifyServiceVersionResult.h
	include/alibabacloud/eas/model/ReleaseServiceRequest.h
	include/alibabacloud/eas/model/ReleaseServiceResult.h
	include/alibabacloud/eas/model/StartServiceRequest.h
	include/alibabacloud/eas/model/StartServiceResult.h
	include/alibabacloud/eas/model/StopServiceRequest.h
	include/alibabacloud/eas/model/StopServiceResult.h
	include/alibabacloud/eas/model/SyncServiceRequest.h
	include/alibabacloud/eas/model/SyncServiceResult.h )

set(eas_src 
	src/EasClient.cc
	src/model/CheckServiceExistsRequest.cc
	src/model/CheckServiceExistsResult.cc
	src/model/CreateNetworkInterfaceRequest.cc
	src/model/CreateNetworkInterfaceResult.cc
	src/model/CreateSLSRequest.cc
	src/model/CreateSLSResult.cc
	src/model/CreateServiceRequest.cc
	src/model/CreateServiceResult.cc
	src/model/CreateTaskRequest.cc
	src/model/CreateTaskResult.cc
	src/model/DeleteResourceRequest.cc
	src/model/DeleteResourceResult.cc
	src/model/DeleteResourceInstanceRequest.cc
	src/model/DeleteResourceInstanceResult.cc
	src/model/DeleteServiceRequest.cc
	src/model/DeleteServiceResult.cc
	src/model/DeleteTaskRequest.cc
	src/model/DeleteTaskResult.cc
	src/model/GetAvailableRegionsRequest.cc
	src/model/GetAvailableRegionsResult.cc
	src/model/GetNetworkInterfaceRequest.cc
	src/model/GetNetworkInterfaceResult.cc
	src/model/GetOrCreateResourceRequest.cc
	src/model/GetOrCreateResourceResult.cc
	src/model/GetOssAuthorizeRequest.cc
	src/model/GetOssAuthorizeResult.cc
	src/model/GetResourceInstancesRequest.cc
	src/model/GetResourceInstancesResult.cc
	src/model/GetResourcePodsRequest.cc
	src/model/GetResourcePodsResult.cc
	src/model/GetResourceServicesRequest.cc
	src/model/GetResourceServicesResult.cc
	src/model/GetResourceStatusRequest.cc
	src/model/GetResourceStatusResult.cc
	src/model/GetServiceRequest.cc
	src/model/GetServiceResult.cc
	src/model/GetServiceRegionRequest.cc
	src/model/GetServiceRegionResult.cc
	src/model/GetServiceStatisticsRequest.cc
	src/model/GetServiceStatisticsResult.cc
	src/model/GetServiceTokenRequest.cc
	src/model/GetServiceTokenResult.cc
	src/model/GetServiceVersionRequest.cc
	src/model/GetServiceVersionResult.cc
	src/model/GetServiceWorkerRequest.cc
	src/model/GetServiceWorkerResult.cc
	src/model/GetTaskRequest.cc
	src/model/GetTaskResult.cc
	src/model/ListServicesRequest.cc
	src/model/ListServicesResult.cc
	src/model/ListTasksRequest.cc
	src/model/ListTasksResult.cc
	src/model/ListVIndexRequest.cc
	src/model/ListVIndexResult.cc
	src/model/ModifyServiceRequest.cc
	src/model/ModifyServiceResult.cc
	src/model/ModifyServiceVersionRequest.cc
	src/model/ModifyServiceVersionResult.cc
	src/model/ReleaseServiceRequest.cc
	src/model/ReleaseServiceResult.cc
	src/model/StartServiceRequest.cc
	src/model/StartServiceResult.cc
	src/model/StopServiceRequest.cc
	src/model/StopServiceResult.cc
	src/model/SyncServiceRequest.cc
	src/model/SyncServiceResult.cc )

add_library(eas ${LIB_TYPE}
	${eas_public_header}
	${eas_public_header_model}
	${eas_src})

set_target_properties(eas
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}eas
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(eas
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_EAS_LIBRARY)
endif()

target_include_directories(eas
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(eas
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(eas
		jsoncpp)
	target_include_directories(eas
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(eas
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(eas
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(eas
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(eas
		jsoncpp)
endif()

install(FILES ${eas_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/eas)
install(FILES ${eas_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/eas/model)
install(TARGETS eas
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)