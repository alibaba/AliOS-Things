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

set(visionai-poc_public_header 
	include/alibabacloud/visionai-poc/Visionai_pocClient.h
	include/alibabacloud/visionai-poc/Visionai_pocExport.h )

set(visionai-poc_public_header_model 
	include/alibabacloud/visionai-poc/model/CheckMultiagentRequest.h
	include/alibabacloud/visionai-poc/model/CheckMultiagentResult.h
	include/alibabacloud/visionai-poc/model/PredictCategoryRequest.h
	include/alibabacloud/visionai-poc/model/PredictCategoryResult.h
	include/alibabacloud/visionai-poc/model/RecognizeCarRequest.h
	include/alibabacloud/visionai-poc/model/RecognizeCarResult.h
	include/alibabacloud/visionai-poc/model/RecognizeEntityRequest.h
	include/alibabacloud/visionai-poc/model/RecognizeEntityResult.h
	include/alibabacloud/visionai-poc/model/RecognizeFlowerRequest.h
	include/alibabacloud/visionai-poc/model/RecognizeFlowerResult.h
	include/alibabacloud/visionai-poc/model/RecognizeLabelRequest.h
	include/alibabacloud/visionai-poc/model/RecognizeLabelResult.h
	include/alibabacloud/visionai-poc/model/RecognizePetRequest.h
	include/alibabacloud/visionai-poc/model/RecognizePetResult.h )

set(visionai-poc_src 
	src/Visionai-pocClient.cc
	src/model/CheckMultiagentRequest.cc
	src/model/CheckMultiagentResult.cc
	src/model/PredictCategoryRequest.cc
	src/model/PredictCategoryResult.cc
	src/model/RecognizeCarRequest.cc
	src/model/RecognizeCarResult.cc
	src/model/RecognizeEntityRequest.cc
	src/model/RecognizeEntityResult.cc
	src/model/RecognizeFlowerRequest.cc
	src/model/RecognizeFlowerResult.cc
	src/model/RecognizeLabelRequest.cc
	src/model/RecognizeLabelResult.cc
	src/model/RecognizePetRequest.cc
	src/model/RecognizePetResult.cc )

add_library(visionai-poc ${LIB_TYPE}
	${visionai-poc_public_header}
	${visionai-poc_public_header_model}
	${visionai-poc_src})

set_target_properties(visionai-poc
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}visionai-poc
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(visionai-poc
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_VISIONAI_POC_LIBRARY)
endif()

target_include_directories(visionai-poc
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(visionai-poc
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(visionai-poc
		jsoncpp)
	target_include_directories(visionai-poc
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(visionai-poc
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(visionai-poc
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(visionai-poc
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(visionai-poc
		jsoncpp)
endif()

install(FILES ${visionai-poc_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/visionai-poc)
install(FILES ${visionai-poc_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/visionai-poc/model)
install(TARGETS visionai-poc
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)