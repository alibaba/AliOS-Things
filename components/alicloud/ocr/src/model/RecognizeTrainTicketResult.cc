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

#include <alibabacloud/ocr/model/RecognizeTrainTicketResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeTrainTicketResult::RecognizeTrainTicketResult() :
	ServiceResult()
{}

RecognizeTrainTicketResult::RecognizeTrainTicketResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeTrainTicketResult::~RecognizeTrainTicketResult()
{}

void RecognizeTrainTicketResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Date"].isNull())
		data_.date = dataNode["Date"].asString();
	if(!dataNode["Destination"].isNull())
		data_.destination = dataNode["Destination"].asString();
	if(!dataNode["Level"].isNull())
		data_.level = dataNode["Level"].asString();
	if(!dataNode["Number"].isNull())
		data_.number = dataNode["Number"].asString();
	if(!dataNode["Name"].isNull())
		data_.name = dataNode["Name"].asString();
	if(!dataNode["DepartureStation"].isNull())
		data_.departureStation = dataNode["DepartureStation"].asString();
	if(!dataNode["Seat"].isNull())
		data_.seat = dataNode["Seat"].asString();
	if(!dataNode["Price"].isNull())
		data_.price = std::stof(dataNode["Price"].asString());

}

RecognizeTrainTicketResult::Data RecognizeTrainTicketResult::getData()const
{
	return data_;
}

