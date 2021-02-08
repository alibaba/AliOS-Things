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

#include <alibabacloud/iot/model/ListThingModelVersionResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

ListThingModelVersionResult::ListThingModelVersionResult() :
	ServiceResult()
{}

ListThingModelVersionResult::ListThingModelVersionResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListThingModelVersionResult::~ListThingModelVersionResult()
{}

void ListThingModelVersionResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allModelVersionsNode = dataNode["ModelVersions"]["ModelVersion"];
	for (auto dataNodeModelVersionsModelVersion : allModelVersionsNode)
	{
		Data::ModelVersion modelVersionObject;
		if(!dataNodeModelVersionsModelVersion["ModelVersion"].isNull())
			modelVersionObject.modelVersion = dataNodeModelVersionsModelVersion["ModelVersion"].asString();
		if(!dataNodeModelVersionsModelVersion["Description"].isNull())
			modelVersionObject.description = dataNodeModelVersionsModelVersion["Description"].asString();
		if(!dataNodeModelVersionsModelVersion["GmtCreate"].isNull())
			modelVersionObject.gmtCreate = std::stol(dataNodeModelVersionsModelVersion["GmtCreate"].asString());
		data_.modelVersions.push_back(modelVersionObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

ListThingModelVersionResult::Data ListThingModelVersionResult::getData()const
{
	return data_;
}

std::string ListThingModelVersionResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string ListThingModelVersionResult::getCode()const
{
	return code_;
}

bool ListThingModelVersionResult::getSuccess()const
{
	return success_;
}

