/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <alibabacloud/cloudphoto/model/RemoveAlbumPhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::RemoveAlbumPhotosRequest;

RemoveAlbumPhotosRequest::RemoveAlbumPhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "RemoveAlbumPhotos")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveAlbumPhotosRequest::~RemoveAlbumPhotosRequest()
{}

std::string RemoveAlbumPhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void RemoveAlbumPhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

long RemoveAlbumPhotosRequest::getAlbumId()const
{
	return albumId_;
}

void RemoveAlbumPhotosRequest::setAlbumId(long albumId)
{
	albumId_ = albumId;
	setParameter("AlbumId", std::to_string(albumId));
}

std::vector<long> RemoveAlbumPhotosRequest::getPhotoId()const
{
	return photoId_;
}

void RemoveAlbumPhotosRequest::setPhotoId(const std::vector<long>& photoId)
{
	photoId_ = photoId;
	for(int dep1 = 0; dep1!= photoId.size(); dep1++) {
		setParameter("PhotoId."+ std::to_string(dep1), std::to_string(photoId.at(dep1)));
	}
}

std::string RemoveAlbumPhotosRequest::getStoreName()const
{
	return storeName_;
}

void RemoveAlbumPhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

