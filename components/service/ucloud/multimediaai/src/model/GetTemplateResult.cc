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

#include <alibabacloud/multimediaai/model/GetTemplateResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Multimediaai;
using namespace AlibabaCloud::Multimediaai::Model;

GetTemplateResult::GetTemplateResult() :
	ServiceResult()
{}

GetTemplateResult::GetTemplateResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetTemplateResult::~GetTemplateResult()
{}

void GetTemplateResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["TemplateId"].isNull())
		templateId_ = value["TemplateId"].asString();
	if(!value["TemplateName"].isNull())
		templateName_ = value["TemplateName"].asString();
	if(!value["Content"].isNull())
		content_ = value["Content"].asString();
	if(!value["Category"].isNull())
		category_ = std::stoi(value["Category"].asString());
	if(!value["IsDefault"].isNull())
		isDefault_ = value["IsDefault"].asString() == "true";
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["UpdateTime"].isNull())
		updateTime_ = value["UpdateTime"].asString();

}

bool GetTemplateResult::getIsDefault()const
{
	return isDefault_;
}

int GetTemplateResult::getCategory()const
{
	return category_;
}

std::string GetTemplateResult::getContent()const
{
	return content_;
}

std::string GetTemplateResult::getCreateTime()const
{
	return createTime_;
}

std::string GetTemplateResult::getUpdateTime()const
{
	return updateTime_;
}

std::string GetTemplateResult::getTemplateName()const
{
	return templateName_;
}

std::string GetTemplateResult::getTemplateId()const
{
	return templateId_;
}

