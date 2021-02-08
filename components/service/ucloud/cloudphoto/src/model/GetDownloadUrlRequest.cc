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

#include <alibabacloud/cloudphoto/model/GetDownloadUrlRequest.h>

using AlibabaCloud::CloudPhoto::Model::GetDownloadUrlRequest;

GetDownloadUrlRequest::GetDownloadUrlRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "GetDownloadUrl")
{
	setMethod(HttpRequest::Method::Post);
}

GetDownloadUrlRequest::~GetDownloadUrlRequest()
{}

std::string GetDownloadUrlRequest::getLibraryId()const
{
	return libraryId_;
}

void GetDownloadUrlRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

long GetDownloadUrlRequest::getPhotoId()const
{
	return photoId_;
}

void GetDownloadUrlRequest::setPhotoId(long photoId)
{
	photoId_ = photoId;
	setParameter("PhotoId", std::to_string(photoId));
}

std::string GetDownloadUrlRequest::getStoreName()const
{
	return storeName_;
}

void GetDownloadUrlRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

