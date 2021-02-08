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

#include <alibabacloud/linkface/model/QueryFaceRequest.h>

using AlibabaCloud::LinkFace::Model::QueryFaceRequest;

QueryFaceRequest::QueryFaceRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "QueryFace")
{
	setMethod(HttpRequest::Method::Post);
}

QueryFaceRequest::~QueryFaceRequest()
{}

std::string QueryFaceRequest::getUserId()const
{
	return userId_;
}

void QueryFaceRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

std::string QueryFaceRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryFaceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryFaceRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryFaceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

