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

#include <alibabacloud/imm/model/ConvertOfficeFormatRequest.h>

using AlibabaCloud::Imm::Model::ConvertOfficeFormatRequest;

ConvertOfficeFormatRequest::ConvertOfficeFormatRequest() :
	RpcServiceRequest("imm", "2017-09-06", "ConvertOfficeFormat")
{
	setMethod(HttpRequest::Method::Post);
}

ConvertOfficeFormatRequest::~ConvertOfficeFormatRequest()
{}

std::string ConvertOfficeFormatRequest::getSrcType()const
{
	return srcType_;
}

void ConvertOfficeFormatRequest::setSrcType(const std::string& srcType)
{
	srcType_ = srcType;
	setParameter("SrcType", srcType);
}

std::string ConvertOfficeFormatRequest::getProject()const
{
	return project_;
}

void ConvertOfficeFormatRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

bool ConvertOfficeFormatRequest::getPdfVector()const
{
	return pdfVector_;
}

void ConvertOfficeFormatRequest::setPdfVector(bool pdfVector)
{
	pdfVector_ = pdfVector;
	setParameter("PdfVector", pdfVector ? "true" : "false");
}

std::string ConvertOfficeFormatRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ConvertOfficeFormatRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ConvertOfficeFormatRequest::getPassword()const
{
	return password_;
}

void ConvertOfficeFormatRequest::setPassword(const std::string& password)
{
	password_ = password;
	setParameter("Password", password);
}

long ConvertOfficeFormatRequest::getStartPage()const
{
	return startPage_;
}

void ConvertOfficeFormatRequest::setStartPage(long startPage)
{
	startPage_ = startPage;
	setParameter("StartPage", std::to_string(startPage));
}

bool ConvertOfficeFormatRequest::getFitToPagesWide()const
{
	return fitToPagesWide_;
}

void ConvertOfficeFormatRequest::setFitToPagesWide(bool fitToPagesWide)
{
	fitToPagesWide_ = fitToPagesWide;
	setParameter("FitToPagesWide", fitToPagesWide ? "true" : "false");
}

std::string ConvertOfficeFormatRequest::getTgtFilePrefix()const
{
	return tgtFilePrefix_;
}

void ConvertOfficeFormatRequest::setTgtFilePrefix(const std::string& tgtFilePrefix)
{
	tgtFilePrefix_ = tgtFilePrefix;
	setParameter("TgtFilePrefix", tgtFilePrefix);
}

std::string ConvertOfficeFormatRequest::getModelId()const
{
	return modelId_;
}

void ConvertOfficeFormatRequest::setModelId(const std::string& modelId)
{
	modelId_ = modelId;
	setParameter("ModelId", modelId);
}

long ConvertOfficeFormatRequest::getMaxSheetRow()const
{
	return maxSheetRow_;
}

void ConvertOfficeFormatRequest::setMaxSheetRow(long maxSheetRow)
{
	maxSheetRow_ = maxSheetRow;
	setParameter("MaxSheetRow", std::to_string(maxSheetRow));
}

long ConvertOfficeFormatRequest::getMaxSheetCount()const
{
	return maxSheetCount_;
}

void ConvertOfficeFormatRequest::setMaxSheetCount(long maxSheetCount)
{
	maxSheetCount_ = maxSheetCount;
	setParameter("MaxSheetCount", std::to_string(maxSheetCount));
}

long ConvertOfficeFormatRequest::getEndPage()const
{
	return endPage_;
}

void ConvertOfficeFormatRequest::setEndPage(long endPage)
{
	endPage_ = endPage;
	setParameter("EndPage", std::to_string(endPage));
}

std::string ConvertOfficeFormatRequest::getTgtFileSuffix()const
{
	return tgtFileSuffix_;
}

void ConvertOfficeFormatRequest::setTgtFileSuffix(const std::string& tgtFileSuffix)
{
	tgtFileSuffix_ = tgtFileSuffix;
	setParameter("TgtFileSuffix", tgtFileSuffix);
}

bool ConvertOfficeFormatRequest::getSheetOnePage()const
{
	return sheetOnePage_;
}

void ConvertOfficeFormatRequest::setSheetOnePage(bool sheetOnePage)
{
	sheetOnePage_ = sheetOnePage;
	setParameter("SheetOnePage", sheetOnePage ? "true" : "false");
}

long ConvertOfficeFormatRequest::getMaxSheetCol()const
{
	return maxSheetCol_;
}

void ConvertOfficeFormatRequest::setMaxSheetCol(long maxSheetCol)
{
	maxSheetCol_ = maxSheetCol;
	setParameter("MaxSheetCol", std::to_string(maxSheetCol));
}

std::string ConvertOfficeFormatRequest::getTgtType()const
{
	return tgtType_;
}

void ConvertOfficeFormatRequest::setTgtType(const std::string& tgtType)
{
	tgtType_ = tgtType;
	setParameter("TgtType", tgtType);
}

bool ConvertOfficeFormatRequest::getHidecomments()const
{
	return hidecomments_;
}

void ConvertOfficeFormatRequest::setHidecomments(bool hidecomments)
{
	hidecomments_ = hidecomments;
	setParameter("Hidecomments", hidecomments ? "true" : "false");
}

bool ConvertOfficeFormatRequest::getFitToPagesTall()const
{
	return fitToPagesTall_;
}

void ConvertOfficeFormatRequest::setFitToPagesTall(bool fitToPagesTall)
{
	fitToPagesTall_ = fitToPagesTall;
	setParameter("FitToPagesTall", fitToPagesTall ? "true" : "false");
}

std::string ConvertOfficeFormatRequest::getSrcUri()const
{
	return srcUri_;
}

void ConvertOfficeFormatRequest::setSrcUri(const std::string& srcUri)
{
	srcUri_ = srcUri;
	setParameter("SrcUri", srcUri);
}

std::string ConvertOfficeFormatRequest::getTgtFilePages()const
{
	return tgtFilePages_;
}

void ConvertOfficeFormatRequest::setTgtFilePages(const std::string& tgtFilePages)
{
	tgtFilePages_ = tgtFilePages;
	setParameter("TgtFilePages", tgtFilePages);
}

std::string ConvertOfficeFormatRequest::getTgtUri()const
{
	return tgtUri_;
}

void ConvertOfficeFormatRequest::setTgtUri(const std::string& tgtUri)
{
	tgtUri_ = tgtUri;
	setParameter("TgtUri", tgtUri);
}

