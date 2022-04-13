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

#include <alibabacloud/ocr/model/RecognizeTaxiInvoiceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeTaxiInvoiceResult::RecognizeTaxiInvoiceResult() :
	ServiceResult()
{}

RecognizeTaxiInvoiceResult::RecognizeTaxiInvoiceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeTaxiInvoiceResult::~RecognizeTaxiInvoiceResult()
{}

void RecognizeTaxiInvoiceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allInvoicesNode = dataNode["Invoices"]["Invoice"];
	for (auto dataNodeInvoicesInvoice : allInvoicesNode)
	{
		Data::Invoice invoiceObject;
		if(!dataNodeInvoicesInvoice["RotateType"].isNull())
			invoiceObject.rotateType = std::stoi(dataNodeInvoicesInvoice["RotateType"].asString());
		auto allItemsNode = dataNodeInvoicesInvoice["Items"]["Item"];
		for (auto dataNodeInvoicesInvoiceItemsItem : allItemsNode)
		{
			Data::Invoice::Item itemsObject;
			if(!dataNodeInvoicesInvoiceItemsItem["Text"].isNull())
				itemsObject.text = dataNodeInvoicesInvoiceItemsItem["Text"].asString();
			auto itemRoiNode = value["ItemRoi"];
			if(!itemRoiNode["Angle"].isNull())
				itemsObject.itemRoi.angle = std::stof(itemRoiNode["Angle"].asString());
			auto centerNode = itemRoiNode["Center"];
			if(!centerNode["X"].isNull())
				itemsObject.itemRoi.center.x = std::stof(centerNode["X"].asString());
			if(!centerNode["Y"].isNull())
				itemsObject.itemRoi.center.y = std::stof(centerNode["Y"].asString());
			auto sizeNode = itemRoiNode["Size"];
			if(!sizeNode["H"].isNull())
				itemsObject.itemRoi.size.h = std::stof(sizeNode["H"].asString());
			if(!sizeNode["W"].isNull())
				itemsObject.itemRoi.size.w = std::stof(sizeNode["W"].asString());
			invoiceObject.items.push_back(itemsObject);
		}
		auto invoiceRoiNode = value["InvoiceRoi"];
		if(!invoiceRoiNode["H"].isNull())
			invoiceObject.invoiceRoi.h = std::stof(invoiceRoiNode["H"].asString());
		if(!invoiceRoiNode["W"].isNull())
			invoiceObject.invoiceRoi.w = std::stof(invoiceRoiNode["W"].asString());
		if(!invoiceRoiNode["X"].isNull())
			invoiceObject.invoiceRoi.x = std::stof(invoiceRoiNode["X"].asString());
		if(!invoiceRoiNode["Y"].isNull())
			invoiceObject.invoiceRoi.y = std::stof(invoiceRoiNode["Y"].asString());
		data_.invoices.push_back(invoiceObject);
	}

}

RecognizeTaxiInvoiceResult::Data RecognizeTaxiInvoiceResult::getData()const
{
	return data_;
}

