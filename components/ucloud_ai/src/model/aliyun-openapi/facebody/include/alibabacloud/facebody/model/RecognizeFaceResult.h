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

#ifndef ALIBABACLOUD_FACEBODY_MODEL_RECOGNIZEFACERESULT_H_
#define ALIBABACLOUD_FACEBODY_MODEL_RECOGNIZEFACERESULT_H_

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
			class ALIBABACLOUD_FACEBODY_EXPORT RecognizeFaceResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct Qualities
					{
						std::vector<std::string> noiseList;
						std::vector<std::string> scoreList;
						std::vector<std::string> blurList;
						std::vector<std::string> maskList;
						std::vector<std::string> glassList;
						std::vector<std::string> fnfList;
						std::vector<std::string> poseList1;
						std::vector<std::string> illuList;
					};
					std::vector<std::string> beuatyList;
					std::vector<std::string> faceRectangles;
					Qualities qualities;
					int denseFeatureLength;
					std::vector<std::string> poseList;
					std::vector<std::string> genderList;
					std::vector<std::string> pupils;
					std::vector<std::string> denseFeatures;
					std::vector<std::string> faceProbabilityList;
					std::vector<std::string> hatList;
					int landmarkCount;
					std::vector<std::string> ageList;
					std::vector<std::string> glasses;
					std::vector<std::string> landmarks;
					std::vector<std::string> expressions;
					int faceCount;
				};


				RecognizeFaceResult();
				explicit RecognizeFaceResult(const std::string &payload);
				~RecognizeFaceResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_FACEBODY_MODEL_RECOGNIZEFACERESULT_H_