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

set(imageseg_public_header 
	include/alibabacloud/imageseg/ImagesegClient.h
	include/alibabacloud/imageseg/ImagesegExport.h )

set(imageseg_public_header_model 
	include/alibabacloud/imageseg/model/ChangeSkyRequest.h
	include/alibabacloud/imageseg/model/ChangeSkyResult.h
	include/alibabacloud/imageseg/model/GetAsyncJobResultRequest.h
	include/alibabacloud/imageseg/model/GetAsyncJobResultResult.h
	include/alibabacloud/imageseg/model/ParseFaceRequest.h
	include/alibabacloud/imageseg/model/ParseFaceResult.h
	include/alibabacloud/imageseg/model/RefineMaskRequest.h
	include/alibabacloud/imageseg/model/RefineMaskResult.h
	include/alibabacloud/imageseg/model/SegmentAnimalRequest.h
	include/alibabacloud/imageseg/model/SegmentAnimalResult.h
	include/alibabacloud/imageseg/model/SegmentBodyRequest.h
	include/alibabacloud/imageseg/model/SegmentBodyResult.h
	include/alibabacloud/imageseg/model/SegmentClothRequest.h
	include/alibabacloud/imageseg/model/SegmentClothResult.h
	include/alibabacloud/imageseg/model/SegmentCommodityRequest.h
	include/alibabacloud/imageseg/model/SegmentCommodityResult.h
	include/alibabacloud/imageseg/model/SegmentCommonImageRequest.h
	include/alibabacloud/imageseg/model/SegmentCommonImageResult.h
	include/alibabacloud/imageseg/model/SegmentFaceRequest.h
	include/alibabacloud/imageseg/model/SegmentFaceResult.h
	include/alibabacloud/imageseg/model/SegmentFoodRequest.h
	include/alibabacloud/imageseg/model/SegmentFoodResult.h
	include/alibabacloud/imageseg/model/SegmentFurnitureRequest.h
	include/alibabacloud/imageseg/model/SegmentFurnitureResult.h
	include/alibabacloud/imageseg/model/SegmentHDBodyRequest.h
	include/alibabacloud/imageseg/model/SegmentHDBodyResult.h
	include/alibabacloud/imageseg/model/SegmentHDCommonImageRequest.h
	include/alibabacloud/imageseg/model/SegmentHDCommonImageResult.h
	include/alibabacloud/imageseg/model/SegmentHDSkyRequest.h
	include/alibabacloud/imageseg/model/SegmentHDSkyResult.h
	include/alibabacloud/imageseg/model/SegmentHairRequest.h
	include/alibabacloud/imageseg/model/SegmentHairResult.h
	include/alibabacloud/imageseg/model/SegmentHeadRequest.h
	include/alibabacloud/imageseg/model/SegmentHeadResult.h
	include/alibabacloud/imageseg/model/SegmentLogoRequest.h
	include/alibabacloud/imageseg/model/SegmentLogoResult.h
	include/alibabacloud/imageseg/model/SegmentSceneRequest.h
	include/alibabacloud/imageseg/model/SegmentSceneResult.h
	include/alibabacloud/imageseg/model/SegmentSkinRequest.h
	include/alibabacloud/imageseg/model/SegmentSkinResult.h
	include/alibabacloud/imageseg/model/SegmentSkyRequest.h
	include/alibabacloud/imageseg/model/SegmentSkyResult.h
	include/alibabacloud/imageseg/model/SegmentVehicleRequest.h
	include/alibabacloud/imageseg/model/SegmentVehicleResult.h )

set(imageseg_src 
	src/ImagesegClient.cc
	src/model/ChangeSkyRequest.cc
	src/model/ChangeSkyResult.cc
	src/model/GetAsyncJobResultRequest.cc
	src/model/GetAsyncJobResultResult.cc
	src/model/ParseFaceRequest.cc
	src/model/ParseFaceResult.cc
	src/model/RefineMaskRequest.cc
	src/model/RefineMaskResult.cc
	src/model/SegmentAnimalRequest.cc
	src/model/SegmentAnimalResult.cc
	src/model/SegmentBodyRequest.cc
	src/model/SegmentBodyResult.cc
	src/model/SegmentClothRequest.cc
	src/model/SegmentClothResult.cc
	src/model/SegmentCommodityRequest.cc
	src/model/SegmentCommodityResult.cc
	src/model/SegmentCommonImageRequest.cc
	src/model/SegmentCommonImageResult.cc
	src/model/SegmentFaceRequest.cc
	src/model/SegmentFaceResult.cc
	src/model/SegmentFoodRequest.cc
	src/model/SegmentFoodResult.cc
	src/model/SegmentFurnitureRequest.cc
	src/model/SegmentFurnitureResult.cc
	src/model/SegmentHDBodyRequest.cc
	src/model/SegmentHDBodyResult.cc
	src/model/SegmentHDCommonImageRequest.cc
	src/model/SegmentHDCommonImageResult.cc
	src/model/SegmentHDSkyRequest.cc
	src/model/SegmentHDSkyResult.cc
	src/model/SegmentHairRequest.cc
	src/model/SegmentHairResult.cc
	src/model/SegmentHeadRequest.cc
	src/model/SegmentHeadResult.cc
	src/model/SegmentLogoRequest.cc
	src/model/SegmentLogoResult.cc
	src/model/SegmentSceneRequest.cc
	src/model/SegmentSceneResult.cc
	src/model/SegmentSkinRequest.cc
	src/model/SegmentSkinResult.cc
	src/model/SegmentSkyRequest.cc
	src/model/SegmentSkyResult.cc
	src/model/SegmentVehicleRequest.cc
	src/model/SegmentVehicleResult.cc )

add_library(imageseg ${LIB_TYPE}
	${imageseg_public_header}
	${imageseg_public_header_model}
	${imageseg_src})

set_target_properties(imageseg
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}imageseg
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(imageseg
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_IMAGESEG_LIBRARY)
endif()

target_include_directories(imageseg
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(imageseg
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(imageseg
		jsoncpp)
	target_include_directories(imageseg
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(imageseg
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(imageseg
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(imageseg
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(imageseg
		jsoncpp)
endif()

install(FILES ${imageseg_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imageseg)
install(FILES ${imageseg_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imageseg/model)
install(TARGETS imageseg
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)