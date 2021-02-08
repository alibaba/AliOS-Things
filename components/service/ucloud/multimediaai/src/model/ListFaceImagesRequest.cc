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

#include <alibabacloud/multimediaai/model/ListFaceImagesRequest.h>

using AlibabaCloud::Multimediaai::Model::ListFaceImagesRequest;

ListFaceImagesRequest::ListFaceImagesRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "ListFaceImages")
{
	setMethod(HttpRequest::Method::Post);
}

ListFaceImagesRequest::~ListFaceImagesRequest()
{}

long ListFaceImagesRequest::getFaceGroupId()const
{
	return faceGroupId_;
}

void ListFaceImagesRequest::setFaceGroupId(long faceGroupId)
{
	faceGroupId_ = faceGroupId;
	setParameter("FaceGroupId", std::to_string(faceGroupId));
}

long ListFaceImagesRequest::getFacePersonId()const
{
	return facePersonId_;
}

void ListFaceImagesRequest::setFacePersonId(long facePersonId)
{
	facePersonId_ = facePersonId;
	setParameter("FacePersonId", std::to_string(facePersonId));
}

int ListFaceImagesRequest::getPageNumber()const
{
	return pageNumber_;
}

void ListFaceImagesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

int ListFaceImagesRequest::getPageSize()const
{
	return pageSize_;
}

void ListFaceImagesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

