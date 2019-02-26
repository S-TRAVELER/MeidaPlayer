﻿/*
 * MIT License
 *
 * Copyright (c) 2016 xiongziliang <771730766@qq.com>
 *
 * This file is part of ZLMediaKit(https://github.com/xiongziliang/ZLMediaKit).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cctype>
#include <algorithm>
#include "RtspDemuxer.h"
#include "Util/base64.h"
#include "H264/SPSParser.h"
#include "Extension/Factory.h"

using namespace std;

namespace mediakit {

RtspDemuxer::RtspDemuxer(const string& sdp) {
	loadSdp(SdpAttr(sdp));
}

RtspDemuxer::RtspDemuxer(const SdpAttr &attr) {
	loadSdp(attr);
}

void RtspDemuxer::loadSdp(const SdpAttr &attr) {
	auto tracks = attr.getAvailableTrack();
	for (auto &track : tracks){
		switch (track->_type) {
			case TrackVideo: {
				makeVideoTrack(track);
			}
				break;
			case TrackAudio: {
				makeAudioTrack(track);
			}
				break;
			default:
				break;
		}
	}
	auto titleTrack = attr.getTrack(TrackTitle);
	if(titleTrack){
		_fDuration = titleTrack->_duration;
	}
}
bool RtspDemuxer::inputRtp(const RtpPacket::Ptr & rtp) {
	switch (rtp->type) {
	case TrackVideo:{
		if(_videoRtpDecoder){
			return _videoRtpDecoder->inputRtp(rtp, true);
		}
		return false;
	}
	case TrackAudio:{
		if(_audioRtpDecoder){
			_audioRtpDecoder->inputRtp(rtp, false);
			return false;
		}
		return false;
	}
	default:
		return false;
	}
}


void RtspDemuxer::makeAudioTrack(const SdpTrack::Ptr &audio) {
	//生成Track对象
    _audioTrack = dynamic_pointer_cast<AudioTrack>(Factory::getTrackBySdp(audio));
    if(_audioTrack){
    	//生成RtpCodec对象以便解码rtp
		_audioRtpDecoder = Factory::getRtpDecoderByTrack(_audioTrack);
		if(_audioRtpDecoder){
			//设置rtp解码器代理，生成的frame写入该Track
			_audioRtpDecoder->addDelegate(_audioTrack);
		} else{
			//找不到相应的rtp解码器，该track无效
			_audioTrack.reset();
		}
    }
}

void RtspDemuxer::makeVideoTrack(const SdpTrack::Ptr &video) {
	//生成Track对象
	_videoTrack = dynamic_pointer_cast<VideoTrack>(Factory::getTrackBySdp(video));
	if(_videoTrack){
		//生成RtpCodec对象以便解码rtp
		_videoRtpDecoder = Factory::getRtpDecoderByTrack(_videoTrack);
		if(_videoRtpDecoder){
			//设置rtp解码器代理，生成的frame写入该Track
			_videoRtpDecoder->addDelegate(_videoTrack);
		}else{
			//找不到相应的rtp解码器，该track无效
			_videoTrack.reset();
		}
	}
}

} /* namespace mediakit */
