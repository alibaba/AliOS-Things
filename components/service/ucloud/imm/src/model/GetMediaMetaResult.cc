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

#include <alibabacloud/imm/model/GetMediaMetaResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetMediaMetaResult::GetMediaMetaResult() :
	ServiceResult()
{}

GetMediaMetaResult::GetMediaMetaResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetMediaMetaResult::~GetMediaMetaResult()
{}

void GetMediaMetaResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto mediaMetaNode = value["MediaMeta"];
	auto mediaFormatNode = mediaMetaNode["MediaFormat"];
	if(!mediaFormatNode["NumberStreams"].isNull())
		mediaMeta_.mediaFormat.numberStreams = std::stoi(mediaFormatNode["NumberStreams"].asString());
	if(!mediaFormatNode["NumberPrograms"].isNull())
		mediaMeta_.mediaFormat.numberPrograms = std::stoi(mediaFormatNode["NumberPrograms"].asString());
	if(!mediaFormatNode["FormatName"].isNull())
		mediaMeta_.mediaFormat.formatName = mediaFormatNode["FormatName"].asString();
	if(!mediaFormatNode["FormatLongName"].isNull())
		mediaMeta_.mediaFormat.formatLongName = mediaFormatNode["FormatLongName"].asString();
	if(!mediaFormatNode["Size"].isNull())
		mediaMeta_.mediaFormat.size = mediaFormatNode["Size"].asString();
	if(!mediaFormatNode["Duration"].isNull())
		mediaMeta_.mediaFormat.duration = mediaFormatNode["Duration"].asString();
	if(!mediaFormatNode["StartTime"].isNull())
		mediaMeta_.mediaFormat.startTime = mediaFormatNode["StartTime"].asString();
	if(!mediaFormatNode["Bitrate"].isNull())
		mediaMeta_.mediaFormat.bitrate = mediaFormatNode["Bitrate"].asString();
	if(!mediaFormatNode["CreationTime"].isNull())
		mediaMeta_.mediaFormat.creationTime = mediaFormatNode["CreationTime"].asString();
	if(!mediaFormatNode["Location"].isNull())
		mediaMeta_.mediaFormat.location = mediaFormatNode["Location"].asString();
	auto addressNode = mediaFormatNode["Address"];
	if(!addressNode["AddressLine"].isNull())
		mediaMeta_.mediaFormat.address.addressLine = addressNode["AddressLine"].asString();
	if(!addressNode["Country"].isNull())
		mediaMeta_.mediaFormat.address.country = addressNode["Country"].asString();
	if(!addressNode["Province"].isNull())
		mediaMeta_.mediaFormat.address.province = addressNode["Province"].asString();
	if(!addressNode["City"].isNull())
		mediaMeta_.mediaFormat.address.city = addressNode["City"].asString();
	if(!addressNode["District"].isNull())
		mediaMeta_.mediaFormat.address.district = addressNode["District"].asString();
	if(!addressNode["Township"].isNull())
		mediaMeta_.mediaFormat.address.township = addressNode["Township"].asString();
	auto tagNode = mediaFormatNode["Tag"];
	if(!tagNode["Language"].isNull())
		mediaMeta_.mediaFormat.tag.language = tagNode["Language"].asString();
	if(!tagNode["CreationTime"].isNull())
		mediaMeta_.mediaFormat.tag.creationTime = tagNode["CreationTime"].asString();
	if(!tagNode["Album"].isNull())
		mediaMeta_.mediaFormat.tag.album = tagNode["Album"].asString();
	if(!tagNode["AlbumArtist"].isNull())
		mediaMeta_.mediaFormat.tag.albumArtist = tagNode["AlbumArtist"].asString();
	if(!tagNode["Artist"].isNull())
		mediaMeta_.mediaFormat.tag.artist = tagNode["Artist"].asString();
	if(!tagNode["Composer"].isNull())
		mediaMeta_.mediaFormat.tag.composer = tagNode["Composer"].asString();
	if(!tagNode["Title"].isNull())
		mediaMeta_.mediaFormat.tag.title = tagNode["Title"].asString();
	if(!tagNode["Performer"].isNull())
		mediaMeta_.mediaFormat.tag.performer = tagNode["Performer"].asString();
	auto mediaStreamsNode = mediaMetaNode["MediaStreams"];
	auto allVideoStreamsNode = mediaStreamsNode["VideoStreams"]["VideoStream"];
	for (auto mediaStreamsNodeVideoStreamsVideoStream : allVideoStreamsNode)
	{
		MediaMeta::MediaStreams::VideoStream videoStreamObject;
		if(!mediaStreamsNodeVideoStreamsVideoStream["Index"].isNull())
			videoStreamObject.index = std::stoi(mediaStreamsNodeVideoStreamsVideoStream["Index"].asString());
		if(!mediaStreamsNodeVideoStreamsVideoStream["Language"].isNull())
			videoStreamObject.language = mediaStreamsNodeVideoStreamsVideoStream["Language"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["CodecName"].isNull())
			videoStreamObject.codecName = mediaStreamsNodeVideoStreamsVideoStream["CodecName"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["CodecLongName"].isNull())
			videoStreamObject.codecLongName = mediaStreamsNodeVideoStreamsVideoStream["CodecLongName"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["Profile"].isNull())
			videoStreamObject.profile = mediaStreamsNodeVideoStreamsVideoStream["Profile"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["CodecTimeBase"].isNull())
			videoStreamObject.codecTimeBase = mediaStreamsNodeVideoStreamsVideoStream["CodecTimeBase"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["CodecTagString"].isNull())
			videoStreamObject.codecTagString = mediaStreamsNodeVideoStreamsVideoStream["CodecTagString"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["CodecTag"].isNull())
			videoStreamObject.codecTag = mediaStreamsNodeVideoStreamsVideoStream["CodecTag"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["Width"].isNull())
			videoStreamObject.width = std::stoi(mediaStreamsNodeVideoStreamsVideoStream["Width"].asString());
		if(!mediaStreamsNodeVideoStreamsVideoStream["Height"].isNull())
			videoStreamObject.height = std::stoi(mediaStreamsNodeVideoStreamsVideoStream["Height"].asString());
		if(!mediaStreamsNodeVideoStreamsVideoStream["HasBFrames"].isNull())
			videoStreamObject.hasBFrames = std::stoi(mediaStreamsNodeVideoStreamsVideoStream["HasBFrames"].asString());
		if(!mediaStreamsNodeVideoStreamsVideoStream["SampleAspectRatio"].isNull())
			videoStreamObject.sampleAspectRatio = mediaStreamsNodeVideoStreamsVideoStream["SampleAspectRatio"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["DisplayAspectRatio"].isNull())
			videoStreamObject.displayAspectRatio = mediaStreamsNodeVideoStreamsVideoStream["DisplayAspectRatio"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["PixelFormat"].isNull())
			videoStreamObject.pixelFormat = mediaStreamsNodeVideoStreamsVideoStream["PixelFormat"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["Level"].isNull())
			videoStreamObject.level = std::stoi(mediaStreamsNodeVideoStreamsVideoStream["Level"].asString());
		if(!mediaStreamsNodeVideoStreamsVideoStream["FrameRrate"].isNull())
			videoStreamObject.frameRrate = mediaStreamsNodeVideoStreamsVideoStream["FrameRrate"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["AverageFrameRate"].isNull())
			videoStreamObject.averageFrameRate = mediaStreamsNodeVideoStreamsVideoStream["AverageFrameRate"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["TimeBase"].isNull())
			videoStreamObject.timeBase = mediaStreamsNodeVideoStreamsVideoStream["TimeBase"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["StartTime"].isNull())
			videoStreamObject.startTime = mediaStreamsNodeVideoStreamsVideoStream["StartTime"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["Duration"].isNull())
			videoStreamObject.duration = mediaStreamsNodeVideoStreamsVideoStream["Duration"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["Bitrate"].isNull())
			videoStreamObject.bitrate = mediaStreamsNodeVideoStreamsVideoStream["Bitrate"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["Frames"].isNull())
			videoStreamObject.frames = mediaStreamsNodeVideoStreamsVideoStream["Frames"].asString();
		if(!mediaStreamsNodeVideoStreamsVideoStream["Rotate"].isNull())
			videoStreamObject.rotate = mediaStreamsNodeVideoStreamsVideoStream["Rotate"].asString();
		mediaMeta_.mediaStreams.videoStreams.push_back(videoStreamObject);
	}
	auto allAudioStreamsNode = mediaStreamsNode["AudioStreams"]["AudioStream"];
	for (auto mediaStreamsNodeAudioStreamsAudioStream : allAudioStreamsNode)
	{
		MediaMeta::MediaStreams::AudioStream audioStreamObject;
		if(!mediaStreamsNodeAudioStreamsAudioStream["Index"].isNull())
			audioStreamObject.index = std::stoi(mediaStreamsNodeAudioStreamsAudioStream["Index"].asString());
		if(!mediaStreamsNodeAudioStreamsAudioStream["CodecName"].isNull())
			audioStreamObject.codecName = mediaStreamsNodeAudioStreamsAudioStream["CodecName"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["CodecLongName"].isNull())
			audioStreamObject.codecLongName = mediaStreamsNodeAudioStreamsAudioStream["CodecLongName"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["CodecTimeBase"].isNull())
			audioStreamObject.codecTimeBase = mediaStreamsNodeAudioStreamsAudioStream["CodecTimeBase"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["CodecTagString"].isNull())
			audioStreamObject.codecTagString = mediaStreamsNodeAudioStreamsAudioStream["CodecTagString"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["CodecTag"].isNull())
			audioStreamObject.codecTag = mediaStreamsNodeAudioStreamsAudioStream["CodecTag"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["SampleFormat"].isNull())
			audioStreamObject.sampleFormat = mediaStreamsNodeAudioStreamsAudioStream["SampleFormat"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["SampleRate"].isNull())
			audioStreamObject.sampleRate = mediaStreamsNodeAudioStreamsAudioStream["SampleRate"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["Channels"].isNull())
			audioStreamObject.channels = std::stoi(mediaStreamsNodeAudioStreamsAudioStream["Channels"].asString());
		if(!mediaStreamsNodeAudioStreamsAudioStream["ChannelLayout"].isNull())
			audioStreamObject.channelLayout = mediaStreamsNodeAudioStreamsAudioStream["ChannelLayout"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["TimeBase"].isNull())
			audioStreamObject.timeBase = mediaStreamsNodeAudioStreamsAudioStream["TimeBase"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["StartTime"].isNull())
			audioStreamObject.startTime = mediaStreamsNodeAudioStreamsAudioStream["StartTime"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["Duration"].isNull())
			audioStreamObject.duration = mediaStreamsNodeAudioStreamsAudioStream["Duration"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["Bitrate"].isNull())
			audioStreamObject.bitrate = mediaStreamsNodeAudioStreamsAudioStream["Bitrate"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["Frames"].isNull())
			audioStreamObject.frames = mediaStreamsNodeAudioStreamsAudioStream["Frames"].asString();
		if(!mediaStreamsNodeAudioStreamsAudioStream["Language"].isNull())
			audioStreamObject.language = mediaStreamsNodeAudioStreamsAudioStream["Language"].asString();
		mediaMeta_.mediaStreams.audioStreams.push_back(audioStreamObject);
	}
	auto allSubtitleStreamsNode = mediaStreamsNode["SubtitleStreams"]["SubtitleStream"];
	for (auto mediaStreamsNodeSubtitleStreamsSubtitleStream : allSubtitleStreamsNode)
	{
		MediaMeta::MediaStreams::SubtitleStream subtitleStreamObject;
		if(!mediaStreamsNodeSubtitleStreamsSubtitleStream["Index"].isNull())
			subtitleStreamObject.index = std::stoi(mediaStreamsNodeSubtitleStreamsSubtitleStream["Index"].asString());
		if(!mediaStreamsNodeSubtitleStreamsSubtitleStream["Language"].isNull())
			subtitleStreamObject.language = mediaStreamsNodeSubtitleStreamsSubtitleStream["Language"].asString();
		mediaMeta_.mediaStreams.subtitleStreams.push_back(subtitleStreamObject);
	}
	if(!value["MediaUri"].isNull())
		mediaUri_ = value["MediaUri"].asString();

}

std::string GetMediaMetaResult::getMediaUri()const
{
	return mediaUri_;
}

GetMediaMetaResult::MediaMeta GetMediaMetaResult::getMediaMeta()const
{
	return mediaMeta_;
}

