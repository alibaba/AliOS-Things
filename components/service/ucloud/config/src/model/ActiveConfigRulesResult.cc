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

#include <alibabacloud/config/model/ActiveConfigRulesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

ActiveConfigRulesResult::ActiveConfigRulesResult() :
	ServiceResult()
{}

ActiveConfigRulesResult::ActiveConfigRulesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ActiveConfigRulesResult::~ActiveConfigRulesResult()
{}

void ActiveConfigRulesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto operateRuleResultNode = value["OperateRuleResult"];
	auto allOperateRuleItemListNode = operateRuleResultNode["OperateRuleItemList"]["OperateRuleItem"];
	for (auto operateRuleResultNodeOperateRuleItemListOperateRuleItem : allOperateRuleItemListNode)
	{
		OperateRuleResult::OperateRuleItem operateRuleItemObject;
		if(!operateRuleResultNodeOperateRuleItemListOperateRuleItem["ConfigRuleId"].isNull())
			operateRuleItemObject.configRuleId = operateRuleResultNodeOperateRuleItemListOperateRuleItem["ConfigRuleId"].asString();
		if(!operateRuleResultNodeOperateRuleItemListOperateRuleItem["ErrorCode"].isNull())
			operateRuleItemObject.errorCode = operateRuleResultNodeOperateRuleItemListOperateRuleItem["ErrorCode"].asString();
		if(!operateRuleResultNodeOperateRuleItemListOperateRuleItem["Success"].isNull())
			operateRuleItemObject.success = operateRuleResultNodeOperateRuleItemListOperateRuleItem["Success"].asString() == "true";
		operateRuleResult_.operateRuleItemList.push_back(operateRuleItemObject);
	}

}

ActiveConfigRulesResult::OperateRuleResult ActiveConfigRulesResult::getOperateRuleResult()const
{
	return operateRuleResult_;
}

