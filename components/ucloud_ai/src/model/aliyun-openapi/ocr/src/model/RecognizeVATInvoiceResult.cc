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

#include <alibabacloud/ocr/model/RecognizeVATInvoiceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeVATInvoiceResult::RecognizeVATInvoiceResult() :
	ServiceResult()
{}

RecognizeVATInvoiceResult::RecognizeVATInvoiceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeVATInvoiceResult::~RecognizeVATInvoiceResult()
{}

void RecognizeVATInvoiceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto contentNode = dataNode["Content"];
	if(!contentNode["InvoiceCode"].isNull())
		data_.content.invoiceCode = contentNode["InvoiceCode"].asString();
	if(!contentNode["InvoiceNo"].isNull())
		data_.content.invoiceNo = contentNode["InvoiceNo"].asString();
	if(!contentNode["InvoiceDate"].isNull())
		data_.content.invoiceDate = contentNode["InvoiceDate"].asString();
	if(!contentNode["AntiFakeCode"].isNull())
		data_.content.antiFakeCode = contentNode["AntiFakeCode"].asString();
	if(!contentNode["PayerName"].isNull())
		data_.content.payerName = contentNode["PayerName"].asString();
	if(!contentNode["PayerRegisterNo"].isNull())
		data_.content.payerRegisterNo = contentNode["PayerRegisterNo"].asString();
	if(!contentNode["PayerAddress"].isNull())
		data_.content.payerAddress = contentNode["PayerAddress"].asString();
	if(!contentNode["PayerBankName"].isNull())
		data_.content.payerBankName = contentNode["PayerBankName"].asString();
	if(!contentNode["WithoutTaxAmount"].isNull())
		data_.content.withoutTaxAmount = contentNode["WithoutTaxAmount"].asString();
	if(!contentNode["TaxAmount"].isNull())
		data_.content.taxAmount = contentNode["TaxAmount"].asString();
	if(!contentNode["SumAmount"].isNull())
		data_.content.sumAmount = contentNode["SumAmount"].asString();
	if(!contentNode["InvoiceAmount"].isNull())
		data_.content.invoiceAmount = contentNode["InvoiceAmount"].asString();
	if(!contentNode["PayeeName"].isNull())
		data_.content.payeeName = contentNode["PayeeName"].asString();
	if(!contentNode["PayeeRegisterNo"].isNull())
		data_.content.payeeRegisterNo = contentNode["PayeeRegisterNo"].asString();
	if(!contentNode["PayeeAddress"].isNull())
		data_.content.payeeAddress = contentNode["PayeeAddress"].asString();
	if(!contentNode["PayeeBankName"].isNull())
		data_.content.payeeBankName = contentNode["PayeeBankName"].asString();
	if(!contentNode["Payee"].isNull())
		data_.content.payee = contentNode["Payee"].asString();
	if(!contentNode["Checker"].isNull())
		data_.content.checker = contentNode["Checker"].asString();
	if(!contentNode["Clerk"].isNull())
		data_.content.clerk = contentNode["Clerk"].asString();
	auto boxNode = dataNode["Box"];
		auto allInvoiceCodes = boxNode["InvoiceCodes"]["InvoiceCode"];
		for (auto value : allInvoiceCodes)
			data_.box.invoiceCodes.push_back(value.asString());
		auto allInvoiceNoes = boxNode["InvoiceNoes"]["InvoiceNo"];
		for (auto value : allInvoiceNoes)
			data_.box.invoiceNoes.push_back(value.asString());
		auto allInvoiceDates = boxNode["InvoiceDates"]["InvoiceDate"];
		for (auto value : allInvoiceDates)
			data_.box.invoiceDates.push_back(value.asString());
		auto allInvoiceFakeCodes = boxNode["InvoiceFakeCodes"]["InvoiceFakeCode"];
		for (auto value : allInvoiceFakeCodes)
			data_.box.invoiceFakeCodes.push_back(value.asString());
		auto allPayerNames = boxNode["PayerNames"]["PayerName"];
		for (auto value : allPayerNames)
			data_.box.payerNames.push_back(value.asString());
		auto allPayerRegisterNoes = boxNode["PayerRegisterNoes"]["PayerRegisterNo"];
		for (auto value : allPayerRegisterNoes)
			data_.box.payerRegisterNoes.push_back(value.asString());
		auto allPayerAddresses = boxNode["PayerAddresses"]["PayerAddress"];
		for (auto value : allPayerAddresses)
			data_.box.payerAddresses.push_back(value.asString());
		auto allPayerBankNames = boxNode["PayerBankNames"]["PayerBankNames"];
		for (auto value : allPayerBankNames)
			data_.box.payerBankNames.push_back(value.asString());
		auto allWithoutTaxAmounts = boxNode["WithoutTaxAmounts"]["WithoutTaxAmount"];
		for (auto value : allWithoutTaxAmounts)
			data_.box.withoutTaxAmounts.push_back(value.asString());
		auto allTaxAmounts = boxNode["TaxAmounts"]["TaxAmount"];
		for (auto value : allTaxAmounts)
			data_.box.taxAmounts.push_back(value.asString());
		auto allSumAmounts = boxNode["SumAmounts"]["SumAmount"];
		for (auto value : allSumAmounts)
			data_.box.sumAmounts.push_back(value.asString());
		auto allInvoiceAmounts = boxNode["InvoiceAmounts"]["InvoiceAmount"];
		for (auto value : allInvoiceAmounts)
			data_.box.invoiceAmounts.push_back(value.asString());
		auto allPayeeNames = boxNode["PayeeNames"]["PayeeName"];
		for (auto value : allPayeeNames)
			data_.box.payeeNames.push_back(value.asString());
		auto allPayeeRegisterNoes = boxNode["PayeeRegisterNoes"]["PayeeRegisterNo"];
		for (auto value : allPayeeRegisterNoes)
			data_.box.payeeRegisterNoes.push_back(value.asString());
		auto allPayeeAddresses = boxNode["PayeeAddresses"]["PayeeAddress"];
		for (auto value : allPayeeAddresses)
			data_.box.payeeAddresses.push_back(value.asString());
		auto allPayeeBankNames = boxNode["PayeeBankNames"]["PayeeBankName"];
		for (auto value : allPayeeBankNames)
			data_.box.payeeBankNames.push_back(value.asString());
		auto allPayees = boxNode["Payees"]["Payee"];
		for (auto value : allPayees)
			data_.box.payees.push_back(value.asString());
		auto allCheckers = boxNode["Checkers"]["Checker"];
		for (auto value : allCheckers)
			data_.box.checkers.push_back(value.asString());
		auto allClerks = boxNode["Clerks"]["Clerk"];
		for (auto value : allClerks)
			data_.box.clerks.push_back(value.asString());

}

RecognizeVATInvoiceResult::Data RecognizeVATInvoiceResult::getData()const
{
	return data_;
}

