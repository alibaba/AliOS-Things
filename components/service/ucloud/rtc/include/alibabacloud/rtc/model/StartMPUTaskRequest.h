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

#ifndef ALIBABACLOUD_RTC_MODEL_STARTMPUTASKREQUEST_H_
#define ALIBABACLOUD_RTC_MODEL_STARTMPUTASKREQUEST_H_

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
			class ALIBABACLOUD_RTC_EXPORT StartMPUTaskRequest : public RpcServiceRequest
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
				StartMPUTaskRequest();
				~StartMPUTaskRequest();

				int getPayloadType()const;
				void setPayloadType(int payloadType);
				std::vector<UserPanes> getUserPanes()const;
				void setUserPanes(const std::vector<UserPanes>& userPanes);
				int getBackgroundColor()const;
				void setBackgroundColor(int backgroundColor);
				int getReportVad()const;
				void setReportVad(int reportVad);
				std::string getSourceType()const;
				void setSourceType(const std::string& sourceType);
				std::string getTaskId()const;
				void setTaskId(const std::string& taskId);
				std::vector<ClockWidgets> getClockWidgets()const;
				void setClockWidgets(const std::vector<ClockWidgets>& clockWidgets);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				long getVadInterval()const;
				void setVadInterval(long vadInterval);
				std::vector<Watermarks> getWatermarks()const;
				void setWatermarks(const std::vector<Watermarks>& watermarks);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				int getMediaEncode()const;
				void setMediaEncode(int mediaEncode);
				int getRtpExtInfo()const;
				void setRtpExtInfo(int rtpExtInfo);
				int getCropMode()const;
				void setCropMode(int cropMode);
				std::string getTaskProfile()const;
				void setTaskProfile(const std::string& taskProfile);
				std::vector<long> getLayoutIds()const;
				void setLayoutIds(const std::vector<long>& layoutIds);
				std::string getStreamURL()const;
				void setStreamURL(const std::string& streamURL);
				int getStreamType()const;
				void setStreamType(int streamType);
				std::vector<std::string> getSubSpecUsers()const;
				void setSubSpecUsers(const std::vector<std::string>& subSpecUsers);
				std::string getAppId()const;
				void setAppId(const std::string& appId);
				std::vector<Backgrounds> getBackgrounds()const;
				void setBackgrounds(const std::vector<Backgrounds>& backgrounds);
				long getTimeStampRef()const;
				void setTimeStampRef(long timeStampRef);
				int getMixMode()const;
				void setMixMode(int mixMode);
				std::string getChannelId()const;
				void setChannelId(const std::string& channelId);

            private:
				int payloadType_;
				std::vector<UserPanes> userPanes_;
				int backgroundColor_;
				int reportVad_;
				std::string sourceType_;
				std::string taskId_;
				std::vector<ClockWidgets> clockWidgets_;
				std::string showLog_;
				long vadInterval_;
				std::vector<Watermarks> watermarks_;
				long ownerId_;
				int mediaEncode_;
				int rtpExtInfo_;
				int cropMode_;
				std::string taskProfile_;
				std::vector<long> layoutIds_;
				std::string streamURL_;
				int streamType_;
				std::vector<std::string> subSpecUsers_;
				std::string appId_;
				std::vector<Backgrounds> backgrounds_;
				long timeStampRef_;
				int mixMode_;
				std::string channelId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_STARTMPUTASKREQUEST_H_