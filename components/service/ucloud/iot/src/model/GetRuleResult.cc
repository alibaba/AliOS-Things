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

#include <alibabacloud/iot/model/GetRuleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetRuleResult::GetRuleResult() :
	ServiceResult()
{}

GetRuleResult::GetRuleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetRuleResult::~GetRuleResult()
{}

void GetRuleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto ruleInfoNode = value["RuleInfo"];
	if(!ruleInfoNode["Id"].isNull())
		ruleInfo_.id = std::stol(ruleInfoNode["Id"].asString());
	if(!ruleInfoNode["Select"].isNull())
		ruleInfo_.select = ruleInfoNode["Select"].asString();
	if(!ruleInfoNode["Topic"].isNull())
		ruleInfo_.topic = ruleInfoNode["Topic"].asString();
	if(!ruleInfoNode["ShortTopic"].isNull())
		ruleInfo_.shortTopic = ruleInfoNode["ShortTopic"].asString();
	if(!ruleInfoNode["Where"].isNull())
		ruleInfo_.where = ruleInfoNode["Where"].asString();
	if(!ruleInfoNode["Status"].isNull())
		ruleInfo_.status = ruleInfoNode["Status"].asString();
	if(!ruleInfoNode["CreateUserId"].isNull())
		ruleInfo_.createUserId = std::stol(ruleInfoNode["CreateUserId"].asString());
	if(!ruleInfoNode["ProductKey"].isNull())
		ruleInfo_.productKey = ruleInfoNode["ProductKey"].asString();
	if(!ruleInfoNode["Name"].isNull())
		ruleInfo_.name = ruleInfoNode["Name"].asString();
	if(!ruleInfoNode["RuleDesc"].isNull())
		ruleInfo_.ruleDesc = ruleInfoNode["RuleDesc"].asString();
	if(!ruleInfoNode["DataType"].isNull())
		ruleInfo_.dataType = ruleInfoNode["DataType"].asString();
	if(!ruleInfoNode["Created"].isNull())
		ruleInfo_.created = ruleInfoNode["Created"].asString();
	if(!ruleInfoNode["UtcCreated"].isNull())
		ruleInfo_.utcCreated = ruleInfoNode["UtcCreated"].asString();
	if(!ruleInfoNode["Modified"].isNull())
		ruleInfo_.modified = ruleInfoNode["Modified"].asString();
	if(!ruleInfoNode["UtcModified"].isNull())
		ruleInfo_.utcModified = ruleInfoNode["UtcModified"].asString();
	if(!ruleInfoNode["TopicType"].isNull())
		ruleInfo_.topicType = std::stoi(ruleInfoNode["TopicType"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

GetRuleResult::RuleInfo GetRuleResult::getRuleInfo()const
{
	return ruleInfo_;
}

std::string GetRuleResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetRuleResult::getCode()const
{
	return code_;
}

bool GetRuleResult::getSuccess()const
{
	return success_;
}

