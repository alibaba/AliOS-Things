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

#ifndef ALIBABACLOUD_IMAGEPROCESS_MODEL_DETECTHIPKEYPOINTXRAYRESULT_H_
#define ALIBABACLOUD_IMAGEPROCESS_MODEL_DETECTHIPKEYPOINTXRAYRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imageprocess/ImageprocessExport.h>

namespace AlibabaCloud
{
	namespace Imageprocess
	{
		namespace Model
		{
			class ALIBABACLOUD_IMAGEPROCESS_EXPORT DetectHipKeypointXRayResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct KeyPointsItem
					{
						struct Tag
						{
							std::string label;
							std::string direction;
						};
						std::vector<std::string> coordinates;
						float value;
						Tag tag;
					};
					std::string orgId;
					std::vector<KeyPointsItem> keyPoints;
					std::string imageUrl;
					std::string orgName;
				};


				DetectHipKeypointXRayResult();
				explicit DetectHipKeypointXRayResult(const std::string &payload);
				~DetectHipKeypointXRayResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMAGEPROCESS_MODEL_DETECTHIPKEYPOINTXRAYRESULT_H_