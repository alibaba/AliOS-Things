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

set(servicemesh_public_header 
	include/alibabacloud/servicemesh/ServicemeshClient.h
	include/alibabacloud/servicemesh/ServicemeshExport.h )

set(servicemesh_public_header_model 
	include/alibabacloud/servicemesh/model/AddClusterIntoServiceMeshRequest.h
	include/alibabacloud/servicemesh/model/AddClusterIntoServiceMeshResult.h
	include/alibabacloud/servicemesh/model/AddVmAppToMeshRequest.h
	include/alibabacloud/servicemesh/model/AddVmAppToMeshResult.h
	include/alibabacloud/servicemesh/model/CreateServiceMeshRequest.h
	include/alibabacloud/servicemesh/model/CreateServiceMeshResult.h
	include/alibabacloud/servicemesh/model/DeleteServiceMeshRequest.h
	include/alibabacloud/servicemesh/model/DeleteServiceMeshResult.h
	include/alibabacloud/servicemesh/model/DescribeCensRequest.h
	include/alibabacloud/servicemesh/model/DescribeCensResult.h
	include/alibabacloud/servicemesh/model/DescribeClusterGrafanaRequest.h
	include/alibabacloud/servicemesh/model/DescribeClusterGrafanaResult.h
	include/alibabacloud/servicemesh/model/DescribeClusterPrometheusRequest.h
	include/alibabacloud/servicemesh/model/DescribeClusterPrometheusResult.h
	include/alibabacloud/servicemesh/model/DescribeClustersInServiceMeshRequest.h
	include/alibabacloud/servicemesh/model/DescribeClustersInServiceMeshResult.h
	include/alibabacloud/servicemesh/model/DescribeGuestClusterAccessLogDashboardsRequest.h
	include/alibabacloud/servicemesh/model/DescribeGuestClusterAccessLogDashboardsResult.h
	include/alibabacloud/servicemesh/model/DescribeIngressGatewaysRequest.h
	include/alibabacloud/servicemesh/model/DescribeIngressGatewaysResult.h
	include/alibabacloud/servicemesh/model/DescribeServiceMeshDetailRequest.h
	include/alibabacloud/servicemesh/model/DescribeServiceMeshDetailResult.h
	include/alibabacloud/servicemesh/model/DescribeServiceMeshKubeconfigRequest.h
	include/alibabacloud/servicemesh/model/DescribeServiceMeshKubeconfigResult.h
	include/alibabacloud/servicemesh/model/DescribeServiceMeshesRequest.h
	include/alibabacloud/servicemesh/model/DescribeServiceMeshesResult.h
	include/alibabacloud/servicemesh/model/DescribeUpgradeVersionRequest.h
	include/alibabacloud/servicemesh/model/DescribeUpgradeVersionResult.h
	include/alibabacloud/servicemesh/model/GetDiagnosisRequest.h
	include/alibabacloud/servicemesh/model/GetDiagnosisResult.h
	include/alibabacloud/servicemesh/model/GetRegisteredServiceEndpointsRequest.h
	include/alibabacloud/servicemesh/model/GetRegisteredServiceEndpointsResult.h
	include/alibabacloud/servicemesh/model/GetRegisteredServiceNamespacesRequest.h
	include/alibabacloud/servicemesh/model/GetRegisteredServiceNamespacesResult.h
	include/alibabacloud/servicemesh/model/GetRegisteredServicesRequest.h
	include/alibabacloud/servicemesh/model/GetRegisteredServicesResult.h
	include/alibabacloud/servicemesh/model/GetServiceMeshSlbRequest.h
	include/alibabacloud/servicemesh/model/GetServiceMeshSlbResult.h
	include/alibabacloud/servicemesh/model/GetVmAppMeshInfoRequest.h
	include/alibabacloud/servicemesh/model/GetVmAppMeshInfoResult.h
	include/alibabacloud/servicemesh/model/GetVmMetaRequest.h
	include/alibabacloud/servicemesh/model/GetVmMetaResult.h
	include/alibabacloud/servicemesh/model/RemoveClusterFromServiceMeshRequest.h
	include/alibabacloud/servicemesh/model/RemoveClusterFromServiceMeshResult.h
	include/alibabacloud/servicemesh/model/RemoveVmAppFromMeshRequest.h
	include/alibabacloud/servicemesh/model/RemoveVmAppFromMeshResult.h
	include/alibabacloud/servicemesh/model/RunDiagnosisRequest.h
	include/alibabacloud/servicemesh/model/RunDiagnosisResult.h
	include/alibabacloud/servicemesh/model/UpdateIstioInjectionConfigRequest.h
	include/alibabacloud/servicemesh/model/UpdateIstioInjectionConfigResult.h
	include/alibabacloud/servicemesh/model/UpdateMeshFeatureRequest.h
	include/alibabacloud/servicemesh/model/UpdateMeshFeatureResult.h
	include/alibabacloud/servicemesh/model/UpgradeMeshVersionRequest.h
	include/alibabacloud/servicemesh/model/UpgradeMeshVersionResult.h )

