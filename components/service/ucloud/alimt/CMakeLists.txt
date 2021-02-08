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

set(alimt_public_header 
	include/alibabacloud/alimt/AlimtClient.h
	include/alibabacloud/alimt/AlimtExport.h )

set(alimt_public_header_model 
	include/alibabacloud/alimt/model/CreateDocTranslateTaskRequest.h
	include/alibabacloud/alimt/model/CreateDocTranslateTaskResult.h
	include/alibabacloud/alimt/model/CreateImageTranslateTaskRequest.h
	include/alibabacloud/alimt/model/CreateImageTranslateTaskResult.h
	include/alibabacloud/alimt/model/GetDetectLanguageRequest.h
	include/alibabacloud/alimt/model/GetDetectLanguageResult.h
	include/alibabacloud/alimt/model/GetDocTranslateTaskRequest.h
	include/alibabacloud/alimt/model/GetDocTranslateTaskResult.h
	include/alibabacloud/alimt/model/GetImageDiagnoseRequest.h
	include/alibabacloud/alimt/model/GetImageDiagnoseResult.h
	include/alibabacloud/alimt/model/GetImageTranslateRequest.h
	include/alibabacloud/alimt/model/GetImageTranslateResult.h
	include/alibabacloud/alimt/model/GetImageTranslateTaskRequest.h
	include/alibabacloud/alimt/model/GetImageTranslateTaskResult.h
	include/alibabacloud/alimt/model/GetTitleDiagnoseRequest.h
	include/alibabacloud/alimt/model/GetTitleDiagnoseResult.h
	include/alibabacloud/alimt/model/GetTitleGenerateRequest.h
	include/alibabacloud/alimt/model/GetTitleGenerateResult.h
	include/alibabacloud/alimt/model/GetTitleIntelligenceRequest.h
	include/alibabacloud/alimt/model/GetTitleIntelligenceResult.h
	include/alibabacloud/alimt/model/OpenAlimtServiceRequest.h
	include/alibabacloud/alimt/model/OpenAlimtServiceResult.h
	include/alibabacloud/alimt/model/TranslateRequest.h
	include/alibabacloud/alimt/model/TranslateResult.h
	include/alibabacloud/alimt/model/TranslateCertificateRequest.h
	include/alibabacloud/alimt/model/TranslateCertificateResult.h
	include/alibabacloud/alimt/model/TranslateECommerceRequest.h
	include/alibabacloud/alimt/model/TranslateECommerceResult.h
	include/alibabacloud/alimt/model/TranslateGeneralRequest.h
	include/alibabacloud/alimt/model/TranslateGeneralResult.h )

set(alimt_src 
	src/AlimtClient.cc
	src/model/CreateDocTranslateTaskRequest.cc
	src/model/CreateDocTranslateTaskResult.cc
	src/model/CreateImageTranslateTaskRequest.cc
	src/model/CreateImageTranslateTaskResult.cc
	src/model/GetDetectLanguageRequest.cc
	src/model/GetDetectLanguageResult.cc
	src/model/GetDocTranslateTaskRequest.cc
	src/model/GetDocTranslateTaskResult.cc
	src/model/GetImageDiagnoseRequest.cc
	src/model/GetImageDiagnoseResult.cc
	src/model/GetImageTranslateRequest.cc
	src/model/GetImageTranslateResult.cc
	src/model/GetImageTranslateTaskRequest.cc
	src/model/GetImageTranslateTaskResult.cc
	src/model/GetTitleDiagnoseRequest.cc
	src/model/GetTitleDiagnoseResult.cc
	src/model/GetTitleGenerateRequest.cc
	src/model/GetTitleGenerateResult.cc
	src/model/GetTitleIntelligenceRequest.cc
	src/model/GetTitleIntelligenceResult.cc
	src/model/OpenAlimtServiceRequest.cc
	src/model/OpenAlimtServiceResult.cc
	src/model/TranslateRequest.cc
	src/model/TranslateResult.cc
	src/model/TranslateCertificateRequest.cc
	src/model/TranslateCertificateResult.cc
	src/model/TranslateECommerceRequest.cc
	src/model/TranslateECommerceResult.cc
	src/model/TranslateGeneralRequest.cc
	src/model/TranslateGeneralResult.cc )

add_library(alimt ${LIB_TYPE}
	${alimt_public_header}
	${alimt_public_header_model}
	${alimt_src})

set_target_properties(alimt
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}alimt
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(alimt
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_ALIMT_LIBRARY)
endif()

target_include_directories(alimt
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(alimt
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(alimt
		jsoncpp)
	target_include_directories(alimt
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(alimt
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(alimt
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(alimt
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(alimt
		jsoncpp)
endif()

install(FILES ${alimt_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/alimt)
install(FILES ${alimt_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/alimt/model)
install(TARGETS alimt
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)