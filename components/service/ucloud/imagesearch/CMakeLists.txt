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

set(imagesearch_public_header 
	include/alibabacloud/imagesearch/ImageSearchClient.h
	include/alibabacloud/imagesearch/ImageSearchExport.h )

set(imagesearch_public_header_model 
	include/alibabacloud/imagesearch/model/AddItemRequest.h
	include/alibabacloud/imagesearch/model/AddItemResult.h
	include/alibabacloud/imagesearch/model/DeleteItemRequest.h
	include/alibabacloud/imagesearch/model/DeleteItemResult.h
	include/alibabacloud/imagesearch/model/SearchItemRequest.h
	include/alibabacloud/imagesearch/model/SearchItemResult.h )

set(imagesearch_src 
	src/ImageSearchClient.cc
	src/model/AddItemRequest.cc
	src/model/AddItemResult.cc
	src/model/DeleteItemRequest.cc
	src/model/DeleteItemResult.cc
	src/model/SearchItemRequest.cc
	src/model/SearchItemResult.cc )

add_library(imagesearch ${LIB_TYPE}
	${imagesearch_public_header}
	${imagesearch_public_header_model}
	${imagesearch_src})

set_target_properties(imagesearch
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}imagesearch
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(imagesearch
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_IMAGESEARCH_LIBRARY)
endif()

target_include_directories(imagesearch
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(imagesearch
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(imagesearch
		jsoncpp)
	target_include_directories(imagesearch
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(imagesearch
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(imagesearch
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(imagesearch
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(imagesearch
		jsoncpp)
endif()

install(FILES ${imagesearch_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imagesearch)
install(FILES ${imagesearch_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imagesearch/model)
install(TARGETS imagesearch
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)