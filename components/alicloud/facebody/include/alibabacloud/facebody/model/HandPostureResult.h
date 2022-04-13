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

#ifndef ALIBABACLOUD_FACEBODY_MODEL_HANDPOSTURERESULT_H_
#define ALIBABACLOUD_FACEBODY_MODEL_HANDPOSTURERESULT_H_

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
			class ALIBABACLOUD_FACEBODY_EXPORT HandPostureResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct MetaObject
					{
						int height;
						int width;
					};
					struct Output
					{
						struct Result
						{
							struct Box
							{
								struct Position
								{
									std::vector<std::string> points;
								};
								float confident;
								std::vector<Position> positions;
							};
							struct Hands
							{
								struct KeyPoint
								{
									struct Position2
									{
										std::vector<std::string> points3;
									};
									std::vector<KeyPoint::Position2> positions1;
									std::string label;
								};
								float confident;
								std::vector<KeyPoint> keyPoints;
							};
							Hands hands;
							Box box;
						};
						std::vector<Output::Result> results;
						int handCount;
					};
					MetaObject metaObject;
					std::vector<Output> outputs;
				};


				HandPostureResult();
				explicit HandPostureResult(const std::string &payload);
				~HandPostureResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_FACEBODY_MODEL_HANDPOSTURERESULT_H_