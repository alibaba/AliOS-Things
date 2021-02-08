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

#include <alibabacloud/multimediaai/model/UpdateTemplateRequest.h>

using AlibabaCloud::Multimediaai::Model::UpdateTemplateRequest;

UpdateTemplateRequest::UpdateTemplateRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "UpdateTemplate")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateTemplateRequest::~UpdateTemplateRequest()
{}

std::string UpdateTemplateRequest::getTemplateId()const
{
	return templateId_;
}

void UpdateTemplateRequest::setTemplateId(const std::string& templateId)
{
	templateId_ = templateId;
	setParameter("TemplateId", templateId);
}

int UpdateTemplateRequest::getType()const
{
	return type_;
}

void UpdateTemplateRequest::setType(int type)
{
	type_ = type;
	setParameter("Type", std::to_string(type));
}

std::string UpdateTemplateRequest::getContent()const
{
	return content_;
}

void UpdateTemplateRequest::setContent(const std::string& content)
{
	content_ = content;
	setParameter("Content", content);
}

std::string UpdateTemplateRequest::getTemplateName()const
{
	return templateName_;
}

void UpdateTemplateRequest::setTemplateName(const std::string& templateName)
{
	templateName_ = templateName;
	setParameter("TemplateName", templateName);
}

bool UpdateTemplateRequest::getIsDefault()const
{
	return isDefault_;
}

void UpdateTemplateRequest::setIsDefault(bool isDefault)
{
	isDefault_ = isDefault;
	setParameter("IsDefault", isDefault ? "true" : "false");
}

