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

#ifndef ALIBABACLOUD_IMM_MODEL_LISTVIDEOSRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_LISTVIDEOSRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT ListVideosResult : public ServiceResult
			{
			public:
				struct VideosItem
				{
					struct VideoTagsItem
					{
						std::string tagName;
						float tagConfidence;
						int tagLevel;
						std::string parentTagName;
					};
					struct CelebrityItem
					{
						std::string celebrityName;
						int celebrityNum;
						std::string celebrityLibraryName;
					};
					struct PersonsItem
					{
						std::string personId;
						float genderConfidence;
						std::string gender;
						int age;
						float ageConfidence;
					};
					std::string modifyTime;
					std::string processStatus;
					int videoWidth;
					std::string sourceType;
					std::string sourceUri;
					std::string remarksA;
					std::string remarksB;
					std::string remarksC;
					std::string remarksD;
					int videoHeight;
					std::vector<VideosItem::PersonsItem> persons;
					std::vector<VideosItem::CelebrityItem> celebrity;
					std::string sourcePosition;
					std::string videoTagsFailReason;
					std::string videoTagsModifyTime;
					int videoFrames;
					std::string processModifyTime;
					std::string processFailReason;
					std::string createTime;
					std::string externalId;
					std::string videoUri;
					std::string videoFormat;
					std::vector<VideosItem::VideoTagsItem> videoTags;
					std::string celebrityModifyTime;
					float videoDuration;
					std::string celebrityFailReason;
					std::string celebrityStatus;
					std::string videoTagsStatus;
					int fileSize;
				};


				ListVideosResult();
				explicit ListVideosResult(const std::string &payload);
				~ListVideosResult();
				std::string getSetId()const;
				std::string getNextMarker()const;
				std::vector<VideosItem> getVideos()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string setId_;
				std::string nextMarker_;
				std::vector<VideosItem> videos_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_LISTVIDEOSRESULT_H_