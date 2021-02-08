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

#include <alibabacloud/iot/model/UpdateProductTagsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

UpdateProductTagsResult::UpdateProductTagsResult() :
	ServiceResult()
{}

UpdateProductTagsResult::UpdateProductTagsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

UpdateProductTagsResult::~UpdateProductTagsResult()
{}

void UpdateProductTagsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allInvalidProductTagsNode = value["InvalidProductTags"]["ProductTag"];
	for (auto valueInvalidProductTagsProductTag : allInvalidProductTagsNode)
	{
		ProductTag invalidProductTagsObject;
		if(!valueInvalidProductTagsProductTag["TagKey"].isNull())
			invalidProductTagsObject.tagKey = valueInvalidProductTagsProductTag["TagKey"].asString();
		if(!valueInvalidProductTagsProductTag["TagValue"].isNull())
			invalidProductTagsObject.tagValue = valueInvalidProductTagsProductTag["TagValue"].asString();
		invalidProductTags_.push_back(invalidProductTagsObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();

}

std::vector<UpdateProductTagsResult::ProductTag> UpdateProductTagsResult::getInvalidProductTags()const
{
	return invalidProductTags_;
}

std::string UpdateProductTagsResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string UpdateProductTagsResult::getCode()const
{
	return code_;
}

bool UpdateProductTagsResult::getSuccess()const
{
	return success_;
}

