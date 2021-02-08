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

#include <alibabacloud/rtc/model/DescribeRecordTemplatesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRecordTemplatesResult::DescribeRecordTemplatesResult() :
	ServiceResult()
{}

DescribeRecordTemplatesResult::DescribeRecordTemplatesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRecordTemplatesResult::~DescribeRecordTemplatesResult()
{}

void DescribeRecordTemplatesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allTemplatesNode = value["Templates"]["Template"];
	for (auto valueTemplatesTemplate : allTemplatesNode)
	{
		_Template templatesObject;
		if(!valueTemplatesTemplate["CreateTime"].isNull())
			templatesObject.createTime = valueTemplatesTemplate["CreateTime"].asString();
		if(!valueTemplatesTemplate["TemplateId"].isNull())
			templatesObject.templateId = valueTemplatesTemplate["TemplateId"].asString();
		if(!valueTemplatesTemplate["Name"].isNull())
			templatesObject.name = valueTemplatesTemplate["Name"].asString();
		if(!valueTemplatesTemplate["MediaEncode"].isNull())
			templatesObject.mediaEncode = std::stoi(valueTemplatesTemplate["MediaEncode"].asString());
		if(!valueTemplatesTemplate["BackgroundColor"].isNull())
			templatesObject.backgroundColor = std::stoi(valueTemplatesTemplate["BackgroundColor"].asString());
		if(!valueTemplatesTemplate["FileSplitInterval"].isNull())
			templatesObject.fileSplitInterval = std::stoi(valueTemplatesTemplate["FileSplitInterval"].asString());
		if(!valueTemplatesTemplate["TaskProfile"].isNull())
			templatesObject.taskProfile = valueTemplatesTemplate["TaskProfile"].asString();
		if(!valueTemplatesTemplate["OssBucket"].isNull())
			templatesObject.ossBucket = valueTemplatesTemplate["OssBucket"].asString();
		if(!valueTemplatesTemplate["OssFilePrefix"].isNull())
			templatesObject.ossFilePrefix = valueTemplatesTemplate["OssFilePrefix"].asString();
		if(!valueTemplatesTemplate["MnsQueue"].isNull())
			templatesObject.mnsQueue = valueTemplatesTemplate["MnsQueue"].asString();
		auto allLayoutIds = value["LayoutIds"]["LayoutId"];
		for (auto value : allLayoutIds)
			templatesObject.layoutIds.push_back(value.asString());
		auto allFormats = value["Formats"]["Format"];
		for (auto value : allFormats)
			templatesObject.formats.push_back(value.asString());
		templates_.push_back(templatesObject);
	}
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stol(value["TotalNum"].asString());
	if(!value["TotalPage"].isNull())
		totalPage_ = std::stol(value["TotalPage"].asString());

}

long DescribeRecordTemplatesResult::getTotalNum()const
{
	return totalNum_;
}

long DescribeRecordTemplatesResult::getTotalPage()const
{
	return totalPage_;
}

std::vector<DescribeRecordTemplatesResult::_Template> DescribeRecordTemplatesResult::getTemplates()const
{
	return templates_;
}

