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

set(cloudphoto_public_header 
	include/alibabacloud/cloudphoto/CloudPhotoClient.h
	include/alibabacloud/cloudphoto/CloudPhotoExport.h )

set(cloudphoto_public_header_model 
	include/alibabacloud/cloudphoto/model/ActivatePhotosRequest.h
	include/alibabacloud/cloudphoto/model/ActivatePhotosResult.h
	include/alibabacloud/cloudphoto/model/AddAlbumPhotosRequest.h
	include/alibabacloud/cloudphoto/model/AddAlbumPhotosResult.h
	include/alibabacloud/cloudphoto/model/CreateAlbumRequest.h
	include/alibabacloud/cloudphoto/model/CreateAlbumResult.h
	include/alibabacloud/cloudphoto/model/CreateEventRequest.h
	include/alibabacloud/cloudphoto/model/CreateEventResult.h
	include/alibabacloud/cloudphoto/model/CreatePhotoRequest.h
	include/alibabacloud/cloudphoto/model/CreatePhotoResult.h
	include/alibabacloud/cloudphoto/model/CreatePhotoStoreRequest.h
	include/alibabacloud/cloudphoto/model/CreatePhotoStoreResult.h
	include/alibabacloud/cloudphoto/model/CreateTransactionRequest.h
	include/alibabacloud/cloudphoto/model/CreateTransactionResult.h
	include/alibabacloud/cloudphoto/model/DeleteAlbumsRequest.h
	include/alibabacloud/cloudphoto/model/DeleteAlbumsResult.h
	include/alibabacloud/cloudphoto/model/DeleteEventRequest.h
	include/alibabacloud/cloudphoto/model/DeleteEventResult.h
	include/alibabacloud/cloudphoto/model/DeleteFacesRequest.h
	include/alibabacloud/cloudphoto/model/DeleteFacesResult.h
	include/alibabacloud/cloudphoto/model/DeletePhotoStoreRequest.h
	include/alibabacloud/cloudphoto/model/DeletePhotoStoreResult.h
	include/alibabacloud/cloudphoto/model/DeletePhotosRequest.h
	include/alibabacloud/cloudphoto/model/DeletePhotosResult.h
	include/alibabacloud/cloudphoto/model/EditEventRequest.h
	include/alibabacloud/cloudphoto/model/EditEventResult.h
	include/alibabacloud/cloudphoto/model/EditPhotoStoreRequest.h
	include/alibabacloud/cloudphoto/model/EditPhotoStoreResult.h
	include/alibabacloud/cloudphoto/model/EditPhotosRequest.h
	include/alibabacloud/cloudphoto/model/EditPhotosResult.h
	include/alibabacloud/cloudphoto/model/FetchAlbumTagPhotosRequest.h
	include/alibabacloud/cloudphoto/model/FetchAlbumTagPhotosResult.h
	include/alibabacloud/cloudphoto/model/FetchLibrariesRequest.h
	include/alibabacloud/cloudphoto/model/FetchLibrariesResult.h
	include/alibabacloud/cloudphoto/model/FetchMomentPhotosRequest.h
	include/alibabacloud/cloudphoto/model/FetchMomentPhotosResult.h
	include/alibabacloud/cloudphoto/model/FetchPhotosRequest.h
	include/alibabacloud/cloudphoto/model/FetchPhotosResult.h
	include/alibabacloud/cloudphoto/model/GetAlbumsByNamesRequest.h
	include/alibabacloud/cloudphoto/model/GetAlbumsByNamesResult.h
	include/alibabacloud/cloudphoto/model/GetDownloadUrlRequest.h
	include/alibabacloud/cloudphoto/model/GetDownloadUrlResult.h
	include/alibabacloud/cloudphoto/model/GetDownloadUrlsRequest.h
	include/alibabacloud/cloudphoto/model/GetDownloadUrlsResult.h
	include/alibabacloud/cloudphoto/model/GetEventRequest.h
	include/alibabacloud/cloudphoto/model/GetEventResult.h
	include/alibabacloud/cloudphoto/model/GetFramedPhotoUrlsRequest.h
	include/alibabacloud/cloudphoto/model/GetFramedPhotoUrlsResult.h
	include/alibabacloud/cloudphoto/model/GetLibraryRequest.h
	include/alibabacloud/cloudphoto/model/GetLibraryResult.h
	include/alibabacloud/cloudphoto/model/GetPhotoStoreRequest.h
	include/alibabacloud/cloudphoto/model/GetPhotoStoreResult.h
	include/alibabacloud/cloudphoto/model/GetPhotosRequest.h
	include/alibabacloud/cloudphoto/model/GetPhotosResult.h
	include/alibabacloud/cloudphoto/model/GetPhotosByMd5sRequest.h
	include/alibabacloud/cloudphoto/model/GetPhotosByMd5sResult.h
	include/alibabacloud/cloudphoto/model/GetPrivateAccessUrlsRequest.h
	include/alibabacloud/cloudphoto/model/GetPrivateAccessUrlsResult.h
	include/alibabacloud/cloudphoto/model/GetPublicAccessUrlsRequest.h
	include/alibabacloud/cloudphoto/model/GetPublicAccessUrlsResult.h
	include/alibabacloud/cloudphoto/model/GetQuotaRequest.h
	include/alibabacloud/cloudphoto/model/GetQuotaResult.h
	include/alibabacloud/cloudphoto/model/GetSimilarPhotosRequest.h
	include/alibabacloud/cloudphoto/model/GetSimilarPhotosResult.h
	include/alibabacloud/cloudphoto/model/GetThumbnailRequest.h
	include/alibabacloud/cloudphoto/model/GetThumbnailResult.h
	include/alibabacloud/cloudphoto/model/GetThumbnailsRequest.h
	include/alibabacloud/cloudphoto/model/GetThumbnailsResult.h
	include/alibabacloud/cloudphoto/model/GetVideoCoverRequest.h
	include/alibabacloud/cloudphoto/model/GetVideoCoverResult.h
	include/alibabacloud/cloudphoto/model/InactivatePhotosRequest.h
	include/alibabacloud/cloudphoto/model/InactivatePhotosResult.h
	include/alibabacloud/cloudphoto/model/LikePhotoRequest.h
	include/alibabacloud/cloudphoto/model/LikePhotoResult.h
	include/alibabacloud/cloudphoto/model/ListAlbumPhotosRequest.h
	include/alibabacloud/cloudphoto/model/ListAlbumPhotosResult.h
	include/alibabacloud/cloudphoto/model/ListAlbumsRequest.h
	include/alibabacloud/cloudphoto/model/ListAlbumsResult.h
	include/alibabacloud/cloudphoto/model/ListEventsRequest.h
	include/alibabacloud/cloudphoto/model/ListEventsResult.h
	include/alibabacloud/cloudphoto/model/ListFacePhotosRequest.h
	include/alibabacloud/cloudphoto/model/ListFacePhotosResult.h
	include/alibabacloud/cloudphoto/model/ListFacesRequest.h
	include/alibabacloud/cloudphoto/model/ListFacesResult.h
	include/alibabacloud/cloudphoto/model/ListMomentPhotosRequest.h
	include/alibabacloud/cloudphoto/model/ListMomentPhotosResult.h
	include/alibabacloud/cloudphoto/model/ListMomentsRequest.h
	include/alibabacloud/cloudphoto/model/ListMomentsResult.h
	include/alibabacloud/cloudphoto/model/ListPhotoFacesRequest.h
	include/alibabacloud/cloudphoto/model/ListPhotoFacesResult.h
	include/alibabacloud/cloudphoto/model/ListPhotoStoresRequest.h
	include/alibabacloud/cloudphoto/model/ListPhotoStoresResult.h
	include/alibabacloud/cloudphoto/model/ListPhotoTagsRequest.h
	include/alibabacloud/cloudphoto/model/ListPhotoTagsResult.h
	include/alibabacloud/cloudphoto/model/ListPhotosRequest.h
	include/alibabacloud/cloudphoto/model/ListPhotosResult.h
	include/alibabacloud/cloudphoto/model/ListRegisteredTagsRequest.h
	include/alibabacloud/cloudphoto/model/ListRegisteredTagsResult.h
	include/alibabacloud/cloudphoto/model/ListTagPhotosRequest.h
	include/alibabacloud/cloudphoto/model/ListTagPhotosResult.h
	include/alibabacloud/cloudphoto/model/ListTagsRequest.h
	include/alibabacloud/cloudphoto/model/ListTagsResult.h
	include/alibabacloud/cloudphoto/model/ListTimeLinePhotosRequest.h
	include/alibabacloud/cloudphoto/model/ListTimeLinePhotosResult.h
	include/alibabacloud/cloudphoto/model/ListTimeLinesRequest.h
	include/alibabacloud/cloudphoto/model/ListTimeLinesResult.h
	include/alibabacloud/cloudphoto/model/MergeFacesRequest.h
	include/alibabacloud/cloudphoto/model/MergeFacesResult.h
	include/alibabacloud/cloudphoto/model/MoveAlbumPhotosRequest.h
	include/alibabacloud/cloudphoto/model/MoveAlbumPhotosResult.h
	include/alibabacloud/cloudphoto/model/MoveFacePhotosRequest.h
	include/alibabacloud/cloudphoto/model/MoveFacePhotosResult.h
	include/alibabacloud/cloudphoto/model/ReactivatePhotosRequest.h
	include/alibabacloud/cloudphoto/model/ReactivatePhotosResult.h
	include/alibabacloud/cloudphoto/model/RegisterPhotoRequest.h
	include/alibabacloud/cloudphoto/model/RegisterPhotoResult.h
	include/alibabacloud/cloudphoto/model/RegisterTagRequest.h
	include/alibabacloud/cloudphoto/model/RegisterTagResult.h
	include/alibabacloud/cloudphoto/model/RemoveAlbumPhotosRequest.h
	include/alibabacloud/cloudphoto/model/RemoveAlbumPhotosResult.h
	include/alibabacloud/cloudphoto/model/RemoveFacePhotosRequest.h
	include/alibabacloud/cloudphoto/model/RemoveFacePhotosResult.h
	include/alibabacloud/cloudphoto/model/RenameAlbumRequest.h
	include/alibabacloud/cloudphoto/model/RenameAlbumResult.h
	include/alibabacloud/cloudphoto/model/RenameFaceRequest.h
	include/alibabacloud/cloudphoto/model/RenameFaceResult.h
	include/alibabacloud/cloudphoto/model/SearchPhotosRequest.h
	include/alibabacloud/cloudphoto/model/SearchPhotosResult.h
	include/alibabacloud/cloudphoto/model/SetAlbumCoverRequest.h
	include/alibabacloud/cloudphoto/model/SetAlbumCoverResult.h
	include/alibabacloud/cloudphoto/model/SetFaceCoverRequest.h
	include/alibabacloud/cloudphoto/model/SetFaceCoverResult.h
	include/alibabacloud/cloudphoto/model/SetMeRequest.h
	include/alibabacloud/cloudphoto/model/SetMeResult.h
	include/alibabacloud/cloudphoto/model/SetQuotaRequest.h
	include/alibabacloud/cloudphoto/model/SetQuotaResult.h
	include/alibabacloud/cloudphoto/model/TagPhotoRequest.h
	include/alibabacloud/cloudphoto/model/TagPhotoResult.h
	include/alibabacloud/cloudphoto/model/ToggleFeaturesRequest.h
	include/alibabacloud/cloudphoto/model/ToggleFeaturesResult.h )

