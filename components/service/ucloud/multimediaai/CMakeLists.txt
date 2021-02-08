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

set(multimediaai_public_header 
	include/alibabacloud/multimediaai/MultimediaaiClient.h
	include/alibabacloud/multimediaai/MultimediaaiExport.h )

set(multimediaai_public_header_model 
	include/alibabacloud/multimediaai/model/CreateCoverTaskRequest.h
	include/alibabacloud/multimediaai/model/CreateCoverTaskResult.h
	include/alibabacloud/multimediaai/model/CreateFaceGroupRequest.h
	include/alibabacloud/multimediaai/model/CreateFaceGroupResult.h
	include/alibabacloud/multimediaai/model/CreateFacePersonRequest.h
	include/alibabacloud/multimediaai/model/CreateFacePersonResult.h
	include/alibabacloud/multimediaai/model/CreateGifTaskRequest.h
	include/alibabacloud/multimediaai/model/CreateGifTaskResult.h
	include/alibabacloud/multimediaai/model/CreateLabelTaskRequest.h
	include/alibabacloud/multimediaai/model/CreateLabelTaskResult.h
	include/alibabacloud/multimediaai/model/CreateTemplateRequest.h
	include/alibabacloud/multimediaai/model/CreateTemplateResult.h
	include/alibabacloud/multimediaai/model/DeleteFaceGroupRequest.h
	include/alibabacloud/multimediaai/model/DeleteFaceGroupResult.h
	include/alibabacloud/multimediaai/model/DeleteFaceImageRequest.h
	include/alibabacloud/multimediaai/model/DeleteFaceImageResult.h
	include/alibabacloud/multimediaai/model/DeleteFacePersonRequest.h
	include/alibabacloud/multimediaai/model/DeleteFacePersonResult.h
	include/alibabacloud/multimediaai/model/GetTaskResultRequest.h
	include/alibabacloud/multimediaai/model/GetTaskResultResult.h
	include/alibabacloud/multimediaai/model/GetTaskStatusRequest.h
	include/alibabacloud/multimediaai/model/GetTaskStatusResult.h
	include/alibabacloud/multimediaai/model/GetTemplateRequest.h
	include/alibabacloud/multimediaai/model/GetTemplateResult.h
	include/alibabacloud/multimediaai/model/ListFaceGroupsRequest.h
	include/alibabacloud/multimediaai/model/ListFaceGroupsResult.h
	include/alibabacloud/multimediaai/model/ListFaceImagesRequest.h
	include/alibabacloud/multimediaai/model/ListFaceImagesResult.h
	include/alibabacloud/multimediaai/model/ListFacePersonsRequest.h
	include/alibabacloud/multimediaai/model/ListFacePersonsResult.h
	include/alibabacloud/multimediaai/model/ListTemplatesRequest.h
	include/alibabacloud/multimediaai/model/ListTemplatesResult.h
	include/alibabacloud/multimediaai/model/ProcessFaceAlgorithmRequest.h
	include/alibabacloud/multimediaai/model/ProcessFaceAlgorithmResult.h
	include/alibabacloud/multimediaai/model/ProcessImageTagAlgorithmRequest.h
	include/alibabacloud/multimediaai/model/ProcessImageTagAlgorithmResult.h
	include/alibabacloud/multimediaai/model/ProcessLandmarkAlgorithmRequest.h
	include/alibabacloud/multimediaai/model/ProcessLandmarkAlgorithmResult.h
	include/alibabacloud/multimediaai/model/ProcessLogoAlgorithmRequest.h
	include/alibabacloud/multimediaai/model/ProcessLogoAlgorithmResult.h
	include/alibabacloud/multimediaai/model/ProcessNewsAlgorithmRequest.h
	include/alibabacloud/multimediaai/model/ProcessNewsAlgorithmResult.h
	include/alibabacloud/multimediaai/model/ProcessNlpAlgorithmRequest.h
	include/alibabacloud/multimediaai/model/ProcessNlpAlgorithmResult.h
	include/alibabacloud/multimediaai/model/ProcessOcrAlgorithmRequest.h
	include/alibabacloud/multimediaai/model/ProcessOcrAlgorithmResult.h
	include/alibabacloud/multimediaai/model/RegisterFaceImageRequest.h
	include/alibabacloud/multimediaai/model/RegisterFaceImageResult.h
	include/alibabacloud/multimediaai/model/UpdateTemplateRequest.h
	include/alibabacloud/multimediaai/model/UpdateTemplateResult.h )

