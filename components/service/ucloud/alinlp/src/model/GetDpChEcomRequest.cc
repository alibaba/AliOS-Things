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

#include <alibabacloud/alinlp/model/GetDpChEcomRequest.h>

using AlibabaCloud::Alinlp::Model::GetDpChEcomRequest;

GetDpChEcomRequest::GetDpChEcomRequest() :
	RpcServiceRequest("alinlp", "2020-06-29", "GetDpChEcom")
{
	setMethod(HttpRequest::Method::Post);
}

GetDpChEcomRequest::~GetDpChEcomRequest()
{}

std::string GetDpChEcomRequest::getServiceCode()const
{
	return serviceCode_;
}

void GetDpChEcomRequest::setServiceCode(const std::string& serviceCode)
{
	serviceCode_ = serviceCode;
	setBodyParameter("ServiceCode", serviceCode);
}

std::string GetDpChEcomRequest::getText()const
{
	return text_;
}

void GetDpChEcomRequest::setText(const std::string& text)
{
	text_ = text;
	setBodyParameter("Text", text);
}

