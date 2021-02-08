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

set(facebody_public_header 
	include/alibabacloud/facebody/FacebodyClient.h
	include/alibabacloud/facebody/FacebodyExport.h )

set(facebody_public_header_model 
	include/alibabacloud/facebody/model/AddFaceRequest.h
	include/alibabacloud/facebody/model/AddFaceResult.h
	include/alibabacloud/facebody/model/AddFaceEntityRequest.h
	include/alibabacloud/facebody/model/AddFaceEntityResult.h
	include/alibabacloud/facebody/model/BlurFaceRequest.h
	include/alibabacloud/facebody/model/BlurFaceResult.h
	include/alibabacloud/facebody/model/BodyPostureRequest.h
	include/alibabacloud/facebody/model/BodyPostureResult.h
	include/alibabacloud/facebody/model/CompareFaceRequest.h
	include/alibabacloud/facebody/model/CompareFaceResult.h
	include/alibabacloud/facebody/model/CountCrowdRequest.h
	include/alibabacloud/facebody/model/CountCrowdResult.h
	include/alibabacloud/facebody/model/CreateFaceDbRequest.h
	include/alibabacloud/facebody/model/CreateFaceDbResult.h
	include/alibabacloud/facebody/model/DeleteFaceRequest.h
	include/alibabacloud/facebody/model/DeleteFaceResult.h
	include/alibabacloud/facebody/model/DeleteFaceDbRequest.h
	include/alibabacloud/facebody/model/DeleteFaceDbResult.h
	include/alibabacloud/facebody/model/DeleteFaceEntityRequest.h
	include/alibabacloud/facebody/model/DeleteFaceEntityResult.h
	include/alibabacloud/facebody/model/DetectBodyCountRequest.h
	include/alibabacloud/facebody/model/DetectBodyCountResult.h
	include/alibabacloud/facebody/model/DetectCelebrityRequest.h
	include/alibabacloud/facebody/model/DetectCelebrityResult.h
	include/alibabacloud/facebody/model/DetectChefCapRequest.h
	include/alibabacloud/facebody/model/DetectChefCapResult.h
	include/alibabacloud/facebody/model/DetectFaceRequest.h
	include/alibabacloud/facebody/model/DetectFaceResult.h
	include/alibabacloud/facebody/model/DetectIPCPedestrianRequest.h
	include/alibabacloud/facebody/model/DetectIPCPedestrianResult.h
	include/alibabacloud/facebody/model/DetectLivingFaceRequest.h
	include/alibabacloud/facebody/model/DetectLivingFaceResult.h
	include/alibabacloud/facebody/model/DetectMaskRequest.h
	include/alibabacloud/facebody/model/DetectMaskResult.h
	include/alibabacloud/facebody/model/DetectPedestrianRequest.h
	include/alibabacloud/facebody/model/DetectPedestrianResult.h
	include/alibabacloud/facebody/model/DetectVideoLivingFaceRequest.h
	include/alibabacloud/facebody/model/DetectVideoLivingFaceResult.h
	include/alibabacloud/facebody/model/EnhanceFaceRequest.h
	include/alibabacloud/facebody/model/EnhanceFaceResult.h
	include/alibabacloud/facebody/model/ExtractPedestrianFeatureAttrRequest.h
	include/alibabacloud/facebody/model/ExtractPedestrianFeatureAttrResult.h
	include/alibabacloud/facebody/model/ExtractPedestrianFeatureAttributeRequest.h
	include/alibabacloud/facebody/model/ExtractPedestrianFeatureAttributeResult.h
	include/alibabacloud/facebody/model/FaceBeautyRequest.h
	include/alibabacloud/facebody/model/FaceBeautyResult.h
	include/alibabacloud/facebody/model/FaceFilterRequest.h
	include/alibabacloud/facebody/model/FaceFilterResult.h
	include/alibabacloud/facebody/model/FaceMakeupRequest.h
	include/alibabacloud/facebody/model/FaceMakeupResult.h
	include/alibabacloud/facebody/model/FaceTidyupRequest.h
	include/alibabacloud/facebody/model/FaceTidyupResult.h
	include/alibabacloud/facebody/model/GenerateHumanAnimeStyleRequest.h
	include/alibabacloud/facebody/model/GenerateHumanAnimeStyleResult.h
	include/alibabacloud/facebody/model/GetFaceEntityRequest.h
	include/alibabacloud/facebody/model/GetFaceEntityResult.h
	include/alibabacloud/facebody/model/HandPostureRequest.h
	include/alibabacloud/facebody/model/HandPostureResult.h
	include/alibabacloud/facebody/model/ListFaceDbsRequest.h
	include/alibabacloud/facebody/model/ListFaceDbsResult.h
	include/alibabacloud/facebody/model/ListFaceEntitiesRequest.h
	include/alibabacloud/facebody/model/ListFaceEntitiesResult.h
	include/alibabacloud/facebody/model/PedestrianDetectAttributeRequest.h
	include/alibabacloud/facebody/model/PedestrianDetectAttributeResult.h
	include/alibabacloud/facebody/model/RecognizeActionRequest.h
	include/alibabacloud/facebody/model/RecognizeActionResult.h
	include/alibabacloud/facebody/model/RecognizeExpressionRequest.h
	include/alibabacloud/facebody/model/RecognizeExpressionResult.h
	include/alibabacloud/facebody/model/RecognizeFaceRequest.h
	include/alibabacloud/facebody/model/RecognizeFaceResult.h
	include/alibabacloud/facebody/model/RecognizePublicFaceRequest.h
	include/alibabacloud/facebody/model/RecognizePublicFaceResult.h
	include/alibabacloud/facebody/model/SearchFaceRequest.h
	include/alibabacloud/facebody/model/SearchFaceResult.h
	include/alibabacloud/facebody/model/SwapFacialFeaturesRequest.h
	include/alibabacloud/facebody/model/SwapFacialFeaturesResult.h
	include/alibabacloud/facebody/model/UpdateFaceEntityRequest.h
	include/alibabacloud/facebody/model/UpdateFaceEntityResult.h
	include/alibabacloud/facebody/model/VerifyFaceMaskRequest.h
	include/alibabacloud/facebody/model/VerifyFaceMaskResult.h )

