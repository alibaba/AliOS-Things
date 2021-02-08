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

set(schedulerx2_public_header 
	include/alibabacloud/schedulerx2/Schedulerx2Client.h
	include/alibabacloud/schedulerx2/Schedulerx2Export.h )

set(schedulerx2_public_header_model 
	include/alibabacloud/schedulerx2/model/CreateAppGroupRequest.h
	include/alibabacloud/schedulerx2/model/CreateAppGroupResult.h
	include/alibabacloud/schedulerx2/model/CreateJobRequest.h
	include/alibabacloud/schedulerx2/model/CreateJobResult.h
	include/alibabacloud/schedulerx2/model/DeleteJobRequest.h
	include/alibabacloud/schedulerx2/model/DeleteJobResult.h
	include/alibabacloud/schedulerx2/model/DeleteWorkflowRequest.h
	include/alibabacloud/schedulerx2/model/DeleteWorkflowResult.h
	include/alibabacloud/schedulerx2/model/DescribeRegionsRequest.h
	include/alibabacloud/schedulerx2/model/DescribeRegionsResult.h
	include/alibabacloud/schedulerx2/model/DisableJobRequest.h
	include/alibabacloud/schedulerx2/model/DisableJobResult.h
	include/alibabacloud/schedulerx2/model/DisableWorkflowRequest.h
	include/alibabacloud/schedulerx2/model/DisableWorkflowResult.h
	include/alibabacloud/schedulerx2/model/EnableJobRequest.h
	include/alibabacloud/schedulerx2/model/EnableJobResult.h
	include/alibabacloud/schedulerx2/model/EnableWorkflowRequest.h
	include/alibabacloud/schedulerx2/model/EnableWorkflowResult.h
	include/alibabacloud/schedulerx2/model/ExecuteJobRequest.h
	include/alibabacloud/schedulerx2/model/ExecuteJobResult.h
	include/alibabacloud/schedulerx2/model/ExecuteWorkflowRequest.h
	include/alibabacloud/schedulerx2/model/ExecuteWorkflowResult.h
	include/alibabacloud/schedulerx2/model/GetJobInfoRequest.h
	include/alibabacloud/schedulerx2/model/GetJobInfoResult.h
	include/alibabacloud/schedulerx2/model/GetJobInstanceRequest.h
	include/alibabacloud/schedulerx2/model/GetJobInstanceResult.h
	include/alibabacloud/schedulerx2/model/GetJobInstanceListRequest.h
	include/alibabacloud/schedulerx2/model/GetJobInstanceListResult.h
	include/alibabacloud/schedulerx2/model/GrantPermissionRequest.h
	include/alibabacloud/schedulerx2/model/GrantPermissionResult.h
	include/alibabacloud/schedulerx2/model/RevokePermissionRequest.h
	include/alibabacloud/schedulerx2/model/RevokePermissionResult.h
	include/alibabacloud/schedulerx2/model/StopInstanceRequest.h
	include/alibabacloud/schedulerx2/model/StopInstanceResult.h
	include/alibabacloud/schedulerx2/model/UpdateJobRequest.h
	include/alibabacloud/schedulerx2/model/UpdateJobResult.h )

set(schedulerx2_src 
	src/Schedulerx2Client.cc
	src/model/CreateAppGroupRequest.cc
	src/model/CreateAppGroupResult.cc
	src/model/CreateJobRequest.cc
	src/model/CreateJobResult.cc
	src/model/DeleteJobRequest.cc
	src/model/DeleteJobResult.cc
	src/model/DeleteWorkflowRequest.cc
	src/model/DeleteWorkflowResult.cc
	src/model/DescribeRegionsRequest.cc
	src/model/DescribeRegionsResult.cc
	src/model/DisableJobRequest.cc
	src/model/DisableJobResult.cc
	src/model/DisableWorkflowRequest.cc
	src/model/DisableWorkflowResult.cc
	src/model/EnableJobRequest.cc
	src/model/EnableJobResult.cc
	src/model/EnableWorkflowRequest.cc
	src/model/EnableWorkflowResult.cc
	src/model/ExecuteJobRequest.cc
	src/model/ExecuteJobResult.cc
	src/model/ExecuteWorkflowRequest.cc
	src/model/ExecuteWorkflowResult.cc
	src/model/GetJobInfoRequest.cc
	src/model/GetJobInfoResult.cc
	src/model/GetJobInstanceRequest.cc
	src/model/GetJobInstanceResult.cc
	src/model/GetJobInstanceListRequest.cc
	src/model/GetJobInstanceListResult.cc
	src/model/GrantPermissionRequest.cc
	src/model/GrantPermissionResult.cc
	src/model/RevokePermissionRequest.cc
	src/model/RevokePermissionResult.cc
	src/model/StopInstanceRequest.cc
	src/model/StopInstanceResult.cc
	src/model/UpdateJobRequest.cc
	src/model/UpdateJobResult.cc )

add_library(schedulerx2 ${LIB_TYPE}
	${schedulerx2_public_header}
	${schedulerx2_public_header_model}
	${schedulerx2_src})

set_target_properties(schedulerx2
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}schedulerx2
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(schedulerx2
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_SCHEDULERX2_LIBRARY)
endif()

target_include_directories(schedulerx2
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(schedulerx2
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(schedulerx2
		jsoncpp)
	target_include_directories(schedulerx2
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(schedulerx2
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(schedulerx2
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(schedulerx2
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(schedulerx2
		jsoncpp)
endif()

install(FILES ${schedulerx2_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/schedulerx2)
install(FILES ${schedulerx2_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/schedulerx2/model)
install(TARGETS schedulerx2
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)