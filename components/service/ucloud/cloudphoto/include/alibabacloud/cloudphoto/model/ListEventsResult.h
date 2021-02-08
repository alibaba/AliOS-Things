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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTEVENTSRESULT_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTEVENTSRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/cloudphoto/CloudPhotoExport.h>

namespace AlibabaCloud
{
	namespace CloudPhoto
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT ListEventsResult : public ServiceResult
			{
			public:
				struct Event
				{
					std::string idStr1;
					std::string watermarkPhotoId;
					std::string idStr;
					std::string title;
					long mtime;
					std::string bannerPhotoId;
					std::string splashPhotoId;
					long endAt;
					std::string libraryId;
					std::string weixinTitle;
					std::string state;
					long startAt;
					long ctime;
					long viewsCount;
					long id;
					std::string identity;
				};


				ListEventsResult();
				explicit ListEventsResult(const std::string &payload);
				~ListEventsResult();
				std::string getNextCursor()const;
				int getTotalCount()const;
				std::string getAction()const;
				std::string getMessage()const;
				std::vector<Event> getEvents()const;
				std::string getCode()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string nextCursor_;
				int totalCount_;
				std::string action_;
				std::string message_;
				std::vector<Event> events_;
				std::string code_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTEVENTSRESULT_H_