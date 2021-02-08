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

#ifndef ALIBABACLOUD_RTC_MODEL_UPDATERECORDTEMPLATEREQUEST_H_
#define ALIBABACLOUD_RTC_MODEL_UPDATERECORDTEMPLATEREQUEST_H_

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
			class ALIBABACLOUD_RTC_EXPORT UpdateRecordTemplateRequest : public RpcServiceRequest
			{
			public:
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
				UpdateRecordTemplateRequest();
				~UpdateRecordTemplateRequest();

				std::vector<std::string> getFormats()const;
				void setFormats(const std::vector<std::string>& formats);
				std::string getOssFilePrefix()const;
				void setOssFilePrefix(const std::string& ossFilePrefix);
				int getBackgroundColor()const;
				void setBackgroundColor(int backgroundColor);
				std::string getTaskProfile()const;
				void setTaskProfile(const std::string& taskProfile);
				std::vector<long> getLayoutIds()const;
				void setLayoutIds(const std::vector<long>& layoutIds);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				std::string getOssBucket()const;
				void setOssBucket(const std::string& ossBucket);
				std::string getMnsQueue()const;
				void setMnsQueue(const std::string& mnsQueue);
				int getFileSplitInterval()const;
				void setFileSplitInterval(int fileSplitInterval);
				std::string getHttpCallbackUrl()const;
				void setHttpCallbackUrl(const std::string& httpCallbackUrl);
				std::vector<Watermarks> getWatermarks()const;
				void setWatermarks(const std::vector<Watermarks>& watermarks);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getTemplateId()const;
				void setTemplateId(const std::string& templateId);
				std::string getAppId()const;
				void setAppId(const std::string& appId);
				std::vector<Backgrounds> getBackgrounds()const;
				void setBackgrounds(const std::vector<Backgrounds>& backgrounds);
				std::string getName()const;
				void setName(const std::string& name);
				int getMediaEncode()const;
				void setMediaEncode(int mediaEncode);

            private:
				std::vector<std::string> formats_;
				std::string ossFilePrefix_;
				int backgroundColor_;
				std::string taskProfile_;
				std::vector<long> layoutIds_;
				std::string showLog_;
				std::string ossBucket_;
				std::string mnsQueue_;
				int fileSplitInterval_;
				std::string httpCallbackUrl_;
				std::vector<Watermarks> watermarks_;
				long ownerId_;
				std::string templateId_;
				std::string appId_;
				std::vector<Backgrounds> backgrounds_;
				std::string name_;
				int mediaEncode_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_UPDATERECORDTEMPLATEREQUEST_H_