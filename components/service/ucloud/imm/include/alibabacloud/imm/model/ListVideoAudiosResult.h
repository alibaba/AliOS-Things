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

#ifndef ALIBABACLOUD_IMM_MODEL_LISTVIDEOAUDIOSRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_LISTVIDEOAUDIOSRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT ListVideoAudiosResult : public ServiceResult
			{
			public:
				struct AudiosItem
				{
					struct AudioTextsItem
					{
						float endTime;
						float confidence;
						float silenceDuration;
						float beginTime;
						std::string text;
						std::string library;
						int channelId;
						std::string person;
						int speechRate;
						float emotionValue;
					};
					std::string modifyTime;
					std::string processModifyTime;
					std::string processStatus;
					std::string processFailReason;
					std::string sourceType;
					std::string sourceUri;
					std::string createTime;
					std::string externalId;
					std::string remarksA;
					std::string remarksB;
					std::string remarksC;
					std::string audioUri;
					std::string remarksD;
					std::string sourcePosition;
					std::vector<AudiosItem::AudioTextsItem> audioTexts;
					std::string audioTextsStatus;
					std::string audioFormat;
					int audioRate;
					std::string audioTextsModifyTime;
					float audioDuration;
					int fileSize;
					std::string audioTextsFailReason;
				};


				ListVideoAudiosResult();
				explicit ListVideoAudiosResult(const std::string &payload);
				~ListVideoAudiosResult();
				std::vector<AudiosItem> getAudios()const;
				std::string getVideoUri()const;
				std::string getSetId()const;
				std::string getNextMarker()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<AudiosItem> audios_;
				std::string videoUri_;
				std::string setId_;
				std::string nextMarker_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_LISTVIDEOAUDIOSRESULT_H_