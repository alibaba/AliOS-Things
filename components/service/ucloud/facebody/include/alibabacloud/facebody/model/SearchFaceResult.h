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

#ifndef ALIBABACLOUD_FACEBODY_MODEL_SEARCHFACERESULT_H_
#define ALIBABACLOUD_FACEBODY_MODEL_SEARCHFACERESULT_H_

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
			class ALIBABACLOUD_FACEBODY_EXPORT SearchFaceResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct MatchListItem
					{
						struct Location
						{
							int x;
							int y;
							int height;
							int width;
						};
						struct FaceItemsItem
						{
							std::string faceId;
							std::string entityId;
							float score;
							std::string dbName;
							std::string extraData;
						};
						std::vector<MatchListItem::FaceItemsItem> faceItems;
						Location location;
					};
					std::vector<MatchListItem> matchList;
				};


				SearchFaceResult();
				explicit SearchFaceResult(const std::string &payload);
				~SearchFaceResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_FACEBODY_MODEL_SEARCHFACERESULT_H_