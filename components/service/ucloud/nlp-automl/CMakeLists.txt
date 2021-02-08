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

set(nlp-automl_public_header 
	include/alibabacloud/nlp-automl/Nlp_automlClient.h
	include/alibabacloud/nlp-automl/Nlp_automlExport.h )

set(nlp-automl_public_header_model 
	include/alibabacloud/nlp-automl/model/CreateAsyncPredictRequest.h
	include/alibabacloud/nlp-automl/model/CreateAsyncPredictResult.h
	include/alibabacloud/nlp-automl/model/GetAsyncPredictRequest.h
	include/alibabacloud/nlp-automl/model/GetAsyncPredictResult.h
	include/alibabacloud/nlp-automl/model/GetPredictResultRequest.h
	include/alibabacloud/nlp-automl/model/GetPredictResultResult.h
	include/alibabacloud/nlp-automl/model/RunContactReviewRequest.h
	include/alibabacloud/nlp-automl/model/RunContactReviewResult.h
	include/alibabacloud/nlp-automl/model/RunPreTrainServiceRequest.h
	include/alibabacloud/nlp-automl/model/RunPreTrainServiceResult.h
	include/alibabacloud/nlp-automl/model/RunSmartCallServiceRequest.h
	include/alibabacloud/nlp-automl/model/RunSmartCallServiceResult.h )

set(nlp-automl_src 
	src/Nlp-automlClient.cc
	src/model/CreateAsyncPredictRequest.cc
	src/model/CreateAsyncPredictResult.cc
	src/model/GetAsyncPredictRequest.cc
	src/model/GetAsyncPredictResult.cc
	src/model/GetPredictResultRequest.cc
	src/model/GetPredictResultResult.cc
	src/model/RunContactReviewRequest.cc
	src/model/RunContactReviewResult.cc
	src/model/RunPreTrainServiceRequest.cc
	src/model/RunPreTrainServiceResult.cc
	src/model/RunSmartCallServiceRequest.cc
	src/model/RunSmartCallServiceResult.cc )

add_library(nlp-automl ${LIB_TYPE}
	${nlp-automl_public_header}
	${nlp-automl_public_header_model}
	${nlp-automl_src})

set_target_properties(nlp-automl
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}nlp-automl
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(nlp-automl
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_NLP_AUTOML_LIBRARY)
endif()

target_include_directories(nlp-automl
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(nlp-automl
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(nlp-automl
		jsoncpp)
	target_include_directories(nlp-automl
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(nlp-automl
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(nlp-automl
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(nlp-automl
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(nlp-automl
		jsoncpp)
endif()

install(FILES ${nlp-automl_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/nlp-automl)
install(FILES ${nlp-automl_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/nlp-automl/model)
install(TARGETS nlp-automl
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)