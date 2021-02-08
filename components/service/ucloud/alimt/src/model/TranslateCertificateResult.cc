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

#include <alibabacloud/alimt/model/TranslateCertificateResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Alimt;
using namespace AlibabaCloud::Alimt::Model;

TranslateCertificateResult::TranslateCertificateResult() :
	ServiceResult()
{}

TranslateCertificateResult::TranslateCertificateResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

TranslateCertificateResult::~TranslateCertificateResult()
{}

void TranslateCertificateResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allTranslatedValuesNode = dataNode["TranslatedValues"]["CertificateTranslateItemDTO"];
	for (auto dataNodeTranslatedValuesCertificateTranslateItemDTO : allTranslatedValuesNode)
	{
		Data::CertificateTranslateItemDTO certificateTranslateItemDTOObject;
		if(!dataNodeTranslatedValuesCertificateTranslateItemDTO["Key"].isNull())
			certificateTranslateItemDTOObject.key = dataNodeTranslatedValuesCertificateTranslateItemDTO["Key"].asString();
		if(!dataNodeTranslatedValuesCertificateTranslateItemDTO["Value"].isNull())
			certificateTranslateItemDTOObject.value = dataNodeTranslatedValuesCertificateTranslateItemDTO["Value"].asString();
		if(!dataNodeTranslatedValuesCertificateTranslateItemDTO["ValueTranslation"].isNull())
			certificateTranslateItemDTOObject.valueTranslation = dataNodeTranslatedValuesCertificateTranslateItemDTO["ValueTranslation"].asString();
		if(!dataNodeTranslatedValuesCertificateTranslateItemDTO["KeyTranslation"].isNull())
			certificateTranslateItemDTOObject.keyTranslation = dataNodeTranslatedValuesCertificateTranslateItemDTO["KeyTranslation"].asString();
		data_.translatedValues.push_back(certificateTranslateItemDTOObject);
	}

}

TranslateCertificateResult::Data TranslateCertificateResult::getData()const
{
	return data_;
}