set(servicemesh_src 
	src/ServicemeshClient.cc
	src/model/AddClusterIntoServiceMeshRequest.cc
	src/model/AddClusterIntoServiceMeshResult.cc
	src/model/AddVmAppToMeshRequest.cc
	src/model/AddVmAppToMeshResult.cc
	src/model/CreateServiceMeshRequest.cc
	src/model/CreateServiceMeshResult.cc
	src/model/DeleteServiceMeshRequest.cc
	src/model/DeleteServiceMeshResult.cc
	src/model/DescribeCensRequest.cc
	src/model/DescribeCensResult.cc
	src/model/DescribeClusterGrafanaRequest.cc
	src/model/DescribeClusterGrafanaResult.cc
	src/model/DescribeClusterPrometheusRequest.cc
	src/model/DescribeClusterPrometheusResult.cc
	src/model/DescribeClustersInServiceMeshRequest.cc
	src/model/DescribeClustersInServiceMeshResult.cc
	src/model/DescribeGuestClusterAccessLogDashboardsRequest.cc
	src/model/DescribeGuestClusterAccessLogDashboardsResult.cc
	src/model/DescribeIngressGatewaysRequest.cc
	src/model/DescribeIngressGatewaysResult.cc
	src/model/DescribeServiceMeshDetailRequest.cc
	src/model/DescribeServiceMeshDetailResult.cc
	src/model/DescribeServiceMeshKubeconfigRequest.cc
	src/model/DescribeServiceMeshKubeconfigResult.cc
	src/model/DescribeServiceMeshesRequest.cc
	src/model/DescribeServiceMeshesResult.cc
	src/model/DescribeUpgradeVersionRequest.cc
	src/model/DescribeUpgradeVersionResult.cc
	src/model/GetDiagnosisRequest.cc
	src/model/GetDiagnosisResult.cc
	src/model/GetRegisteredServiceEndpointsRequest.cc
	src/model/GetRegisteredServiceEndpointsResult.cc
	src/model/GetRegisteredServiceNamespacesRequest.cc
	src/model/GetRegisteredServiceNamespacesResult.cc
	src/model/GetRegisteredServicesRequest.cc
	src/model/GetRegisteredServicesResult.cc
	src/model/GetServiceMeshSlbRequest.cc
	src/model/GetServiceMeshSlbResult.cc
	src/model/GetVmAppMeshInfoRequest.cc
	src/model/GetVmAppMeshInfoResult.cc
	src/model/GetVmMetaRequest.cc
	src/model/GetVmMetaResult.cc
	src/model/RemoveClusterFromServiceMeshRequest.cc
	src/model/RemoveClusterFromServiceMeshResult.cc
	src/model/RemoveVmAppFromMeshRequest.cc
	src/model/RemoveVmAppFromMeshResult.cc
	src/model/RunDiagnosisRequest.cc
	src/model/RunDiagnosisResult.cc
	src/model/UpdateIstioInjectionConfigRequest.cc
	src/model/UpdateIstioInjectionConfigResult.cc
	src/model/UpdateMeshFeatureRequest.cc
	src/model/UpdateMeshFeatureResult.cc
	src/model/UpgradeMeshVersionRequest.cc
	src/model/UpgradeMeshVersionResult.cc )

add_library(servicemesh ${LIB_TYPE}
	${servicemesh_public_header}
	${servicemesh_public_header_model}
	${servicemesh_src})

set_target_properties(servicemesh
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}servicemesh
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(servicemesh
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_SERVICEMESH_LIBRARY)
endif()

target_include_directories(servicemesh
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(servicemesh
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(servicemesh
		jsoncpp)
	target_include_directories(servicemesh
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(servicemesh
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(servicemesh
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(servicemesh
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(servicemesh
		jsoncpp)
endif()

install(FILES ${servicemesh_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/servicemesh)
install(FILES ${servicemesh_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/servicemesh/model)
install(TARGETS servicemesh
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)