set(multimediaai_src 
	src/MultimediaaiClient.cc
	src/model/CreateCoverTaskRequest.cc
	src/model/CreateCoverTaskResult.cc
	src/model/CreateFaceGroupRequest.cc
	src/model/CreateFaceGroupResult.cc
	src/model/CreateFacePersonRequest.cc
	src/model/CreateFacePersonResult.cc
	src/model/CreateGifTaskRequest.cc
	src/model/CreateGifTaskResult.cc
	src/model/CreateLabelTaskRequest.cc
	src/model/CreateLabelTaskResult.cc
	src/model/CreateTemplateRequest.cc
	src/model/CreateTemplateResult.cc
	src/model/DeleteFaceGroupRequest.cc
	src/model/DeleteFaceGroupResult.cc
	src/model/DeleteFaceImageRequest.cc
	src/model/DeleteFaceImageResult.cc
	src/model/DeleteFacePersonRequest.cc
	src/model/DeleteFacePersonResult.cc
	src/model/GetTaskResultRequest.cc
	src/model/GetTaskResultResult.cc
	src/model/GetTaskStatusRequest.cc
	src/model/GetTaskStatusResult.cc
	src/model/GetTemplateRequest.cc
	src/model/GetTemplateResult.cc
	src/model/ListFaceGroupsRequest.cc
	src/model/ListFaceGroupsResult.cc
	src/model/ListFaceImagesRequest.cc
	src/model/ListFaceImagesResult.cc
	src/model/ListFacePersonsRequest.cc
	src/model/ListFacePersonsResult.cc
	src/model/ListTemplatesRequest.cc
	src/model/ListTemplatesResult.cc
	src/model/ProcessFaceAlgorithmRequest.cc
	src/model/ProcessFaceAlgorithmResult.cc
	src/model/ProcessImageTagAlgorithmRequest.cc
	src/model/ProcessImageTagAlgorithmResult.cc
	src/model/ProcessLandmarkAlgorithmRequest.cc
	src/model/ProcessLandmarkAlgorithmResult.cc
	src/model/ProcessLogoAlgorithmRequest.cc
	src/model/ProcessLogoAlgorithmResult.cc
	src/model/ProcessNewsAlgorithmRequest.cc
	src/model/ProcessNewsAlgorithmResult.cc
	src/model/ProcessNlpAlgorithmRequest.cc
	src/model/ProcessNlpAlgorithmResult.cc
	src/model/ProcessOcrAlgorithmRequest.cc
	src/model/ProcessOcrAlgorithmResult.cc
	src/model/RegisterFaceImageRequest.cc
	src/model/RegisterFaceImageResult.cc
	src/model/UpdateTemplateRequest.cc
	src/model/UpdateTemplateResult.cc )

add_library(multimediaai ${LIB_TYPE}
	${multimediaai_public_header}
	${multimediaai_public_header_model}
	${multimediaai_src})

set_target_properties(multimediaai
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}multimediaai
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(multimediaai
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_MULTIMEDIAAI_LIBRARY)
endif()

target_include_directories(multimediaai
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(multimediaai
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(multimediaai
		jsoncpp)
	target_include_directories(multimediaai
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(multimediaai
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(multimediaai
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(multimediaai
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(multimediaai
		jsoncpp)
endif()

install(FILES ${multimediaai_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/multimediaai)
install(FILES ${multimediaai_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/multimediaai/model)
install(TARGETS multimediaai
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)