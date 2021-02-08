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

#include <alibabacloud/multimediaai/model/ListTemplatesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Multimediaai;
using namespace AlibabaCloud::Multimediaai::Model;

ListTemplatesResult::ListTemplatesResult() :
	ServiceResult()
{}

ListTemplatesResult::ListTemplatesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListTemplatesResult::~ListTemplatesResult()
{}

void ListTemplatesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allTemplatesNode = value["Templates"]["Template"];
	for (auto valueTemplatesTemplate : allTemplatesNode)
	{
		_Template templatesObject;
		if(!valueTemplatesTemplate["TemplateId"].isNull())
			templatesObject.templateId = valueTemplatesTemplate["TemplateId"].asString();
		if(!valueTemplatesTemplate["TemplateName"].isNull())
			templatesObject.templateName = valueTemplatesTemplate["TemplateName"].asString();
		if(!valueTemplatesTemplate["Type"].isNull())
			templatesObject.type = std::stoi(valueTemplatesTemplate["Type"].asString());
		if(!valueTemplatesTemplate["IsDefault"].isNull())
			templatesObject.isDefault = valueTemplatesTemplate["IsDefault"].asString() == "true";
		if(!valueTemplatesTemplate["CreateTime"].isNull())
			templatesObject.createTime = valueTemplatesTemplate["CreateTime"].asString();
		if(!valueTemplatesTemplate["UpdateTime"].isNull())
			templatesObject.updateTime = valueTemplatesTemplate["UpdateTime"].asString();
		templates_.push_back(templatesObject);
	}
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stol(value["TotalCount"].asString());
	if(!value["TotalAmount"].isNull())
		totalAmount_ = std::stol(value["TotalAmount"].asString());

}

long ListTemplatesResult::getTotalCount()const
{
	return totalCount_;
}

int ListTemplatesResult::getPageSize()const
{
	return pageSize_;
}

int ListTemplatesResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<ListTemplatesResult::_Template> ListTemplatesResult::getTemplates()const
{
	return templates_;
}

long ListTemplatesResult::getTotalAmount()const
{
	return totalAmount_;
}

