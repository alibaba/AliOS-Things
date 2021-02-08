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

#include <alibabacloud/live/model/DeleteLiveStreamTranscodeRequest.h>

using AlibabaCloud::Live::Model::DeleteLiveStreamTranscodeRequest;

DeleteLiveStreamTranscodeRequest::DeleteLiveStreamTranscodeRequest() :
	RpcServiceRequest("live", "2016-11-01", "DeleteLiveStreamTranscode")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteLiveStreamTranscodeRequest::~DeleteLiveStreamTranscodeRequest()
{}

std::string DeleteLiveStreamTranscodeRequest::get_Template()const
{
	return _template_;
}

void DeleteLiveStreamTranscodeRequest::set_Template(const std::string& _template)
{
	_template_ = _template;
	setParameter("_Template", _template);
}

std::string DeleteLiveStreamTranscodeRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeleteLiveStreamTranscodeRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DeleteLiveStreamTranscodeRequest::getApp()const
{
	return app_;
}

void DeleteLiveStreamTranscodeRequest::setApp(const std::string& app)
{
	app_ = app;
	setParameter("App", app);
}

long DeleteLiveStreamTranscodeRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteLiveStreamTranscodeRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DeleteLiveStreamTranscodeRequest::getDomain()const
{
	return domain_;
}

void DeleteLiveStreamTranscodeRequest::setDomain(const std::string& domain)
{
	domain_ = domain;
	setParameter("Domain", domain);
}

