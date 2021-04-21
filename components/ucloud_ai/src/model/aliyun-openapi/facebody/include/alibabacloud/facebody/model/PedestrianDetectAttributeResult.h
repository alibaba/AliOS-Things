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

#ifndef ALIBABACLOUD_FACEBODY_MODEL_PEDESTRIANDETECTATTRIBUTERESULT_H_
#define ALIBABACLOUD_FACEBODY_MODEL_PEDESTRIANDETECTATTRIBUTERESULT_H_

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
			class ALIBABACLOUD_FACEBODY_EXPORT PedestrianDetectAttributeResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct AttributesItem
					{
						struct Age
						{
							float score;
							std::string name;
						};
						struct Backpack
						{
							float score;
							std::string name;
						};
						struct Gender
						{
							float score;
							std::string name;
						};
						struct Glasses
						{
							float score;
							std::string name;
						};
						struct Handbag
						{
							float score;
							std::string name;
						};
						struct Hat
						{
							float score;
							std::string name;
						};
						struct LowerColor
						{
							float score;
							std::string name;
						};
						struct LowerWear
						{
							float score;
							std::string name;
						};
						struct Orient
						{
							float score;
							std::string name;
						};
						struct ShoulderBag
						{
							float score;
							std::string name;
						};
						struct UpperColor
						{
							float score;
							std::string name;
						};
						struct UpperWear
						{
							float score;
							std::string name;
						};
						Backpack backpack;
						LowerWear lowerWear;
						Orient orient;
						ShoulderBag shoulderBag;
						LowerColor lowerColor;
						Glasses glasses;
						UpperColor upperColor;
						Handbag handbag;
						Hat hat;
						Gender gender;
						Age age;
						UpperWear upperWear;
					};
					struct BoxesItem
					{
						float score;
						float bottomRightY;
						float bottomRightX;
						float topLeftY;
						float topLeftX;
					};
					int personNumber;
					std::vector<AttributesItem> attributes;
					std::vector<BoxesItem> boxes;
				};


				PedestrianDetectAttributeResult();
				explicit PedestrianDetectAttributeResult(const std::string &payload);
				~PedestrianDetectAttributeResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_FACEBODY_MODEL_PEDESTRIANDETECTATTRIBUTERESULT_H_