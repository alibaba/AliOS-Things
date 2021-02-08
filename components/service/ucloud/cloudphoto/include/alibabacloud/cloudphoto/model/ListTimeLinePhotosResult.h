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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTTIMELINEPHOTOSRESULT_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTTIMELINEPHOTOSRESULT_H_

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
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT ListTimeLinePhotosResult : public ServiceResult
			{
			public:
				struct Photo
				{
					bool isVideo;
					long shareExpireTime;
					std::string idStr;
					long size;
					std::string title;
					long mtime;
					std::string remark;
					long like;
					std::string state;
					long ctime;
					std::string fileId;
					long height;
					long id;
					long width;
					long takenAt;
					std::string location;
					std::string md5;
				};


				ListTimeLinePhotosResult();
				explicit ListTimeLinePhotosResult(const std::string &payload);
				~ListTimeLinePhotosResult();
				std::vector<Photo> getPhotos()const;
				int getTotalCount()const;
				std::string getAction()const;
				std::string getMessage()const;
				std::string getCode()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<Photo> photos_;
				int totalCount_;
				std::string action_;
				std::string message_;
				std::string code_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTTIMELINEPHOTOSRESULT_H_