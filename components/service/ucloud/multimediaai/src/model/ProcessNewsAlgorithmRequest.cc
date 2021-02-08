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

#include <alibabacloud/multimediaai/model/ProcessNewsAlgorithmRequest.h>

using AlibabaCloud::Multimediaai::Model::ProcessNewsAlgorithmRequest;

ProcessNewsAlgorithmRequest::ProcessNewsAlgorithmRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "ProcessNewsAlgorithm")
{
	setMethod(HttpRequest::Method::Post);
}

ProcessNewsAlgorithmRequest::~ProcessNewsAlgorithmRequest()
{}

std::string ProcessNewsAlgorithmRequest::getData()const
{
	return data_;
}

void ProcessNewsAlgorithmRequest::setData(const std::string& data)
{
	data_ = data;
	setBodyParameter("Data", data);
}

std::string ProcessNewsAlgorithmRequest::getAppKey()const
{
	return appKey_;
}

void ProcessNewsAlgorithmRequest::setAppKey(const std::string& appKey)
{
	appKey_ = appKey;
	setBodyParameter("AppKey", appKey);
}

