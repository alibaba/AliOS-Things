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

#include <alibabacloud/cloudphoto/model/RenameFaceRequest.h>

using AlibabaCloud::CloudPhoto::Model::RenameFaceRequest;

RenameFaceRequest::RenameFaceRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "RenameFace")
{
	setMethod(HttpRequest::Method::Post);
}

RenameFaceRequest::~RenameFaceRequest()
{}

std::string RenameFaceRequest::getLibraryId()const
{
	return libraryId_;
}

void RenameFaceRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string RenameFaceRequest::getStoreName()const
{
	return storeName_;
}

void RenameFaceRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

long RenameFaceRequest::getFaceId()const
{
	return faceId_;
}

void RenameFaceRequest::setFaceId(long faceId)
{
	faceId_ = faceId;
	setParameter("FaceId", std::to_string(faceId));
}

std::string RenameFaceRequest::getFaceName()const
{
	return faceName_;
}

void RenameFaceRequest::setFaceName(const std::string& faceName)
{
	faceName_ = faceName;
	setParameter("FaceName", faceName);
}

