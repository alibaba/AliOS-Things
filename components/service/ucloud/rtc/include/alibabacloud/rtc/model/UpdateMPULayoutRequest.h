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

#ifndef ALIBABACLOUD_RTC_MODEL_UPDATEMPULAYOUTREQUEST_H_
#define ALIBABACLOUD_RTC_MODEL_UPDATEMPULAYOUTREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/rtc/RtcExport.h>

namespace AlibabaCloud
{
	namespace Rtc
	{
		namespace Model
		{
			class ALIBABACLOUD_RTC_EXPORT UpdateMPULayoutRequest : public RpcServiceRequest
			{
			public:
				struct UserPanes
				{
					int paneId;
					std::string userId;
					std::string sourceType;
					struct Images
					{
						std::string url;
						int display;
						float x;
						float y;
						float width;
						float height;
						int zOrder;
					};
					std::vector<Images> images;
					struct Texts
					{
						std::string text;
						float x;
						float y;
						int fontType;
						int fontSize;
						int fontColor;
						int zOrder;
					};
					std::vector<Texts> texts;
				};
				struct ClockWidgets
				{
					float x;
					float y;
					int fontType;
					int fontSize;
					int fontColor;
					int zOrder;
				};
				struct Watermarks
				{
					std::string url;
					float alpha;
					int display;
					float x;
					float y;
					float width;
					float height;
					int zOrder;
				};
				struct Backgrounds
				{
					std::string url;
					int display;
					float x;
					float y;
					float width;
					float height;
					int zOrder;
				};

			public:
				UpdateMPULayoutRequest();
				~UpdateMPULayoutRequest();

				std::vector<UserPanes> getUserPanes()const;
				void setUserPanes(const std::vector<UserPanes>& userPanes);
				int getBackgroundColor()const;
				void setBackgroundColor(int backgroundColor);
				int getCropMode()const;
				void setCropMode(int cropMode);
				std::vector<long> getLayoutIds()const;
				void setLayoutIds(const std::vector<long>& layoutIds);
				std::string getTaskId()const;
				void setTaskId(const std::string& taskId);
				std::vector<ClockWidgets> getClockWidgets()const;
				void setClockWidgets(const std::vector<ClockWidgets>& clockWidgets);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				std::vector<Watermarks> getWatermarks()const;
				void setWatermarks(const std::vector<Watermarks>& watermarks);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getAppId()const;
				void setAppId(const std::string& appId);
				std::vector<Backgrounds> getBackgrounds()const;
				void setBackgrounds(const std::vector<Backgrounds>& backgrounds);

            private:
				std::vector<UserPanes> userPanes_;
				int backgroundColor_;
				int cropMode_;
				std::vector<long> layoutIds_;
				std::string taskId_;
				std::vector<ClockWidgets> clockWidgets_;
				std::string showLog_;
				std::vector<Watermarks> watermarks_;
				long ownerId_;
				std::string appId_;
				std::vector<Backgrounds> backgrounds_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_UPDATEMPULAYOUTREQUEST_H_