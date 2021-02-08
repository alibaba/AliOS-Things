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

set(alinlp_public_header 
	include/alibabacloud/alinlp/AlinlpClient.h
	include/alibabacloud/alinlp/AlinlpExport.h )

set(alinlp_public_header_model 
	include/alibabacloud/alinlp/model/GetDpChEcomRequest.h
	include/alibabacloud/alinlp/model/GetDpChEcomResult.h
	include/alibabacloud/alinlp/model/GetEcChGeneralRequest.h
	include/alibabacloud/alinlp/model/GetEcChGeneralResult.h
	include/alibabacloud/alinlp/model/GetNerChMedicalRequest.h
	include/alibabacloud/alinlp/model/GetNerChMedicalResult.h
	include/alibabacloud/alinlp/model/GetNerCustomizedChEcomRequest.h
	include/alibabacloud/alinlp/model/GetNerCustomizedChEcomResult.h
	include/alibabacloud/alinlp/model/GetPosChEcomRequest.h
	include/alibabacloud/alinlp/model/GetPosChEcomResult.h
	include/alibabacloud/alinlp/model/GetSummaryChEcomRequest.h
	include/alibabacloud/alinlp/model/GetSummaryChEcomResult.h
	include/alibabacloud/alinlp/model/GetTsChEcomRequest.h
	include/alibabacloud/alinlp/model/GetTsChEcomResult.h
	include/alibabacloud/alinlp/model/GetWeChEcomRequest.h
	include/alibabacloud/alinlp/model/GetWeChEcomResult.h
	include/alibabacloud/alinlp/model/GetWsCustomizedChGeneralRequest.h
	include/alibabacloud/alinlp/model/GetWsCustomizedChGeneralResult.h
	include/alibabacloud/alinlp/model/GetWsCustomizedSeaGeneralRequest.h
	include/alibabacloud/alinlp/model/GetWsCustomizedSeaGeneralResult.h )

set(alinlp_src 
	src/AlinlpClient.cc
	src/model/GetDpChEcomRequest.cc
	src/model/GetDpChEcomResult.cc
	src/model/GetEcChGeneralRequest.cc
	src/model/GetEcChGeneralResult.cc
	src/model/GetNerChMedicalRequest.cc
	src/model/GetNerChMedicalResult.cc
	src/model/GetNerCustomizedChEcomRequest.cc
	src/model/GetNerCustomizedChEcomResult.cc
	src/model/GetPosChEcomRequest.cc
	src/model/GetPosChEcomResult.cc
	src/model/GetSummaryChEcomRequest.cc
	src/model/GetSummaryChEcomResult.cc
	src/model/GetTsChEcomRequest.cc
	src/model/GetTsChEcomResult.cc
	src/model/GetWeChEcomRequest.cc
	src/model/GetWeChEcomResult.cc
	src/model/GetWsCustomizedChGeneralRequest.cc
	src/model/GetWsCustomizedChGeneralResult.cc
	src/model/GetWsCustomizedSeaGeneralRequest.cc
	src/model/GetWsCustomizedSeaGeneralResult.cc )

add_library(alinlp ${LIB_TYPE}
	${alinlp_public_header}
	${alinlp_public_header_model}
	${alinlp_src})

set_target_properties(alinlp
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}alinlp
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(alinlp
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_ALINLP_LIBRARY)
endif()

target_include_directories(alinlp
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(alinlp
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(alinlp
		jsoncpp)
	target_include_directories(alinlp
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(alinlp
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(alinlp
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(alinlp
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(alinlp
		jsoncpp)
endif()

install(FILES ${alinlp_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/alinlp)
install(FILES ${alinlp_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/alinlp/model)
install(TARGETS alinlp
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)