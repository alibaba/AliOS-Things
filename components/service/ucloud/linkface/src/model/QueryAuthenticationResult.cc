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

#include <alibabacloud/linkface/model/QueryAuthenticationResult.h>
#include <json/json.h>

using namespace AlibabaCloud::LinkFace;
using namespace AlibabaCloud::LinkFace::Model;

QueryAuthenticationResult::QueryAuthenticationResult() :
	ServiceResult()
{}

QueryAuthenticationResult::QueryAuthenticationResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryAuthenticationResult::~QueryAuthenticationResult()
{}

void QueryAuthenticationResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["DataItem"];
	for (auto valueDataDataItem : allDataNode)
	{
		DataItem dataObject;
		if(!valueDataDataItem["LicenseType"].isNull())
			dataObject.licenseType = std::stoi(valueDataDataItem["LicenseType"].asString());
		if(!valueDataDataItem["IotId"].isNull())
			dataObject.iotId = valueDataDataItem["IotId"].asString();
		if(!valueDataDataItem["BeginTime"].isNull())
			dataObject.beginTime = valueDataDataItem["BeginTime"].asString();
		if(!valueDataDataItem["ExpiredTime"].isNull())
			dataObject.expiredTime = valueDataDataItem["ExpiredTime"].asString();
		if(!valueDataDataItem["ApkPubkey"].isNull())
			dataObject.apkPubkey = valueDataDataItem["ApkPubkey"].asString();
		if(!valueDataDataItem["PackageName"].isNull())
			dataObject.packageName = valueDataDataItem["PackageName"].asString();
		if(!valueDataDataItem["ClientId"].isNull())
			dataObject.clientId = valueDataDataItem["ClientId"].asString();
		if(!valueDataDataItem["ProductKey"].isNull())
			dataObject.productKey = valueDataDataItem["ProductKey"].asString();
		if(!valueDataDataItem["DeviceName"].isNull())
			dataObject.deviceName = valueDataDataItem["DeviceName"].asString();
		data_.push_back(dataObject);
	}
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["Page"].isNull())
		page_ = std::stoi(value["Page"].asString());
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string QueryAuthenticationResult::getMessage()const
{
	return message_;
}

int QueryAuthenticationResult::getPageCount()const
{
	return pageCount_;
}

int QueryAuthenticationResult::getPageSize()const
{
	return pageSize_;
}

int QueryAuthenticationResult::getTotal()const
{
	return total_;
}

std::vector<QueryAuthenticationResult::DataItem> QueryAuthenticationResult::getData()const
{
	return data_;
}

int QueryAuthenticationResult::getPage()const
{
	return page_;
}

int QueryAuthenticationResult::getCode()const
{
	return code_;
}

bool QueryAuthenticationResult::getSuccess()const
{
	return success_;
}

