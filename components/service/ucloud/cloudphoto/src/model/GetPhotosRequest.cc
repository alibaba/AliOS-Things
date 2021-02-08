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

#include <alibabacloud/cloudphoto/model/GetPhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::GetPhotosRequest;

GetPhotosRequest::GetPhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "GetPhotos")
{
	setMethod(HttpRequest::Method::Post);
}

GetPhotosRequest::~GetPhotosRequest()
{}

std::string GetPhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void GetPhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::vector<long> GetPhotosRequest::getPhotoId()const
{
	return photoId_;
}

void GetPhotosRequest::setPhotoId(const std::vector<long>& photoId)
{
	photoId_ = photoId;
	for(int dep1 = 0; dep1!= photoId.size(); dep1++) {
		setParameter("PhotoId."+ std::to_string(dep1), std::to_string(photoId.at(dep1)));
	}
}

std::string GetPhotosRequest::getStoreName()const
{
	return storeName_;
}

void GetPhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

