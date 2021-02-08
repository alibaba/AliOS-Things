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

set(metering_public_header 
	include/alibabacloud/metering/MeteringClient.h
	include/alibabacloud/metering/MeteringExport.h )

set(metering_public_header_model 
	include/alibabacloud/metering/model/PostDataRequest.h
	include/alibabacloud/metering/model/PostDataResult.h
	include/alibabacloud/metering/model/SyncDataRequest.h
	include/alibabacloud/metering/model/SyncDataResult.h )

set(metering_src 
	src/MeteringClient.cc
	src/model/PostDataRequest.cc
	src/model/PostDataResult.cc
	src/model/SyncDataRequest.cc
	src/model/SyncDataResult.cc )

add_library(metering ${LIB_TYPE}
	${metering_public_header}
	${metering_public_header_model}
	${metering_src})

set_target_properties(metering
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}metering
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(metering
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_METERING_LIBRARY)
endif()

target_include_directories(metering
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(metering
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(metering
		jsoncpp)
	target_include_directories(metering
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(metering
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(metering
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(metering
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(metering
		jsoncpp)
endif()

install(FILES ${metering_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/metering)
install(FILES ${metering_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/metering/model)
install(TARGETS metering
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)