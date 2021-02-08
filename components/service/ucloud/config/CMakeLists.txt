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

set(config_public_header 
	include/alibabacloud/config/ConfigClient.h
	include/alibabacloud/config/ConfigExport.h )

set(config_public_header_model 
	include/alibabacloud/config/model/ActiveConfigRulesRequest.h
	include/alibabacloud/config/model/ActiveConfigRulesResult.h
	include/alibabacloud/config/model/DeleteConfigRulesRequest.h
	include/alibabacloud/config/model/DeleteConfigRulesResult.h
	include/alibabacloud/config/model/DescribeComplianceRequest.h
	include/alibabacloud/config/model/DescribeComplianceResult.h
	include/alibabacloud/config/model/DescribeComplianceSummaryRequest.h
	include/alibabacloud/config/model/DescribeComplianceSummaryResult.h
	include/alibabacloud/config/model/DescribeConfigRuleRequest.h
	include/alibabacloud/config/model/DescribeConfigRuleResult.h
	include/alibabacloud/config/model/DescribeConfigurationRecorderRequest.h
	include/alibabacloud/config/model/DescribeConfigurationRecorderResult.h
	include/alibabacloud/config/model/DescribeDeliveryChannelsRequest.h
	include/alibabacloud/config/model/DescribeDeliveryChannelsResult.h
	include/alibabacloud/config/model/DescribeDiscoveredResourceRequest.h
	include/alibabacloud/config/model/DescribeDiscoveredResourceResult.h
	include/alibabacloud/config/model/DescribeEvaluationResultsRequest.h
	include/alibabacloud/config/model/DescribeEvaluationResultsResult.h
	include/alibabacloud/config/model/GetDiscoveredResourceCountsRequest.h
	include/alibabacloud/config/model/GetDiscoveredResourceCountsResult.h
	include/alibabacloud/config/model/GetDiscoveredResourceSummaryRequest.h
	include/alibabacloud/config/model/GetDiscoveredResourceSummaryResult.h
	include/alibabacloud/config/model/GetResourceComplianceTimelineRequest.h
	include/alibabacloud/config/model/GetResourceComplianceTimelineResult.h
	include/alibabacloud/config/model/GetResourceConfigurationTimelineRequest.h
	include/alibabacloud/config/model/GetResourceConfigurationTimelineResult.h
	include/alibabacloud/config/model/GetSupportedResourceTypesRequest.h
	include/alibabacloud/config/model/GetSupportedResourceTypesResult.h
	include/alibabacloud/config/model/ListConfigRulesRequest.h
	include/alibabacloud/config/model/ListConfigRulesResult.h
	include/alibabacloud/config/model/ListDiscoveredResourcesRequest.h
	include/alibabacloud/config/model/ListDiscoveredResourcesResult.h
	include/alibabacloud/config/model/PutConfigRuleRequest.h
	include/alibabacloud/config/model/PutConfigRuleResult.h
	include/alibabacloud/config/model/PutConfigurationRecorderRequest.h
	include/alibabacloud/config/model/PutConfigurationRecorderResult.h
	include/alibabacloud/config/model/PutDeliveryChannelRequest.h
	include/alibabacloud/config/model/PutDeliveryChannelResult.h
	include/alibabacloud/config/model/PutEvaluationsRequest.h
	include/alibabacloud/config/model/PutEvaluationsResult.h
	include/alibabacloud/config/model/StartConfigRuleEvaluationRequest.h
	include/alibabacloud/config/model/StartConfigRuleEvaluationResult.h
	include/alibabacloud/config/model/StartConfigurationRecorderRequest.h
	include/alibabacloud/config/model/StartConfigurationRecorderResult.h
	include/alibabacloud/config/model/StopConfigRulesRequest.h
	include/alibabacloud/config/model/StopConfigRulesResult.h )

set(config_src 
	src/ConfigClient.cc
	src/model/ActiveConfigRulesRequest.cc
	src/model/ActiveConfigRulesResult.cc
	src/model/DeleteConfigRulesRequest.cc
	src/model/DeleteConfigRulesResult.cc
	src/model/DescribeComplianceRequest.cc
	src/model/DescribeComplianceResult.cc
	src/model/DescribeComplianceSummaryRequest.cc
	src/model/DescribeComplianceSummaryResult.cc
	src/model/DescribeConfigRuleRequest.cc
	src/model/DescribeConfigRuleResult.cc
	src/model/DescribeConfigurationRecorderRequest.cc
	src/model/DescribeConfigurationRecorderResult.cc
	src/model/DescribeDeliveryChannelsRequest.cc
	src/model/DescribeDeliveryChannelsResult.cc
	src/model/DescribeDiscoveredResourceRequest.cc
	src/model/DescribeDiscoveredResourceResult.cc
	src/model/DescribeEvaluationResultsRequest.cc
	src/model/DescribeEvaluationResultsResult.cc
	src/model/GetDiscoveredResourceCountsRequest.cc
	src/model/GetDiscoveredResourceCountsResult.cc
	src/model/GetDiscoveredResourceSummaryRequest.cc
	src/model/GetDiscoveredResourceSummaryResult.cc
	src/model/GetResourceComplianceTimelineRequest.cc
	src/model/GetResourceComplianceTimelineResult.cc
	src/model/GetResourceConfigurationTimelineRequest.cc
	src/model/GetResourceConfigurationTimelineResult.cc
	src/model/GetSupportedResourceTypesRequest.cc
	src/model/GetSupportedResourceTypesResult.cc
	src/model/ListConfigRulesRequest.cc
	src/model/ListConfigRulesResult.cc
	src/model/ListDiscoveredResourcesRequest.cc
	src/model/ListDiscoveredResourcesResult.cc
	src/model/PutConfigRuleRequest.cc
	src/model/PutConfigRuleResult.cc
	src/model/PutConfigurationRecorderRequest.cc
	src/model/PutConfigurationRecorderResult.cc
	src/model/PutDeliveryChannelRequest.cc
	src/model/PutDeliveryChannelResult.cc
	src/model/PutEvaluationsRequest.cc
	src/model/PutEvaluationsResult.cc
	src/model/StartConfigRuleEvaluationRequest.cc
	src/model/StartConfigRuleEvaluationResult.cc
	src/model/StartConfigurationRecorderRequest.cc
	src/model/StartConfigurationRecorderResult.cc
	src/model/StopConfigRulesRequest.cc
	src/model/StopConfigRulesResult.cc )

add_library(config ${LIB_TYPE}
	${config_public_header}
	${config_public_header_model}
	${config_src})

set_target_properties(config
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}config
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(config
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_CONFIG_LIBRARY)
endif()

target_include_directories(config
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(config
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(config
		jsoncpp)
	target_include_directories(config
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(config
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(config
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(config
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(config
		jsoncpp)
endif()

install(FILES ${config_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/config)
install(FILES ${config_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/config/model)
install(TARGETS config
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)