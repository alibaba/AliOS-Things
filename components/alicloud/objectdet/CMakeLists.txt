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

set(objectdet_public_header 
	include/alibabacloud/objectdet/ObjectdetClient.h
	include/alibabacloud/objectdet/ObjectdetExport.h )

set(objectdet_public_header_model 
	include/alibabacloud/objectdet/model/ClassifyVehicleInsuranceRequest.h
	include/alibabacloud/objectdet/model/ClassifyVehicleInsuranceResult.h
	include/alibabacloud/objectdet/model/DetectMainBodyRequest.h
	include/alibabacloud/objectdet/model/DetectMainBodyResult.h
	include/alibabacloud/objectdet/model/DetectObjectRequest.h
	include/alibabacloud/objectdet/model/DetectObjectResult.h
	include/alibabacloud/objectdet/model/DetectTransparentImageRequest.h
	include/alibabacloud/objectdet/model/DetectTransparentImageResult.h
	include/alibabacloud/objectdet/model/DetectVehicleRequest.h
	include/alibabacloud/objectdet/model/DetectVehicleResult.h
	include/alibabacloud/objectdet/model/DetectWhiteBaseImageRequest.h
	include/alibabacloud/objectdet/model/DetectWhiteBaseImageResult.h
	include/alibabacloud/objectdet/model/GenerateVehicleRepairPlanRequest.h
	include/alibabacloud/objectdet/model/GenerateVehicleRepairPlanResult.h
	include/alibabacloud/objectdet/model/GetVehicleRepairPlanRequest.h
	include/alibabacloud/objectdet/model/GetVehicleRepairPlanResult.h
	include/alibabacloud/objectdet/model/RecognizeVehicleDamageRequest.h
	include/alibabacloud/objectdet/model/RecognizeVehicleDamageResult.h
	include/alibabacloud/objectdet/model/RecognizeVehicleDashboardRequest.h
	include/alibabacloud/objectdet/model/RecognizeVehicleDashboardResult.h
	include/alibabacloud/objectdet/model/RecognizeVehiclePartsRequest.h
	include/alibabacloud/objectdet/model/RecognizeVehiclePartsResult.h )

set(objectdet_src 
	src/ObjectdetClient.cc
	src/model/ClassifyVehicleInsuranceRequest.cc
	src/model/ClassifyVehicleInsuranceResult.cc
	src/model/DetectMainBodyRequest.cc
	src/model/DetectMainBodyResult.cc
	src/model/DetectObjectRequest.cc
	src/model/DetectObjectResult.cc
	src/model/DetectTransparentImageRequest.cc
	src/model/DetectTransparentImageResult.cc
	src/model/DetectVehicleRequest.cc
	src/model/DetectVehicleResult.cc
	src/model/DetectWhiteBaseImageRequest.cc
	src/model/DetectWhiteBaseImageResult.cc
	src/model/GenerateVehicleRepairPlanRequest.cc
	src/model/GenerateVehicleRepairPlanResult.cc
	src/model/GetVehicleRepairPlanRequest.cc
	src/model/GetVehicleRepairPlanResult.cc
	src/model/RecognizeVehicleDamageRequest.cc
	src/model/RecognizeVehicleDamageResult.cc
	src/model/RecognizeVehicleDashboardRequest.cc
	src/model/RecognizeVehicleDashboardResult.cc
	src/model/RecognizeVehiclePartsRequest.cc
	src/model/RecognizeVehiclePartsResult.cc )

add_library(objectdet ${LIB_TYPE}
	${objectdet_public_header}
	${objectdet_public_header_model}
	${objectdet_src})

set_target_properties(objectdet
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}objectdet
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(objectdet
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_OBJECTDET_LIBRARY)
endif()

target_include_directories(objectdet
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(objectdet
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(objectdet
		jsoncpp)
	target_include_directories(objectdet
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(objectdet
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(objectdet
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(objectdet
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(objectdet
		jsoncpp)
endif()

install(FILES ${objectdet_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/objectdet)
install(FILES ${objectdet_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/objectdet/model)
install(TARGETS objectdet
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)