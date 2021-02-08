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

set(videosearch_public_header 
	include/alibabacloud/videosearch/VideosearchClient.h
	include/alibabacloud/videosearch/VideosearchExport.h )

set(videosearch_public_header_model 
	include/alibabacloud/videosearch/model/AddDeletionVideoTaskRequest.h
	include/alibabacloud/videosearch/model/AddDeletionVideoTaskResult.h
	include/alibabacloud/videosearch/model/AddSearchVideoTaskRequest.h
	include/alibabacloud/videosearch/model/AddSearchVideoTaskResult.h
	include/alibabacloud/videosearch/model/AddStorageVideoTaskRequest.h
	include/alibabacloud/videosearch/model/AddStorageVideoTaskResult.h
	include/alibabacloud/videosearch/model/GetTaskStatusRequest.h
	include/alibabacloud/videosearch/model/GetTaskStatusResult.h )

set(videosearch_src 
	src/VideosearchClient.cc
	src/model/AddDeletionVideoTaskRequest.cc
	src/model/AddDeletionVideoTaskResult.cc
	src/model/AddSearchVideoTaskRequest.cc
	src/model/AddSearchVideoTaskResult.cc
	src/model/AddStorageVideoTaskRequest.cc
	src/model/AddStorageVideoTaskResult.cc
	src/model/GetTaskStatusRequest.cc
	src/model/GetTaskStatusResult.cc )

add_library(videosearch ${LIB_TYPE}
	${videosearch_public_header}
	${videosearch_public_header_model}
	${videosearch_src})

set_target_properties(videosearch
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}videosearch
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(videosearch
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_VIDEOSEARCH_LIBRARY)
endif()

target_include_directories(videosearch
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(videosearch
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(videosearch
		jsoncpp)
	target_include_directories(videosearch
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(videosearch
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(videosearch
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(videosearch
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(videosearch
		jsoncpp)
endif()

install(FILES ${videosearch_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/videosearch)
install(FILES ${videosearch_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/videosearch/model)
install(TARGETS videosearch
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)