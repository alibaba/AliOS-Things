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

#ifndef ALIBABACLOUD_OCR_MODEL_RECOGNIZEACCOUNTPAGERESULT_H_
#define ALIBABACLOUD_OCR_MODEL_RECOGNIZEACCOUNTPAGERESULT_H_

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
			class ALIBABACLOUD_OCR_EXPORT RecognizeAccountPageResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct TitleArea
					{
						int left;
						int top;
						int height;
						int width;
					};
					struct InvalidStampArea
					{
						int left;
						int top;
						int height;
						int width;
					};
					struct UndertakeStampArea
					{
						int left;
						int top;
						int height;
						int width;
					};
					struct RegisterStampArea
					{
						int left;
						int top;
						int height;
						int width;
					};
					struct OtherStampArea
					{
						int left;
						int top;
						int height;
						int width;
					};
					std::string nativePlace;
					std::vector<OtherStampArea> otherStampAreas;
					std::string birthPlace;
					std::vector<RegisterStampArea> registerStampAreas;
					std::string gender;
					std::vector<InvalidStampArea> invalidStampAreas;
					std::string nationality;
					std::string name;
					std::vector<UndertakeStampArea> undertakeStampAreas;
					std::string relation;
					float angle;
					TitleArea titleArea;
					std::string birthDate;
					std::string iDNumber;
				};


				RecognizeAccountPageResult();
				explicit RecognizeAccountPageResult(const std::string &payload);
				~RecognizeAccountPageResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OCR_MODEL_RECOGNIZEACCOUNTPAGERESULT_H_