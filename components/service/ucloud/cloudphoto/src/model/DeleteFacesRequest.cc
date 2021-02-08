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

#include <alibabacloud/cloudphoto/model/DeleteFacesRequest.h>

using AlibabaCloud::CloudPhoto::Model::DeleteFacesRequest;

DeleteFacesRequest::DeleteFacesRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "DeleteFaces")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteFacesRequest::~DeleteFacesRequest()
{}

std::string DeleteFacesRequest::getLibraryId()const
{
	return libraryId_;
}

void DeleteFacesRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string DeleteFacesRequest::getStoreName()const
{
	return storeName_;
}

void DeleteFacesRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::vector<long> DeleteFacesRequest::getFaceId()const
{
	return faceId_;
}

void DeleteFacesRequest::setFaceId(const std::vector<long>& faceId)
{
	faceId_ = faceId;
	for(int dep1 = 0; dep1!= faceId.size(); dep1++) {
		setParameter("FaceId."+ std::to_string(dep1), std::to_string(faceId.at(dep1)));
	}
}

