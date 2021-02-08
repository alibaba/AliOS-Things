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

set(videoenhan_public_header 
	include/alibabacloud/videoenhan/VideoenhanClient.h
	include/alibabacloud/videoenhan/VideoenhanExport.h )

set(videoenhan_public_header_model 
	include/alibabacloud/videoenhan/model/AbstractEcommerceVideoRequest.h
	include/alibabacloud/videoenhan/model/AbstractEcommerceVideoResult.h
	include/alibabacloud/videoenhan/model/AbstractFilmVideoRequest.h
	include/alibabacloud/videoenhan/model/AbstractFilmVideoResult.h
	include/alibabacloud/videoenhan/model/AdjustVideoColorRequest.h
	include/alibabacloud/videoenhan/model/AdjustVideoColorResult.h
	include/alibabacloud/videoenhan/model/ChangeVideoSizeRequest.h
	include/alibabacloud/videoenhan/model/ChangeVideoSizeResult.h
	include/alibabacloud/videoenhan/model/EnhanceVideoQualityRequest.h
	include/alibabacloud/videoenhan/model/EnhanceVideoQualityResult.h
	include/alibabacloud/videoenhan/model/EraseVideoLogoRequest.h
	include/alibabacloud/videoenhan/model/EraseVideoLogoResult.h
	include/alibabacloud/videoenhan/model/EraseVideoSubtitlesRequest.h
	include/alibabacloud/videoenhan/model/EraseVideoSubtitlesResult.h
	include/alibabacloud/videoenhan/model/GenerateVideoRequest.h
	include/alibabacloud/videoenhan/model/GenerateVideoResult.h
	include/alibabacloud/videoenhan/model/GetAsyncJobResultRequest.h
	include/alibabacloud/videoenhan/model/GetAsyncJobResultResult.h
	include/alibabacloud/videoenhan/model/MergeVideoFaceRequest.h
	include/alibabacloud/videoenhan/model/MergeVideoFaceResult.h
	include/alibabacloud/videoenhan/model/SuperResolveVideoRequest.h
	include/alibabacloud/videoenhan/model/SuperResolveVideoResult.h )

set(videoenhan_src 
	src/VideoenhanClient.cc
	src/model/AbstractEcommerceVideoRequest.cc
	src/model/AbstractEcommerceVideoResult.cc
	src/model/AbstractFilmVideoRequest.cc
	src/model/AbstractFilmVideoResult.cc
	src/model/AdjustVideoColorRequest.cc
	src/model/AdjustVideoColorResult.cc
	src/model/ChangeVideoSizeRequest.cc
	src/model/ChangeVideoSizeResult.cc
	src/model/EnhanceVideoQualityRequest.cc
	src/model/EnhanceVideoQualityResult.cc
	src/model/EraseVideoLogoRequest.cc
	src/model/EraseVideoLogoResult.cc
	src/model/EraseVideoSubtitlesRequest.cc
	src/model/EraseVideoSubtitlesResult.cc
	src/model/GenerateVideoRequest.cc
	src/model/GenerateVideoResult.cc
	src/model/GetAsyncJobResultRequest.cc
	src/model/GetAsyncJobResultResult.cc
	src/model/MergeVideoFaceRequest.cc
	src/model/MergeVideoFaceResult.cc
	src/model/SuperResolveVideoRequest.cc
	src/model/SuperResolveVideoResult.cc )

add_library(videoenhan ${LIB_TYPE}
	${videoenhan_public_header}
	${videoenhan_public_header_model}
	${videoenhan_src})

set_target_properties(videoenhan
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}videoenhan
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(videoenhan
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_VIDEOENHAN_LIBRARY)
endif()

target_include_directories(videoenhan
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(videoenhan
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(videoenhan
		jsoncpp)
	target_include_directories(videoenhan
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(videoenhan
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(videoenhan
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(videoenhan
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(videoenhan
		jsoncpp)
endif()

install(FILES ${videoenhan_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/videoenhan)
install(FILES ${videoenhan_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/videoenhan/model)
install(TARGETS videoenhan
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)