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

#include <alibabacloud/live/model/DescribeHtmlResourceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeHtmlResourceResult::DescribeHtmlResourceResult() :
	ServiceResult()
{}

DescribeHtmlResourceResult::DescribeHtmlResourceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeHtmlResourceResult::~DescribeHtmlResourceResult()
{}

void DescribeHtmlResourceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto htmlResourceNode = value["HtmlResource"];
	if(!htmlResourceNode["HtmlResourceId"].isNull())
		htmlResource_.htmlResourceId = htmlResourceNode["HtmlResourceId"].asString();
	if(!htmlResourceNode["HtmlUrl"].isNull())
		htmlResource_.htmlUrl = htmlResourceNode["HtmlUrl"].asString();
	if(!htmlResourceNode["HtmlContent"].isNull())
		htmlResource_.htmlContent = htmlResourceNode["HtmlContent"].asString();
	if(!htmlResourceNode["CasterId"].isNull())
		htmlResource_.casterId = htmlResourceNode["CasterId"].asString();
	if(!htmlResourceNode["Config"].isNull())
		htmlResource_.config = htmlResourceNode["Config"].asString();
	if(!htmlResourceNode["StreamId"].isNull())
		htmlResource_.streamId = htmlResourceNode["StreamId"].asString();

}

DescribeHtmlResourceResult::HtmlResource DescribeHtmlResourceResult::getHtmlResource()const
{
	return htmlResource_;
}

