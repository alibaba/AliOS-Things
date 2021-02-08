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

#include <alibabacloud/alinlp/model/GetTsChEcomRequest.h>

using AlibabaCloud::Alinlp::Model::GetTsChEcomRequest;

GetTsChEcomRequest::GetTsChEcomRequest() :
	RpcServiceRequest("alinlp", "2020-06-29", "GetTsChEcom")
{
	setMethod(HttpRequest::Method::Post);
}

GetTsChEcomRequest::~GetTsChEcomRequest()
{}

std::string GetTsChEcomRequest::getType()const
{
	return type_;
}

void GetTsChEcomRequest::setType(const std::string& type)
{
	type_ = type;
	setBodyParameter("Type", type);
}

std::string GetTsChEcomRequest::getServiceCode()const
{
	return serviceCode_;
}

void GetTsChEcomRequest::setServiceCode(const std::string& serviceCode)
{
	serviceCode_ = serviceCode;
	setBodyParameter("ServiceCode", serviceCode);
}

std::string GetTsChEcomRequest::getOriginT()const
{
	return originT_;
}

void GetTsChEcomRequest::setOriginT(const std::string& originT)
{
	originT_ = originT;
	setBodyParameter("OriginT", originT);
}

std::string GetTsChEcomRequest::getOriginQ()const
{
	return originQ_;
}

void GetTsChEcomRequest::setOriginQ(const std::string& originQ)
{
	originQ_ = originQ;
	setBodyParameter("OriginQ", originQ);
}

