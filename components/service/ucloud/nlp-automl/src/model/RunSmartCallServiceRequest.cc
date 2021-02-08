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

#include <alibabacloud/nlp-automl/model/RunSmartCallServiceRequest.h>

using AlibabaCloud::Nlp_automl::Model::RunSmartCallServiceRequest;

RunSmartCallServiceRequest::RunSmartCallServiceRequest() :
	RpcServiceRequest("nlp-automl", "2019-11-11", "RunSmartCallService")
{
	setMethod(HttpRequest::Method::Post);
}

RunSmartCallServiceRequest::~RunSmartCallServiceRequest()
{}

std::string RunSmartCallServiceRequest::getSessionId()const
{
	return sessionId_;
}

void RunSmartCallServiceRequest::setSessionId(const std::string& sessionId)
{
	sessionId_ = sessionId;
	setBodyParameter("SessionId", sessionId);
}

long RunSmartCallServiceRequest::getRobotId()const
{
	return robotId_;
}

void RunSmartCallServiceRequest::setRobotId(long robotId)
{
	robotId_ = robotId;
	setBodyParameter("RobotId", std::to_string(robotId));
}

std::string RunSmartCallServiceRequest::getParamContent()const
{
	return paramContent_;
}

void RunSmartCallServiceRequest::setParamContent(const std::string& paramContent)
{
	paramContent_ = paramContent;
	setBodyParameter("ParamContent", paramContent);
}

std::string RunSmartCallServiceRequest::getServiceName()const
{
	return serviceName_;
}

void RunSmartCallServiceRequest::setServiceName(const std::string& serviceName)
{
	serviceName_ = serviceName;
	setBodyParameter("ServiceName", serviceName);
}

