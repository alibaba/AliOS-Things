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

#ifndef ALIBABACLOUD_FACEBODY_MODEL_DETECTFACERESULT_H_
#define ALIBABACLOUD_FACEBODY_MODEL_DETECTFACERESULT_H_

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
			class ALIBABACLOUD_FACEBODY_EXPORT DetectFaceResult : public ServiceResult
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
					std::vector<std::string> faceProbabilityList;
					int landmarkCount;
					std::vector<std::string> faceRectangles;
					std::vector<std::string> landmarks;
					Qualities qualities;
					int faceCount;
					std::vector<std::string> poseList;
					std::vector<std::string> pupils;
				};


				DetectFaceResult();
				explicit DetectFaceResult(const std::string &payload);
				~DetectFaceResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_FACEBODY_MODEL_DETECTFACERESULT_H_