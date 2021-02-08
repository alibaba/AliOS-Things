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

#include <alibabacloud/imm/model/SearchDocIndexResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

SearchDocIndexResult::SearchDocIndexResult() :
	ServiceResult()
{}

SearchDocIndexResult::SearchDocIndexResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

SearchDocIndexResult::~SearchDocIndexResult()
{}

void SearchDocIndexResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDocInfosNode = value["DocInfos"]["DocInfosItem"];
	for (auto valueDocInfosDocInfosItem : allDocInfosNode)
	{
		DocInfosItem docInfosObject;
		if(!valueDocInfosDocInfosItem["UniqueId"].isNull())
			docInfosObject.uniqueId = valueDocInfosDocInfosItem["UniqueId"].asString();
		if(!valueDocInfosDocInfosItem["SrcUri"].isNull())
			docInfosObject.srcUri = valueDocInfosDocInfosItem["SrcUri"].asString();
		if(!valueDocInfosDocInfosItem["Name"].isNull())
			docInfosObject.name = valueDocInfosDocInfosItem["Name"].asString();
		if(!valueDocInfosDocInfosItem["ContentType"].isNull())
			docInfosObject.contentType = valueDocInfosDocInfosItem["ContentType"].asString();
		if(!valueDocInfosDocInfosItem["LastModified"].isNull())
			docInfosObject.lastModified = valueDocInfosDocInfosItem["LastModified"].asString();
		if(!valueDocInfosDocInfosItem["Size"].isNull())
			docInfosObject.size = std::stoi(valueDocInfosDocInfosItem["Size"].asString());
		if(!valueDocInfosDocInfosItem["PageNum"].isNull())
			docInfosObject.pageNum = std::stoi(valueDocInfosDocInfosItem["PageNum"].asString());
		if(!valueDocInfosDocInfosItem["CustomKey1"].isNull())
			docInfosObject.customKey1 = valueDocInfosDocInfosItem["CustomKey1"].asString();
		if(!valueDocInfosDocInfosItem["CustomKey2"].isNull())
			docInfosObject.customKey2 = valueDocInfosDocInfosItem["CustomKey2"].asString();
		if(!valueDocInfosDocInfosItem["CustomKey3"].isNull())
			docInfosObject.customKey3 = valueDocInfosDocInfosItem["CustomKey3"].asString();
		if(!valueDocInfosDocInfosItem["CustomKey4"].isNull())
			docInfosObject.customKey4 = valueDocInfosDocInfosItem["CustomKey4"].asString();
		if(!valueDocInfosDocInfosItem["CustomKey5"].isNull())
			docInfosObject.customKey5 = valueDocInfosDocInfosItem["CustomKey5"].asString();
		if(!valueDocInfosDocInfosItem["CustomKey6"].isNull())
			docInfosObject.customKey6 = valueDocInfosDocInfosItem["CustomKey6"].asString();
		docInfos_.push_back(docInfosObject);
	}

}

std::vector<SearchDocIndexResult::DocInfosItem> SearchDocIndexResult::getDocInfos()const
{
	return docInfos_;
}

