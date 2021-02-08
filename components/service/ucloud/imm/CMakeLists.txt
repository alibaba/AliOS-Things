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

set(imm_public_header 
	include/alibabacloud/imm/ImmClient.h
	include/alibabacloud/imm/ImmExport.h )

set(imm_public_header_model 
	include/alibabacloud/imm/model/CompareImageFacesRequest.h
	include/alibabacloud/imm/model/CompareImageFacesResult.h
	include/alibabacloud/imm/model/ConvertOfficeFormatRequest.h
	include/alibabacloud/imm/model/ConvertOfficeFormatResult.h
	include/alibabacloud/imm/model/CreateCADConversionTaskRequest.h
	include/alibabacloud/imm/model/CreateCADConversionTaskResult.h
	include/alibabacloud/imm/model/CreateDocIndexTaskRequest.h
	include/alibabacloud/imm/model/CreateDocIndexTaskResult.h
	include/alibabacloud/imm/model/CreateGrabFrameTaskRequest.h
	include/alibabacloud/imm/model/CreateGrabFrameTaskResult.h
	include/alibabacloud/imm/model/CreateGroupFacesJobRequest.h
	include/alibabacloud/imm/model/CreateGroupFacesJobResult.h
	include/alibabacloud/imm/model/CreateImageProcessTaskRequest.h
	include/alibabacloud/imm/model/CreateImageProcessTaskResult.h
	include/alibabacloud/imm/model/CreateMediaComplexTaskRequest.h
	include/alibabacloud/imm/model/CreateMediaComplexTaskResult.h
	include/alibabacloud/imm/model/CreateMergeFaceGroupsJobRequest.h
	include/alibabacloud/imm/model/CreateMergeFaceGroupsJobResult.h
	include/alibabacloud/imm/model/CreateOfficeConversionTaskRequest.h
	include/alibabacloud/imm/model/CreateOfficeConversionTaskResult.h
	include/alibabacloud/imm/model/CreateSetRequest.h
	include/alibabacloud/imm/model/CreateSetResult.h
	include/alibabacloud/imm/model/CreateStreamAnalyseTaskRequest.h
	include/alibabacloud/imm/model/CreateStreamAnalyseTaskResult.h
	include/alibabacloud/imm/model/CreateVideoAbstractTaskRequest.h
	include/alibabacloud/imm/model/CreateVideoAbstractTaskResult.h
	include/alibabacloud/imm/model/CreateVideoAnalyseTaskRequest.h
	include/alibabacloud/imm/model/CreateVideoAnalyseTaskResult.h
	include/alibabacloud/imm/model/CreateVideoCompressTaskRequest.h
	include/alibabacloud/imm/model/CreateVideoCompressTaskResult.h
	include/alibabacloud/imm/model/CreateVideoProduceTaskRequest.h
	include/alibabacloud/imm/model/CreateVideoProduceTaskResult.h
	include/alibabacloud/imm/model/DecodeBlindWatermarkRequest.h
	include/alibabacloud/imm/model/DecodeBlindWatermarkResult.h
	include/alibabacloud/imm/model/DeleteDocIndexRequest.h
	include/alibabacloud/imm/model/DeleteDocIndexResult.h
	include/alibabacloud/imm/model/DeleteImageRequest.h
	include/alibabacloud/imm/model/DeleteImageResult.h
	include/alibabacloud/imm/model/DeleteImageJobRequest.h
	include/alibabacloud/imm/model/DeleteImageJobResult.h
	include/alibabacloud/imm/model/DeleteOfficeConversionTaskRequest.h
	include/alibabacloud/imm/model/DeleteOfficeConversionTaskResult.h
	include/alibabacloud/imm/model/DeleteProjectRequest.h
	include/alibabacloud/imm/model/DeleteProjectResult.h
	include/alibabacloud/imm/model/DeleteSetRequest.h
	include/alibabacloud/imm/model/DeleteSetResult.h
	include/alibabacloud/imm/model/DeleteVideoRequest.h
	include/alibabacloud/imm/model/DeleteVideoResult.h
	include/alibabacloud/imm/model/DeleteVideoTaskRequest.h
	include/alibabacloud/imm/model/DeleteVideoTaskResult.h
	include/alibabacloud/imm/model/DescribeRegionsRequest.h
	include/alibabacloud/imm/model/DescribeRegionsResult.h
	include/alibabacloud/imm/model/DetectImageBodiesRequest.h
	include/alibabacloud/imm/model/DetectImageBodiesResult.h
	include/alibabacloud/imm/model/DetectImageCelebrityRequest.h
	include/alibabacloud/imm/model/DetectImageCelebrityResult.h
	include/alibabacloud/imm/model/DetectImageFacesRequest.h
	include/alibabacloud/imm/model/DetectImageFacesResult.h
	include/alibabacloud/imm/model/DetectImageLogosRequest.h
	include/alibabacloud/imm/model/DetectImageLogosResult.h
	include/alibabacloud/imm/model/DetectImageQRCodesRequest.h
	include/alibabacloud/imm/model/DetectImageQRCodesResult.h
	include/alibabacloud/imm/model/DetectImageTagsRequest.h
	include/alibabacloud/imm/model/DetectImageTagsResult.h
	include/alibabacloud/imm/model/DetectImageTextsRequest.h
	include/alibabacloud/imm/model/DetectImageTextsResult.h
	include/alibabacloud/imm/model/DetectQRCodesRequest.h
	include/alibabacloud/imm/model/DetectQRCodesResult.h
	include/alibabacloud/imm/model/EncodeBlindWatermarkRequest.h
	include/alibabacloud/imm/model/EncodeBlindWatermarkResult.h
	include/alibabacloud/imm/model/FindImagesRequest.h
	include/alibabacloud/imm/model/FindImagesResult.h
	include/alibabacloud/imm/model/FindSimilarFacesRequest.h
	include/alibabacloud/imm/model/FindSimilarFacesResult.h
	include/alibabacloud/imm/model/GetContentKeyRequest.h
	include/alibabacloud/imm/model/GetContentKeyResult.h
	include/alibabacloud/imm/model/GetDRMLicenseRequest.h
	include/alibabacloud/imm/model/GetDRMLicenseResult.h
	include/alibabacloud/imm/model/GetDocIndexRequest.h
	include/alibabacloud/imm/model/GetDocIndexResult.h
	include/alibabacloud/imm/model/GetDocIndexTaskRequest.h
	include/alibabacloud/imm/model/GetDocIndexTaskResult.h
	include/alibabacloud/imm/model/GetImageRequest.h
	include/alibabacloud/imm/model/GetImageResult.h
	include/alibabacloud/imm/model/GetImageCroppingSuggestionsRequest.h
	include/alibabacloud/imm/model/GetImageCroppingSuggestionsResult.h
	include/alibabacloud/imm/model/GetImageJobRequest.h
	include/alibabacloud/imm/model/GetImageJobResult.h
	include/alibabacloud/imm/model/GetImageQualityRequest.h
	include/alibabacloud/imm/model/GetImageQualityResult.h
	include/alibabacloud/imm/model/GetMediaMetaRequest.h
	include/alibabacloud/imm/model/GetMediaMetaResult.h
	include/alibabacloud/imm/model/GetOfficeConversionTaskRequest.h
	include/alibabacloud/imm/model/GetOfficeConversionTaskResult.h
	include/alibabacloud/imm/model/GetOfficeEditURLRequest.h
	include/alibabacloud/imm/model/GetOfficeEditURLResult.h
	include/alibabacloud/imm/model/GetOfficePreviewURLRequest.h
	include/alibabacloud/imm/model/GetOfficePreviewURLResult.h
	include/alibabacloud/imm/model/GetProjectRequest.h
	include/alibabacloud/imm/model/GetProjectResult.h
	include/alibabacloud/imm/model/GetSetRequest.h
	include/alibabacloud/imm/model/GetSetResult.h
	include/alibabacloud/imm/model/GetVideoRequest.h
	include/alibabacloud/imm/model/GetVideoResult.h
	include/alibabacloud/imm/model/GetVideoTaskRequest.h
	include/alibabacloud/imm/model/GetVideoTaskResult.h
	include/alibabacloud/imm/model/IndexImageRequest.h
	include/alibabacloud/imm/model/IndexImageResult.h
	include/alibabacloud/imm/model/IndexVideoRequest.h
	include/alibabacloud/imm/model/IndexVideoResult.h
	include/alibabacloud/imm/model/ListFaceGroupsRequest.h
	include/alibabacloud/imm/model/ListFaceGroupsResult.h
	include/alibabacloud/imm/model/ListImageJobsRequest.h
	include/alibabacloud/imm/model/ListImageJobsResult.h
	include/alibabacloud/imm/model/ListImagesRequest.h
	include/alibabacloud/imm/model/ListImagesResult.h
	include/alibabacloud/imm/model/ListOfficeConversionTaskRequest.h
	include/alibabacloud/imm/model/ListOfficeConversionTaskResult.h
	include/alibabacloud/imm/model/ListProjectAPIsRequest.h
	include/alibabacloud/imm/model/ListProjectAPIsResult.h
	include/alibabacloud/imm/model/ListProjectsRequest.h
	include/alibabacloud/imm/model/ListProjectsResult.h
	include/alibabacloud/imm/model/ListSetTagsRequest.h
	include/alibabacloud/imm/model/ListSetTagsResult.h
	include/alibabacloud/imm/model/ListSetsRequest.h
	include/alibabacloud/imm/model/ListSetsResult.h
	include/alibabacloud/imm/model/ListVideoAudiosRequest.h
	include/alibabacloud/imm/model/ListVideoAudiosResult.h
	include/alibabacloud/imm/model/ListVideoFramesRequest.h
	include/alibabacloud/imm/model/ListVideoFramesResult.h
	include/alibabacloud/imm/model/ListVideoTasksRequest.h
	include/alibabacloud/imm/model/ListVideoTasksResult.h
	include/alibabacloud/imm/model/ListVideosRequest.h
	include/alibabacloud/imm/model/ListVideosResult.h
	include/alibabacloud/imm/model/OpenImmServiceRequest.h
	include/alibabacloud/imm/model/OpenImmServiceResult.h
	include/alibabacloud/imm/model/PutProjectRequest.h
	include/alibabacloud/imm/model/PutProjectResult.h
	include/alibabacloud/imm/model/RefreshOfficeEditTokenRequest.h
	include/alibabacloud/imm/model/RefreshOfficeEditTokenResult.h
	include/alibabacloud/imm/model/RefreshOfficePreviewTokenRequest.h
	include/alibabacloud/imm/model/RefreshOfficePreviewTokenResult.h
	include/alibabacloud/imm/model/SearchDocIndexRequest.h
	include/alibabacloud/imm/model/SearchDocIndexResult.h
	include/alibabacloud/imm/model/StopStreamAnalyseTaskRequest.h
	include/alibabacloud/imm/model/StopStreamAnalyseTaskResult.h
	include/alibabacloud/imm/model/UpdateDocIndexMetaRequest.h
	include/alibabacloud/imm/model/UpdateDocIndexMetaResult.h
	include/alibabacloud/imm/model/UpdateFaceGroupRequest.h
	include/alibabacloud/imm/model/UpdateFaceGroupResult.h
	include/alibabacloud/imm/model/UpdateImageRequest.h
	include/alibabacloud/imm/model/UpdateImageResult.h
	include/alibabacloud/imm/model/UpdateProjectRequest.h
	include/alibabacloud/imm/model/UpdateProjectResult.h
	include/alibabacloud/imm/model/UpdateSetRequest.h
	include/alibabacloud/imm/model/UpdateSetResult.h
	include/alibabacloud/imm/model/VideoAnalyseFeedbackRequest.h
	include/alibabacloud/imm/model/VideoAnalyseFeedbackResult.h )