set(facebody_src 
	src/FacebodyClient.cc
	src/model/AddFaceRequest.cc
	src/model/AddFaceResult.cc
	src/model/AddFaceEntityRequest.cc
	src/model/AddFaceEntityResult.cc
	src/model/BlurFaceRequest.cc
	src/model/BlurFaceResult.cc
	src/model/BodyPostureRequest.cc
	src/model/BodyPostureResult.cc
	src/model/CompareFaceRequest.cc
	src/model/CompareFaceResult.cc
	src/model/CountCrowdRequest.cc
	src/model/CountCrowdResult.cc
	src/model/CreateFaceDbRequest.cc
	src/model/CreateFaceDbResult.cc
	src/model/DeleteFaceRequest.cc
	src/model/DeleteFaceResult.cc
	src/model/DeleteFaceDbRequest.cc
	src/model/DeleteFaceDbResult.cc
	src/model/DeleteFaceEntityRequest.cc
	src/model/DeleteFaceEntityResult.cc
	src/model/DetectBodyCountRequest.cc
	src/model/DetectBodyCountResult.cc
	src/model/DetectCelebrityRequest.cc
	src/model/DetectCelebrityResult.cc
	src/model/DetectChefCapRequest.cc
	src/model/DetectChefCapResult.cc
	src/model/DetectFaceRequest.cc
	src/model/DetectFaceResult.cc
	src/model/DetectIPCPedestrianRequest.cc
	src/model/DetectIPCPedestrianResult.cc
	src/model/DetectLivingFaceRequest.cc
	src/model/DetectLivingFaceResult.cc
	src/model/DetectMaskRequest.cc
	src/model/DetectMaskResult.cc
	src/model/DetectPedestrianRequest.cc
	src/model/DetectPedestrianResult.cc
	src/model/DetectVideoLivingFaceRequest.cc
	src/model/DetectVideoLivingFaceResult.cc
	src/model/EnhanceFaceRequest.cc
	src/model/EnhanceFaceResult.cc
	src/model/ExtractPedestrianFeatureAttrRequest.cc
	src/model/ExtractPedestrianFeatureAttrResult.cc
	src/model/ExtractPedestrianFeatureAttributeRequest.cc
	src/model/ExtractPedestrianFeatureAttributeResult.cc
	src/model/FaceBeautyRequest.cc
	src/model/FaceBeautyResult.cc
	src/model/FaceFilterRequest.cc
	src/model/FaceFilterResult.cc
	src/model/FaceMakeupRequest.cc
	src/model/FaceMakeupResult.cc
	src/model/FaceTidyupRequest.cc
	src/model/FaceTidyupResult.cc
	src/model/GenerateHumanAnimeStyleRequest.cc
	src/model/GenerateHumanAnimeStyleResult.cc
	src/model/GetFaceEntityRequest.cc
	src/model/GetFaceEntityResult.cc
	src/model/HandPostureRequest.cc
	src/model/HandPostureResult.cc
	src/model/ListFaceDbsRequest.cc
	src/model/ListFaceDbsResult.cc
	src/model/ListFaceEntitiesRequest.cc
	src/model/ListFaceEntitiesResult.cc
	src/model/PedestrianDetectAttributeRequest.cc
	src/model/PedestrianDetectAttributeResult.cc
	src/model/RecognizeActionRequest.cc
	src/model/RecognizeActionResult.cc
	src/model/RecognizeExpressionRequest.cc
	src/model/RecognizeExpressionResult.cc
	src/model/RecognizeFaceRequest.cc
	src/model/RecognizeFaceResult.cc
	src/model/RecognizePublicFaceRequest.cc
	src/model/RecognizePublicFaceResult.cc
	src/model/SearchFaceRequest.cc
	src/model/SearchFaceResult.cc
	src/model/SwapFacialFeaturesRequest.cc
	src/model/SwapFacialFeaturesResult.cc
	src/model/UpdateFaceEntityRequest.cc
	src/model/UpdateFaceEntityResult.cc
	src/model/VerifyFaceMaskRequest.cc
	src/model/VerifyFaceMaskResult.cc )

add_library(facebody ${LIB_TYPE}
	${facebody_public_header}
	${facebody_public_header_model}
	${facebody_src})

set_target_properties(facebody
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}facebody
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(facebody
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_FACEBODY_LIBRARY)
endif()

target_include_directories(facebody
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(facebody
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(facebody
		jsoncpp)
	target_include_directories(facebody
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(facebody
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(facebody
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(facebody
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(facebody
		jsoncpp)
endif()

install(FILES ${facebody_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/facebody)
install(FILES ${facebody_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/facebody/model)
install(TARGETS facebody
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)