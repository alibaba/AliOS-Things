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

set(imgsearch_public_header 
	include/alibabacloud/imgsearch/ImgsearchClient.h
	include/alibabacloud/imgsearch/ImgsearchExport.h )

set(imgsearch_public_header_model 
	include/alibabacloud/imgsearch/model/AddImageRequest.h
	include/alibabacloud/imgsearch/model/AddImageResult.h
	include/alibabacloud/imgsearch/model/CreateImageDbRequest.h
	include/alibabacloud/imgsearch/model/CreateImageDbResult.h
	include/alibabacloud/imgsearch/model/DeleteImageRequest.h
	include/alibabacloud/imgsearch/model/DeleteImageResult.h
	include/alibabacloud/imgsearch/model/DeleteImageDbRequest.h
	include/alibabacloud/imgsearch/model/DeleteImageDbResult.h
	include/alibabacloud/imgsearch/model/ListImageDbsRequest.h
	include/alibabacloud/imgsearch/model/ListImageDbsResult.h
	include/alibabacloud/imgsearch/model/ListImagesRequest.h
	include/alibabacloud/imgsearch/model/ListImagesResult.h
	include/alibabacloud/imgsearch/model/SearchImageRequest.h
	include/alibabacloud/imgsearch/model/SearchImageResult.h )

set(imgsearch_src 
	src/ImgsearchClient.cc
	src/model/AddImageRequest.cc
	src/model/AddImageResult.cc
	src/model/CreateImageDbRequest.cc
	src/model/CreateImageDbResult.cc
	src/model/DeleteImageRequest.cc
	src/model/DeleteImageResult.cc
	src/model/DeleteImageDbRequest.cc
	src/model/DeleteImageDbResult.cc
	src/model/ListImageDbsRequest.cc
	src/model/ListImageDbsResult.cc
	src/model/ListImagesRequest.cc
	src/model/ListImagesResult.cc
	src/model/SearchImageRequest.cc
	src/model/SearchImageResult.cc )

add_library(imgsearch ${LIB_TYPE}
	${imgsearch_public_header}
	${imgsearch_public_header_model}
	${imgsearch_src})

set_target_properties(imgsearch
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}imgsearch
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(imgsearch
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_IMGSEARCH_LIBRARY)
endif()

target_include_directories(imgsearch
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(imgsearch
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(imgsearch
		jsoncpp)
	target_include_directories(imgsearch
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(imgsearch
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(imgsearch
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(imgsearch
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(imgsearch
		jsoncpp)
endif()

install(FILES ${imgsearch_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imgsearch)
install(FILES ${imgsearch_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imgsearch/model)
install(TARGETS imgsearch
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)