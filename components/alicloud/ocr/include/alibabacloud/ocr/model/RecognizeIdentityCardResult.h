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

#ifndef ALIBABACLOUD_OCR_MODEL_RECOGNIZEIDENTITYCARDRESULT_H_
#define ALIBABACLOUD_OCR_MODEL_RECOGNIZEIDENTITYCARDRESULT_H_

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
			class ALIBABACLOUD_OCR_EXPORT RecognizeIdentityCardResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct FrontResult
					{
						struct FaceRectangle
						{
							struct Center
							{
								float x;
								float y;
							};
							struct Size
							{
								float height;
								float width;
							};
							Center center;
							float angle;
							Size size;
						};
						struct CardArea
						{
							float x;
							float y;
						};
						struct FaceRectVertice
						{
							float x;
							float y;
						};
						std::string address;
						FaceRectangle faceRectangle;
						std::vector<FaceRectVertice> faceRectVertices;
						std::vector<CardArea> cardAreas;
						std::string gender;
						std::string nationality;
						std::string name;
						std::string iDNumber;
						std::string birthDate;
					};
					struct BackResult
					{
						std::string startDate;
						std::string issue;
						std::string endDate;
					};
					FrontResult frontResult;
					BackResult backResult;
				};


				RecognizeIdentityCardResult();
				explicit RecognizeIdentityCardResult(const std::string &payload);
				~RecognizeIdentityCardResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OCR_MODEL_RECOGNIZEIDENTITYCARDRESULT_H_