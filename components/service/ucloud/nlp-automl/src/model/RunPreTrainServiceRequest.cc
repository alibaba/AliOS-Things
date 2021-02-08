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

#include <alibabacloud/nlp-automl/model/RunPreTrainServiceRequest.h>

using AlibabaCloud::Nlp_automl::Model::RunPreTrainServiceRequest;

RunPreTrainServiceRequest::RunPreTrainServiceRequest() :
	RpcServiceRequest("nlp-automl", "2019-11-11", "RunPreTrainService")
{
	setMethod(HttpRequest::Method::Post);
}

RunPreTrainServiceRequest::~RunPreTrainServiceRequest()
{}

std::string RunPreTrainServiceRequest::getPredictContent()const
{
	return predictContent_;
}

void RunPreTrainServiceRequest::setPredictContent(const std::string& predictContent)
{
	predictContent_ = predictContent;
	setBodyParameter("PredictContent", predictContent);
}

std::string RunPreTrainServiceRequest::getServiceVersion()const
{
	return serviceVersion_;
}

void RunPreTrainServiceRequest::setServiceVersion(const std::string& serviceVersion)
{
	serviceVersion_ = serviceVersion;
	setBodyParameter("ServiceVersion", serviceVersion);
}

std::string RunPreTrainServiceRequest::getServiceName()const
{
	return serviceName_;
}

void RunPreTrainServiceRequest::setServiceName(const std::string& serviceName)
{
	serviceName_ = serviceName;
	setBodyParameter("ServiceName", serviceName);
}

