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

set(imageaudit_public_header 
	include/alibabacloud/imageaudit/ImageauditClient.h
	include/alibabacloud/imageaudit/ImageauditExport.h )

set(imageaudit_public_header_model 
	include/alibabacloud/imageaudit/model/ScanImageRequest.h
	include/alibabacloud/imageaudit/model/ScanImageResult.h
	include/alibabacloud/imageaudit/model/ScanTextRequest.h
	include/alibabacloud/imageaudit/model/ScanTextResult.h )

set(imageaudit_src 
	src/ImageauditClient.cc
	src/model/ScanImageRequest.cc
	src/model/ScanImageResult.cc
	src/model/ScanTextRequest.cc
	src/model/ScanTextResult.cc )

add_library(imageaudit ${LIB_TYPE}
	${imageaudit_public_header}
	${imageaudit_public_header_model}
	${imageaudit_src})

set_target_properties(imageaudit
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}imageaudit
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(imageaudit
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_IMAGEAUDIT_LIBRARY)
endif()

target_include_directories(imageaudit
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(imageaudit
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(imageaudit
		jsoncpp)
	target_include_directories(imageaudit
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(imageaudit
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(imageaudit
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(imageaudit
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(imageaudit
		jsoncpp)
endif()

install(FILES ${imageaudit_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imageaudit)
install(FILES ${imageaudit_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imageaudit/model)
install(TARGETS imageaudit
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)