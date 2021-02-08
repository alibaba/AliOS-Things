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

#include <alibabacloud/iot/model/CreateProductResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

CreateProductResult::CreateProductResult() :
	ServiceResult()
{}

CreateProductResult::CreateProductResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateProductResult::~CreateProductResult()
{}

void CreateProductResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["DataFormat"].isNull())
		data_.dataFormat = std::stoi(dataNode["DataFormat"].asString());
	if(!dataNode["Description"].isNull())
		data_.description = dataNode["Description"].asString();
	if(!dataNode["NodeType"].isNull())
		data_.nodeType = std::stoi(dataNode["NodeType"].asString());
	if(!dataNode["ProductKey"].isNull())
		data_.productKey = dataNode["ProductKey"].asString();
	if(!dataNode["ProductName"].isNull())
		data_.productName = dataNode["ProductName"].asString();
	if(!dataNode["AliyunCommodityCode"].isNull())
		data_.aliyunCommodityCode = dataNode["AliyunCommodityCode"].asString();
	if(!dataNode["Id2"].isNull())
		data_.id2 = dataNode["Id2"].asString() == "true";
	if(!dataNode["ProtocolType"].isNull())
		data_.protocolType = dataNode["ProtocolType"].asString();
	if(!dataNode["AuthType"].isNull())
		data_.authType = dataNode["AuthType"].asString();
	if(!dataNode["ProductSecret"].isNull())
		data_.productSecret = dataNode["ProductSecret"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["ProductKey"].isNull())
		productKey_ = value["ProductKey"].asString();

}

CreateProductResult::Data CreateProductResult::getData()const
{
	return data_;
}

std::string CreateProductResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string CreateProductResult::getCode()const
{
	return code_;
}

bool CreateProductResult::getSuccess()const
{
	return success_;
}

std::string CreateProductResult::getProductKey()const
{
	return productKey_;
}

