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

#include <alibabacloud/multimediaai/model/ListFacePersonsRequest.h>

using AlibabaCloud::Multimediaai::Model::ListFacePersonsRequest;

ListFacePersonsRequest::ListFacePersonsRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "ListFacePersons")
{
	setMethod(HttpRequest::Method::Post);
}

ListFacePersonsRequest::~ListFacePersonsRequest()
{}

long ListFacePersonsRequest::getFaceGroupId()const
{
	return faceGroupId_;
}

void ListFacePersonsRequest::setFaceGroupId(long faceGroupId)
{
	faceGroupId_ = faceGroupId;
	setParameter("FaceGroupId", std::to_string(faceGroupId));
}

std::string ListFacePersonsRequest::getFacePersonName()const
{
	return facePersonName_;
}

void ListFacePersonsRequest::setFacePersonName(const std::string& facePersonName)
{
	facePersonName_ = facePersonName;
	setParameter("FacePersonName", facePersonName);
}

int ListFacePersonsRequest::getPageNumber()const
{
	return pageNumber_;
}

void ListFacePersonsRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

int ListFacePersonsRequest::getPageSize()const
{
	return pageSize_;
}

void ListFacePersonsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

