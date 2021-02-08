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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBEBOARDSNAPSHOTRESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBEBOARDSNAPSHOTRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/live/LiveExport.h>

namespace AlibabaCloud
{
	namespace Live
	{
		namespace Model
		{
			class ALIBABACLOUD_LIVE_EXPORT DescribeBoardSnapshotResult : public ServiceResult
			{
			public:
				struct Snapshot
				{
					struct Board
					{
						struct Page
						{
							struct Element
							{
								int elementType;
								std::string ownerId;
								std::string data;
								long updateTimestamp;
								std::string elementIndex;
							};
							int pageIndex;
							std::vector<Page::Element> elements;
						};
						struct Config
						{
							std::string appUid;
							std::string data;
						};
						std::vector<Page> pages;
						std::string boardId;
						long eventTimestamp;
						std::string appUid;
						std::vector<Config> configs;
						long updateTimestamp;
						long createTimestamp;
					};
					Board board;
				};


				DescribeBoardSnapshotResult();
				explicit DescribeBoardSnapshotResult(const std::string &payload);
				~DescribeBoardSnapshotResult();
				Snapshot getSnapshot()const;

			protected:
				void parse(const std::string &payload);
			private:
				Snapshot snapshot_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBEBOARDSNAPSHOTRESULT_H_