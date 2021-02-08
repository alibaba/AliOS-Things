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

set(ivision_public_header 
	include/alibabacloud/ivision/IvisionClient.h
	include/alibabacloud/ivision/IvisionExport.h )

set(ivision_public_header_model 
	include/alibabacloud/ivision/model/CreateFaceGroupRequest.h
	include/alibabacloud/ivision/model/CreateFaceGroupResult.h
	include/alibabacloud/ivision/model/CreateStreamPredictRequest.h
	include/alibabacloud/ivision/model/CreateStreamPredictResult.h
	include/alibabacloud/ivision/model/DeleteFaceGroupRequest.h
	include/alibabacloud/ivision/model/DeleteFaceGroupResult.h
	include/alibabacloud/ivision/model/DeleteStreamPredictRequest.h
	include/alibabacloud/ivision/model/DeleteStreamPredictResult.h
	include/alibabacloud/ivision/model/DescribeFaceGroupsRequest.h
	include/alibabacloud/ivision/model/DescribeFaceGroupsResult.h
	include/alibabacloud/ivision/model/DescribeStreamPredictResultRequest.h
	include/alibabacloud/ivision/model/DescribeStreamPredictResultResult.h
	include/alibabacloud/ivision/model/DescribeStreamPredictsRequest.h
	include/alibabacloud/ivision/model/DescribeStreamPredictsResult.h
	include/alibabacloud/ivision/model/ImagePredictRequest.h
	include/alibabacloud/ivision/model/ImagePredictResult.h
	include/alibabacloud/ivision/model/RegisterFaceRequest.h
	include/alibabacloud/ivision/model/RegisterFaceResult.h
	include/alibabacloud/ivision/model/SearchFaceRequest.h
	include/alibabacloud/ivision/model/SearchFaceResult.h
	include/alibabacloud/ivision/model/StartStreamPredictRequest.h
	include/alibabacloud/ivision/model/StartStreamPredictResult.h
	include/alibabacloud/ivision/model/StopStreamPredictRequest.h
	include/alibabacloud/ivision/model/StopStreamPredictResult.h
	include/alibabacloud/ivision/model/UnregisterFaceRequest.h
	include/alibabacloud/ivision/model/UnregisterFaceResult.h )

set(ivision_src 
	src/IvisionClient.cc
	src/model/CreateFaceGroupRequest.cc
	src/model/CreateFaceGroupResult.cc
	src/model/CreateStreamPredictRequest.cc
	src/model/CreateStreamPredictResult.cc
	src/model/DeleteFaceGroupRequest.cc
	src/model/DeleteFaceGroupResult.cc
	src/model/DeleteStreamPredictRequest.cc
	src/model/DeleteStreamPredictResult.cc
	src/model/DescribeFaceGroupsRequest.cc
	src/model/DescribeFaceGroupsResult.cc
	src/model/DescribeStreamPredictResultRequest.cc
	src/model/DescribeStreamPredictResultResult.cc
	src/model/DescribeStreamPredictsRequest.cc
	src/model/DescribeStreamPredictsResult.cc
	src/model/ImagePredictRequest.cc
	src/model/ImagePredictResult.cc
	src/model/RegisterFaceRequest.cc
	src/model/RegisterFaceResult.cc
	src/model/SearchFaceRequest.cc
	src/model/SearchFaceResult.cc
	src/model/StartStreamPredictRequest.cc
	src/model/StartStreamPredictResult.cc
	src/model/StopStreamPredictRequest.cc
	src/model/StopStreamPredictResult.cc
	src/model/UnregisterFaceRequest.cc
	src/model/UnregisterFaceResult.cc )

add_library(ivision ${LIB_TYPE}
	${ivision_public_header}
	${ivision_public_header_model}
	${ivision_src})

set_target_properties(ivision
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}ivision
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(ivision
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_IVISION_LIBRARY)
endif()

target_include_directories(ivision
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(ivision
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(ivision
		jsoncpp)
	target_include_directories(ivision
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(ivision
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(ivision
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(ivision
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(ivision
		jsoncpp)
endif()

install(FILES ${ivision_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/ivision)
install(FILES ${ivision_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/ivision/model)
install(TARGETS ivision
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)