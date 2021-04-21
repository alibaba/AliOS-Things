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

set(imageenhan_public_header 
	include/alibabacloud/imageenhan/ImageenhanClient.h
	include/alibabacloud/imageenhan/ImageenhanExport.h )

set(imageenhan_public_header_model 
	include/alibabacloud/imageenhan/model/AssessCompositionRequest.h
	include/alibabacloud/imageenhan/model/AssessCompositionResult.h
	include/alibabacloud/imageenhan/model/AssessExposureRequest.h
	include/alibabacloud/imageenhan/model/AssessExposureResult.h
	include/alibabacloud/imageenhan/model/AssessSharpnessRequest.h
	include/alibabacloud/imageenhan/model/AssessSharpnessResult.h
	include/alibabacloud/imageenhan/model/ChangeImageSizeRequest.h
	include/alibabacloud/imageenhan/model/ChangeImageSizeResult.h
	include/alibabacloud/imageenhan/model/ColorizeImageRequest.h
	include/alibabacloud/imageenhan/model/ColorizeImageResult.h
	include/alibabacloud/imageenhan/model/EnhanceImageColorRequest.h
	include/alibabacloud/imageenhan/model/EnhanceImageColorResult.h
	include/alibabacloud/imageenhan/model/ErasePersonRequest.h
	include/alibabacloud/imageenhan/model/ErasePersonResult.h
	include/alibabacloud/imageenhan/model/ExtendImageStyleRequest.h
	include/alibabacloud/imageenhan/model/ExtendImageStyleResult.h
	include/alibabacloud/imageenhan/model/GenerateDynamicImageRequest.h
	include/alibabacloud/imageenhan/model/GenerateDynamicImageResult.h
	include/alibabacloud/imageenhan/model/GetAsyncJobResultRequest.h
	include/alibabacloud/imageenhan/model/GetAsyncJobResultResult.h
	include/alibabacloud/imageenhan/model/ImageBlindCharacterWatermarkRequest.h
	include/alibabacloud/imageenhan/model/ImageBlindCharacterWatermarkResult.h
	include/alibabacloud/imageenhan/model/ImageBlindPicWatermarkRequest.h
	include/alibabacloud/imageenhan/model/ImageBlindPicWatermarkResult.h
	include/alibabacloud/imageenhan/model/ImitatePhotoStyleRequest.h
	include/alibabacloud/imageenhan/model/ImitatePhotoStyleResult.h
	include/alibabacloud/imageenhan/model/IntelligentCompositionRequest.h
	include/alibabacloud/imageenhan/model/IntelligentCompositionResult.h
	include/alibabacloud/imageenhan/model/MakeSuperResolutionImageRequest.h
	include/alibabacloud/imageenhan/model/MakeSuperResolutionImageResult.h
	include/alibabacloud/imageenhan/model/RecolorHDImageRequest.h
	include/alibabacloud/imageenhan/model/RecolorHDImageResult.h
	include/alibabacloud/imageenhan/model/RecolorImageRequest.h
	include/alibabacloud/imageenhan/model/RecolorImageResult.h
	include/alibabacloud/imageenhan/model/RemoveImageSubtitlesRequest.h
	include/alibabacloud/imageenhan/model/RemoveImageSubtitlesResult.h
	include/alibabacloud/imageenhan/model/RemoveImageWatermarkRequest.h
	include/alibabacloud/imageenhan/model/RemoveImageWatermarkResult.h )

set(imageenhan_src 
	src/ImageenhanClient.cc
	src/model/AssessCompositionRequest.cc
	src/model/AssessCompositionResult.cc
	src/model/AssessExposureRequest.cc
	src/model/AssessExposureResult.cc
	src/model/AssessSharpnessRequest.cc
	src/model/AssessSharpnessResult.cc
	src/model/ChangeImageSizeRequest.cc
	src/model/ChangeImageSizeResult.cc
	src/model/ColorizeImageRequest.cc
	src/model/ColorizeImageResult.cc
	src/model/EnhanceImageColorRequest.cc
	src/model/EnhanceImageColorResult.cc
	src/model/ErasePersonRequest.cc
	src/model/ErasePersonResult.cc
	src/model/ExtendImageStyleRequest.cc
	src/model/ExtendImageStyleResult.cc
	src/model/GenerateDynamicImageRequest.cc
	src/model/GenerateDynamicImageResult.cc
	src/model/GetAsyncJobResultRequest.cc
	src/model/GetAsyncJobResultResult.cc
	src/model/ImageBlindCharacterWatermarkRequest.cc
	src/model/ImageBlindCharacterWatermarkResult.cc
	src/model/ImageBlindPicWatermarkRequest.cc
	src/model/ImageBlindPicWatermarkResult.cc
	src/model/ImitatePhotoStyleRequest.cc
	src/model/ImitatePhotoStyleResult.cc
	src/model/IntelligentCompositionRequest.cc
	src/model/IntelligentCompositionResult.cc
	src/model/MakeSuperResolutionImageRequest.cc
	src/model/MakeSuperResolutionImageResult.cc
	src/model/RecolorHDImageRequest.cc
	src/model/RecolorHDImageResult.cc
	src/model/RecolorImageRequest.cc
	src/model/RecolorImageResult.cc
	src/model/RemoveImageSubtitlesRequest.cc
	src/model/RemoveImageSubtitlesResult.cc
	src/model/RemoveImageWatermarkRequest.cc
	src/model/RemoveImageWatermarkResult.cc )

add_library(imageenhan ${LIB_TYPE}
	${imageenhan_public_header}
	${imageenhan_public_header_model}
	${imageenhan_src})

set_target_properties(imageenhan
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}imageenhan
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(imageenhan
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_IMAGEENHAN_LIBRARY)
endif()

target_include_directories(imageenhan
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(imageenhan
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(imageenhan
		jsoncpp)
	target_include_directories(imageenhan
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(imageenhan
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(imageenhan
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(imageenhan
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(imageenhan
		jsoncpp)
endif()

install(FILES ${imageenhan_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imageenhan)
install(FILES ${imageenhan_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imageenhan/model)
install(TARGETS imageenhan
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)