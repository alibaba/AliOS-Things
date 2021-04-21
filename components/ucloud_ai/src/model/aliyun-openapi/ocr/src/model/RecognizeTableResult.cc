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

#include <alibabacloud/ocr/model/RecognizeTableResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeTableResult::RecognizeTableResult() :
	ServiceResult()
{}

RecognizeTableResult::RecognizeTableResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeTableResult::~RecognizeTableResult()
{}

void RecognizeTableResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["FileContent"].isNull())
		data_.fileContent = dataNode["FileContent"].asString();
	auto allTablesNode = dataNode["Tables"]["Table"];
	for (auto dataNodeTablesTable : allTablesNode)
	{
		Data::Table tableObject;
		auto allTableRowsNode = dataNodeTablesTable["TableRows"]["TableRow"];
		for (auto dataNodeTablesTableTableRowsTableRow : allTableRowsNode)
		{
			Data::Table::TableRow tableRowsObject;
			auto allTableColumnsNode = dataNodeTablesTableTableRowsTableRow["TableColumns"]["TableColumn"];
			for (auto dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn : allTableColumnsNode)
			{
				Data::Table::TableRow::TableColumn tableColumnsObject;
				if(!dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["StartColumn"].isNull())
					tableColumnsObject.startColumn = std::stoi(dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["StartColumn"].asString());
				if(!dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["StartRow"].isNull())
					tableColumnsObject.startRow = std::stoi(dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["StartRow"].asString());
				if(!dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["EndColumn"].isNull())
					tableColumnsObject.endColumn = std::stoi(dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["EndColumn"].asString());
				if(!dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["EndRow"].isNull())
					tableColumnsObject.endRow = std::stoi(dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["EndRow"].asString());
				if(!dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["Height"].isNull())
					tableColumnsObject.height = std::stoi(dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["Height"].asString());
				if(!dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["Width"].isNull())
					tableColumnsObject.width = std::stoi(dataNodeTablesTableTableRowsTableRowTableColumnsTableColumn["Width"].asString());
				auto allTexts = value["Texts"]["Text"];
				for (auto value : allTexts)
					tableColumnsObject.texts.push_back(value.asString());
				tableRowsObject.tableColumns.push_back(tableColumnsObject);
			}
			tableObject.tableRows.push_back(tableRowsObject);
		}
		auto allHead = value["Head"]["Head"];
		for (auto value : allHead)
			tableObject.head.push_back(value.asString());
		auto allTail = value["Tail"]["Tail"];
		for (auto value : allTail)
			tableObject.tail.push_back(value.asString());
		data_.tables.push_back(tableObject);
	}

}

RecognizeTableResult::Data RecognizeTableResult::getData()const
{
	return data_;
}

