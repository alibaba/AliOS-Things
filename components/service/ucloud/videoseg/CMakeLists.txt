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

set(videoseg_public_header 
	include/alibabacloud/videoseg/VideosegClient.h
	include/alibabacloud/videoseg/VideosegExport.h )

set(videoseg_public_header_model 
	include/alibabacloud/videoseg/model/GetAsyncJobResultRequest.h
	include/alibabacloud/videoseg/model/GetAsyncJobResultResult.h
	include/alibabacloud/videoseg/model/SegmentHalfBodyRequest.h
	include/alibabacloud/videoseg/model/SegmentHalfBodyResult.h
	include/alibabacloud/videoseg/model/SegmentVideoBodyRequest.h
	include/alibabacloud/videoseg/model/SegmentVideoBodyResult.h )

set(videoseg_src 
	src/VideosegClient.cc
	src/model/GetAsyncJobResultRequest.cc
	src/model/GetAsyncJobResultResult.cc
	src/model/SegmentHalfBodyRequest.cc
	src/model/SegmentHalfBodyResult.cc
	src/model/SegmentVideoBodyRequest.cc
	src/model/SegmentVideoBodyResult.cc )

add_library(videoseg ${LIB_TYPE}
	${videoseg_public_header}
	${videoseg_public_header_model}
	${videoseg_src})

set_target_properties(videoseg
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}videoseg
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(videoseg
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_VIDEOSEG_LIBRARY)
endif()

target_include_directories(videoseg
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(videoseg
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(videoseg
		jsoncpp)
	target_include_directories(videoseg
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(videoseg
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(videoseg
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(videoseg
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(videoseg
		jsoncpp)
endif()

install(FILES ${videoseg_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/videoseg)
install(FILES ${videoseg_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/videoseg/model)
install(TARGETS videoseg
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)