set(imm_src 
	src/ImmClient.cc
	src/model/CompareImageFacesRequest.cc
	src/model/CompareImageFacesResult.cc
	src/model/ConvertOfficeFormatRequest.cc
	src/model/ConvertOfficeFormatResult.cc
	src/model/CreateCADConversionTaskRequest.cc
	src/model/CreateCADConversionTaskResult.cc
	src/model/CreateDocIndexTaskRequest.cc
	src/model/CreateDocIndexTaskResult.cc
	src/model/CreateGrabFrameTaskRequest.cc
	src/model/CreateGrabFrameTaskResult.cc
	src/model/CreateGroupFacesJobRequest.cc
	src/model/CreateGroupFacesJobResult.cc
	src/model/CreateImageProcessTaskRequest.cc
	src/model/CreateImageProcessTaskResult.cc
	src/model/CreateMediaComplexTaskRequest.cc
	src/model/CreateMediaComplexTaskResult.cc
	src/model/CreateMergeFaceGroupsJobRequest.cc
	src/model/CreateMergeFaceGroupsJobResult.cc
	src/model/CreateOfficeConversionTaskRequest.cc
	src/model/CreateOfficeConversionTaskResult.cc
	src/model/CreateSetRequest.cc
	src/model/CreateSetResult.cc
	src/model/CreateStreamAnalyseTaskRequest.cc
	src/model/CreateStreamAnalyseTaskResult.cc
	src/model/CreateVideoAbstractTaskRequest.cc
	src/model/CreateVideoAbstractTaskResult.cc
	src/model/CreateVideoAnalyseTaskRequest.cc
	src/model/CreateVideoAnalyseTaskResult.cc
	src/model/CreateVideoCompressTaskRequest.cc
	src/model/CreateVideoCompressTaskResult.cc
	src/model/CreateVideoProduceTaskRequest.cc
	src/model/CreateVideoProduceTaskResult.cc
	src/model/DecodeBlindWatermarkRequest.cc
	src/model/DecodeBlindWatermarkResult.cc
	src/model/DeleteDocIndexRequest.cc
	src/model/DeleteDocIndexResult.cc
	src/model/DeleteImageRequest.cc
	src/model/DeleteImageResult.cc
	src/model/DeleteImageJobRequest.cc
	src/model/DeleteImageJobResult.cc
	src/model/DeleteOfficeConversionTaskRequest.cc
	src/model/DeleteOfficeConversionTaskResult.cc
	src/model/DeleteProjectRequest.cc
	src/model/DeleteProjectResult.cc
	src/model/DeleteSetRequest.cc
	src/model/DeleteSetResult.cc
	src/model/DeleteVideoRequest.cc
	src/model/DeleteVideoResult.cc
	src/model/DeleteVideoTaskRequest.cc
	src/model/DeleteVideoTaskResult.cc
	src/model/DescribeRegionsRequest.cc
	src/model/DescribeRegionsResult.cc
	src/model/DetectImageBodiesRequest.cc
	src/model/DetectImageBodiesResult.cc
	src/model/DetectImageCelebrityRequest.cc
	src/model/DetectImageCelebrityResult.cc
	src/model/DetectImageFacesRequest.cc
	src/model/DetectImageFacesResult.cc
	src/model/DetectImageLogosRequest.cc
	src/model/DetectImageLogosResult.cc
	src/model/DetectImageQRCodesRequest.cc
	src/model/DetectImageQRCodesResult.cc
	src/model/DetectImageTagsRequest.cc
	src/model/DetectImageTagsResult.cc
	src/model/DetectImageTextsRequest.cc
	src/model/DetectImageTextsResult.cc
	src/model/DetectQRCodesRequest.cc
	src/model/DetectQRCodesResult.cc
	src/model/EncodeBlindWatermarkRequest.cc
	src/model/EncodeBlindWatermarkResult.cc
	src/model/FindImagesRequest.cc
	src/model/FindImagesResult.cc
	src/model/FindSimilarFacesRequest.cc
	src/model/FindSimilarFacesResult.cc
	src/model/GetContentKeyRequest.cc
	src/model/GetContentKeyResult.cc
	src/model/GetDRMLicenseRequest.cc
	src/model/GetDRMLicenseResult.cc
	src/model/GetDocIndexRequest.cc
	src/model/GetDocIndexResult.cc
	src/model/GetDocIndexTaskRequest.cc
	src/model/GetDocIndexTaskResult.cc
	src/model/GetImageRequest.cc
	src/model/GetImageResult.cc
	src/model/GetImageCroppingSuggestionsRequest.cc
	src/model/GetImageCroppingSuggestionsResult.cc
	src/model/GetImageJobRequest.cc
	src/model/GetImageJobResult.cc
	src/model/GetImageQualityRequest.cc
	src/model/GetImageQualityResult.cc
	src/model/GetMediaMetaRequest.cc
	src/model/GetMediaMetaResult.cc
	src/model/GetOfficeConversionTaskRequest.cc
	src/model/GetOfficeConversionTaskResult.cc
	src/model/GetOfficeEditURLRequest.cc
	src/model/GetOfficeEditURLResult.cc
	src/model/GetOfficePreviewURLRequest.cc
	src/model/GetOfficePreviewURLResult.cc
	src/model/GetProjectRequest.cc
	src/model/GetProjectResult.cc
	src/model/GetSetRequest.cc
	src/model/GetSetResult.cc
	src/model/GetVideoRequest.cc
	src/model/GetVideoResult.cc
	src/model/GetVideoTaskRequest.cc
	src/model/GetVideoTaskResult.cc
	src/model/IndexImageRequest.cc
	src/model/IndexImageResult.cc
	src/model/IndexVideoRequest.cc
	src/model/IndexVideoResult.cc
	src/model/ListFaceGroupsRequest.cc
	src/model/ListFaceGroupsResult.cc
	src/model/ListImageJobsRequest.cc
	src/model/ListImageJobsResult.cc
	src/model/ListImagesRequest.cc
	src/model/ListImagesResult.cc
	src/model/ListOfficeConversionTaskRequest.cc
	src/model/ListOfficeConversionTaskResult.cc
	src/model/ListProjectAPIsRequest.cc
	src/model/ListProjectAPIsResult.cc
	src/model/ListProjectsRequest.cc
	src/model/ListProjectsResult.cc
	src/model/ListSetTagsRequest.cc
	src/model/ListSetTagsResult.cc
	src/model/ListSetsRequest.cc
	src/model/ListSetsResult.cc
	src/model/ListVideoAudiosRequest.cc
	src/model/ListVideoAudiosResult.cc
	src/model/ListVideoFramesRequest.cc
	src/model/ListVideoFramesResult.cc
	src/model/ListVideoTasksRequest.cc
	src/model/ListVideoTasksResult.cc
	src/model/ListVideosRequest.cc
	src/model/ListVideosResult.cc
	src/model/OpenImmServiceRequest.cc
	src/model/OpenImmServiceResult.cc
	src/model/PutProjectRequest.cc
	src/model/PutProjectResult.cc
	src/model/RefreshOfficeEditTokenRequest.cc
	src/model/RefreshOfficeEditTokenResult.cc
	src/model/RefreshOfficePreviewTokenRequest.cc
	src/model/RefreshOfficePreviewTokenResult.cc
	src/model/SearchDocIndexRequest.cc
	src/model/SearchDocIndexResult.cc
	src/model/StopStreamAnalyseTaskRequest.cc
	src/model/StopStreamAnalyseTaskResult.cc
	src/model/UpdateDocIndexMetaRequest.cc
	src/model/UpdateDocIndexMetaResult.cc
	src/model/UpdateFaceGroupRequest.cc
	src/model/UpdateFaceGroupResult.cc
	src/model/UpdateImageRequest.cc
	src/model/UpdateImageResult.cc
	src/model/UpdateProjectRequest.cc
	src/model/UpdateProjectResult.cc
	src/model/UpdateSetRequest.cc
	src/model/UpdateSetResult.cc
	src/model/VideoAnalyseFeedbackRequest.cc
	src/model/VideoAnalyseFeedbackResult.cc )

add_library(imm ${LIB_TYPE}
	${imm_public_header}
	${imm_public_header_model}
	${imm_src})

set_target_properties(imm
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}imm
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(imm
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_IMM_LIBRARY)
endif()

target_include_directories(imm
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(imm
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(imm
		jsoncpp)
	target_include_directories(imm
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(imm
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(imm
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(imm
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(imm
		jsoncpp)
endif()

install(FILES ${imm_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imm)
install(FILES ${imm_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/imm/model)
install(TARGETS imm
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)