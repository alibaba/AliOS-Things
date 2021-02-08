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

set(videorecog_public_header 
	include/alibabacloud/videorecog/VideorecogClient.h
	include/alibabacloud/videorecog/VideorecogExport.h )

set(videorecog_public_header_model 
	include/alibabacloud/videorecog/model/DetectVideoShotRequest.h
	include/alibabacloud/videorecog/model/DetectVideoShotResult.h
	include/alibabacloud/videorecog/model/GenerateVideoCoverRequest.h
	include/alibabacloud/videorecog/model/GenerateVideoCoverResult.h
	include/alibabacloud/videorecog/model/GetAsyncJobResultRequest.h
	include/alibabacloud/videorecog/model/GetAsyncJobResultResult.h )

set(videorecog_src 
	src/VideorecogClient.cc
	src/model/DetectVideoShotRequest.cc
	src/model/DetectVideoShotResult.cc
	src/model/GenerateVideoCoverRequest.cc
	src/model/GenerateVideoCoverResult.cc
	src/model/GetAsyncJobResultRequest.cc
	src/model/GetAsyncJobResultResult.cc )

add_library(videorecog ${LIB_TYPE}
	${videorecog_public_header}
	${videorecog_public_header_model}
	${videorecog_src})

set_target_properties(videorecog
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}videorecog
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(videorecog
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_VIDEORECOG_LIBRARY)
endif()

target_include_directories(videorecog
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(videorecog
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(videorecog
		jsoncpp)
	target_include_directories(videorecog
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(videorecog
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(videorecog
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(videorecog
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(videorecog
		jsoncpp)
endif()

install(FILES ${videorecog_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/videorecog)
install(FILES ${videorecog_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/videorecog/model)
install(TARGETS videorecog
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)