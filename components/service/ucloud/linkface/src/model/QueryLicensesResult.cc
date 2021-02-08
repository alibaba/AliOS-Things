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

#include <alibabacloud/linkface/model/QueryLicensesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::LinkFace;
using namespace AlibabaCloud::LinkFace::Model;

QueryLicensesResult::QueryLicensesResult() :
	ServiceResult()
{}

QueryLicensesResult::QueryLicensesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryLicensesResult::~QueryLicensesResult()
{}

void QueryLicensesResult::parse(const std::string &payload)
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
		if(!valueDataDataItem["Quantity"].isNull())
			dataObject.quantity = std::stoi(valueDataDataItem["Quantity"].asString());
		if(!valueDataDataItem["CostQuantity"].isNull())
			dataObject.costQuantity = std::stoi(valueDataDataItem["CostQuantity"].asString());
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

std::string QueryLicensesResult::getMessage()const
{
	return message_;
}

int QueryLicensesResult::getPageCount()const
{
	return pageCount_;
}

int QueryLicensesResult::getPageSize()const
{
	return pageSize_;
}

int QueryLicensesResult::getTotal()const
{
	return total_;
}

std::vector<QueryLicensesResult::DataItem> QueryLicensesResult::getData()const
{
	return data_;
}

int QueryLicensesResult::getPage()const
{
	return page_;
}

int QueryLicensesResult::getCode()const
{
	return code_;
}

bool QueryLicensesResult::getSuccess()const
{
	return success_;
}

