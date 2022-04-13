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

set(imagerecog_public_header 
	include/alibabacloud/imagerecog/ImagerecogClient.h
	include/alibabacloud/imagerecog/ImagerecogExport.h )

set(imagerecog_public_header_model 
	include/alibabacloud/imagerecog/model/ClassifyingRubbishRequest.h
	include/alibabacloud/imagerecog/model/ClassifyingRubbishResult.h
	include/alibabacloud/imagerecog/model/DetectFruitsRequest.h
	include/alibabacloud/imagerecog/model/DetectFruitsResult.h
	include/alibabacloud/imagerecog/model/DetectImageElementsRequest.h
	include/alibabacloud/imagerecog/model/DetectImageElementsResult.h
	include/alibabacloud/imagerecog/model/RecognizeImageColorRequest.h
	include/alibabacloud/imagerecog/model/RecognizeImageColorResult.h
	include/alibabacloud/imagerecog/model/RecognizeImageStyleRequest.h
	include/alibabacloud/imagerecog/model/RecognizeImageStyleResult.h
	include/alibabacloud/imagerecog/model/RecognizeLogoRequest.h
	include/alibabacloud/imagerecog/model/RecognizeLogoResult.h
	include/alibabacloud/imagerecog/model/RecognizeSceneRequest.h
	include/alibabacloud/imagerecog/model/RecognizeSceneResult.h
	include/alibabacloud/imagerecog/model/RecognizeVehicleTypeRequest.h
	include/alibabacloud/imagerecog/model/RecognizeVehicleTypeResult.h
	include/alibabacloud/imagerecog/model/TaggingImageRequest.h
	include/alibabacloud/imagerecog/model/TaggingImageResult.h )

set(imagerecog_src 
	src/ImagerecogClient.cc
	src/model/ClassifyingRubbishRequest.cc
	src/model/ClassifyingRubbishResult.cc
	src/model/DetectFruitsRequest.cc
	src/model/DetectFruitsResult.cc
	src/model/DetectImageElementsRequest.cc
	src/model/DetectImageElementsResult.cc
	src/model/RecognizeImageColorRequest.cc
	src/model/RecognizeImageColorResult.cc
	src/model/RecognizeImageStyleRequest.cc
	src/model/RecognizeImageStyleResult.cc
	src/model/RecognizeLogoRequest.cc
	src/model/RecognizeLogoResult.cc
	src/model/RecognizeSceneRequest.cc
	src/model/RecognizeSceneResult.cc
	src/model/RecognizeVehicleTypeRequest.cc
	src/model/RecognizeVehicleTypeResult.cc
	src/model/TaggingImageRequest.cc
	src/model/TaggingImageResult.cc )

add_library(imagerecog ${LIB_TYPE}
	${imagerecog_public_header}
	${imagerecog_public_header_model}
	${imagerecog_src})

set_target_properties(imagerecog
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}imagerecog
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(imagerecog
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_IMAGERECOG_LIBRARY)
endif()

target_include_directories(imagerecog
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(imagerecog
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(imagerecog
		jsoncpp)
	target_include_directories(imagerecog
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(imagerecog
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(imagerecog
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(imagerecog
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(imagerecog
		jsoncpp)
endif()

install(FILES ${imagerecog_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imagerecog)
install(FILES ${imagerecog_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imagerecog/model)
install(TARGETS imagerecog
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)