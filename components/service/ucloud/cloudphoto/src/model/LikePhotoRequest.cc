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

#include <alibabacloud/cloudphoto/model/LikePhotoRequest.h>

using AlibabaCloud::CloudPhoto::Model::LikePhotoRequest;

LikePhotoRequest::LikePhotoRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "LikePhoto")
{
	setMethod(HttpRequest::Method::Post);
}

LikePhotoRequest::~LikePhotoRequest()
{}

std::string LikePhotoRequest::getLibraryId()const
{
	return libraryId_;
}

void LikePhotoRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

long LikePhotoRequest::getPhotoId()const
{
	return photoId_;
}

void LikePhotoRequest::setPhotoId(long photoId)
{
	photoId_ = photoId;
	setParameter("PhotoId", std::to_string(photoId));
}

std::string LikePhotoRequest::getStoreName()const
{
	return storeName_;
}

void LikePhotoRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

