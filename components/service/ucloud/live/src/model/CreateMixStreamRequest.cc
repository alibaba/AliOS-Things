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

#include <alibabacloud/live/model/CreateMixStreamRequest.h>

using AlibabaCloud::Live::Model::CreateMixStreamRequest;

CreateMixStreamRequest::CreateMixStreamRequest() :
	RpcServiceRequest("live", "2016-11-01", "CreateMixStream")
{
	setMethod(HttpRequest::Method::Post);
}

CreateMixStreamRequest::~CreateMixStreamRequest()
{}

std::string CreateMixStreamRequest::getOutputConfig()const
{
	return outputConfig_;
}

void CreateMixStreamRequest::setOutputConfig(const std::string& outputConfig)
{
	outputConfig_ = outputConfig;
	setParameter("OutputConfig", outputConfig);
}

std::string CreateMixStreamRequest::getLayoutId()const
{
	return layoutId_;
}

void CreateMixStreamRequest::setLayoutId(const std::string& layoutId)
{
	layoutId_ = layoutId;
	setParameter("LayoutId", layoutId);
}

std::string CreateMixStreamRequest::getDomainName()const
{
	return domainName_;
}

void CreateMixStreamRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string CreateMixStreamRequest::getInputStreamList()const
{
	return inputStreamList_;
}

void CreateMixStreamRequest::setInputStreamList(const std::string& inputStreamList)
{
	inputStreamList_ = inputStreamList;
	setParameter("InputStreamList", inputStreamList);
}

long CreateMixStreamRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateMixStreamRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateMixStreamRequest::getCallbackConfig()const
{
	return callbackConfig_;
}

void CreateMixStreamRequest::setCallbackConfig(const std::string& callbackConfig)
{
	callbackConfig_ = callbackConfig;
	setParameter("CallbackConfig", callbackConfig);
}

