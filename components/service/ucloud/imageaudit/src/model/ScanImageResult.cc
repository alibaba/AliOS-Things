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

#include <alibabacloud/imageaudit/model/ScanImageResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageaudit;
using namespace AlibabaCloud::Imageaudit::Model;

ScanImageResult::ScanImageResult() :
	ServiceResult()
{}

ScanImageResult::ScanImageResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ScanImageResult::~ScanImageResult()
{}

void ScanImageResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allResultsNode = dataNode["Results"]["Result"];
	for (auto dataNodeResultsResult : allResultsNode)
	{
		Data::Result resultObject;
		if(!dataNodeResultsResult["DataId"].isNull())
			resultObject.dataId = dataNodeResultsResult["DataId"].asString();
		if(!dataNodeResultsResult["TaskId"].isNull())
			resultObject.taskId = dataNodeResultsResult["TaskId"].asString();
		if(!dataNodeResultsResult["ImageURL"].isNull())
			resultObject.imageURL = dataNodeResultsResult["ImageURL"].asString();
		auto allSubResultsNode = dataNodeResultsResult["SubResults"]["SubResult"];
		for (auto dataNodeResultsResultSubResultsSubResult : allSubResultsNode)
		{
			Data::Result::SubResult subResultsObject;
			if(!dataNodeResultsResultSubResultsSubResult["Label"].isNull())
				subResultsObject.label = dataNodeResultsResultSubResultsSubResult["Label"].asString();
			if(!dataNodeResultsResultSubResultsSubResult["Suggestion"].isNull())
				subResultsObject.suggestion = dataNodeResultsResultSubResultsSubResult["Suggestion"].asString();
			if(!dataNodeResultsResultSubResultsSubResult["Rate"].isNull())
				subResultsObject.rate = std::stof(dataNodeResultsResultSubResultsSubResult["Rate"].asString());
			if(!dataNodeResultsResultSubResultsSubResult["Scene"].isNull())
				subResultsObject.scene = dataNodeResultsResultSubResultsSubResult["Scene"].asString();
			auto allFramesNode = dataNodeResultsResultSubResultsSubResult["Frames"]["Frame"];
			for (auto dataNodeResultsResultSubResultsSubResultFramesFrame : allFramesNode)
			{
				Data::Result::SubResult::Frame framesObject;
				if(!dataNodeResultsResultSubResultsSubResultFramesFrame["Rate"].isNull())
					framesObject.rate = std::stof(dataNodeResultsResultSubResultsSubResultFramesFrame["Rate"].asString());
				if(!dataNodeResultsResultSubResultsSubResultFramesFrame["URL"].isNull())
					framesObject.uRL = dataNodeResultsResultSubResultsSubResultFramesFrame["URL"].asString();
				subResultsObject.frames.push_back(framesObject);
			}
			auto allHintWordsInfoListNode = dataNodeResultsResultSubResultsSubResult["HintWordsInfoList"]["HintWordsInfo"];
			for (auto dataNodeResultsResultSubResultsSubResultHintWordsInfoListHintWordsInfo : allHintWordsInfoListNode)
			{
				Data::Result::SubResult::HintWordsInfo hintWordsInfoListObject;
				if(!dataNodeResultsResultSubResultsSubResultHintWordsInfoListHintWordsInfo["Context"].isNull())
					hintWordsInfoListObject.context = dataNodeResultsResultSubResultsSubResultHintWordsInfoListHintWordsInfo["Context"].asString();
				subResultsObject.hintWordsInfoList.push_back(hintWordsInfoListObject);
			}
			auto allProgramCodeDataListNode = dataNodeResultsResultSubResultsSubResult["ProgramCodeDataList"]["ProgramCodeData"];
			for (auto dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData : allProgramCodeDataListNode)
			{
				Data::Result::SubResult::ProgramCodeData programCodeDataListObject;
				if(!dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData["X"].isNull())
					programCodeDataListObject.x = std::stof(dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData["X"].asString());
				if(!dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData["Y"].isNull())
					programCodeDataListObject.y = std::stof(dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData["Y"].asString());
				if(!dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData["Width"].isNull())
					programCodeDataListObject.width = std::stof(dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData["Width"].asString());
				if(!dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData["Height"].isNull())
					programCodeDataListObject.height = std::stof(dataNodeResultsResultSubResultsSubResultProgramCodeDataListProgramCodeData["Height"].asString());
				subResultsObject.programCodeDataList.push_back(programCodeDataListObject);
			}
			auto allLogoDataListNode = dataNodeResultsResultSubResultsSubResult["LogoDataList"]["LogoData"];
			for (auto dataNodeResultsResultSubResultsSubResultLogoDataListLogoData : allLogoDataListNode)
			{
				Data::Result::SubResult::LogoData logoDataListObject;
				if(!dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Type"].isNull())
					logoDataListObject.type = dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Type"].asString();
				if(!dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Name"].isNull())
					logoDataListObject.name = dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Name"].asString();
				if(!dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["X"].isNull())
					logoDataListObject.x = std::stof(dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["X"].asString());
				if(!dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Y"].isNull())
					logoDataListObject.y = std::stof(dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Y"].asString());
				if(!dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Width"].isNull())
					logoDataListObject.width = std::stof(dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Width"].asString());
				if(!dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Height"].isNull())
					logoDataListObject.height = std::stof(dataNodeResultsResultSubResultsSubResultLogoDataListLogoData["Height"].asString());
				subResultsObject.logoDataList.push_back(logoDataListObject);
			}
			auto allSfaceDataListNode = dataNodeResultsResultSubResultsSubResult["SfaceDataList"]["SfaceData"];
			for (auto dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData : allSfaceDataListNode)
			{
				Data::Result::SubResult::SfaceData sfaceDataListObject;
				if(!dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["X"].isNull())
					sfaceDataListObject.x = std::stof(dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["X"].asString());
				if(!dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["Y"].isNull())
					sfaceDataListObject.y = std::stof(dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["Y"].asString());
				if(!dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["Width"].isNull())
					sfaceDataListObject.width = std::stof(dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["Width"].asString());
				if(!dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["Height"].isNull())
					sfaceDataListObject.height = std::stof(dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["Height"].asString());
				auto allFacesNode = dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceData["Faces"]["Face"];
				for (auto dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceDataFacesFace : allFacesNode)
				{
					Data::Result::SubResult::SfaceData::Face facesObject;
					if(!dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceDataFacesFace["Name"].isNull())
						facesObject.name = dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceDataFacesFace["Name"].asString();
					if(!dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceDataFacesFace["Rate"].isNull())
						facesObject.rate = std::stof(dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceDataFacesFace["Rate"].asString());
					if(!dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceDataFacesFace["Id"].isNull())
						facesObject.id = dataNodeResultsResultSubResultsSubResultSfaceDataListSfaceDataFacesFace["Id"].asString();
					sfaceDataListObject.faces.push_back(facesObject);
				}
				subResultsObject.sfaceDataList.push_back(sfaceDataListObject);
			}
			auto allOCRDataList = value["OCRDataList"]["OCRData"];
			for (auto value : allOCRDataList)
				subResultsObject.oCRDataList.push_back(value.asString());
			resultObject.subResults.push_back(subResultsObject);
		}
		data_.results.push_back(resultObject);
	}

}

ScanImageResult::Data ScanImageResult::getData()const
{
	return data_;
}