set(cloudphoto_src 
	src/CloudPhotoClient.cc
	src/model/ActivatePhotosRequest.cc
	src/model/ActivatePhotosResult.cc
	src/model/AddAlbumPhotosRequest.cc
	src/model/AddAlbumPhotosResult.cc
	src/model/CreateAlbumRequest.cc
	src/model/CreateAlbumResult.cc
	src/model/CreateEventRequest.cc
	src/model/CreateEventResult.cc
	src/model/CreatePhotoRequest.cc
	src/model/CreatePhotoResult.cc
	src/model/CreatePhotoStoreRequest.cc
	src/model/CreatePhotoStoreResult.cc
	src/model/CreateTransactionRequest.cc
	src/model/CreateTransactionResult.cc
	src/model/DeleteAlbumsRequest.cc
	src/model/DeleteAlbumsResult.cc
	src/model/DeleteEventRequest.cc
	src/model/DeleteEventResult.cc
	src/model/DeleteFacesRequest.cc
	src/model/DeleteFacesResult.cc
	src/model/DeletePhotoStoreRequest.cc
	src/model/DeletePhotoStoreResult.cc
	src/model/DeletePhotosRequest.cc
	src/model/DeletePhotosResult.cc
	src/model/EditEventRequest.cc
	src/model/EditEventResult.cc
	src/model/EditPhotoStoreRequest.cc
	src/model/EditPhotoStoreResult.cc
	src/model/EditPhotosRequest.cc
	src/model/EditPhotosResult.cc
	src/model/FetchAlbumTagPhotosRequest.cc
	src/model/FetchAlbumTagPhotosResult.cc
	src/model/FetchLibrariesRequest.cc
	src/model/FetchLibrariesResult.cc
	src/model/FetchMomentPhotosRequest.cc
	src/model/FetchMomentPhotosResult.cc
	src/model/FetchPhotosRequest.cc
	src/model/FetchPhotosResult.cc
	src/model/GetAlbumsByNamesRequest.cc
	src/model/GetAlbumsByNamesResult.cc
	src/model/GetDownloadUrlRequest.cc
	src/model/GetDownloadUrlResult.cc
	src/model/GetDownloadUrlsRequest.cc
	src/model/GetDownloadUrlsResult.cc
	src/model/GetEventRequest.cc
	src/model/GetEventResult.cc
	src/model/GetFramedPhotoUrlsRequest.cc
	src/model/GetFramedPhotoUrlsResult.cc
	src/model/GetLibraryRequest.cc
	src/model/GetLibraryResult.cc
	src/model/GetPhotoStoreRequest.cc
	src/model/GetPhotoStoreResult.cc
	src/model/GetPhotosRequest.cc
	src/model/GetPhotosResult.cc
	src/model/GetPhotosByMd5sRequest.cc
	src/model/GetPhotosByMd5sResult.cc
	src/model/GetPrivateAccessUrlsRequest.cc
	src/model/GetPrivateAccessUrlsResult.cc
	src/model/GetPublicAccessUrlsRequest.cc
	src/model/GetPublicAccessUrlsResult.cc
	src/model/GetQuotaRequest.cc
	src/model/GetQuotaResult.cc
	src/model/GetSimilarPhotosRequest.cc
	src/model/GetSimilarPhotosResult.cc
	src/model/GetThumbnailRequest.cc
	src/model/GetThumbnailResult.cc
	src/model/GetThumbnailsRequest.cc
	src/model/GetThumbnailsResult.cc
	src/model/GetVideoCoverRequest.cc
	src/model/GetVideoCoverResult.cc
	src/model/InactivatePhotosRequest.cc
	src/model/InactivatePhotosResult.cc
	src/model/LikePhotoRequest.cc
	src/model/LikePhotoResult.cc
	src/model/ListAlbumPhotosRequest.cc
	src/model/ListAlbumPhotosResult.cc
	src/model/ListAlbumsRequest.cc
	src/model/ListAlbumsResult.cc
	src/model/ListEventsRequest.cc
	src/model/ListEventsResult.cc
	src/model/ListFacePhotosRequest.cc
	src/model/ListFacePhotosResult.cc
	src/model/ListFacesRequest.cc
	src/model/ListFacesResult.cc
	src/model/ListMomentPhotosRequest.cc
	src/model/ListMomentPhotosResult.cc
	src/model/ListMomentsRequest.cc
	src/model/ListMomentsResult.cc
	src/model/ListPhotoFacesRequest.cc
	src/model/ListPhotoFacesResult.cc
	src/model/ListPhotoStoresRequest.cc
	src/model/ListPhotoStoresResult.cc
	src/model/ListPhotoTagsRequest.cc
	src/model/ListPhotoTagsResult.cc
	src/model/ListPhotosRequest.cc
	src/model/ListPhotosResult.cc
	src/model/ListRegisteredTagsRequest.cc
	src/model/ListRegisteredTagsResult.cc
	src/model/ListTagPhotosRequest.cc
	src/model/ListTagPhotosResult.cc
	src/model/ListTagsRequest.cc
	src/model/ListTagsResult.cc
	src/model/ListTimeLinePhotosRequest.cc
	src/model/ListTimeLinePhotosResult.cc
	src/model/ListTimeLinesRequest.cc
	src/model/ListTimeLinesResult.cc
	src/model/MergeFacesRequest.cc
	src/model/MergeFacesResult.cc
	src/model/MoveAlbumPhotosRequest.cc
	src/model/MoveAlbumPhotosResult.cc
	src/model/MoveFacePhotosRequest.cc
	src/model/MoveFacePhotosResult.cc
	src/model/ReactivatePhotosRequest.cc
	src/model/ReactivatePhotosResult.cc
	src/model/RegisterPhotoRequest.cc
	src/model/RegisterPhotoResult.cc
	src/model/RegisterTagRequest.cc
	src/model/RegisterTagResult.cc
	src/model/RemoveAlbumPhotosRequest.cc
	src/model/RemoveAlbumPhotosResult.cc
	src/model/RemoveFacePhotosRequest.cc
	src/model/RemoveFacePhotosResult.cc
	src/model/RenameAlbumRequest.cc
	src/model/RenameAlbumResult.cc
	src/model/RenameFaceRequest.cc
	src/model/RenameFaceResult.cc
	src/model/SearchPhotosRequest.cc
	src/model/SearchPhotosResult.cc
	src/model/SetAlbumCoverRequest.cc
	src/model/SetAlbumCoverResult.cc
	src/model/SetFaceCoverRequest.cc
	src/model/SetFaceCoverResult.cc
	src/model/SetMeRequest.cc
	src/model/SetMeResult.cc
	src/model/SetQuotaRequest.cc
	src/model/SetQuotaResult.cc
	src/model/TagPhotoRequest.cc
	src/model/TagPhotoResult.cc
	src/model/ToggleFeaturesRequest.cc
	src/model/ToggleFeaturesResult.cc )

add_library(cloudphoto ${LIB_TYPE}
	${cloudphoto_public_header}
	${cloudphoto_public_header_model}
	${cloudphoto_src})

set_target_properties(cloudphoto
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}cloudphoto
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(cloudphoto
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_CLOUDPHOTO_LIBRARY)
endif()

target_include_directories(cloudphoto
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(cloudphoto
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(cloudphoto
		jsoncpp)
	target_include_directories(cloudphoto
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(cloudphoto
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(cloudphoto
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(cloudphoto
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(cloudphoto
		jsoncpp)
endif()

install(FILES ${cloudphoto_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/cloudphoto)
install(FILES ${cloudphoto_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/cloudphoto/model)
install(TARGETS cloudphoto
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)