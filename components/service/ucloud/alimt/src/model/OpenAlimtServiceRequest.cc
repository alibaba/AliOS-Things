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

#include <alibabacloud/alimt/model/OpenAlimtServiceRequest.h>

using AlibabaCloud::Alimt::Model::OpenAlimtServiceRequest;

OpenAlimtServiceRequest::OpenAlimtServiceRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "OpenAlimtService")
{
	setMethod(HttpRequest::Method::Post);
}

OpenAlimtServiceRequest::~OpenAlimtServiceRequest()
{}

long OpenAlimtServiceRequest::getOwnerId()const
{
	return ownerId_;
}

void OpenAlimtServiceRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string OpenAlimtServiceRequest::getType()const
{
	return type_;
}

void OpenAlimtServiceRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

