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

#ifndef SRC_RTSPPLAYER_RTSPPLAYER_H_TXT_
#define SRC_RTSPPLAYER_RTSPPLAYER_H_TXT_

#include <string>
#include <memory>
#include "Rtsp.h"
#include "RtspSession.h"
#include "RtspMediaSource.h"
#include "Player/PlayerBase.h"
#include "Util/util.h"
#include "Util/logger.h"
#include "Util/TimeTicker.h"
#include "Poller/Timer.h"
#include "Network/Socket.h"
#include "Network/TcpClient.h"
#include "RtspSplitter.h"
#include "RtpReceiver.h"

using namespace std;
using namespace toolkit;

namespace mediakit {

//实现了rtsp播放器协议部分的功能
class RtspPlayer: public PlayerBase,public TcpClient, public RtspSplitter, public RtpReceiver {
public:
	typedef std::shared_ptr<RtspPlayer> Ptr;

	RtspPlayer();
	virtual ~RtspPlayer(void);
	void play(const char* strUrl) override;
	void pause(bool bPause) override;
	void teardown() override;
	float getPacketLossRate(TrackType type) const override;
protected:
	//派生类回调函数
	virtual bool onCheckSDP(const string &strSdp, const SdpAttr &sdpAttr) = 0;
	virtual void onRecvRTP(const RtpPacket::Ptr &pRtppt, const SdpTrack::Ptr &track) = 0;
    uint32_t getProgressMilliSecond() const;
    void seekToMilliSecond(uint32_t ms);

    /**
     * 收到完整的rtsp包回调，包括sdp等content数据
     * @param parser rtsp包
     */
    void onWholeRtspPacket(Parser &parser) override ;

    /**
     * 收到rtp包回调
     * @param data
     * @param len
     */
    void onRtpPacket(const char *data,uint64_t len) override ;

    /**
     * rtp数据包排序后输出
     * @param rtppt rtp数据包
     * @param trackidx track索引
     */
	void onRtpSorted(const RtpPacket::Ptr &rtppt, int trackidx) override;
private:
	void onShutdown_l(const SockException &ex);
	void onRecvRTP_l(const RtpPacket::Ptr &pRtppt, const SdpTrack::Ptr &track);
	void onPlayResult_l(const SockException &ex);

    int getTrackIndexByControlSuffix(const string &controlSuffix) const;
    int getTrackIndexByInterleaved(int interleaved) const;
	int getTrackIndexByTrackType(TrackType trackId) const;

	void play(const char* strUrl, const char *strUser, const char *strPwd,  eRtpType eType);
	void onConnect(const SockException &err) override;
	void onRecv(const Buffer::Ptr &pBuf) override;
	void onErr(const SockException &ex) override;

	void handleResSETUP(const Parser &parser, unsigned int uiTrackIndex);
	void handleResDESCRIBE(const Parser &parser);
	bool handleAuthenticationFailure(const string &wwwAuthenticateParamsStr);
	void handleResPAUSE(const Parser &parser, bool bPause);

	//发送SETUP命令
    bool sendSetup(unsigned int uiTrackIndex);
    bool sendPause(bool bPause,uint32_t ms);
	bool sendOptions();
	bool sendDescribe();

    bool sendRtspRequest(const string &cmd, const string &url ,const StrCaseMap &header = StrCaseMap());
<<<<<<< HEAD

=======
	bool sendRtspRequest(const string &cmd, const string &url ,const std::initializer_list<string> &header);
>>>>>>> 63b8477f0d639675e541bb6f11a53f722fb7494c
private:
	string _strUrl;
	SdpAttr _sdpAttr;
	vector<SdpTrack::Ptr> _aTrackInfo;

	function<void(const Parser&)> _onHandshake;
	Socket::Ptr _apUdpSock[2];
	//rtsp info
	string _strSession;
	unsigned int _uiCseq = 1;
	string _strContentBase;
	eRtpType _eType = RTP_TCP;

	/* 丢包率统计需要用到的参数 */
	uint16_t _aui16FirstSeq[2] = { 0 , 0};
	uint16_t _aui16NowSeq[2] = { 0 , 0 };
	uint64_t _aui64RtpRecv[2] = { 0 , 0};

	//超时功能实现
	Ticker _rtpTicker;
	std::shared_ptr<Timer> _pPlayTimer;
	std::shared_ptr<Timer> _pRtpTimer;
	//心跳定时器
	std::shared_ptr<Timer> _pBeatTimer;
    
    //播放进度控制,单位毫秒
    uint32_t _iSeekTo = 0;

    //单位毫秒
	uint32_t _aiFistStamp[2] = {0,0};
	uint32_t _aiNowStamp[2] = {0,0};
};

} /* namespace mediakit */

#endif /* SRC_RTSPPLAYER_RTSPPLAYER_H_TXT_ */
