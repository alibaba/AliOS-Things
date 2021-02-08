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

#ifndef ALIBABACLOUD_FACEBODY_MODEL_RECOGNIZEPUBLICFACERESULT_H_
#define ALIBABACLOUD_FACEBODY_MODEL_RECOGNIZEPUBLICFACERESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/facebody/FacebodyExport.h>

namespace AlibabaCloud
{
	namespace Facebody
	{
		namespace Model
		{
			class ALIBABACLOUD_FACEBODY_EXPORT RecognizePublicFaceResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct Element
					{
						struct Result
						{
							struct SubResult
							{
								struct Face
								{
									float rate;
									std::string id;
									std::string name;
								};
								float w;
								float h;
								float x;
								float y;
								std::vector<SubResult::Face> faces;
							};
							std::string suggestion;
							float rate;
							std::string label;
							std::vector<Result::SubResult> subResults;
						};
						std::string taskId;
						std::vector<Element::Result> results;
						std::string imageURL;
					};
					std::vector<Element> elements;
				};


				RecognizePublicFaceResult();
				explicit RecognizePublicFaceResult(const std::string &payload);
				~RecognizePublicFaceResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_FACEBODY_MODEL_RECOGNIZEPUBLICFACERESULT_H_