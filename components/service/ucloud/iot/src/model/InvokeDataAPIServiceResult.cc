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

#include <alibabacloud/iot/model/InvokeDataAPIServiceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

InvokeDataAPIServiceResult::InvokeDataAPIServiceResult() :
	ServiceResult()
{}

InvokeDataAPIServiceResult::InvokeDataAPIServiceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

InvokeDataAPIServiceResult::~InvokeDataAPIServiceResult()
{}

void InvokeDataAPIServiceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["PageNo"].isNull())
		data_.pageNo = std::stoi(dataNode["PageNo"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["ApiSrn"].isNull())
		data_.apiSrn = dataNode["ApiSrn"].asString();
		auto allFieldNameList = dataNode["FieldNameList"]["FieldNameList"];
		for (auto value : allFieldNameList)
			data_.fieldNameList.push_back(value.asString());
		auto allResultList = dataNode["ResultList"]["ResultList"];
		for (auto value : allResultList)
			data_.resultList.push_back(value.asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

InvokeDataAPIServiceResult::Data InvokeDataAPIServiceResult::getData()const
{
	return data_;
}

std::string InvokeDataAPIServiceResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string InvokeDataAPIServiceResult::getCode()const
{
	return code_;
}

bool InvokeDataAPIServiceResult::getSuccess()const
{
	return success_;
}

