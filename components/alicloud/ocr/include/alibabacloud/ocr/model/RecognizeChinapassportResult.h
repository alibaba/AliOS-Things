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

#ifndef ALIBABACLOUD_OCR_MODEL_RECOGNIZECHINAPASSPORTRESULT_H_
#define ALIBABACLOUD_OCR_MODEL_RECOGNIZECHINAPASSPORTRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/ocr/OcrExport.h>

namespace AlibabaCloud
{
	namespace Ocr
	{
		namespace Model
		{
			class ALIBABACLOUD_OCR_EXPORT RecognizeChinapassportResult : public ServiceResult
			{
			public:
				struct Data
				{
					std::string expiryDate;
					std::string sourceCountry;
					std::string birthPlace;
					std::string issuePlaceRaw;
					std::string nameChineseRaw;
					std::string authority;
					std::string sex;
					std::string issueDate;
					std::string birthDay;
					std::string lineZero;
					std::string issuePlace;
					std::string expiryDay;
					bool success;
					std::string name;
					std::string lineOne;
					std::string nameChinese;
					std::string type;
					std::string personId;
					std::string passportNo;
					std::string birthPlaceRaw;
					std::string country;
					std::string birthDate;
				};


				RecognizeChinapassportResult();
				explicit RecognizeChinapassportResult(const std::string &payload);
				~RecognizeChinapassportResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OCR_MODEL_RECOGNIZECHINAPASSPORTRESULT_H_