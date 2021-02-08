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

#include <alibabacloud/iot/model/QueryProductListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryProductListResult::QueryProductListResult() :
	ServiceResult()
{}

QueryProductListResult::QueryProductListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryProductListResult::~QueryProductListResult()
{}

void QueryProductListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["CurrentPage"].isNull())
		data_.currentPage = std::stoi(dataNode["CurrentPage"].asString());
	if(!dataNode["PageCount"].isNull())
		data_.pageCount = std::stoi(dataNode["PageCount"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["Total"].isNull())
		data_.total = std::stoi(dataNode["Total"].asString());
	auto allListNode = dataNode["List"]["ProductInfo"];
	for (auto dataNodeListProductInfo : allListNode)
	{
		Data::ProductInfo productInfoObject;
		if(!dataNodeListProductInfo["GmtCreate"].isNull())
			productInfoObject.gmtCreate = std::stol(dataNodeListProductInfo["GmtCreate"].asString());
		if(!dataNodeListProductInfo["DataFormat"].isNull())
			productInfoObject.dataFormat = std::stoi(dataNodeListProductInfo["DataFormat"].asString());
		if(!dataNodeListProductInfo["Description"].isNull())
			productInfoObject.description = dataNodeListProductInfo["Description"].asString();
		if(!dataNodeListProductInfo["DeviceCount"].isNull())
			productInfoObject.deviceCount = std::stoi(dataNodeListProductInfo["DeviceCount"].asString());
		if(!dataNodeListProductInfo["NodeType"].isNull())
			productInfoObject.nodeType = std::stoi(dataNodeListProductInfo["NodeType"].asString());
		if(!dataNodeListProductInfo["ProductKey"].isNull())
			productInfoObject.productKey = dataNodeListProductInfo["ProductKey"].asString();
		if(!dataNodeListProductInfo["ProductName"].isNull())
			productInfoObject.productName = dataNodeListProductInfo["ProductName"].asString();
		if(!dataNodeListProductInfo["AuthType"].isNull())
			productInfoObject.authType = dataNodeListProductInfo["AuthType"].asString();
		data_.list.push_back(productInfoObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryProductListResult::Data QueryProductListResult::getData()const
{
	return data_;
}

std::string QueryProductListResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryProductListResult::getCode()const
{
	return code_;
}

bool QueryProductListResult::getSuccess()const
{
	return success_;
}

