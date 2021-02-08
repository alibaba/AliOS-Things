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

#include <alibabacloud/iot/model/ListRuleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

ListRuleResult::ListRuleResult() :
	ServiceResult()
{}

ListRuleResult::ListRuleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListRuleResult::~ListRuleResult()
{}

void ListRuleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["RuleInfo"];
	for (auto valueDataRuleInfo : allDataNode)
	{
		RuleInfo dataObject;
		if(!valueDataRuleInfo["Id"].isNull())
			dataObject.id = std::stol(valueDataRuleInfo["Id"].asString());
		if(!valueDataRuleInfo["Select"].isNull())
			dataObject.select = valueDataRuleInfo["Select"].asString();
		if(!valueDataRuleInfo["Topic"].isNull())
			dataObject.topic = valueDataRuleInfo["Topic"].asString();
		if(!valueDataRuleInfo["ShortTopic"].isNull())
			dataObject.shortTopic = valueDataRuleInfo["ShortTopic"].asString();
		if(!valueDataRuleInfo["Where"].isNull())
			dataObject.where = valueDataRuleInfo["Where"].asString();
		if(!valueDataRuleInfo["Status"].isNull())
			dataObject.status = valueDataRuleInfo["Status"].asString();
		if(!valueDataRuleInfo["CreateUserId"].isNull())
			dataObject.createUserId = std::stol(valueDataRuleInfo["CreateUserId"].asString());
		if(!valueDataRuleInfo["ProductKey"].isNull())
			dataObject.productKey = valueDataRuleInfo["ProductKey"].asString();
		if(!valueDataRuleInfo["Name"].isNull())
			dataObject.name = valueDataRuleInfo["Name"].asString();
		if(!valueDataRuleInfo["DataType"].isNull())
			dataObject.dataType = valueDataRuleInfo["DataType"].asString();
		if(!valueDataRuleInfo["RuleDesc"].isNull())
			dataObject.ruleDesc = valueDataRuleInfo["RuleDesc"].asString();
		if(!valueDataRuleInfo["Created"].isNull())
			dataObject.created = valueDataRuleInfo["Created"].asString();
		if(!valueDataRuleInfo["UtcCreated"].isNull())
			dataObject.utcCreated = valueDataRuleInfo["UtcCreated"].asString();
		if(!valueDataRuleInfo["Modified"].isNull())
			dataObject.modified = valueDataRuleInfo["Modified"].asString();
		if(!valueDataRuleInfo["UtcModified"].isNull())
			dataObject.utcModified = valueDataRuleInfo["UtcModified"].asString();
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Page"].isNull())
		page_ = std::stoi(value["Page"].asString());
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());

}

int ListRuleResult::getPageSize()const
{
	return pageSize_;
}

int ListRuleResult::getTotal()const
{
	return total_;
}

std::vector<ListRuleResult::RuleInfo> ListRuleResult::getData()const
{
	return data_;
}

int ListRuleResult::getPage()const
{
	return page_;
}

std::string ListRuleResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string ListRuleResult::getCode()const
{
	return code_;
}

bool ListRuleResult::getSuccess()const
{
	return success_;
}

